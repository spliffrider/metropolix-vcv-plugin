#pragma once
#include "Modulation.hpp"
#include <cstdio>
#include <string>
namespace metro {
enum DisplayRounding {ContinuousDisplay,RoundDisplay,TruncateDisplay};
struct ModDisplay {float multiplier;const char* unit;int decimals;DisplayRounding rounding;};
inline ModDisplay modDisplay(ModDestination destination){
    switch(destination){
        case ModOctave:return {1," oct",0,TruncateDisplay};
        case ModPitchPre:case ModPitchPost:return {12," st",0,RoundDisplay};
        case ModPitchOffset:return {12," st",2,ContinuousDisplay};
        case ModGateLength:case ModGateScale:case ModProbability:case ModSlideAmount:return {20,"%",1,ContinuousDisplay};
        case ModSwing:return {5.6f,"%",1,ContinuousDisplay};
        case ModTempo:return {80," BPM",1,ContinuousDisplay};
        case ModClockDiv:return {12.6f," div",0,RoundDisplay};
        case ModRatchet:return {1.4f," ratch",0,RoundDisplay};
        case ModPulseCount:return {1.4f," pulses",0,RoundDisplay};
        case ModPulseDiv:return {1.4f," div",0,RoundDisplay};
        case ModPulsesLength:case ModLoopyPulses:return {12.8f," pulses",0,RoundDisplay};
        case ModLoopyOffset:case ModStagesLength:case ModStagesOffset:return {1.4f," stages",0,RoundDisplay};
        case ModOrder:return {4," orders",0,RoundDisplay};
        case ModRoot:return {12," st",0,RoundDisplay};
        case ModUserScale:return {1," scales",0,TruncateDisplay};
        case ModSliderRange:return {.6f," oct",0,RoundDisplay};
        default:return {1," V",2,ContinuousDisplay};
    }
}
inline float modDisplayValue(float volts,ModDestination destination){auto spec=modDisplay(destination);float value=volts*spec.multiplier;if(destination==ModRoot)value=modClamp(value,-12.f,12.f);return spec.rounding==RoundDisplay?std::round(value):spec.rounding==TruncateDisplay?std::trunc(value):value;}
inline std::string modDisplayText(float volts,ModDestination destination,bool unit=true){auto spec=modDisplay(destination);char buffer[64];float value=modDisplayValue(volts,destination);if(std::abs(value)<.000001f)value=0;std::snprintf(buffer,sizeof(buffer),"%+.*f%s",spec.decimals,double(value),unit?spec.unit:"");return buffer;}
}
