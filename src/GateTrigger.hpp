#pragma once
#include <algorithm>
#include <cmath>

namespace metro {
// Rising CV gates produce a fixed 5 ms trigger (firmware 1.6.5). The threshold
// follows the plugin's established 1.5 V gate conversion; held gates do not
// retrigger, and a shorter input still produces the full pulse.
struct GateTrigger {
    bool high=false;
    double remaining=0;
    void clear(){high=false;remaining=0;}
    float process(float voltage,double dt){
        bool next=std::isfinite(voltage) && voltage>=1.5f;
        if(next && !high)remaining=.005;
        // Rack supplies sampleTime as float. Its rounding error must not turn
        // an exact240-sample pulse at48kHz into241 samples.
        double epsilon=std::isfinite(dt) && dt>0?dt*1e-5:1e-12;
        high=next;float output=remaining>epsilon?5.f:0.f;
        if(std::isfinite(dt))remaining=std::max(0.,remaining-std::max(0.,dt));
        return output;
    }
};
}
