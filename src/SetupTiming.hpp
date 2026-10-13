#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
namespace metro {
// The first incoming edge starts the divided clock; subsequent accepted edges
// are exactly division input pulses apart. Changing division restarts this phase.
struct ExternalDivider {
    int divisor=1,index=0;
    void reset(){index=0;}
    bool process(bool edge,int requested){requested=std::max(1,std::min(256,requested));if(requested!=divisor){divisor=requested;reset();}if(!edge)return false;bool accepted=index==0;index=(index+1)%divisor;return accepted;}
};
template<size_t Channels> struct OutputDelay {
    std::vector<std::array<float,Channels>> frames;
    size_t write=0;
    double sampleRate=48000;
    OutputDelay(){prepare(sampleRate);}
    void prepare(double rate){sampleRate=rate;frames.assign(size_t(std::ceil(rate*.021))+2,{});write=0;}
    std::array<float,Channels> process(const std::array<float,Channels>& input,float milliseconds){
        size_t offset=std::min(frames.size()-1,size_t(std::round(std::max(0.f,std::min(21.f,milliseconds))*.001*sampleRate)));
        frames[write]=input;auto result=frames[(write+frames.size()-offset)%frames.size()];write=(write+1)%frames.size();return result;
    }
};
}
