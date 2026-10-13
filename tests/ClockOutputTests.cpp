#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
int checks=0;
void require(bool value,const char* message){++checks;if(!value){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
int main(){
    using namespace metro;
    for(int div=1;div<=256;++div)for(int offset=0;offset<div;++offset){
        ClockOutput clock;
        int count=0;
        for(int pulse=0;pulse<3*div;++pulse){
            clock.tick(div,offset);bool high=clock.render(1.0/48000,true)>0;
            require(high==(pulse%div==offset),"every legal divider/offset follows master pulse positions");count+=high;
            // One full pulse does not persist until the next master clock.
            clock.render(.01,true);require(clock.render(.01,true)==0,"one millisecond triggers decay");
        }
        require(count==3,"exactly one output pulse in each divided cycle");
    }
    {
        ClockOutput c;c.tick(-9,255);require(c.render(.001,true)==5,"invalid settings clamp to /1 offset0");
        c.reset();for(int p=0;p<256;++p){c.tick(900,999);require((c.render(.01,true)>0)==(p==255),"upper limits clamp to /256 offset255");}
        c.reset();c.tick(4,0);require(c.render(0,false)==0,"stop immediately closes an existing trigger");
        c.tick(4,0);require(c.render(.01,true)==0,"stop preserves counter for pause/resume");c.reset();c.tick(4,0);require(c.render(.01,true)==5,"reset restarts phase at master pulse0");
        c.reset();for(int p=0;p<10;++p){c.tick(p<5?4:3,1);require((c.render(.01,true)>0)==(p%(p<5?4:3)==1),"live division changes preserve master pulse count");}
    }
    // Sample-rate trigger length, actual sequencer transport, and independent clocks.
    for(int rate:{44100,48000,96000})for(float bpm:{60.f,240.f}){
        Sequencer master,divided;master.settings.bpm=divided.settings.bpm=bpm;
        divided.settings.clockOutDivision=4;divided.settings.clockOutOffset=2;
        divided.settings.tracks[0].swing=78;divided.settings.tracks[1].division=3;
        divided.settings.modLanes[0].division=7;
        int pulses=0,divPulses=0,triggerFrames=0;bool previous=false,previousDiv=false;
        for(int frame=0;frame<rate*2;++frame){
            double dt=1.0/rate;master.process(dt,true,false,false,false);divided.process(dt,true,false,false,false);
            bool edge=master.clock>0 && !previous;
            if(edge){require((divided.clock>0)==(pulses%4==2),"internal clock output follows unswung master");++pulses;}
            if(divided.clock>0 && !previousDiv)++divPulses;
            if(frame<rate/100 && master.clock>0)++triggerFrames;
            previous=master.clock>0;previousDiv=divided.clock>0;
        }
        require(divPulses==(pulses+1)/4,"expected number of divided internal triggers");
        require(triggerFrames>=int(.001*rate)-1 && triggerFrames<=int(.001*rate)+1,"one millisecond trigger quantized only by sample rate");
    }
    {
        Sequencer s;s.settings.clockOutDivision=4;s.settings.clockOutOffset=2;
        s.settings.tracks[0].mute=true;s.settings.tracks[1].division=64;s.settings.tracks[1].swing=78;
        for(int p=0;p<12;++p){s.process(.01,true,true,true,false);require((s.clock>0)==(p%4==2),"external output ignores track mute/division/swing");s.process(.01,true,true,false,false);require(s.clock==0,"stalled external input produces no extra output clocks");}
        s.process(.01,true,true,false,true);require(s.clock==0,"reset emits no synthetic clock trigger");
        s.process(.01,true,true,true,false);require(s.clock==0,"reset phase starts at zero with offset2");
        s.process(.01,true,true,true,false);s.process(.01,true,true,true,false);require(s.clock==5,"offset counts two master clocks after reset");
        s.process(.01,false,true,false,false);s.process(.01,true,true,true,false);require(s.clock==0,"normal RUN restarts at zero");
        s.resumeOnRun=true;s.process(.01,false,true,false,false);s.process(.01,true,true,true,false);require(s.clock==0,"pause resumes next master pulse1");s.process(.01,true,true,true,false);require(s.clock==5,"pause keeps divider phase");
        s.process(.01,true,true,true,false,[&]{s.restartAtClock();});require(s.clock==0,"preset transport restart resets output phase on same edge");
    }
    std::cout<<"PASS: ClockOutputTests "<<checks<<" checks\n";
}
