#pragma once
#include <array>
#include <algorithm>
#include <cmath>

namespace metro {
// Stable IDs for the destinations supported by this engine. Unimplemented
// hardware destinations are listed separately in the UI, never silent no-ops.
enum CtrlDestination { CtrlNone, CtrlAccumMode, CtrlAccumPolar, CtrlTempo,
    CtrlClockDiv, CtrlDirection, CtrlGateLength, CtrlOctave, CtrlPitchOffset,
    CtrlPitchPost, CtrlPitchPre, CtrlOrder, CtrlProbability, CtrlPulseCount,
    CtrlPulseDiv, CtrlPulsesLength, CtrlRatchet, CtrlRoot, CtrlSkipInvert,
    CtrlSlideAmount, CtrlSliderInvert, CtrlStagesLength, CtrlStagesOffset,
    CtrlOutputSwap, CtrlSwing, CtrlUserScale, CtrlVelocity, CtrlSliderRange, CtrlGateScale, CtrlAccumDir, CtrlDestinationCount };

inline float ctrlClamp(float v) { return std::isfinite(v)?std::max(-1.f,std::min(1.f,v)):0.f; }
struct Control {
    float value=0.f; // -1 = CCW, 0 = noon, +1 = CW
    CtrlDestination destination=CtrlNone;
    int assignment=2; // Track 1 / Track 2 / both
    bool targets(int t) const { return assignment==2 || assignment==t; }
};
inline bool ctrlDirect(CtrlDestination d) {
    switch(d) {
        case CtrlAccumMode: case CtrlAccumPolar: case CtrlTempo: case CtrlClockDiv:
        case CtrlGateLength: case CtrlOrder: case CtrlPulseDiv: case CtrlPulsesLength:
        case CtrlSlideAmount: case CtrlStagesLength: case CtrlStagesOffset: case CtrlSwing: case CtrlSliderRange: case CtrlGateScale: return true;
        default: return false;
    }
}
inline float ctrlDirectValue(CtrlDestination d,float value) {
    float x=ctrlClamp(value),u=(x+1.f)*.5f;
    switch(d) {
        case CtrlAccumMode: return x>0?1:0;
        case CtrlAccumPolar: return x==0?1:0;
        case CtrlTempo: return .1f+399.9f*u;
        case CtrlClockDiv: return std::round(1+63*u);
        case CtrlGateLength: return 1+98*u;
        case CtrlOrder: return std::round(20*u);
        case CtrlPulseDiv: return std::round(1+7*u);
        case CtrlPulsesLength: return std::round(64*u);
        case CtrlSlideAmount: return 100*u;
        case CtrlStagesLength: return std::round(1+7*u);
        case CtrlStagesOffset: return std::round(7*u);
        case CtrlSwing: return std::round(50+28*u);
        case CtrlSliderRange: return std::round(1+3*u);
        case CtrlGateScale: return 100*u;
        default: return 0;
    }
}
struct ControlRoutes {
    std::array<std::array<float,CtrlDestinationCount>,2> values{};
    bool swap=false;
    void read(const std::array<Control,2>& controls) {
        for(auto& track:values)track.fill(0);
        swap=false;
        for(const auto& c:controls) {
            if(c.destination<=CtrlNone || c.destination>=CtrlDestinationCount)continue;
            float x=ctrlClamp(c.value);
            if(c.destination==CtrlOutputSwap){swap=x>0;continue;}
            for(int t=0;t<2;++t)if(c.targets(t)) {
                auto& v=values[t][c.destination];
                if(c.destination==CtrlDirection || c.destination==CtrlSkipInvert || c.destination==CtrlSliderInvert || c.destination==CtrlAccumPolar || c.destination==CtrlAccumDir)v=x;
                else v=ctrlClamp(v+x);
            }
        }
    }
};
} // namespace metro
