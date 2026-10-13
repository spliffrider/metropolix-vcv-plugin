#pragma once
#include "PlayOrder.hpp"
#include <algorithm>
#include <cmath>
namespace metro {
struct WorkflowRange {float low=0,high=1,unit=1;bool discrete=true,toggle=false;};
inline float workflowRandom(float value,const WorkflowRange& range,RandomSource& random,bool morph){
    float span=range.high-range.low;if(span<=0)return range.low;
    auto clip=[&](float v){return std::max(range.low,std::min(v,range.high));};
    if(range.toggle){if(!morph || random.percent()<20)return morph?(value>(range.low+range.high)*.5f?range.low:range.high):(random.percent()<50?range.low:range.high);return value;}
    int options=int(std::round(span/range.unit))+1;
    if(!morph){double fraction=random.percent()*.01;return range.discrete?range.low+std::min(options-1,int(fraction*options))*range.unit:range.low+float(fraction)*span;}
    if(options<=10){if(random.percent()>=20)return value;return clip(value+(random.percent()<50?-range.unit:range.unit));}
    float maximum=std::max(range.unit,span*.1f),delta=range.unit+float(random.percent()*.01)*(maximum-range.unit);
    if(range.discrete)delta=std::max(range.unit,std::floor(delta/range.unit)*range.unit);
    return clip(value+(random.percent()<50?-delta:delta));
}
}
