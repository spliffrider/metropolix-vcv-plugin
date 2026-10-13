#pragma once
#include <algorithm>
#include <cstdint>

namespace metro {
// Global clock output follows unswung master pulses, independent of track clocks.
struct ClockOutput {
    std::uint64_t pulseIndex = 0;
    double remaining = 0.0;
    static int division(int value) { return std::max(1,std::min(value,256)); }
    static int offset(int value,int div) { return std::max(0,std::min(value,division(div)-1)); }
    void reset() { pulseIndex=0;remaining=0.0; }
    void tick(int div,int shift) {
        div=division(div);shift=offset(shift,div);
        if(pulseIndex % unsigned(div)==unsigned(shift))remaining=0.001;
        ++pulseIndex;
    }
    float render(double dt,bool run) {
        if(!run)remaining=0.0;
        float voltage=run && remaining>0.0?5.f:0.f;
        remaining=std::max(0.0,remaining-dt);
        return voltage;
    }
};
}
