// A test-only sink records the actual stereo signal sent to Rack's AUDIO module.
// It is absent from production builds and distributable manifests.
#include <atomic>
#include <fstream>
#include <vector>
#include <cmath>
#include "VideoCaptureState.hpp"

struct RackAudioQa : Module {
    const bool video = std::getenv("METROPOLIX_VIDEO_QA") != nullptr;
    const int frames = (video && std::getenv("METROPOLIX_VIDEO_SECONDS") ?
                        clamp(std::atoi(std::getenv("METROPOLIX_VIDEO_SECONDS")), 1, 80) :
                        std::getenv("METROPOLIX_VARIATIONS_QA") ? 80 :
                        std::getenv("METROPOLIX_MUSICAL_QA") ? 24 : 12) * 48000;
    std::vector<float> samples;
    std::vector<float> cvSamples;
    const bool performance = std::getenv("METROPOLIX_PERFORMANCE_VIDEO") != nullptr;
    std::vector<float> trace;
    float tracePeak[10] = {};
    bool cvConnected = false;
    int cursor = 0;
    std::atomic<bool> ready{false};
    RackAudioQa() : samples(frames * 2, 0.f), cvSamples(frames * 2, 0.f),
        trace(performance ? frames / 480 * 10 : 0, 0.f) { config(0, 12, 0); }
    void process(const ProcessArgs& args) override {
        if (video && !metrovideo::armed.load(std::memory_order_acquire)) return;
        if (ready.load(std::memory_order_relaxed) || !inputs[0].isConnected() || !inputs[1].isConnected()) return;
        samples[cursor * 2] = inputs[0].getVoltage();
        samples[cursor * 2 + 1] = inputs[1].getVoltage();
        if (performance) {
            for (int i = 0; i < 10; ++i) {
                float v = inputs[i + 2].getVoltage();
                tracePeak[i] = (i == 0 || i == 2) ? v : std::max(tracePeak[i], std::abs(v));
                if (cursor % 480 == 479) {
                    trace[(cursor / 480) * 10 + i] = tracePeak[i];
                    tracePeak[i] = 0.f;
                }
            }
        }
        cvConnected = inputs[2].isConnected() && inputs[3].isConnected();
        if (cvConnected) {
            cvSamples[cursor * 2] = inputs[2].getVoltage();
            cvSamples[cursor * 2 + 1] = inputs[3].getVoltage();
        }
        ++cursor;
        if (video) metrovideo::samples.store(cursor, std::memory_order_release);
        if (cursor == frames) ready.store(true, std::memory_order_release);
    }
};
struct RackAudioQaWidget : ModuleWidget {
    bool saved = false;
    RackAudioQaWidget(RackAudioQa* module) {
        setModule(module);
        box.size = Vec(30, 380);
        for (int i = 0; i < 12; ++i)
            addInput(createInputCentered<PJ301MPort>(Vec(15, 20 + i * 28), module, i));
    }
    void step() override {
        ModuleWidget::step();
        auto* m = getModule<RackAudioQa>();
        const char* directory = std::getenv("METROPOLIX_QA_DIR");
        if (!m || !directory || saved || !m->ready.load(std::memory_order_acquire)) return;
        saved = true;
        std::ofstream report(std::string(directory) + "/report.txt", std::ios::app);
        std::ofstream wav(std::string(directory) + "/two-track-demo.wav", std::ios::binary);
        auto u16 = [&](uint16_t n) { char bytes[] = {char(n), char(n >> 8)}; wav.write(bytes, 2); };
        auto u32 = [&](uint32_t n) { u16(n & 65535); u16(n >> 16); };
        wav.write("RIFF", 4); u32(36 + m->frames * 4); wav.write("WAVEfmt ", 8);
        u32(16); u16(1); u16(2); u32(48000); u32(48000 * 4); u16(4); u16(16);
        wav.write("data", 4); u32(m->frames * 4);
        double energy[2] = {}, mean[2] = {}, peak[2] = {};
        bool finite = true;
        for (int frame = 0; frame < m->frames; ++frame) for (int ch = 0; ch < 2; ++ch) {
            float sample = m->samples[frame * 2 + ch];
            finite &= std::isfinite(sample);
            if (!std::isfinite(sample)) sample = 0.f;
            energy[ch] += sample * sample;
            mean[ch] += sample;
            peak[ch] = std::max(peak[ch], double(std::abs(sample)));
            int16_t pcm = static_cast<int16_t>(std::round(clamp(sample / 10.f, -1.f, 1.f) * 32767));
            u16(static_cast<uint16_t>(pcm));
        }
        bool good = finite;
        for (int ch = 0; ch < 2; ++ch) {
            double rms = std::sqrt(energy[ch] / m->frames);
            double dc = mean[ch] / m->frames;
            bool channelGood = rms > .05 && peak[ch] < 4 && std::abs(dc) < .05;
            good &= channelGood;
            report << (channelGood ? "PASS " : "FAIL ") << "channel " << ch + 1
                << ": RMS=" << rms << " V, peak=" << peak[ch] << " V, DC=" << dc << " V\n";
        }
        wav.close();
        good &= !wav.fail();
        if (m->performance) {
            std::ofstream trace(std::string(directory) + "/signals.csv");
            trace << "seconds,pitch1,gate1,pitch2,gate2,kick,snare,hat,bass_peak,lead_peak,drum_peak\n";
            for (int block = 0; block < m->frames / 480; ++block) {
                trace << (block + 1) / 100.0;
                for (int i = 0; i < 10; ++i) trace << ',' << m->trace[block * 10 + i];
                trace << '\n';
            }
            trace.close();
            good &= !trace.fail();
        }
        if (std::getenv("METROPOLIX_MOD_QA")) {
            double minimum[2] = {100,100}, maximum[2] = {-100,-100};
            int changes[2] = {};
            bool cvFinite = true;
            std::ofstream csv(std::string(directory) + "/mod-cv.csv");
            csv << "seconds,out_a_volts,out_b_volts\n";
            for (int frame = 0; frame < m->frames; ++frame) {
                for (int ch = 0; ch < 2; ++ch) {
                    float value = m->cvSamples[frame * 2 + ch];
                    cvFinite &= std::isfinite(value);
                    minimum[ch] = std::min(minimum[ch], double(value));
                    maximum[ch] = std::max(maximum[ch], double(value));
                    if (frame && std::abs(value - m->cvSamples[(frame - 1) * 2 + ch]) > .00001f) ++changes[ch];
                }
                if (frame % 480 == 0) csv << frame / 48000.0 << ',' << m->cvSamples[frame*2] << ',' << m->cvSamples[frame*2+1] << '\n';
            }
            bool cvGood = m->cvConnected && cvFinite && minimum[0] == -2 && maximum[0] == 2
                && minimum[1] >= 0 && maximum[1] > 4.99 && maximum[1] <= 5
                && changes[0] > 15 && changes[1] > 100000;
            csv.close();
            cvGood &= !csv.fail();
            good &= cvGood;
            report << (cvGood ? "PASS " : "FAIL ") << "Rack cable CV: A=[" << minimum[0] << ',' << maximum[0]
                << "] V, B=[" << minimum[1] << ',' << maximum[1] << "] V; changes=" << changes[0] << '/' << changes[1] << '\n';
        }
        report << (good ? "PASS " : "FAIL ") << "AUDIO QA COMPLETE (" << m->frames / 48000 << " seconds at 48 kHz)\n";
        report.close();
        APP->window->close();
    }
};
