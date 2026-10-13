#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>
using namespace metro;
unsigned checks=0;
void check(bool v,const char* why){++checks;if(!v){std::cerr<<"FAIL: "<<why<<'\n';std::exit(1);}}
void near(float a,float b,const char* why){check(std::abs(a-b)<.0002f,why);}
void edge(Sequencer& s,int rate){s.process(1.0/rate,true,true,false,false);s.process(1.0/rate,true,true,true,false);}
void wait(Sequencer& s,int rate,double seconds,bool run=true){for(int i=0;i<int(std::round(seconds*rate));++i)s.process(1.0/rate,run,true,false,false);}
void setup(Sequencer& s,SlideType type,bool outgoing=false){s.settings.scale=0;s.settings.tracks[0].length=2;s.settings.tracks[0].slideTime=1;s.settings.tracks[0].slideType=type;s.settings.tracks[0].stages[0].pitchOverride=0;s.settings.tracks[0].stages[1].pitchOverride=12;s.settings.tracks[0].stages[outgoing?0:1].slide=true;}
int main(){
    check(nextSlideType(AnalogSlide)==TempoSlide && nextSlideType(TempoSlide)==AcidSlide && nextSlideType(AcidSlide)==AnalogSlide,"encoder follows hardware Analog/Tempo/Acid cycle");
    check(analogSlideTime(0)==0 && analogSlideTime(1)==1,"analog calibration has instant zero and one second/octave maximum");
    double previous=0;for(int amount=1;amount<=100;++amount){double time=analogSlideTime(amount*.01f);check(time>previous && time<=1.000001,"analog amount increases monotonic timing");previous=time;}
    check(analogSlideTime(std::numeric_limits<float>::quiet_NaN())==0,"nonfinite amount falls back to instant");
    for(int rate:{44100,48000,96000})for(float bpm:{60.f,120.f,240.f}) {
        double period=15.0/bpm;
        {
            Sequencer s;setup(s,TempoSlide);s.settings.bpm=bpm;edge(s,rate);wait(s,rate,period-2.0/rate);edge(s,rate);
            near(s.tracks[0].pitch,0,"Tempo begins at previous note");wait(s,rate,period*.5);near(s.tracks[0].pitch,.5f,"Tempo midpoint depends on pulse duration");wait(s,rate,period*.5+.001);near(s.tracks[0].pitch,1,"Tempo arrives within one pulse independent of interval");
        }
        {
            Sequencer s;setup(s,AnalogSlide);s.settings.bpm=bpm;edge(s,rate);wait(s,rate,period-2.0/rate);edge(s,rate);
            wait(s,rate,.02);near(s.tracks[0].pitch,.02f,"Analog slew speed is independent of tempo");
            s.settings.tracks[0].stages[1].pitchOverride=24;wait(s,rate,.02);near(s.tracks[0].pitch,.04f,"Analog speed is independent of changing pitch interval");
            wait(s,rate,2.1);near(s.tracks[0].pitch,2,"Analog reaches distant target without overshoot");
        }
        for(int gate=0;gate<4;++gate) {
            Sequencer s;setup(s,AcidSlide,true);s.settings.bpm=bpm;s.settings.stages[0].gate=static_cast<Gate>(gate);s.settings.stages[0].pulses=3;
            s.settings.tracks[0].stages[0].pitchOverride=12;s.settings.tracks[0].stages[1].pitchOverride=24;
            edge(s,rate);near(s.tracks[0].pitch,1,"marked Acid stage plays its own pitch immediately");
            for(int pulse=0;pulse<3;++pulse){if(pulse)edge(s,rate);wait(s,rate,.01);check(s.tracks[0].stage==0 && s.tracks[0].gate==5,"Acid forces Hold for every pulse and physical gate type");}
            edge(s,rate);check(s.tracks[0].stage==1 && s.tracks[0].pitch>1 && s.tracks[0].pitch<1.001,"Acid slides on following stage");
            wait(s,rate,.02);near(s.tracks[0].pitch,1.02f,"Acid uses analog constant-rate slew on next stage");
        }
    }
    for(int source=0;source<2;++source)for(int skipped=0;skipped<2;++skipped) {
        Sequencer s;setup(s,AcidSlide);s.settings.tracks[0].length=3;s.settings.tracks[0].stages[0].slide=source;s.settings.tracks[0].stages[1].slide=skipped;s.settings.tracks[0].stages[1].skip=true;s.settings.tracks[0].stages[2].pitchOverride=24;
        edge(s,48000);edge(s,48000);check(s.tracks[0].stage==2,"skip consumes no extra stage/pulse");near(s.tracks[0].pitch,source?0.f:2.f,"Acid looks at previous played stage, not skipped stage");
    }
    for(int cause=0;cause<4;++cause) {
        Sequencer s;setup(s,AcidSlide,true);
        if(cause==0)s.settings.tracks[0].stages[0].gateOverride=0;
        if(cause==1)s.settings.tracks[0].gateScale=0;
        if(cause==2)s.settings.tracks[0].mute=true;
        if(cause==3)s.settings.tracks[0].stages[0].probability=0;
        edge(s,48000);check(s.tracks[0].gate==0,"Rest override/zero scale/mute/probability still suppress Acid gate");
    }
    for(int type=0;type<3;++type) {
        Sequencer s;setup(s,static_cast<SlideType>(type));edge(s,48000);s.process(1.0/48000,false,true,false,false);s.loopy.stages=2;
        s.process(1.0/48000,false,true,false,false);float initial=s.sounding(0).pitch;
        check(type==AnalogSlide?(initial>0 && initial<.001):(initial==1),"stopped Stage Player respects Analog only as p44 specifies");
        s.settings.tracks[0].stages[1].pitchOverride=24;s.settings.tracks[0].slideTime=0;
        wait(s,48000,.02,false);near(s.sounding(0).pitch,type==AnalogSlide?.02f:1.f,"player freezes note and slide calibration during held press");
        check(s.sounding(0).gate==5 && s.tracks[0].stage==0,"player glide holds gate without moving original transport");
        s.loopy.stages=0;s.process(1.0/48000,false,true,false,false);check(s.sounding(0).gate==0,"release ends audition gate");
    }
    for(int rate:{44100,48000,96000})for(int percent:{0,1,25,50,75,100})for(int semitone=1;semitone<=60;++semitone) {
        double dt=1.0/rate,time=analogSlideTime(percent*.01f);float pitch=0,target=semitone/12.f;
        for(int i=0;i<1000;++i){float next=slewPitch(pitch,target,dt,time);check(next>=pitch && next<=target && std::isfinite(next),"all calibrated upward glides stay bounded");pitch=next;}
        for(int i=0;i<1000;++i){float next=slewPitch(pitch,-target,dt,time);check(next<=pitch && next>=-target && std::isfinite(next),"all calibrated downward glides stay bounded");pitch=next;}
    }
    std::cout<<"PASS: "<<checks<<" slide checks\n";
}
