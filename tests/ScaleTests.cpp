#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>
using namespace metro;
unsigned checks=0;
void check(bool v,const char* why){++checks;if(!v){std::cerr<<"FAIL: "<<why<<'\n';std::exit(1);}}
void near(float a,float b,const char* why){check(std::abs(a-b)<1e-5f,why);}
void tick(Sequencer& s){s.process(.001,true,true,false,false);s.process(.001,true,true,true,false);}
int main(){
    const int major[]={0,2,4,5,7,9,11},melodic[]={0,2,3,5,7,9,11},harmonic[]={0,2,3,5,7,8,11};
    const int families[][7]={{1,3,7,8,4,2,9},{10,11,12,13,14,15,16},{17,18,19,20,21,22,23}};
    const int* source[]={major,melodic,harmonic};
    for(int f=0;f<3;++f)for(int mode=0;mode<7;++mode) {
        unsigned mask=0;for(int i=0;i<7;++i)mask|=1u<<modulo(source[f][(mode+i)%7]-source[f][mode],12);
        check(scaleMask(scaleAt(families[f][mode]))==mask,"factory modes follow the manual's parent scale rotations");
    }
    // p64 keyboard diagrams, including the less conventional World spellings.
    const unsigned world[]={0x9B3,0x9CD,0x8D1,0x18D,0x4A3,0x28D,0x18B};
    for(int i=0;i<7;++i)check(scaleMask(scaleAt(28+i))==world[i],"World notes match manual keyboard dots");
    check(scaleMask(scaleAt(35))==0x91 && scaleMask(scaleAt(42))==0x89,"major/minor triads have three notes");
    check(scaleMask(scaleAt(41))==0x895 && scaleMask(scaleAt(48))==0x4AD,"ninth/eleventh chord diagrams");
    bool seen[149]={};for(int p=0;p<149;++p){int id=scaleBrowseId(p);check(!seen[id] && scaleBrowsePosition(id)==p,"hardware browse order visits all IDs exactly once");seen[id]=true;}
    for(unsigned mask=1;mask<4096;++mask) {
        Scale scale=scaleFromMask(mask);check(scaleMask(scale)==mask,"all 4095 nonempty custom note sets survive conversion");
        for(int half=-73;half<=121;++half) {
            float input=half*.5f;int oracle=-100;float distance=1000;
            for(int candidate=-48;candidate<=72;++candidate)if(mask&(1u<<modulo(candidate,12))) {
                float d=std::abs(candidate-input);if(d<distance){distance=d;oracle=candidate;}
            }
            int degree=nearestDegree(input,scale),note=degreeToSemitone(degree,scale);
            check(note==oracle,"quantizer chooses the nearest enabled note, including lower-note ties and negative octaves");
            check(degreeToSemitone(degree+scale.count,scale)==note+12,"custom accumulator degrees preserve octave spacing");
        }
    }
    check(scaleFromMask(0).count==1 && scaleFromMask(0x1000).notes[0]==0,"empty/invalid masks have safe root-only fallback");
    for(int locked=49;locked<149;++locked)for(int offset=-32;offset<=32;++offset) {
        int result=modulatedUserScale(locked,float(offset));
        check(result==49+(locked-49)/10*10+modulo((locked-49)%10+offset,10),"custom modulation wraps inside the locked ten-slot bank");
    }
    check(modulatedUserScale(67,1)==68 && modulatedUserScale(67,2)==59,"manual Scale 2-9 +1/+2V example");
    check(modulatedUserScale(49,.99f)==49 && modulatedUserScale(49,-.99f)==49,"fractional voltages wait for whole-volt threshold");
    check(modulatedUserScale(49,std::numeric_limits<float>::infinity())==49,"nonfinite modulation safe");
    for(int target=0;target<3;++target)for(int route=0;route<3;++route) {
        Sequencer s;s.settings.scale=49;s.settings.lockedScale=49;
        s.settings.userScales[0].mask=0x91;s.settings.userScales[0].root=0;
        s.settings.userScales[1].mask=0x89;s.settings.userScales[1].root=9;
        s.settings.tracks[0].length=s.settings.tracks[1].length=1;
        if(route==0){auto& a=s.settings.auxInputs[0];a.connected=true;a.amount=1;a.voltage=1;a.destination=ModUserScale;a.assignment=target;}
        if(route==1){auto& l=s.settings.modLanes[0];l.length=1;l.values.fill(1);l.destination=ModUserScale;l.assignment=target;}
        if(route==2){auto& c=s.settings.controls[0];c.destination=CtrlUserScale;c.value=.2f;c.assignment=target;}
        tick(s);
        for(int t=0;t<2;++t){bool affected=target==2 || target==t;check(s.scaleIndex(t)==(affected?50:49),"AUX/MOD/CTRL scale modulation respects track assignment");near(s.tracks[t].pitch,affected?.75f:0,"custom scale root recalls with its notes on same clock");}
        s.settings.lockedScale=-1;tick(s);check(s.scaleIndex(0)==49 && s.scaleIndex(1)==49,"user scale modulation requires a lock");
        s.settings.lockedScale=1;tick(s);check(s.scaleIndex(0)==1,"factory lock ignores user-scale modulation");
    }
    for(int index=0;index<149;++index)for(int mode=0;mode<2;++mode) {
        Sequencer s;s.settings.scale=index;s.settings.tracks[0].length=1;s.settings.tracks[0].accumMode=static_cast<AccumMode>(mode);
        s.settings.userScales.fill(UserScale());if(userScale(index)){s.settings.userScales[index-49].mask=0x249;s.settings.userScales[index-49].root=5;}
        auto& e=s.settings.tracks[0].stages[0];e.pitchOverride=17;e.accumulation=1;
        Scale scale=s.currentScale(0);int degree=nearestDegree(17,scale);
        for(int i=0;i<5;++i){tick(s);near(s.tracks[0].pitch,float(degreeToSemitone(degree+i,scale)+s.scaleRoot(0))/12,"overrides accumulate in every factory/user scale");}
        s.loopy.stages=1;s.loopy.targets=1;tick(s);check(std::isfinite(s.sounding(0).pitch),"Loopy uses current scale safely");
    }
    std::cout<<"PASS: "<<checks<<" scale checks\n";
}
