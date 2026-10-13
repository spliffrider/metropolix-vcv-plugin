#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>
using namespace metro;
unsigned checks=0;
void check(bool v,const char* why){++checks;if(!v){std::cerr<<"FAIL: "<<why<<'\n';std::exit(1);}}
void near(float a,float b,const char* why){check(std::abs(a-b)<.002f,why);}
void edge(Sequencer& s,int rate){s.process(1.0/rate,true,true,false,false);s.process(1.0/rate,true,true,true,false);}
void wait(Sequencer& s,int rate,double seconds,bool run=true){for(int i=0;i<int(std::round(seconds*rate));++i)s.process(1.0/rate,run,true,false,false);}
int main(){
    for(int ramp=0;ramp<3;++ramp)for(int pulses=1;pulses<=8;++pulses)for(int pulse=0;pulse<pulses;++pulse) {
        for(int part=0;part<=100;++part){double phase=part*.01;
            float expected=ramp==0?4.f:float(-4+8*std::min(1.0,(pulse+phase)/(ramp==1?pulses:1)));
            near(stageCvValue(-4,4,BipolarCv,static_cast<StageCvRamp>(ramp),pulse,phase*.125,.125,pulses),expected,"manual Step/sRamp/pRamp timing, including four-pulse example");
            near(stageCvValue(-4,4,GateCv,static_cast<StageCvRamp>(ramp),pulse,phase*.125,.125,pulses),5,"Gate Toggle ignores ramp types");
        }
    }
    for(int type=0;type<3;++type)for(int v=-100;v<=100;++v) {
        float value=trackCvVoltage(v*.1f,static_cast<TrackCvType>(type));
        check(std::isfinite(value) && value>=(type==0?-5:0) && value<=5,"CV type voltage bounds");
        if(type==2)check(value==0 || value==5,"gate output is exactly zero/five volts");
    }
    check(trackCvVoltage(std::numeric_limits<float>::quiet_NaN(),BipolarCv)==0,"nonfinite CV is silent");
    near(stageCvValue(-4,4,UnipolarCv,CvStageRamp,0,.0625,.125,4),.5,"unipolar ramp clamps negative source and follows positive target");
    for(int rate:{44100,48000,96000})for(int ramp=0;ramp<3;++ramp) {
        Sequencer s;s.settings.tracks[0].length=2;s.settings.tracks[0].stages[1].cv=4;
        s.settings.tracks[0].stages[1].cvRamp=static_cast<StageCvRamp>(ramp);s.settings.stages[1].pulses=4;
        edge(s,rate);wait(s,rate,.125-2.0/rate);edge(s,rate);check(s.tracks[0].stage==1,"CV transition enters second stage");
        near(s.aux[0],ramp==0?4.f:0.f,"ramp starts from actual previous CV and reaches A output");
        wait(s,rate,.0625);near(s.aux[0],ramp==0?4.f:ramp==1?.5f:2.f,"CV halfway through first pulse matches manual example");
        wait(s,rate,.0625-2.0/rate);edge(s,rate);near(s.aux[0],ramp==1?1.f:4.f,"sRamp spans stage and pRamp finishes first pulse");
        // A missing external edge holds progress at the end of this physical pulse.
        wait(s,rate,2);near(s.aux[0],ramp==1?2.f:4.f,"stalled clock does not invent additional sRamp pulses");
        s.settings.tracks[0].mute=true;wait(s,rate,.01);near(s.aux[0],0,"mute suppresses track CV output");
        s.settings.tracks[0].mute=false;wait(s,rate,.001);near(s.aux[0],ramp==1?2.f:4.f,"unmute restores internal CV without restarting ramp");
    }
    for(int order=0;order<21;++order)for(int type=0;type<3;++type) {
        Sequencer s;s.seed(154);s.settings.tracks[0].order=static_cast<Order>(order);s.settings.tracks[0].cvType=static_cast<TrackCvType>(type);
        for(int stage=0;stage<8;++stage){s.settings.tracks[0].stages[stage].cv=stage-3.5f;s.settings.stages[stage].gate=Rest;s.settings.tracks[0].stages[stage].probability=0;}
        s.settings.tracks[0].stages[1].skip=true;s.settings.tracks[0].stages[5].skip=true;
        for(int i=0;i<100;++i){edge(s,48000);int stage=s.tracks[0].stage;check(stage!=1 && stage!=5,"CV follows skipped traversal without an extra event");near(s.aux[0],trackCvVoltage(stage-3.5f,static_cast<TrackCvType>(type)),"CV runs independently of physical note Rest/probability across all orders");}
    }
    for(int rate:{44100,48000,96000})for(int division:{1,3})for(float swing:{50.f,78.f}) {
        Sequencer s;s.settings.tracks[0].division=division;s.settings.tracks[0].swing=swing;
        for(int stage=0;stage<8;++stage){s.settings.stages[stage].pulses=stage+1;s.settings.tracks[0].stages[stage].cv=stage%2?5.f:-5.f;s.settings.tracks[0].stages[stage].cvRamp=stage%2?CvStageRamp:CvPulseRamp;}
        for(int sample=0;sample<rate*3;++sample){s.process(1.0/rate,true,false,false,false);check(std::isfinite(s.aux[0]) && s.aux[0]>=-5 && s.aux[0]<=5,"swung/divided CV remains finite and bounded at all sample rates");}
    }
    for(int type=0;type<3;++type){Sequencer s;s.settings.tracks[0].cvType=static_cast<TrackCvType>(type);s.settings.tracks[0].stages[2].cv=4;s.settings.tracks[0].stages[2].cvRamp=CvStageRamp;s.loopy.stages=4;
        s.process(1.0/48000,false,true,false,false);near(s.aux[0],type==2?5.f:4.f,"stopped Stage Player sends typed CV immediately without ramps");
        s.settings.tracks[0].stages[2].cv=-4;wait(s,48000,.01,false);near(s.aux[0],type==2?5.f:4.f,"audition CV freezes until another press");
        s.loopy.stages=0;s.process(1.0/48000,false,true,false,false);near(s.aux[0],0,"audition release silences track CV");
    }
    std::cout<<"PASS: "<<checks<<" stage CV checks\n";
}
