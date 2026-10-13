#include "../src/Sequencer.hpp"
#include "../src/Presets.hpp"
#include <iostream>
#include <cstdlib>
#include <thread>

int checks = 0;
void require(bool ok, const char* what) {
    ++checks;
    if (!ok) { std::cerr << "FAIL: " << what << '\n'; std::exit(1); }
}
int main() {
    metro::PresetTiming timing;
    require(!timing.loadDue(metro::LoadQueuePulses,32,0), "a fresh reset is not a queued pulse boundary");
    for (int i=0;i<8200;++i) {
        require(timing.loadDue(metro::LoadQueuePulses,32,0)==(i>0 && i%32==0), "queue uses multiples since reset, not since request");
        require(timing.loadDue(metro::LoadTotalPulses,32,0)==(i>0 && i%32==0), "Total Off falls back to Queue");
        require(!timing.loadDue(metro::LoadQueuePulses,0,0), "Queue Off waits for explicit reset");
        require(timing.loadDue(metro::LoadQueuePulses,4096,0)==(i>0 && i%4096==0), "maximum queue interval supports 4096 pulses");
        timing.tick();
    }
    timing.reset();
    for(int i=0;i<19;++i)timing.tick();
    require(timing.loadDue(metro::LoadTotalPulses,32,18), "Total length uses the playing preset");
    {
        metro::Sequencer s;
        s.process(1.0/48000,true,false,false,false,[&] {s.settings.bpm=240;s.restartAtClock();});
        require(std::abs(s.tracks[0].duration-15.0/240)<1e-9,"new CTRL tempo on a boundary determines the first note duration");
    }
    // Exact clock positions and first-note output over repeated transitions.
    // A fractional sample period detects drift caused by discarding phase.
    for(int rate : {44100,48000,96000}) {
        metro::Sequencer s, control;
        s.settings.bpm=control.settings.bpm=137;
        s.settings.scale=control.settings.scale=0;
        for(int stage=0;stage<8;++stage) {
            s.settings.stages[stage].pitch=float(stage)/12;
            s.settings.stages[stage].pulses=stage+1;
            s.settings.tracks[0].stages[stage].skip=stage==1 || stage==5;
        }
        metro::PresetTiming counter;
        int ticks=0, transitions=0, controlTicks=0;
        int samples=int(rate*15.0/137*65)+10;
        for(int sample=0;sample<samples;++sample) {
            bool edge=false,referenceEdge=false,transition=false;
            s.process(1.0/rate,true,false,false,false,[&] {
                edge=true;
                if(counter.loadDue(metro::LoadQueuePulses,4,0)) {
                    s.settings.root=(s.settings.root+1)%12;
                    s.restartAtClock();counter.reset();transition=true;++transitions;
                }
                counter.tick();++ticks;
            });
            control.process(1.0/rate,true,false,false,false,[&] {referenceEdge=true;++controlTicks;});
            require(edge==referenceEdge, "queued recall preserves every internal clock edge and fractional phase");
            if(transition) {
                require(s.tracks[0].stage==0 && s.tracks[0].pulse==0, "recall plays stage one on the boundary itself");
                require(std::abs(s.tracks[0].pitch-s.settings.root/12.f)<1e-5, "first boundary note uses recalled pitch settings");
                require(s.modulation.lanes[0].stage==0, "MOD lanes restart on the same boundary");
            }
        }
        require(ticks==controlTicks && transitions==16, "sixteen transitions neither add nor drop clock pulses");
    }
    // Exercise acquire/release transfer with a realistic snapshot payload.
    struct Payload { unsigned index=0; std::array<float,422> values{}; };
    metro::PresetMailbox<Payload,2> mailbox;
    std::thread producer([&] {
        for(unsigned i=1;i<=10000;++i) {
            Payload p;p.index=i;p.values.fill(float(i));
            while(!mailbox.push(p))std::this_thread::yield();
        }
    });
    for(unsigned i=1;i<=10000;++i) {
        Payload p;
        while(!mailbox.pop(p))std::this_thread::yield();
        require(p.index==i,"mailbox preserves command order");
        for(float v:p.values)require(v==float(i),"snapshot never tears across threads");
    }
    producer.join();
    std::cout << "Preset tests: " << checks << " assertions passed\n";
}
