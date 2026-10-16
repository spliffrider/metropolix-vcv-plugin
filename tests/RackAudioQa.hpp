// A test-only sink records the actual stereo signal sent to Rack's AUDIO module.
// It is absent from production builds and distributable manifests.
#include <atomic>
#include <array>
#include <fstream>
#include <vector>
#include <cmath>
#include "VideoCaptureState.hpp"
#include "RackCapture.hpp"

struct RackAudioQa : Module {
    const bool video = std::getenv("METROPOLIX_VIDEO_QA") != nullptr;
    const int frames = (video && std::getenv("METROPOLIX_VIDEO_SECONDS") ?
                        clamp(std::atoi(std::getenv("METROPOLIX_VIDEO_SECONDS")), 1, 180) :
                        std::getenv("METROPOLIX_VARIATIONS_QA") ? 80 :
                        std::getenv("METROPOLIX_MUSICAL_QA") ? 24 : 12) * 48000;
    std::vector<float> samples;
    std::vector<float> cvSamples;
    const bool mxTest=std::getenv("METROPOLIX_MX_AUDIO_QA")!=nullptr;
    const bool performance = std::getenv("METROPOLIX_PERFORMANCE_VIDEO") != nullptr || mxTest;
    std::array<float,8> previousMx{};std::array<unsigned,8> mxEdges{};bool mxSignalGood=true;
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
        if(mxTest){int high=0;for(int out=0;out<8;++out){float v=inputs[out+2].getVoltage();mxSignalGood &= inputs[out+2].isConnected() && (v==0 || v==5);if(v>0){++high;if(previousMx[out]==0)++mxEdges[out];}previousMx[out]=v;}mxSignalGood &= high<=1;}
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
        // A software-rendered CI window may reach fewer UI frames during the
        // recording than a desktop GPU. Capture when audio is complete too.
        if (!m->video) metroqa::screenshot(std::string(directory) + "/two-voice.png");
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
        if(std::getenv("METROPOLIX_CLOCK_AUDIO_QA")) {
            std::ofstream csv(std::string(directory)+"/clock-triggers.csv");csv<<"frame,seconds,width_frames\n";
            std::vector<int> edges;int width=0;bool timing=m->cvConnected;
            for(int frame=0;frame<m->frames;++frame){
                float v=m->cvSamples[frame*2];timing &= v==0 || v==5;
                if(v>0){if(!width)edges.push_back(frame);++width;}
                else if(width){timing &= width>=47 && width<=49;csv<<edges.back()<<','<<edges.back()/48000.0<<','<<width<<'\n';width=0;}
            }
            for(size_t i=1;i<edges.size();++i)timing &= std::abs(edges[i]-edges[i-1]-24000)<=1;
            timing &= edges.size()>=23 && edges.size()<=24 && !edges.empty() && edges.front()>=11998 && edges.front()<=12034;
            good &= timing;
            report<<(timing?"PASS ":"FAIL ")<<"CLOCK AUDIO TIMING: /4 offset2 through actual cable, "<<edges.size()<<" triggers at 0.5 second intervals\n";
        }
        if(std::getenv("METROPOLIX_OUTPUT_AUDIO_QA")) {
            double minimum=100,maximum=-100;int changes=0,width=0,edges=0,last=-1;bool routing=m->cvConnected;
            bool arrangement=std::getenv("METROPOLIX_OUTPUT_ARRANGEMENT_QA")!=nullptr;
            std::ofstream csv(std::string(directory)+"/output-signals.csv");csv<<"seconds,velocity_v,mod_clock_v\n";
            for(int frame=0;frame<m->frames;++frame){
                float a=m->cvSamples[frame*2],b=m->cvSamples[frame*2+1];
                routing &= std::isfinite(a) && a>=0 && a<=5 && (b==0 || b==5);
                minimum=std::min(minimum,double(a));maximum=std::max(maximum,double(a));
                if(frame && std::abs(a-m->cvSamples[(frame-1)*2])>.00001f)++changes;
                if(b>0){if(!width){if(last>=0 && !arrangement)routing &= std::abs(frame-last-18000)<=1;last=frame;++edges;}++width;}
                else if(width){routing &= width>=47 && width<=49;width=0;}
                if(frame%480==0)csv<<frame/48000.0<<','<<a<<','<<b<<'\n';
            }
            routing &= minimum<=2.5 && maximum>=4.99 && changes>15 && (arrangement?edges>15:edges>=31 && edges<=33);
            good &= routing;report<<(routing?"PASS ":"FAIL ")<<"OUTPUT AUDIO ROUTING: Velocity A=["<<minimum<<','<<maximum<<"] V, changes="<<changes<<"; MOD8 clock B "<<(arrangement?"arrangement":"/3")<<", "<<edges<<" one-millisecond triggers through actual cables\n";
        }
        wav.close();
        good &= !wav.fail();
        if(m->mxTest){bool mxGood=m->mxSignalGood;report<<"MX stage cable edges:";for(auto count:m->mxEdges){report<<' '<<count;mxGood &= count>0;}report<<'\n'<<(mxGood?"PASS ":"FAIL ")<<"MX AUDIO SIGNALS: all eight 0/5V stage outputs observed through actual cables; at most one stage high\n";good &= mxGood;}
        if (m->performance) {
            std::ofstream trace(std::string(directory) + "/signals.csv");
            trace << (m->mxTest?"seconds,mx1,mx2,mx3,mx4,mx5,mx6,mx7,mx8,kick_envelope,snare_envelope\n":"seconds,pitch1,gate1,pitch2,gate2,kick,snare,hat,bass_peak,lead_peak,drum_peak\n");
            for (int block = 0; block < m->frames / 480; ++block) {
                trace << (block + 1) / 100.0;
                for (int i = 0; i < 10; ++i) trace << ',' << m->trace[block * 10 + i];
                trace << '\n';
            }
            trace.close();
            good &= !trace.fail();
        }
        if (std::getenv("METROPOLIX_MOD_QA") || std::getenv("METROPOLIX_STAGE_CV_AUDIO_QA")) {
            double minimum[2] = {100,100}, maximum[2] = {-100,-100};
            int changes[2] = {};
            bool cvFinite = true;
            bool stageCv=std::getenv("METROPOLIX_STAGE_CV_AUDIO_QA")!=nullptr;
            std::ofstream csv(std::string(directory) + (stageCv?"/stage-cv.csv":"/mod-cv.csv"));
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
            if(stageCv)cvGood=m->cvConnected && cvFinite && minimum[0]>=-5 && minimum[1]>=-5 && maximum[0]<=5 && maximum[1]<=5 && minimum[0]<-.1 && minimum[1]<-.1 && maximum[0]>4.99 && maximum[1]>4.99 && changes[0]>1000 && changes[1]>1000;
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
