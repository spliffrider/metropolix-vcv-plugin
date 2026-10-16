#include "../src/MxLogic.hpp"
#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
unsigned checks=0;
void check(bool v){++checks;if(!v){std::cerr<<"MX assertion "<<checks<<" failed\n";std::exit(1);}}
std::array<float,8> next(metro::MxLogic& x,int mode,bool tick,double dt=.002,bool enabled=true,bool reset=false){return x.process(dt,mode,tick,.125,enabled,reset,true,true,{},-1,false,false,-1,false,false);}
int main(){using namespace metro;
    for(int mode=0;mode<3;++mode){MxLogic x;const int divs[3][6]={{1,2,4,4,8,16},{1,3,4,5,8,16},{1,1,1,2,4,8}};
        for(int pulse=0;pulse<4096;++pulse){auto v=next(x,mode,true);check(v[0]==5 && v[1]==0);for(int out=2;out<8;++out){int offset=mode==0 && out==5?2:0;check((v[out]>0)==(pulse%divs[mode][out-2]==offset));}}
        auto v=next(x,mode,true,.002,true,true);check(v[1]==5 && x.index==1 && v[7]==5);next(x,mode,false);check(next(x,mode,false)[1]==0);
    }
    for(int rate:{44100,48000,96000})for(double period:{.00375,.0375,.125,.75}){MxLogic x;int frames=int(std::round(period*rate)),pulses4=0,pulses2=0,pulses1=0;std::array<float,8> previous{};
        for(int f=0;f<frames*32;++f){auto v=x.process(1./rate,MxTransportC,f%frames==0,double(frames)/rate,true,false,true,true,{},-1,false,false,-1,false,false);if(v[2]>0 && previous[2]==0)++pulses4;if(v[3]>0 && previous[3]==0)++pulses2;if(v[4]>0 && previous[4]==0)++pulses1;previous=v;}
        check(pulses4==128 && pulses2==64 && pulses1==32);int count=0;for(int f=0;f<frames*4;++f){auto v=x.process(1./rate,MxTransportC,false,period,true,false,true,true,{},-1,false,false,-1,false,false);if(v[2]>0 && previous[2]==0)++count;previous=v;}check(count==0);
    }
    {MxLogic x;x.seed(77);std::array<int,8> counts{};for(int pulse=0;pulse<100000;++pulse){auto v=next(x,MxProbClocks,true);for(int out=0;out<8;++out)if(v[out]>0)++counts[out];}
        for(int out=0;out<8;++out){double expected=100000.*(out<4?1:.5)*(1-(out%4)*.25);check(std::abs(counts[out]-expected)<700);}}
    {MxLogic x;next(x,MxTransportC,true);auto v=next(x,MxTransportC,false,.04,false);for(int out=2;out<8;++out)check(v[out]==0);v=x.process(.002,MxTransportA,true,.125,true,false,false,true,{},-1,false,false,-1,false,false);check(v[0]==0 && v[2]==5);}
    for(int mode=5;mode<7;++mode)for(int stage=-1;stage<9;++stage)for(bool run:{false,true})for(bool mute:{false,true}){MxLogic x;auto v=x.process(.001,mode,false,.125,false,false,false,true,{},stage,run,mute,stage,run,mute);for(int out=0;out<8;++out)check((v[out]>0)==(run && !mute && stage==out));}
    {MxLogic x;std::array<float,8> clocks{{5,0,5,0,0,5,0,5}};check(x.process(.001,MxModClocks,false,.125,true,false,true,true,clocks,-1,false,false,-1,false,false)==clocks);}
    // Stage taps hold through multi-pulse rests and zero gate probability.
    {Sequencer seq;seq.settings.tracks[0].length=2;seq.settings.stages[0].pulses=3;seq.settings.stages[0].gate=Rest;seq.settings.tracks[0].stages[0].probability=0;
        MxLogic x;for(int pulse=0;pulse<3;++pulse){seq.process(.01,true,true,true,false);auto v=x.process(.01,MxStageNum,seq.masterTick,seq.masterPeriod,true,false,true,true,{},seq.sounding(0).stage,true,seq.trackMuted(0),-1,false,false);check(v[0]==5 && seq.sounding(0).gate==0);}}
    #include "MxModGateTests.hpp"
    std::cout<<"PASS: MX nine modes, dividers, offsets, multiplied clocks, probabilities, stage gates and MOD triggers ("<<checks<<" assertions)\n";
}
