#include "../src/Sequencer.hpp"
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

int checks=0;
void require(bool ok,const char* message) {++checks;if(!ok){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
void near(double a,double b,double tolerance,const char* message) {require(std::abs(a-b)<=tolerance,message);}

int main() {
    using namespace metro;
    near(swingPercent(std::numeric_limits<float>::quiet_NaN()),50,0,"invalid swing is straight");
    for(int rate:{44100,48000,96000})for(bool external:{false,true})for(int division:{1,3,7})for(int percent=50;percent<=78;++percent) {
        Sequencer s;s.settings.bpm=300;s.settings.tracks[0].division=division;s.settings.tracks[0].swing=float(percent);
        const double dt=1.0/rate,period=.05*division,amount=percent*.01;
        const int clockSamples=rate/20;
        std::vector<double> events;
        int clocks=0,gateRises=0;float oldClock=0,oldGate=0;
        for(int sample=0;sample<int(rate*(8*period+.01));++sample) {
            s.process(dt,true,external,external && sample%clockSamples==0,false);
            if(s.clock>0 && oldClock==0) {
                near(sample*dt,clocks*.05,2*dt,"global clock pulses stay at their original times");
                require(s.modulation.lanes[0].stage==clocks%8,"MOD lane follows straight global clock");
                ++clocks;
            }
            oldClock=s.clock;
            if(s.tracks[0].gate>0 && oldGate==0) {
                near(sample*dt,(gateRises/2)*2*period+(gateRises%2?2*amount*period:0),2*dt,"audible gate edges follow swung note timing");
                ++gateRises;
            }
            oldGate=s.tracks[0].gate;
            if(s.tracks[0].age<=dt*1.000001) {
                events.push_back(sample*dt);
                int pulse=int(events.size())-1;
                double expected=(pulse/2)*2*period+(pulse%2?2*amount*period:0);
                near(events.back(),expected,2*dt,"track edges follow swing ratio without cumulative drift");
                near(s.tracks[0].duration,period*2*(pulse%2?1-amount:amount),.000001,"gate and slide pulse duration follows long/short intervals");
            }
        }
        require(events.size()>=8,"all expected swung pulses are delivered");
        require(gateRises==int(events.size()),"each swung note has one sounding gate");
        for(unsigned i=2;i<events.size();i+=2)near(events[i]-events[i-2],2*period,2*dt,"each pair retains its original tempo");
        require(clocks>=8*division,"global clock stays on the straight grid");
    }
    // Skips and odd-length automatic track wraps must not restart swing phase.
    {
        Sequencer s;s.settings.bpm=120;s.settings.tracks[0].swing=66;s.settings.tracks[0].pulseLimit=3;
        s.settings.tracks[0].stages[1].skip=true;
        const int stages[]={0,2,3,0,2,3,0,2};int count=0;
        for(int i=0;i<48000;++i) {
            s.process(1.0/48000,true,false,false,false);
            if(s.tracks[0].age<=1.0/48000*1.000001) {
                require(s.tracks[0].stage==stages[count],"skips and three-pulse wrap retain musical traversal");
                near(i/48000.0,(count/2)*.25+(count%2?.165:0),2.0/48000,"odd pattern length preserves swing groove");
                ++count;
            }
        }
        require(count==8,"skip test emitted exactly eight track pulses");
    }
    // Ratchets subdivide each swung interval and still drive the accumulator.
    for(bool stretch:{false,true})for(RatchetMode mode:{MultiplyRatchets,PulseRatchets,GatedRatchets}) {
        Sequencer s;s.settings.bpm=120;s.settings.tracks[0].swing=66;s.settings.tracks[0].gateStretch=stretch;s.settings.tracks[0].ratchetMode=mode;
        for(auto& edit:s.settings.tracks[0].stages){edit.ratchets=4;edit.accumTrigger=AccumRatchet;edit.accumulation=1;}
        float gate=0;int rises=0;
        for(int i=0;i<48000;++i){s.process(1.0/48000,true,false,false,false);if(s.tracks[0].gate>0 && gate==0)++rises;gate=s.tracks[0].gate;}
        int perPulse=stretch && mode==GatedRatchets?2:4;
        require(rises==8*perPulse,"ratchets subdivide swung pulse or gated window");
        for(int a:s.tracks[0].accumulator)require(a==perPulse,"ratchet accumulation advances only on sounding events");
    }
    {
        Sequencer s;s.settings.tracks[0].swing=78;
        auto step=[&](int n){for(int i=0;i<n;++i)s.process(1.0/48000,true,true,false,false);};
        s.process(1.0/48000,true,true,true,false);step(5999);
        s.process(1.0/48000,true,true,true,false); // second input pulse pending
        require(s.tracks[0].stage==0,"external second pulse is delayed");
        step(3359);require(s.tracks[0].stage==0,"external delayed pulse waits for its due sample");
        step(1);require(s.tracks[0].stage==1,"delayed external pulse fires on its measured interval");
        step(48000);require(s.tracks[0].stage==1 && s.tracks[0].gate==0,"stalled input creates no future notes or ratchets");
    }
    {
        Sequencer s;s.settings.bpm=120;s.settings.tracks[0].swing=78;
        for(int i=0;i<6001;++i)s.process(1.0/48000,true,false,false,false);
        s.process(1.0/48000,true,false,false,true);
        require(s.tracks[0].stage==0,"reset discards old pending pulse and starts downbeat");
        for(int i=0;i<3400;++i)s.process(1.0/48000,true,false,false,false);
        require(s.tracks[0].stage==0,"discarded pulse cannot leak after reset");
        s.process(1.0/48000,false,false,false,false);require(s.tracks[0].gate==0,"stop silences swung gates");
        s.process(1.0/48000,true,false,false,false);require(s.tracks[0].stage==0 && s.tracks[0].age<.001,"normal Run restarts on the unswung downbeat");
    }
    // AUX/MOD offsets sum before saturation and are sampled once per pair.
    {
        Sequencer s;s.settings.tracks[0].swing=64;
        auto& input=s.settings.auxInputs[0];input.connected=true;input.amount=1;input.destination=ModSwing;input.voltage=2.5f;
        s.process(.001,true,true,true,false);near(s.tracks[0].duration,.195,.000001,"positive AUX reaches maximum swing");
        input.voltage=-5;s.process(.001,true,true,true,false); // edit cannot retract pending pulse
        for(int i=0;i<10;++i)s.process(.001,true,true,false,false);
        require(s.tracks[0].stage==1,"scheduled swung edge survives live CV change");
        s.process(.001,true,true,true,false);near(s.tracks[0].duration,.011,.000001,"negative AUX gives straight next pair");
        Sequencer q;q.settings=s.settings;
        q.settings.modLanes[0].destination=ModSwing;q.settings.modLanes[0].values.fill(5);
        q.process(.001,true,true,true,false);near(q.tracks[0].duration,.16,.000001,"opposing AUX and MOD restore programmed swing");
        near(s.settings.tracks[0].swing,64,0,"CV never changes saved base swing");
    }
    {
        Sequencer s;s.settings.scale=0;s.settings.octaves=1;s.settings.tracks[0].swing=66;
        s.settings.tracks[0].stages[1].slide=true;s.settings.tracks[0].slideTime=1;
        s.settings.stages[1].pitch=1;
        for(int i=0;i<=9960;++i) {
            s.process(1.0/48000,true,false,false,false);
            if(i==6000)near(s.tracks[0].pitch,0,.000001,"slide does not start on undelayed clock edge");
        }
        near(s.tracks[0].pitch,.5,.001,"Tempo slide reaches midpoint in the short swung interval");
    }
    // Pause holds a pending pulse until resumed; abrupt clock acceleration
    // cannot leave an older event behind the next downbeat.
    {
        Sequencer s;s.settings.tracks[0].swing=78;s.resumeOnRun=true;
        s.process(.001,true,true,true,false);
        for(int i=0;i<124;++i)s.process(.001,true,true,false,false);
        s.process(.001,true,true,true,false);
        for(int i=0;i<100;++i)s.process(.001,false,true,false,false);
        require(s.tracks[0].stage==0,"pause preserves pending pulse");
        for(int i=0;i<70;++i)s.process(.001,true,true,false,false);
        require(s.tracks[0].stage==1,"resume finishes remaining swing delay");
        s.reset();s.process(.001,true,true,true,false);s.process(.001,true,true,true,false);s.process(.001,true,true,true,false);
        require(s.tracks[0].stage==2,"accelerated input drains older pulse before current downbeat");
    }
    std::cout<<"PASS: "<<checks<<" swing timing checks\n";
}
