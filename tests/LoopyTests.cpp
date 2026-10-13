#include "../src/Sequencer.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
int checks=0;
void check(bool ok,const std::string& message){++checks;if(!ok){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
void near(float a,float b,const char* message){check(std::abs(a-b)<.00001f,message);}
void tick(metro::Sequencer& s){s.process(.00001,true,true,true,false);}
int main(){
    // Manual pp42-43, including both linear directions and explicit truncation/wrap.
    for(int reverse=0;reverse<2;++reverse)for(int count:{0,3,5}){
        metro::Sequencer s;s.settings.tracks[0].reversed=reverse;
        s.loopy.stages=1u|8u;s.loopy.pulses=count;
        for(int i=0;i<30;++i){tick(s);int position=i%(count?count:4)%4;
            check(s.sounding(0).stage==(reverse?3-position:position),"manual 2-finger sequence");
            check(s.sounding(1).stage==i%8,"Current target leaves other track alone");}
    }
    for(int reverse=0;reverse<2;++reverse){
        metro::Sequencer s;s.loopy.stages=1u<<3;s.settings.stages[3].pulses=3;s.settings.tracks[0].reversed=reverse;
        for(int i=0;i<12;++i){tick(s);check(s.sounding(0).stage==3 && s.sounding(0).pulse==i%3,"1-finger Auto repeats every stage pulse");}
        s.loopy.pulses=5;s.reset();
        for(int i=0;i<15;++i){tick(s);int j=i%5;check(s.sounding(0).stage==(j<3?3:reverse?5-j:j+1),"fixed 1-finger length extends linearly");}
    }
    {
        metro::Sequencer s;s.loopy.stages=1u|4u;s.loopy.targets=3;
        s.settings.stages[0].pulses=2;s.settings.stages[1].pulses=3;s.settings.stages[2].pulses=1;
        for(auto& t:s.settings.tracks){t.length=1;t.offset=7;t.pulseLimit=1;for(auto& edit:t.stages)edit.skip=true;}
        const int expected[]={0,0,1,1,1,2};
        for(int i=0;i<24;++i){tick(s);for(int t=0;t<2;++t)check(s.sounding(t).stage==expected[i%6],"Loopy ignores skips, length, offset and pulse limit");}
        s.settings.stages[1].pulses=1;s.reset();
        const int changed[]={0,0,1,2};for(int i=0;i<12;++i){tick(s);check(s.sounding(0).stage==changed[i%4],"Auto responds to live pulse-count changes");}
    }
    // Foreground random/probability/accumulation cannot change the background RNG.
    for(int order=0;order<21;++order)for(int swing:{50,66,78}){
        metro::Sequencer s,reference;s.seed(128);reference.seed(128);
        for(auto* q:{&s,&reference}){q->settings.tracks[0].order=static_cast<metro::Order>(order);q->settings.tracks[0].swing=float(swing);q->settings.tracks[0].division=2;
            for(int i=0;i<8;++i){q->settings.stages[i].pitch=i/8.f;q->settings.stages[i].pulses=1+i%3;
                auto& edit=q->settings.tracks[0].stages[i];edit.skip=i==2;edit.accumulation=i%3;edit.ratchets=3;edit.probability=75;edit.accumTrigger=metro::AccumRatchet;}}
        for(int i=0;i<60;++i){
            s.loopy.stages=i>=5 && i<45?2u|32u:0u;s.loopy.pulses=5;
            for(auto* q:{&s,&reference})tick(*q);
            for(int frame=0;frame<500;++frame){for(auto* q:{&s,&reference})q->process(.0002,true,true,false,false);
                check(s.tracks[0].stage==reference.tracks[0].stage && s.tracks[0].pulse==reference.tracks[0].pulse,"background traversal keeps advancing");
                near(s.tracks[0].pitch,reference.tracks[0].pitch,"background accumulation/probability unchanged");
                if(!s.loopy.stages){near(s.sounding(0).pitch,reference.tracks[0].pitch,"release returns to original current pitch");near(s.sounding(0).gate,reference.tracks[0].gate,"release returns to original current gate");}
            }
        }
    }
    {
        metro::Sequencer s;tick(s);int position=s.tracks[0].stage;
        s.process(.001,false,true,false,false);s.loopy.stages=1u<<5;
        s.settings.scale=0;s.settings.tracks[0].stages[5].pitchOverride=24;s.settings.stages[5].gate=metro::Rest;s.settings.tracks[0].stages[5].gateOverride=0;
        s.settings.tracks[0].stages[5].cv=2.5f;
        s.settings.auxInputs[0].connected=true;s.settings.auxInputs[0].amount=1;s.settings.auxInputs[0].voltage=1;s.settings.auxInputs[0].destination=metro::ModPitchPre;
        s.process(.001,false,true,false,false);
        near(s.sounding(0).pitch,3,"Stage Player uses override and external pitch routing");near(s.sounding(0).gate,5,"audition ignores Rest switches/overrides");near(s.aux[0],2.5f,"audition CV reaches A output while stopped");
        s.settings.tracks[0].stages[5].pitchOverride=48;s.settings.tracks[0].stages[5].cv=-1;s.settings.auxInputs[0].voltage=0;
        s.process(.001,false,true,false,false);near(s.sounding(0).pitch,3,"held audition pitch is frozen");near(s.aux[0],2.5f,"held audition CV is frozen");check(s.tracks[0].stage==position,"audition preserves transport position");
        s.loopy.stages=0;s.process(.001,false,true,false,false);near(s.sounding(0).gate,0,"releasing audition drops gate");near(s.aux[0],0,"releasing audition drops CV");
        s.loopy.stages=1u<<5;s.process(.001,false,true,false,false);near(s.sounding(0).pitch,4,"fresh audition samples edited pitch");
        s.settings.tracks[0].mute=true;s.process(.001,false,true,false,false);near(s.sounding(0).gate,0,"mute silences audition");
        s.loopy.stages=0;s.settings.tracks[0].mute=false;s.resumeOnRun=true;tick(s);check(s.tracks[0].stage==(position+1)%8,"Resume run continues original position");
    }
    {
        metro::Sequencer s;s.settings.modLanes[0].values[0]=5;s.settings.modLanes[0].ramps[0]=true;tick(s);
        s.process(.01,true,true,false,false);double age=s.modulation.lanes[0].age;
        s.loopy.stages=2;
        for(int i=0;i<100;++i)s.process(.01,false,true,false,false);
        check(s.modulation.lanes[0].age==age,"Stage Player cannot advance paused MOD ramps");
        near(s.modulation.outputs[0],0,"paused MOD output remains silent during audition");
    }
    std::cout<<"PASS: "<<checks<<" Loopy and Stage Player checks\n";
}
