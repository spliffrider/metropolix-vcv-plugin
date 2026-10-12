// A test-only sink records the actual Fundamental VCO -> ADSR/VCA signal path.
// It is absent from production builds and distributable manifests.
#include <atomic>
#include <fstream>
#include <vector>
#include <cmath>

struct RackAudioQa : Module {
    static constexpr int frames = 12 * 48000;
    std::vector<float> samples;
    int cursor = 0;
    std::atomic<bool> ready{false};
    RackAudioQa() : samples(frames * 2, 0.f) { config(0, 2, 0); }
    void process(const ProcessArgs& args) override {
        if (ready.load(std::memory_order_relaxed) || !inputs[0].isConnected() || !inputs[1].isConnected()) return;
        samples[cursor * 2] = inputs[0].getVoltage();
        samples[cursor * 2 + 1] = inputs[1].getVoltage();
        if (++cursor == frames) ready.store(true, std::memory_order_release);
    }
};
struct RackAudioQaWidget : ModuleWidget {
    bool saved = false;
    RackAudioQaWidget(RackAudioQa* module) {
        setModule(module);
        box.size = Vec(30, 380);
        addInput(createInputCentered<PJ301MPort>(Vec(15, 40), module, 0));
        addInput(createInputCentered<PJ301MPort>(Vec(15, 80), module, 1));
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
        wav.write("RIFF", 4); u32(36 + RackAudioQa::frames * 4); wav.write("WAVEfmt ", 8);
        u32(16); u16(1); u16(2); u32(48000); u32(48000 * 4); u16(4); u16(16);
        wav.write("data", 4); u32(RackAudioQa::frames * 4);
        double energy[2] = {}, mean[2] = {}, peak[2] = {};
        bool finite = true;
        for (int frame = 0; frame < RackAudioQa::frames; ++frame) for (int ch = 0; ch < 2; ++ch) {
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
            double rms = std::sqrt(energy[ch] / RackAudioQa::frames);
            double dc = mean[ch] / RackAudioQa::frames;
            bool channelGood = rms > .05 && peak[ch] < 4 && std::abs(dc) < .05;
            good &= channelGood;
            report << (channelGood ? "PASS " : "FAIL ") << "channel " << ch + 1
                << ": RMS=" << rms << " V, peak=" << peak[ch] << " V, DC=" << dc << " V\n";
        }
        wav.close();
        good &= !wav.fail();
        report << (good ? "PASS " : "FAIL ") << "AUDIO QA COMPLETE (12 seconds at 48 kHz)\n";
        report.close();
        APP->window->close();
    }
};
