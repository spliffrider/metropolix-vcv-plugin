#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
int checks=0;
void require(bool v,const char* message){++checks;if(!v){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
void near(float v,float expected,const char* message){require(std::abs(v-expected)<1e-5f,message);}
void tick(metro::Sequencer& s){s.process(.002,true,true,true,false);}
int main(){
    using namespace metro;
    bool covered[OutputSourceCount]={};
    const int legacyBases[]={10,11,12,13,0,14,16,18,2,26,28,29,31,32,34,36};
    for(int kind=0;kind<OutputKindCount;++kind)for(int target=0;target<std::max(1,outputTargetCount(kind));++target){
        int source=outputSource(kind,target);require(source>=0 && source<OutputSourceCount && !covered[source],"all function/assignment pairs have unique stable IDs");covered[source]=true;
        if(kind<16)require(source==legacyBases[kind]+target,"all38 legacy packed output IDs remain frozen");
        auto decoded=outputSelection(source);require(decoded.kind==kind && decoded.target==target,"all source IDs round-trip");
    }
    for(bool yes:covered)require(yes,"every stored source ID has a function");
    require(outputSource(OutTrackCv,0)==0 && outputSource(OutTrackCv,1)==1,"legacy track CV source IDs remain");
    for(int lane=0;lane<8;++lane)require(outputSource(OutModCv,lane)==lane+2,"legacy MOD source IDs remain");
    // A three-stage window, with stage2 skipped: 3 pulses on stage1, then 4 on stage3.
    for(int fixed:{0,6}){
        Sequencer s;s.settings.tracks[0].length=3;s.settings.tracks[0].pulseLimit=fixed;
        s.settings.stages[0].pulses=3;s.settings.stages[2].pulses=4;s.settings.tracks[0].stages[1].skip=true;
        s.settings.tracks[0].stages[2].probability=0;s.settings.tracks[0].stages[2].accumulation=4;
        for(int p=0;p<30;++p){tick(s);int phase=p%(fixed?6:7);
            near(s.outputVoltage(outputSource(OutFirstPulse)),fixed?p%6==0?5:0:p==0?5:0,"First Pulse uses fixed LEN resets, Auto only initial start");
            near(s.outputVoltage(outputSource(OutLastPulse)),fixed && p%6==5?5:0,"Last Pulse precedes fixed LEN reset; Auto never emits");
            near(s.outputVoltage(outputSource(OutNewStage)),phase==0 || phase==3?5:0,"New Stage respects multi-pulse stages and skips, independent of probability");
            near(s.outputVoltage(outputSource(OutReset)),p==0?5:0,"track LEN resets are not global resets");
        }
    }
    for(int order=0;order<21;++order)for(int length=1;length<=64;++length){
        Sequencer s;s.settings.tracks[0].order=Order(order);s.settings.tracks[0].pulseLimit=length;
        for(int st=0;st<8;++st){s.settings.stages[st].pulses=1+st;s.settings.tracks[0].stages[st].skip=st==2 || st==5;s.settings.tracks[0].stages[st].accumulation=st-3;}
        for(int p=0;p<length*2+1;++p){tick(s);near(s.outputVoltage(outputSource(OutFirstPulse)),p%length==0?5:0,"First Pulse grid is independent of order/skip/pulse counts");near(s.outputVoltage(outputSource(OutLastPulse)),p%length==length-1?5:0,"Last Pulse grid is independent of order/skip/pulse counts");}
    }
    {
        Sequencer s;s.settings.tracks[0].pulseLimit=3;
        for(auto& stage:s.settings.tracks[0].stages)stage.skip=true;
        for(int p=0;p<10;++p){tick(s);near(s.outputVoltage(outputSource(OutFirstPulse)),p%3==0?5:0,"all-skipped tracks retain First Pulse timing");near(s.outputVoltage(outputSource(OutLastPulse)),p%3==2?5:0,"all-skipped tracks retain Last Pulse timing");near(s.outputVoltage(outputSource(OutNewStage)),0,"all-skipped window emits no New Stage");}
        s.process(.002,true,true,false,true);near(s.outputVoltage(outputSource(OutFirstPulse)),5,"manual reset triggers First Pulse immediately");near(s.outputVoltage(outputSource(OutLastPulse)),0,"manual reset never emits Last Pulse");near(s.outputVoltage(outputSource(OutReset)),5,"manual reset emits global Reset");tick(s);near(s.outputVoltage(outputSource(OutFirstPulse)),0,"next post-reset clock does not duplicate manual First Pulse");
        s.process(.002,false,true,false,true);near(s.outputVoltage(outputSource(OutReset)),5,"manual reset also works stopped");near(s.outputVoltage(outputSource(OutRun)),0,"Run is low stopped");
    }
    {
        Sequencer s;s.settings.controls[0].value=.4f;s.settings.controls[1].value=-.6f;
        s.settings.stages[0].pitch=.3f;s.settings.stages[0].pulses=8;s.settings.tracks[0].sliderOctaves=4;s.settings.tracks[0].sliderInverted=true;
        s.settings.tracks[0].stages[0].pitchOverride=60;s.settings.tracks[0].stages[0].cv=3;
        tick(s);near(s.outputVoltage(10),2,"CTRL1 bipolar");near(s.outputVoltage(11),3.5f,"CTRL1 unipolar");near(s.outputVoltage(12),-3,"CTRL2 bipolar");near(s.outputVoltage(13),1,"CTRL2 unipolar");
        near(s.outputVoltage(outputSource(OutSlider)),1.5f,"raw slider ignores inversion/range/pitch override");near(s.outputVoltage(outputSource(OutSliderRange)),2.8f,"Slider Range honours track octaves/direction, ignores pitch override");
        s.settings.controls[0].destination=CtrlSliderInvert;s.settings.controls[0].value=-1;tick(s);near(s.outputVoltage(outputSource(OutSliderRange)),2.8f,"negative CTRL preserves programmed slider direction");
        s.settings.controls[0].value=1;tick(s);near(s.outputVoltage(outputSource(OutSliderRange)),1.2f,"positive CTRL inverts programmed slider direction");
        s.settings.tracks[0].velocitySource=0;near(s.outputVoltage(outputSource(OutVelocity)),2.5f,"50% static velocity");s.settings.tracks[0].velocitySource=1;near(s.outputVoltage(outputSource(OutVelocity)),3.75f,"75% static velocity");s.settings.tracks[0].velocitySource=2;near(s.outputVoltage(outputSource(OutVelocity)),5,"firmware default100% velocity");
        s.settings.tracks[0].velocitySource=3;s.settings.tracks[0].stages[s.tracks[0].stage].cv=3;s.process(.002,true,true,false,false);near(s.outputVoltage(outputSource(OutVelocity)),3,"Track CV velocity");
        s.settings.controls[0].destination=CtrlVelocity;s.settings.controls[0].value=-.2f;s.process(.002,true,true,false,false);near(s.outputVoltage(outputSource(OutVelocity)),2,"CTRL velocity offsets by voltage");
        s.settings.auxInputs[0].destination=ModVelocity;s.settings.auxInputs[0].connected=true;s.settings.auxInputs[0].amount=1;s.settings.auxInputs[0].voltage=2;s.process(.002,true,true,false,false);near(s.outputVoltage(outputSource(OutVelocity)),4,"AUX velocity adds to Track CV and CTRL offset");
    }
    {
        Sequencer s;s.settings.outputSources={{0,1}};s.settings.tracks[0].stages[0].cv=2;s.settings.tracks[1].stages[0].cv=-3;
        tick(s);near(s.aux[0],2,"A legacy track CV");near(s.aux[1],-3,"B legacy track CV");
        s.settings.auxInputs[0].destination=ModOutputSwap;s.settings.auxInputs[0].connected=true;s.settings.auxInputs[0].amount=1;s.settings.auxInputs[0].voltage=5;s.process(.002,true,true,false,false);near(s.aux[0],-3,"AUX gate swaps A track assignment");near(s.aux[1],2,"AUX gate swaps B track assignment");
        s.settings.controls[0].value=.6f;near(s.outputVoltage(outputSource(OutCtrl1Bipolar)),3,"global CTRL output is unaffected by swap");
    }
    {
        Sequencer s;for(int lane=0;lane<8;++lane){s.settings.modLanes[lane].division=lane+1;s.settings.modLanes[lane].values.fill(float(lane)-4);}
        for(int p=0;p<33;++p){tick(s);for(int lane=0;lane<8;++lane){near(s.outputVoltage(outputSource(OutModClock,lane)),p%(lane+1)==0?5:0,"each MOD Clock has its own division");near(s.outputVoltage(outputSource(OutModCv,lane)),float(lane)-4,"MOD voltage remains separate from clock");}}
        s.settings.modLanes[0].mute=true;tick(s);near(s.outputVoltage(outputSource(OutModClock)),0,"muted MOD clock is silent while traversal continues");s.process(.002,false,true,false,false);near(s.outputVoltage(outputSource(OutModClock,7)),0,"MOD clocks stop with transport");
    }
    for(int rate:{44100,48000,96000}){
        Sequencer s;s.settings.tracks[0].pulseLimit=4;s.settings.stages[0].pulses=3;
        s.settings.modLanes[7].division=2;
        const double dt=1.0/rate;const int period=rate/10;
        for(int pulse=0;pulse<10;++pulse){
            int highs[5]={};
            const int ids[]={outputSource(OutFirstPulse),outputSource(OutLastPulse),outputSource(OutNewStage),outputSource(OutModClock,7),outputSource(OutReset)};
            for(int frame=0;frame<period;++frame){s.process(dt,true,true,frame==0,false);for(int i=0;i<5;++i)if(s.outputVoltage(ids[i])>1)++highs[i];}
            const bool expected[]={pulse%4==0,pulse%4==3,pulse%4==0 || pulse%4==3,pulse%2==0,pulse==0};
            for(int i=0;i<5;++i)require(expected[i]?highs[i]>=rate/1000 && highs[i]<=rate/1000+1:highs[i]==0,"real-rate output triggers are one millisecond and follow their own timing");
        }
    }
    {
        Sequencer s,reference;for(auto* engine:{&s,&reference}){engine->settings.tracks[0].pulseLimit=5;engine->settings.stages[3].pulses=2;}
        for(int p=0;p<30;++p){s.loopy.stages=p>=3 && p<23?1u<<3:0;tick(s);tick(reference);
            near(s.outputVoltage(outputSource(OutFirstPulse)),reference.outputVoltage(outputSource(OutFirstPulse)),"Loopy keeps background First Pulse timing");near(s.outputVoltage(outputSource(OutLastPulse)),reference.outputVoltage(outputSource(OutLastPulse)),"Loopy keeps background Last Pulse timing");
            if(s.loopy.stages)near(s.outputVoltage(outputSource(OutNewStage)),(p-3)%2==0?5:0,"Loopy New Stage follows the audible foreground");
        }
    }
    #include "OutputTriggerTests.hpp"
    std::cout<<"PASS: OutputRoutingTests "<<checks<<" checks\n";
}
