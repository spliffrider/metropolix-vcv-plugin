#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
int checks=0;
void require(bool ok,const char* message){++checks;if(!ok){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
void tick(metro::Sequencer& s){s.process(.002,true,true,true,false);}
void gate(metro::Sequencer& s,metro::ModDestination destination,float voltage,int track=0){auto& input=s.settings.auxInputs[0];input.connected=true;input.amount=1;input.destination=destination;input.assignment=track;input.voltage=voltage;}
int main(){
    using namespace metro;
    // Reverse is the inverse of wrapping through the original polarity domain,
    // including negative source increments, rather than changing the domain.
    for(int positive=1;positive<=28;++positive)for(int negative=1;negative<=28;++negative)for(bool bipolar:{false,true})for(int amount=-28;amount<=28;++amount){
        if(!amount)continue;
        int lo=bipolar || amount<0?-negative:0,hi=bipolar || amount>0?positive:0;
        for(int value=lo;value<=hi;++value){RandomSource rng;int dir=1;int forward=advanceAccumulator(value,amount,positive,negative,bipolar,AccumWrap,dir,rng);int reverse=advanceAccumulator(forward,amount,positive,negative,bipolar,AccumWrap,dir,rng,true);require(reverse==value,"Wrap reverse retraces every legal accumulator value without changing polarity");}
    }
    for(int mode=0;mode<2;++mode){
        Sequencer s;s.settings.tracks[0].length=1;s.settings.tracks[0].accumMode=AccumMode(mode);s.settings.tracks[0].stages[0].accumulation=3;s.settings.tracks[0].accumLimit=7;
        const int forward[]={0,3,6,1,4,7,2,5};for(int expected:forward){tick(s);require(s.tracks[0].activeAccumulation==expected,"manual p158 forward wrap offsets");}
        gate(s,ModAccumReverse,5);const int reversed[]={2,7,4,1,6,3};for(int expected:reversed){tick(s);require(s.tracks[0].activeAccumulation==expected,"manual p158 reverse walks from the last sounding pitch");}
        gate(s,ModAccumulate,0);int held=s.tracks[0].activeAccumulation;for(int p=0;p<8;++p){tick(s);require(s.tracks[0].activeAccumulation==held,"assigned low Accumulate retains last emitted pitch, not queued next offset");}
        gate(s,ModAccumulate,5);tick(s);require(s.tracks[0].activeAccumulation!=held,"high Accumulate resumes its pending counter");
        s.settings.controls[0].destination=CtrlAccumDir;s.settings.controls[0].value=0;held=s.tracks[0].activeAccumulation;for(int p=0;p<8;++p){tick(s);require(s.tracks[0].activeAccumulation==held,"CTRL Accum Dir noon freezes both modes");}
        s.settings.controls[0].value=-1;s.settings.auxInputs[0].destination=ModNone;tick(s);require(s.tracks[0].activeAccumulation!=held,"CTRL negative resumes reverse");
        s.settings.controls[0].destination=CtrlNone;gate(s,ModAccumReset,5);int stage=s.tracks[0].stage,played=s.tracks[0].activeAccumulation;
        s.process(.002,true,true,false,false);require(s.tracks[0].stage==stage && s.tracks[0].activeAccumulation==played,"accumulator reset queues off-clock without changing sequence or pitch");tick(s);require(s.tracks[0].activeAccumulation==0 && s.tracks[0].stage==stage,"accumulator reset applies on next track pulse");tick(s);require(s.tracks[0].activeAccumulation==3,"held reset gate does not retrigger");
    }
    {
        Sequencer s;s.settings.tracks[0].length=1;s.settings.tracks[0].stages[0].accumulation=3;gate(s,ModAccumInvert,5);
        const int expected[]={0,-3,-6,-9};for(int value:expected){tick(s);require(s.tracks[0].activeAccumulation==value,"Invert changes sign and uses negative unipolar domain");}
    }
    for(int mode=0;mode<2;++mode){
        Sequencer s;auto& t=s.settings.tracks[0];t.length=1;t.accumMode=AccumMode(mode);t.stages[0].accumulation=3;t.accumLimit=7;t.accumNegativeLimit=4;
        const int forward[]={0,3,6,1,4,7,2,5,0,3,6};for(int value:forward){tick(s);require(s.tracks[0].activeAccumulation==value,"p158 literal pre-Invert phrase");}
        gate(s,ModAccumInvert,5);const int inverted[]={-2,0,-3,-1,-4,-2,0};for(int value:inverted){tick(s);require(s.tracks[0].activeAccumulation==value,"p158 Invert starts from the last sounding offset and wraps into the new domain");}
        gate(s,ModAccumInvert,0);for(int value:{3,6,1,4,7,2}){tick(s);require(s.tracks[0].activeAccumulation==value,"p158 Invert low resumes positive accumulation");}
    }
    for(int mode=0;mode<2;++mode)for(int origin=1;origin<17;++origin){
        Sequencer s;auto& t=s.settings.tracks[0];t.length=1;t.accumMode=AccumMode(mode);t.stages[0].accumulation=3;t.accumOrder=AccumPendulum;t.accumLimit=7;
        int history[18];for(int p=0;p<=origin;++p){tick(s);history[p]=s.tracks[0].activeAccumulation;}
        gate(s,ModAccumReverse,5);for(int p=origin-1;p>=0;--p){tick(s);require(s.tracks[0].activeAccumulation==history[p],"Pendulum reversal retraces pitches across reflected boundaries");}
    }
    for(auto destination:{ModAccumulate,ModAccumReverse,ModAccumInvert,ModAccumMode,ModAccumPolarity,ModAccumReset}){
        AuxInput a;a.destination=destination;a.connected=true;
        for(float voltage:{-5.f,0.f,1.499f,1.5f,5.f})for(float amount:{-1.f,-.01f,0.f,.01f,1.f}){a.voltage=voltage;a.amount=amount;float expected=amount==0?0:((voltage>=1.5f)!=(amount<0))?5:0;require(a.effectiveVoltage()==expected,"all accumulator gate/trigger inputs use threshold and logical attenuation inversion");}
        a.connected=false;a.amount=1;require(a.effectiveVoltage()==5,"unpatched accumulator gate CW is manual high");a.amount=-1;require(a.effectiveVoltage()==0,"unpatched accumulator gate CCW is manual low");
        Modulation mod;std::array<ModLane,8> lanes;auto& lane=lanes[0];lane.destination=destination;lane.assignment=1;lane.values[0]=5;lane.ramps[0]=true;mod.tick(lanes,.125);mod.render(lanes,.001);
        require(mod.routes[1][destination]==5 && mod.routes[0][destination]==0 && mod.assigned[1][destination],"internal lane routes target only assigned track and ignore gate ramps");lane.mute=true;mod.render(lanes,.001);require(mod.routes[1][destination]==0 && mod.assigned[1][destination],"muted assigned gate retains its assignment presence");
    }
    for(int mode=0;mode<2;++mode)for(int trigger=0;trigger<3;++trigger)for(int order=0;order<21;++order){
        Sequencer s,reference;for(auto* q:{&s,&reference}){q->seed(391);auto& t=q->settings.tracks[0];t.accumMode=AccumMode(mode);t.order=Order(order);t.accumLimit=28;t.accumNegativeLimit=28;t.accumBipolar=true;
            for(int st=0;st<8;++st){q->settings.stages[st].pulses=1+st%3;t.stages[st].accumulation=1+st%3;t.stages[st].accumTrigger=AccumTrigger(trigger);t.stages[st].ratchets=3;t.stages[st].skip=st==2 || st==6;}}
        gate(s,ModAccumulate,0);
        for(int p=0;p<24;++p){tick(s);tick(reference);for(int f=0;f<30;++f){s.process(.002,true,true,false,false);reference.process(.002,true,true,false,false);require(s.tracks[0].stage==reference.tracks[0].stage && s.tracks[0].pulse==reference.tracks[0].pulse && s.tracks[0].gate==reference.tracks[0].gate,"accumulation pause preserves multi-pulse rhythm, ratchets and all playback orders with skips");require(s.tracks[0].activeAccumulation==0,"paused triggers never advance an initially neutral bank");}}
        gate(s,ModAccumulate,5);for(int p=0;p<10;++p){tick(s);for(int f=0;f<60;++f)s.process(.001,true,true,false,false);}require(s.tracks[0].trackAccumulator!=0 || s.tracks[0].accumulator!=std::array<int,8>{},"high gate enables accumulation after paused musical traversal");
    }
    for(int rate:{44100,48000,96000})for(int mode=0;mode<2;++mode){
        Sequencer s;s.settings.tracks[0].length=2;s.settings.tracks[0].division=4;s.settings.tracks[0].accumLimit=28;s.settings.tracks[0].accumMode=AccumMode(mode);s.settings.tracks[0].stages[0].accumulation=3;s.settings.tracks[0].stages[1].accumulation=2;
        s.settings.tracks[1].length=1;s.settings.tracks[1].accumLimit=28;s.settings.tracks[1].stages[0].accumulation=1;
        auto clock=[&]{for(int f=0;f<rate/8;++f)s.process(1.0/rate,true,true,false,false);s.process(1.0/rate,true,true,true,false);};for(int p=0;p<17;++p)clock();int stage=s.tracks[0].stage,played=s.tracks[0].activeAccumulation;require(played>0,"divided track is accumulated before reset test");
        gate(s,ModAccumReset,5);s.process(1.0/rate,true,true,false,false);gate(s,ModAccumReset,0);s.process(1.0/rate,true,true,false,false);
        for(int p=0;p<3;++p){clock();require(s.tracks[0].stage==stage && s.tracks[0].activeAccumulation==played,"brief off-clock reset waits through divider instead of truncating stage");}
        clock();require(s.tracks[0].stage!=stage && s.tracks[0].activeAccumulation==0,"queued trigger resets accumulator on next divided track pulse without restarting stage order");require(s.tracks[1].activeAccumulation==20,"other track accumulator continues without resetting during targeted reset");
    }
    {
        Sequencer s;s.settings.tracks[0].length=1;s.settings.tracks[0].stages[0].accumulation=3;for(int p=0;p<4;++p)tick(s);int held=s.tracks[0].activeAccumulation;
        auto& lane=s.settings.modLanes[0];lane.destination=ModAccumulate;lane.mute=true;tick(s);require(s.tracks[0].activeAccumulation==held,"muted Accumulate lane is an assigned low gate and pauses");lane.destination=ModNone;tick(s);require(s.tracks[0].activeAccumulation!=held,"removing the final Accumulate assignment restores default accumulation");
    }
    for(int mode=0;mode<2;++mode)for(int order=0;order<4;++order){
        Sequencer s;s.seed(523);auto& t=s.settings.tracks[0];t.length=1;t.accumMode=AccumMode(mode);t.accumOrder=AccumOrder(order);t.accumLimit=7;t.accumNegativeLimit=4;t.stages[0].accumulation=3;
        for(int p=0;p<120;++p){gate(s,ModAccumPolarity,p%12<6?5:0);tick(s);require(s.tracks[0].activeAccumulation<=7 && s.tracks[0].activeAccumulation>=(p%12<6?-4:0),"live polarity changes respect active bounds on their first sounding event");}
        s.loopy.stages=1;s.loopy.targets=1;for(int p=0;p<4;++p)tick(s);require(s.loopPlaying[0],"foreground loop active before accumulator-only reset");int stage=s.tracks[0].stage;
        gate(s,ModAccumReset,5);tick(s);require(s.tracks[0].stage==stage && s.loopPlaying[0] && s.sounding(0).activeAccumulation==0,"targeted reset retains active Loopy and background traversal while clearing both banks");
    }
    {
        RandomSource random;random.state=7319;int forward=0,backward=0,stay=0;
        for(int n=0;n<50000;++n){int dir=1;int next=advanceAccumulator(0,4,28,28,true,AccumRandom,dir,random,true);if(next>0)++forward;else if(next<0)++backward;else ++stay;require(next>=-4 && next<=4,"reversed Random stays within the programmed maximum increment");}
        require(forward>14300 && forward<15700 && backward>24200 && backward<25800 && stay>9400 && stay<10600,"reverse Random swaps directional bias while retaining its 20 percent hold probability");
    }
    {
        Sequencer s;s.settings.tracks[0].length=1;s.settings.tracks[0].stages[0].accumulation=3;
        gate(s,ModAccumMode,5);tick(s);require(s.tracks[0].accumMode==TrackAccumulation && s.settings.tracks[0].accumMode==StageAccumulation,"Mode toggles effective bank without rewriting base");
        gate(s,ModAccumMode,0);tick(s);require(s.tracks[0].accumMode==StageAccumulation,"Mode low restores base bank");
        gate(s,ModAccumPolarity,5);for(int p=0;p<12;++p)tick(s);require(s.tracks[0].accumulator[0]>=-9 && s.tracks[0].accumulator[0]<=9 && !s.settings.tracks[0].accumBipolar,"Polarity toggles effective bounds without rewriting base");
    }
    std::cout<<"PASS: AccumulatorRoutingTests "<<checks<<" checks\n";
}
