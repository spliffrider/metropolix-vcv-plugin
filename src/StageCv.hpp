#pragma once
#include <algorithm>
#include <cmath>
namespace metro {
enum TrackCvType { BipolarCv, UnipolarCv, GateCv };
enum StageCvRamp { CvStep, CvStageRamp, CvPulseRamp };
inline float trackCvVoltage(float value,TrackCvType type) {
    if(!std::isfinite(value))return 0.f;
    return type==GateCv?(value>=1.5f?5.f:0.f):std::max(type==UnipolarCv?0.f:-5.f,std::min(5.f,value));
}
inline float stageCvValue(float from,float target,TrackCvType type,StageCvRamp ramp,
                          int pulse,double age,double duration,int pulses) {
    target=trackCvVoltage(target,type);
    if(type==GateCv || ramp==CvStep)return target;
    from=trackCvVoltage(from,type);
    // Each physical track pulse advances the stage; a stalled clock cannot
    // create an extra pulse's worth of sRamp progress.
    double position=pulse+std::max(0.0,std::min(1.0,age/std::max(1e-7,duration)));
    double span=ramp==CvStageRamp?std::max(1,pulses):1;
    double mix=std::max(0.0,std::min(1.0,position/span));
    return float(from+(double(target)-from)*mix);
}
} // namespace metro
