#pragma once
#include <array>
#include <cmath>
#include <algorithm>
namespace metro {
// Audio-thread state. Physical controls stay where they were on a Thru load;
// the memorized value sounds until movement reaches/crosses that position.
struct Pickup {
    bool active=false;
    float target=0,last=0;
    void recall(float physical,float saved){last=physical;target=saved;active=std::abs(physical-saved)>1e-6f;}
    float process(float physical,bool thru,bool keep){
        if(!keep)active=false;
        if(active && physical!=last){
            if(!thru || std::abs(physical-target)<=1e-6f || (physical>=target && last<=target) || (physical<=target && last>=target))active=false;
        }
        last=physical;return active?target:physical;
    }
    bool operator==(const Pickup& other)const{return active==other.active && target==other.target && last==other.last;}
    bool operator!=(const Pickup& other)const{return !(*this==other);}
};
using PanelPickup=std::array<Pickup,10>;
}
