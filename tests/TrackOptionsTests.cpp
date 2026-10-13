#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
int checks=0;
void require(bool value,const char* message){++checks;if(!value){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
void near(float a,float b,const char* message){require(std::abs(a-b)<.00001f,message);}
void tick(metro::Sequencer& s){s.process(.00002,true,true,true,false);}
metro::Gate effectiveGate(int choice,metro::Gate physical){return choice==0 || (choice%2==0 && physical==metro::Rest)?physical:choice<=2?metro::Single:choice<=4?metro::Multi:metro::Hold;}
int main(){
    using namespace metro;
    // Uniform sources must sound like programming the corresponding physical
    // switches, while retaining all skip/accumulation/traversal machinery.
    for(int order=0;order<21;++order)for(int pulse=0;pulse<=8;++pulse)for(int gate=0;gate<=6;++gate){
        Sequencer s,reference;for(auto* q:{&s,&reference}){q->seed(125);auto& t=q->settings.tracks[0];t.order=Order(order);t.restPitchUpdate=order%2;t.gateStretch=order%2;t.ratchetMode=RatchetMode(order%3);t.pulseLimit=13;t.autoAccumReset=order%2;
            for(int st=0;st<8;++st){q->settings.stages[st].pulses=1+st;q->settings.stages[st].pitch=st/8.f;q->settings.stages[st].gate=Gate(st%4);t.stages[st].skip=st==2;t.stages[st].accumulation=st%3;t.stages[st].ratchets=1+st%4;}}
        s.settings.tracks[0].pulseSource=pulse;s.settings.tracks[0].gateSource=gate;
        for(auto& stage:reference.settings.stages){if(pulse)stage.pulses=pulse;stage.gate=effectiveGate(gate,stage.gate);}
        for(int p=0;p<27;++p){tick(s);tick(reference);for(int frame=0;frame<35;++frame){s.process(.002,true,true,false,false);reference.process(.002,true,true,false,false);
            require(s.tracks[0].stage==reference.tracks[0].stage && s.tracks[0].pulse==reference.tracks[0].pulse,"source overrides preserve traversal and pulse grid");near(s.tracks[0].gate,reference.tracks[0].gate,"source overrides match physical gate programming");near(s.tracks[0].pitch,reference.tracks[0].pitch,"source overrides match physical pitch/accumulation behavior");}}
        for(int st=0;st<8;++st)require(s.settings.stages[st].pulses==st+1 && s.settings.stages[st].gate==Gate(st%4),"track source leaves shared physical switches unchanged");
    }
    for(bool update:{false,true})for(int type=0;type<3;++type){
        Sequencer s;s.settings.scale=0;s.settings.octaves=1;s.settings.stages[0].pitch=0;s.settings.stages[1].pitch=1;
        auto& t=s.settings.tracks[0];t.length=2;t.restPitchUpdate=update;t.slideType=SlideType(type);s.settings.stages[1].gate=Rest;
        tick(s);tick(s);near(s.tracks[0].pitch,update?1:0,"rest pitch Hold/Update is independent of its silent gate");near(s.tracks[0].gate,0,"rest remains silent under either pitch mode");
    }
    {
        Sequencer s;s.settings.tracks[0].pulseSource=3;s.settings.stages[0].pulses=8;s.settings.controls[0].destination=CtrlPulseCount;s.settings.controls[0].value=1.f/7;
        for(int p=0;p<12;++p){tick(s);require(s.tracks[0].stage==p/4,"CTRL pulse offset applies after uniform track source");}
        s.settings.tracks[0].pulseSource=0;s.settings.controls[0].value=0;s.reset();for(int p=0;p<8;++p){tick(s);require(s.tracks[0].stage==0,"Switch restores untouched eight-pulse physical stage");}
    }
    for(int type=0;type<2;++type)for(bool update:{false,true}){
        Sequencer s;s.settings.scale=0;s.settings.octaves=1;s.settings.stages[1].pitch=1;s.settings.stages[1].gate=Rest;
        auto& t=s.settings.tracks[0];t.length=2;t.slideType=SlideType(type);t.slideTime=.5f;t.restPitchUpdate=update;t.stages[1].slide=true;
        tick(s);for(int frame=0;frame<125;++frame)s.process(.001,true,true,false,false);tick(s);for(int frame=0;frame<20;++frame)s.process(.001,true,true,false,false);
        require(update?s.tracks[0].pitch>0 && s.tracks[0].pitch<1:s.tracks[0].pitch==0,"Tempo/Analog ghost slides move Rest Update pitch while Hold retains its note");near(s.tracks[0].gate,0,"ghost slide has no output gate");
    }
    for(int transpose=-60;transpose<=60;++transpose){Sequencer s;s.settings.scale=0;s.settings.tracks[0].transpose=transpose;tick(s);near(s.tracks[0].pitch,transpose/12.f,"full hardware transpose range reaches V/oct output");}
    std::cout<<"PASS: TrackOptionsTests "<<checks<<" checks\n";
}
