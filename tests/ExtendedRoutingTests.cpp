#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
int checks=0;
void require(bool ok,const char* text){++checks;if(!ok){std::cerr<<"FAIL: "<<text<<'\n';std::exit(1);}}
void near(float a,float b,const char* text){require(std::abs(a-b)<.0001f,text);}
void route(metro::Sequencer& s,metro::ModDestination dest,float value,int target=0,int input=0){auto& a=s.settings.auxInputs[input];a.destination=dest;a.connected=true;a.voltage=value;a.amount=1;a.assignment=target;}
void tick(metro::Sequencer& s){s.process(.00002,true,true,true,false);}
int main(){
    using namespace metro;
    static_assert(ModAccumReset==18 && ModTempo==19 && ModStagesOffset==38 && ModDestinationCount==39,"stable old IDs and complete manual destination list");
    const ModDestination offsets[]={ModClockDiv,ModGateScale,ModOrder,ModPulseCount,ModPulseDiv,ModPulsesLength,ModRoot,ModSlideAmount,ModSliderRange,ModStagesLength,ModStagesOffset};
    for(auto dest:offsets)for(int polarity:{-1,1})for(int order=0;order<21;++order){
        Sequencer s,reference;for(auto* q:{&s,&reference}){q->seed(713);q->settings.scale=0;q->settings.octaves=2;auto& t=q->settings.tracks[0];t.length=4;t.offset=3;t.division=8;t.order=Order(order);t.pulseDivision=4;t.pulseLimit=16;t.slideTime=.5;t.gateScale=.5;t.sliderOctaves=2;
            for(int st=0;st<8;++st){q->settings.stages[st].pulses=4;q->settings.stages[st].pitch=st/8.f;t.stages[st].slide=true;t.stages[st].accumulation=st%3;t.stages[st].ratchets=2;t.stages[st].skip=st==6;}}
        auto& t=reference.settings.tracks[0];bool positive=polarity>0;
        switch(dest){
            case ModClockDiv:t.division=positive?64:1;break;
            case ModGateScale:t.gateScale=positive?1:0;break;
            case ModOrder:t.order=positive?RandomOrder:Forward;break;
            case ModPulseCount:t.pulseSource=positive?8:1;break;
            case ModPulseDiv:t.pulseDivision=positive?8:1;break;
            case ModPulsesLength:t.pulseLimit=positive?64:1;break;
            case ModRoot:reference.settings.controls[0].destination=CtrlRoot;reference.settings.controls[0].value=polarity;reference.settings.controls[0].assignment=0;break;
            case ModSlideAmount:t.slideTime=positive?1:0;break;
            case ModSliderRange:t.sliderOctaves=positive?4:1;break;
            case ModStagesLength:t.length=positive?8:1;break;
            case ModStagesOffset:t.offset=positive?7:0;break;
            default:break;
        }
        route(s,dest,polarity*5.f);
        for(int pulse=0;pulse<70;++pulse){tick(s);tick(reference);for(int frame=0;frame<12;++frame){s.process(.005,true,true,false,false);reference.process(.005,true,true,false,false);require(s.tracks[0].stage==reference.tracks[0].stage && s.tracks[0].pulse==reference.tracks[0].pulse,"offset endpoint follows independently programmed manual range through every play order");near(s.tracks[0].pitch,reference.tracks[0].pitch,"offset endpoint pitch matches unmodulated equivalent");near(s.tracks[0].gate,reference.tracks[0].gate,"offset endpoint gate rhythm matches unmodulated equivalent");require(s.tracks[1].stage==reference.tracks[1].stage && s.tracks[1].pitch==reference.tracks[1].pitch,"track-specific offset leaves the other track unchanged");}}
        require(s.settings.tracks[0].length==4 && s.settings.tracks[0].offset==3 && s.settings.tracks[0].division==8 && s.settings.tracks[0].pulseLimit==16 && s.settings.octaves==2 && s.settings.root==0,"routing leaves all programmed settings intact");
    }
    for(auto dest:{ModDirection,ModSkipInvert,ModSlide,ModSliderInvert})for(int type=0;type<3;++type)for(int order=0;order<21;++order){
        Sequencer s,reference;for(auto* q:{&s,&reference}){q->seed(521);auto& t=q->settings.tracks[0];t.order=Order(order);t.slideType=SlideType(type);t.slideTime=.8f;for(int st=0;st<8;++st){q->settings.stages[st].pitch=st/8.f;q->settings.stages[st].pulses=1+st%3;t.stages[st].skip=st%2;t.stages[st].slide=st%2;}}
        auto& t=reference.settings.tracks[0];if(dest==ModDirection)t.reversed=true;if(dest==ModSkipInvert)for(auto& st:t.stages)st.skip=!st.skip;if(dest==ModSlide)for(auto& st:t.stages)st.slide=true;if(dest==ModSliderInvert)t.sliderInverted=true;
        route(s,dest,5);for(int p=0;p<40;++p){tick(s);tick(reference);for(int f=0;f<20;++f){s.process(.003,true,true,false,false);reference.process(.003,true,true,false,false);require(s.tracks[0].stage==reference.tracks[0].stage,"gate route preserves equivalent order and skip traversal");near(s.tracks[0].pitch,reference.tracks[0].pitch,"gate route matches programmed direction/slide/invert pitch");near(s.tracks[0].gate,reference.tracks[0].gate,"gate route matches programmed Acid/gate rhythm");}}
    }
    for(int target=0;target<3;++target)for(int rate:{44100,48000,96000}){
        Sequencer s,reference;s.settings.bpm=120;reference.settings.bpm=240;route(s,ModTempo,1.5f,target);int clocks=0;bool last=false;
        for(int frame=0;frame<rate*2;++frame){s.process(1.0/rate,true,false,false,false);reference.process(1.0/rate,true,false,false,false);require(s.tracks[0].stage==reference.tracks[0].stage && s.tracks[0].gate==reference.tracks[0].gate,"global AUX tempo follows programmed equivalent at three rates, independent of target");bool high=s.clock>0;if(high && !last)++clocks;last=high;}
        require(clocks==32 && s.settings.bpm==120 && s.effectiveTempo()==240,"tempo routing produces 32 sixteenth clocks in two seconds without rewriting saved BPM");
        route(s,ModTempo,5,target);tick(s);require(s.effectiveTempo()==400,"positive tempo saturates at manual 400 BPM");route(s,ModTempo,-5,target);tick(s);near(s.effectiveTempo(),.1f,"negative tempo saturates at manual 0.1 BPM");
    }
    {
        Sequencer s;for(int p=0;p<4;++p)tick(s);int other=s.tracks[1].stage;float pitch=s.tracks[0].pitch;
        route(s,ModReset,5);s.process(.001,true,true,false,false);require(s.tracks[0].stage==-1 && s.tracks[0].pitch==pitch && s.tracks[1].stage==other,"off-clock targeted reset holds pitch and preserves other track position");tick(s);require(s.tracks[0].stage==0 && s.tracks[1].stage==other+1,"reset track restarts on next master pulse without restarting other track");tick(s);require(s.tracks[0].stage==1,"held targeted reset does not retrigger");
    }
    {
        Sequencer s;route(s,ModGateLength,5);tick(s);for(int f=0;f<1000;++f){s.process(.001,true,true,false,false);require(s.tracks[0].gate==5,"positive Gate Length reaches manual100 percent Hold endpoint");}
        route(s,ModGateLength,-5);s.process(.001,true,true,false,false);require(s.tracks[0].gate==0,"negative Gate Length reaches Rest endpoint");
    }
    {
        Sequencer s;route(s,ModLoopy,5,1);s.loopy.targets=1;s.loopy.pulses=3;route(s,ModLoopyOffset,5.f/7,0,1);route(s,ModLoopyPulses,5.f/64,2,2);
        for(int p=0;p<12;++p){tick(s);require(s.loopPlaying[0] && !s.loopPlaying[1] && s.sounding(0).stage==1+p%4,"hands-free Loopy respects configured target, start offset and modulated four-pulse count");require(s.tracks[0].stage==p%8,"normal background keeps advancing under gate Loopy");}
        require(s.loopy.stages==0 && s.loopy.pulses==3 && s.currentLoopy().pulses==4,"Loopy routing keeps original gesture/configuration intact");route(s,ModLoopy,0);s.process(.001,true,true,false,false);require(!s.loopPlaying[0] && s.sounding(0).stage==s.tracks[0].stage,"low gate returns immediately to current background position");
        s.loopy.stages=6;route(s,ModLoopyOffset,5,0,1);tick(s);require(s.currentLoopy().stages==192,"range offset saturates at right edge without losing its two-stage width");
    }
    std::cout<<"PASS: ExtendedRoutingTests "<<checks<<" checks\n";
}
