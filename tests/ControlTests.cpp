#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>

int checks=0;
void check(bool value,const char* label) {
    ++checks;if(!value){std::cerr<<"FAIL: "<<label<<'\n';std::exit(1);}
}
void near(float a,float b,const char* label){check(std::abs(a-b)<.0001f,label);}
void tick(metro::Sequencer& s){s.process(.002,true,true,true,false);}
void assign(metro::Sequencer& s,int k,metro::CtrlDestination d,float v,int target=0) {
    s.settings.controls[k].destination=d;s.settings.controls[k].value=v;s.settings.controls[k].assignment=target;
}
int main() {
    using namespace metro;
    // Direct ranges are independently specified from the documented panel ranges.
    struct Range { CtrlDestination dest; float low,high; };
    const Range ranges[]={{CtrlTempo,.1f,400},{CtrlClockDiv,1,64},{CtrlGateLength,1,99},
        {CtrlOrder,0,20},{CtrlPulseDiv,1,8},{CtrlPulsesLength,0,64},{CtrlSlideAmount,0,100},
        {CtrlStagesLength,1,8},{CtrlStagesOffset,0,7},{CtrlAccumMode,0,1},{CtrlSwing,50,78}};
    for(auto r:ranges){near(ctrlDirectValue(r.dest,-1),r.low,"direct minimum");near(ctrlDirectValue(r.dest,1),r.high,"direct maximum");}
    near(ctrlDirectValue(CtrlAccumPolar,0),1,"noon selects bipolar");
    near(ctrlDirectValue(CtrlAccumPolar,-1),0,"negative polarity selects unipolar");
    near(ctrlDirectValue(CtrlAccumPolar,1),0,"positive polarity selects unipolar");
    near(ctrlClamp(std::numeric_limits<float>::quiet_NaN()),0,"nonfinite amount safe");
    {
        Sequencer s;assign(s,0,CtrlPitchOffset,.25f,0);assign(s,1,CtrlPitchOffset,-.5f,2);tick(s);
        near(s.tracks[0].pitch,-.25f,"both knob offsets combine on track 1");
        near(s.tracks[1].pitch,-.5f,"track 2 receives only its assignment");
        assign(s,0,CtrlPitchOffset,1,2);assign(s,1,CtrlPitchOffset,1,2);tick(s);
        near(s.tracks[0].pitch,1,"shared CTRL offset saturates at one full range");
        assign(s,0,CtrlNone,1);assign(s,1,CtrlNone,1);tick(s);
        near(s.tracks[0].pitch,0,"unassigning offset immediately restores base");
        near(s.settings.stages[0].pitch,0,"offset does not change base pitch");
    }
    {
        // Pre is quantized to C major; Post is chromatic; Offset is continuous.
        Sequencer s;assign(s,0,CtrlPitchPre,.26f);tick(s);near(s.tracks[0].pitch,4.f/12,"pre pitch enters scale quantizer");
        assign(s,0,CtrlPitchPost,.26f);tick(s);near(s.tracks[0].pitch,3.f/12,"post pitch uses semitones after scale");
        assign(s,0,CtrlPitchOffset,.26f);tick(s);near(s.tracks[0].pitch,.26f,"pitch offset preserves fractional semitones");
        assign(s,0,CtrlOctave,-1);tick(s);near(s.tracks[0].pitch,-2,"octave range reaches minus two");
        assign(s,0,CtrlRoot,1);tick(s);near(s.tracks[0].pitch,1,"root reaches one octave");
        assign(s,0,CtrlSliderInvert,1);tick(s);near(s.tracks[0].pitch,3,"inverted bottom slider becomes range maximum");
        assign(s,0,CtrlSliderInvert,-1);tick(s);near(s.tracks[0].pitch,0,"counterclockwise slider inversion is off");
        s.settings.auxInputs[0].destination=ModPitchOffset;s.settings.auxInputs[0].amount=1;
        s.settings.auxInputs[0].connected=true;s.settings.auxInputs[0].voltage=.25f;
        assign(s,0,CtrlPitchOffset,.5f);tick(s);near(s.tracks[0].pitch,.75f,"CTRL adds alongside AUX");
    }
    {
        Sequencer s;for(auto& stage:s.settings.stages)stage.pulses=2;
        assign(s,0,CtrlPulseCount,1.f/7); // three pulses on track 1, two on track 2
        const int expected1[]={0,0,0,1,1,1,2,2,2},expected2[]={0,0,1,1,2,2,3,3,4};
        for(int i=0;i<9;++i){tick(s);check(s.tracks[0].stage==expected1[i] && s.tracks[1].stage==expected2[i],"pulse count is a per-track offset of physical switches");}
        for(auto stage:s.settings.stages)check(stage.pulses==2,"pulse switches remain unchanged");
    }
    {
        Sequencer s;s.settings.tracks[0].stages[1].skip=true;s.settings.tracks[0].stages[3].skip=true;
        tick(s);tick(s);check(s.tracks[0].stage==2,"ordinary skip removes stage 2");
        s.tracks[0].accumulator[0]=5;
        assign(s,0,CtrlSkipInvert,1);tick(s);
        check(s.tracks[0].stage==1 && s.tracks[0].pulse==0,"skip inversion restarts at first newly enabled stage");
        check(s.tracks[0].accumulator[0]==5,"skip inversion preserves accumulator bank");
        tick(s);check(s.tracks[0].stage==3,"inverted mask visits only previously skipped stages");
        assign(s,0,CtrlSkipInvert,-1);tick(s);check(s.tracks[0].stage==0,"clearing inversion restarts normal traversal");
        for(auto& stage:s.settings.tracks[0].stages)stage.skip=false;
        assign(s,0,CtrlSkipInvert,1);tick(s);check(s.tracks[0].stage==-1 && s.tracks[0].gate==0,"inverting no skips safely silences all stages");
    }
    {
        Sequencer s;assign(s,0,CtrlDirection,-1);tick(s);check(s.tracks[0].stage==7,"reverse direction starts at final stage");
        tick(s);check(s.tracks[0].stage==6,"reverse advances backwards");
        assign(s,1,CtrlDirection,1);s.reset();tick(s);check(s.tracks[0].stage==0,"CTRL 2 wins conflicting switch assignments");
    }
    {
        Sequencer s;s.settings.tracks[0].length=1;s.settings.tracks[0].stages[0].accumulation=1;
        assign(s,0,CtrlAccumPolar,-1);tick(s);tick(s);
        near(s.tracks[0].pitch,-1.f/12,"negative unipolar accumulation descends by scale degree");
    }
    {
        Sequencer s;s.settings.tracks[0].stages[0].cv=2;s.settings.tracks[1].stages[0].cv=-3;
        assign(s,0,CtrlOutputSwap,1);tick(s);near(s.aux[0],-3,"output swap follows track CV at A");near(s.aux[1],2,"output swap follows track CV at B");
        s.settings.outputSources[0]=2;s.settings.modLanes[0].values.fill(4);tick(s);near(s.aux[0],4,"MOD-assigned A is unaffected by track swap");
        s.process(.001,false,true,false,false);near(s.aux[0],0,"swapped outputs silence on stop");
    }
    {
        Sequencer s;assign(s,0,CtrlProbability,-1);tick(s);near(s.tracks[0].gate,0,"minus 100 points prevents gate");
        near(s.tracks[1].gate,5,"probability leaves other track alone");
        Sequencer r;assign(r,0,CtrlRatchet,1);r.process(.0001,true,true,true,false);
        int rises=1;bool wasHigh=true;
        for(int i=0;i<1249;++i){r.process(.0001,true,true,false,false);bool high=r.tracks[0].gate>0;if(high&&!wasHigh)++rises;wasHigh=high;}
        check(rises==8,"ratchet offset produces eight gate attacks per pulse");
    }
    // Seeded traversal stress with live knob changes, all play orders and both
    // accumulator modes. This checks bounds and stability, not hardware identity.
    for(int order=0;order<21;++order)for(int mode=0;mode<2;++mode)for(int seed=1;seed<=8;++seed) {
        Sequencer s;s.seed(seed);s.settings.tracks[0].order=static_cast<Order>(order);
        s.settings.tracks[0].accumMode=static_cast<AccumMode>(mode);
        for(int stage=0;stage<8;++stage){s.settings.stages[stage].pulses=1+(stage+seed)%8;s.settings.tracks[0].stages[stage].skip=(stage+seed)%3==0;s.settings.tracks[0].stages[stage].accumulation=stage-3;}
        for(int i=0;i<128;++i){assign(s,0,CtrlSkipInvert,(i/16)%2?1:-1);assign(s,1,CtrlPulseCount,(i%15-7)/7.f);tick(s);
            auto st=s.tracks[0];check(st.stage>=-1 && st.stage<8 && st.pulse>=0 && st.pulse<8,"live skip/pulse edits stay in bounds");
            check(st.stage<0?st.gate==0:(!s.settings.tracks[0].stages[st.stage].skip!=st.skipInverted),"live traversal never plays an effectively skipped stage");
            check(std::isfinite(st.pitch) && (st.gate==0 || st.gate==5),"live controls emit finite pitch and valid gates");
        }
    }
    std::cout<<"PASS: "<<checks<<" CTRL engine checks\n";
}
