#pragma once
#include <algorithm>
#include <cmath>
namespace metro {
enum SlideType { TempoSlide, AnalogSlide, AcidSlide };
inline int nextSlideType(int type) {const int next[]={AcidSlide,TempoSlide,AnalogSlide};return next[std::max(0,std::min(2,type))];}
inline float slideAmount(float amount) {return std::isfinite(amount)?std::max(0.f,std::min(1.f,amount)):0.f;}
inline double analogSlideTime(float amount) {
    amount=slideAmount(amount);
    // p80 says close notes finish sooner. Use constant volts/second slew.
    // The manual gives no calibration: 1ms..1s per octave is our chosen range.
    return amount>0.f?.001*std::pow(1000.0,double(amount)):0.0;
}
inline float slewPitch(float pitch,float target,double dt,double secondsPerVolt) {
    if(secondsPerVolt<=0)return target;
    double delta=double(target)-pitch,step=std::max(0.0,dt)/secondsPerVolt;
    return float(pitch+std::max(-step,std::min(step,delta)));
}
inline int slideShortcut(int stage) {const int values[]={0,14,28,42,57,71,85,100};return values[std::max(0,std::min(7,stage))];}
} // namespace metro
