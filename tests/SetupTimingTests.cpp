#include "../src/SetupTiming.hpp"
#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
using namespace metro;
int checks=0;
void require(bool ok,const char* message){++checks;if(!ok){std::cerr<<"FAIL "<<message<<'\n';std::exit(1);}}
void tick(Sequencer& s){s.process(.002,true,true,false,false);s.process(.002,true,true,true,false);}
int main(){
    for(int d=1;d<=256;++d){ExternalDivider divider;
        for(int p=0;p<d*5;++p){require(!divider.process(false,d),"low samples never clock");require(divider.process(true,d)==(p%d==0),"first edge and every D edges advance");}
        divider.reset();require(divider.process(true,d),"reset restarts external phase");
        require(divider.process(true,d==256?1:d+1),"changed division starts a new phase");
    }
    for(int rate:{44100,48000,96000,192000})for(int ms=0;ms<=21;++ms){OutputDelay<7> delay;delay.prepare(rate);auto capacity=delay.frames.capacity();int latency=int(std::round(rate*.001*ms));
        for(int f=0;f<latency+8;++f){std::array<float,7> in{};if(f==0)for(int c=0;c<7;++c)in[c]=float(c+1);auto out=delay.process(in,float(ms));for(int c=0;c<7;++c)require(out[c]==(f==latency?float(c+1):0),"all seven CV/gates delay by the same exact sample count");}
        require(capacity==delay.frames.capacity(),"processing never reallocates delay buffer");
        delay.prepare(rate);for(int f=0;f<latency+1;++f){std::array<float,7> in{};in[0]=float(f+1);delay.process(in,0);}
        std::array<float,7> in{};in[0]=999;require(delay.process(in,float(ms))[0]==(latency?2.f:999.f),"live delay reads existing history without a new allocation");
        delay.prepare(rate*2);require(delay.process({},21)[0]==0,"sample-rate change flushes old history");
    }
    for(int order=0;order<21;++order)for(int mode=0;mode<2;++mode)for(int skip=0;skip<8;++skip)for(unsigned mask:{1u,2u,4u}){
        Sequencer s;s.seed(47);for(int t=0;t<2;++t){auto& track=s.settings.tracks[t];track.order=Order(order);track.division=t+1;track.globalReset=false;track.accumMode=AccumMode(mode);for(int st=0;st<8;++st){s.settings.stages[st].pitch=st/7.f;s.settings.stages[st].pulses=st%3+1;track.stages[st].accumulation=st%3-1;track.stages[st].skip=st==skip;}}
        for(int p=0;p<23;++p)tick(s);
        Sequencer reference=s;s.manualResetForce=mask;s.process(.0001,true,true,false,true);reference.process(.0001,true,true,false,false);
        require(s.clock==reference.clock && s.masterClock==reference.masterClock,"targeted reset retains current master clock waveform");
        for(int lane=0;lane<8;++lane)require(s.modulation.lanes[lane].stage==(mask==4?-1:reference.modulation.lanes[lane].stage),"held MOD resets every lane, held TRK leaves MOD traversal intact");
        for(int t=0;t<2;++t)if(mask&(1u<<t))require(s.tracks[t].stage==-1 && s.tracks[t].divider==0,"held reset affects protected selected track");
        for(int p=0;p<30;++p){tick(s);tick(reference);require(s.clock==reference.clock,"targeted reset retains divided clock timing");for(int t=0;t<2;++t)if(!(mask&(1u<<t)))require(s.tracks[t].stage==reference.tracks[t].stage && s.tracks[t].pulse==reference.tracks[t].pulse && s.tracks[t].pitch==reference.tracks[t].pitch && s.tracks[t].gate==reference.tracks[t].gate,"other track retains skip, pulse, divider and accumulator behavior");}
        s.manualResetForce=mask;s.process(.002,true,true,false,false);s.process(.002,true,true,true,true);require(s.masterClock==5,"targeted reset does not suppress a simultaneous clock edge");
    }
    std::cout<<"PASS: SetupTimingTests "<<checks<<" checks\n";
}
