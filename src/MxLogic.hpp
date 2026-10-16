#pragma once
#include <array>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include "GateTrigger.hpp"

namespace metro {
enum MxMode {MxTransportA, MxTransportB, MxTransportC, MxProbClocks,
    MxModClocks, MxStageNum, MxModStageNum, MxModGate, MxModTrig, MxModeCount};
// Unswinged master-clock taps (manual pp.122–125). Multipliers predict only
// the three intervening pulses; a missing next master pulse stops them.
struct MxLogic {
    std::uint64_t index=0;
    std::uint32_t random=1;
    std::array<double,8> remaining{};
    double elapsed=0,period=.125,resetRemaining=0;
    int quarter=4,mode=-1;
    std::array<GateTrigger,8> triggers;
    void seed(std::uint32_t value){random=value?value:1;}
    void reset(){index=0;remaining.fill(0);elapsed=0;quarter=4;resetRemaining=.001;for(auto& trigger:triggers)trigger.clear();}
    float chance(){random^=random<<13;random^=random>>17;random^=random<<5;return float(random>>8)/16777216.f;}
    void fire(int output,int multiplier=1){remaining[output]=std::min(.001,period/(2*multiplier));}
    std::array<float,8> process(double dt,int requested,bool tick,double masterPeriod,
        bool clocksEnabled,bool resetEvent,bool run,bool resetEnabled,
        const std::array<float,8>& modClocks,int trackStage,bool trackPlaying,bool trackMute,
        int modStage,bool modPlaying,bool modMute,const std::array<float,8>& modValues={}){
        requested=std::max(0,std::min(int(MxModeCount)-1,requested));
        if(requested!=mode){mode=requested;remaining.fill(0);quarter=4;for(auto& trigger:triggers)trigger.clear();}
        if(resetEvent)reset();
        elapsed+=dt;
        if(!clocksEnabled){remaining.fill(0);quarter=4;}
        if(tick && clocksEnabled){
            period=std::max(.001,std::min(150.,masterPeriod));elapsed=0;quarter=1;
            if(mode<=MxTransportC){
                const int divs[3][6]={{1,2,4,4,8,16},{1,3,4,5,8,16},{1,1,1,2,4,8}};
                for(int out=2;out<8;++out){int div=divs[mode][out-2];int offset=mode==MxTransportA && out==5?2:0;
                    if(index%unsigned(div)==unsigned(offset))fire(out,mode==MxTransportC && out==2?4:mode==MxTransportC && out==3?2:1);}
            }else if(mode==MxProbClocks){
                for(int out=0;out<8;++out)if((out<4 || index%2==0) && (out%4==0 || chance()<1.f-float(out%4)*.25f))fire(out);
            }
            ++index;
        }else if(clocksEnabled && mode==MxTransportC){
            while(quarter<4 && elapsed+1e-12>=period*quarter*.25){fire(2,4);if(quarter==2)fire(3,2);++quarter;}
        }
        std::array<float,8> output{};
        if(mode<=MxTransportC){output[0]=run?5.f:0.f;output[1]=resetEnabled && resetRemaining>0?5.f:0.f;
            for(int out=2;out<8;++out)output[out]=remaining[out]>0?5.f:0.f;
        }else if(mode==MxProbClocks){for(int out=0;out<8;++out)output[out]=remaining[out]>0?5.f:0.f;}
        else if(mode==MxModClocks){output=modClocks;}
        else if(mode==MxModGate){for(int lane=0;lane<8;++lane)output[lane]=run && std::isfinite(modValues[lane]) && modValues[lane]>=1.5f?5.f:0.f;}
        else if(mode==MxModTrig){for(int lane=0;lane<8;++lane){if(run)output[lane]=triggers[lane].process(modValues[lane],dt);else triggers[lane].clear();}}
        else {int stage=mode==MxStageNum?trackStage:modStage;bool playing=mode==MxStageNum?trackPlaying:modPlaying;bool mute=mode==MxStageNum?trackMute:modMute;
            if(playing && !mute && stage>=0 && stage<8)output[stage]=5.f;}
        for(auto& pulse:remaining)pulse=std::max(0.,pulse-dt);
        resetRemaining=std::max(0.,resetRemaining-dt);
        return output;
    }
};
}
