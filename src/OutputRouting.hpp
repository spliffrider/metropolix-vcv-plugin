#pragma once
#include <algorithm>
#include <array>
#include <string>
#include "GateTrigger.hpp"

namespace metro {
enum OutputKind { OutCtrl1Bipolar, OutCtrl1Unipolar, OutCtrl2Bipolar, OutCtrl2Unipolar,
    OutTrackCv, OutFirstPulse, OutLastPulse, OutModClock, OutModCv, OutNewStage,
    OutReset, OutRoot, OutRun, OutSlider, OutSliderRange, OutVelocity, OutModTrigs, OutputKindCount };
constexpr int OutputSourceCount=40;
inline const char* outputKindName(int kind) {
    const char* names[]={"Ctrl 1 ±","Ctrl 1 +","Ctrl 2 ±","Ctrl 2 +","CV","First Pulse","Last Pulse",
        "MOD Clock","Mod","New Stage","Reset","Root","Run","Slider (0-5V)","Slider (Range)","Velocity","Mod Trigs"};
    return names[std::max(0,std::min(kind,int(OutputKindCount)-1))];
}
inline bool outputIsMod(int kind) {return kind==OutModClock || kind==OutModCv;}
inline bool outputIsTrack(int kind) {return kind==OutTrackCv || kind==OutFirstPulse || kind==OutLastPulse || kind==OutNewStage || kind==OutRoot || kind==OutSlider || kind==OutSliderRange || kind==OutVelocity || kind==OutModTrigs;}
inline int outputTargetCount(int kind) {return outputIsMod(kind)?8:outputIsTrack(kind)?2:0;}
// Legacy sources 0/1=track CV and 2..9=MOD voltage remain unchanged in patches.
inline int outputSource(int kind,int target=0) {
    const int bases[]={10,11,12,13,0,14,16,18,2,26,28,29,31,32,34,36,38};
    kind=std::max(0,std::min(kind,int(OutputKindCount)-1));
    return bases[kind]+std::max(0,std::min(target,std::max(0,outputTargetCount(kind)-1)));
}
struct OutputSelection {int kind=OutTrackCv,target=0;};
inline OutputSelection outputSelection(int source) {
    source=std::max(0,std::min(source,OutputSourceCount-1));
    OutputSelection result;
    for(int kind=0;kind<OutputKindCount;++kind){
        int base=outputSource(kind),count=std::max(1,outputTargetCount(kind));
        if(source>=base && source<base+count){result.kind=kind;result.target=source-base;break;}
    }
    return result;
}
inline std::string outputLabel(int source) {
    source=std::max(0,std::min(source,OutputSourceCount-1));
    if(source<2)return "Track "+std::to_string(source+1)+" CV";
    if(source<10)return "MOD "+std::to_string(source-1);
    auto s=outputSelection(source);std::string label=outputKindName(s.kind);
    if(outputIsTrack(s.kind))label+=" — Track "+std::to_string(s.target+1);
    else if(outputIsMod(s.kind))label+=" — MOD "+std::to_string(s.target+1);
    return label;
}
struct OutputPulse {
    double remaining=0;
    void fire(){remaining=.001;}
    void clear(){remaining=0;}
    float render(double dt){float v=remaining>0?5.f:0.f;remaining=std::max(0.0,remaining-dt);return v;}
};
struct TrackOutputEvents {
    enum Event {First,Last,NewStage};
    std::array<OutputPulse,3> pulses;
    std::array<float,3> values{};
    bool firstDue=true;
    void clear(){for(auto& p:pulses)p.clear();values.fill(0);}
    void render(double dt){for(int i=0;i<3;++i)values[i]=pulses[i].render(dt);}
};
}
