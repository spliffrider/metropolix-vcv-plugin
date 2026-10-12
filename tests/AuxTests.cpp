#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>

int checks=0;
void check(bool value,const char* label) {
    ++checks;
    if(!value){std::cerr<<"FAIL: "<<label<<'\n';std::exit(1);}
}
void near(float value,float expected,const char* label) { check(std::abs(value-expected)<.0001f,label); }
void tick(metro::Sequencer& seq) { seq.process(.002,true,true,true,false); }

int main() {
    using namespace metro;
    {
        AuxInput input;input.connected=true;input.voltage=10;input.amount=.25f;
        near(input.effectiveVoltage(),2.5f,"attenuation happens before 5V saturation");
        input.amount=-.25f;near(input.effectiveVoltage(),-2.5f,"negative amount inverts continuous CV");
        input.voltage=-10;near(input.effectiveVoltage(),2.5f,"negative input and amount produce positive CV");
        input.amount=1;near(input.effectiveVoltage(),-5,"continuous voltage bounded to -5V");
        input.voltage=10;near(input.effectiveVoltage(),5,"continuous voltage bounded to +5V");
        input.amount=0;near(input.effectiveVoltage(),0,"noon removes modulation");
        input.amount=1;input.connected=false;near(input.effectiveVoltage(),0,"unplugged continuous CV is zero");
        input.destination=ModMute;
        near(input.effectiveVoltage(),5,"unpatched clockwise gate acts as manual high");
        input.amount=-1;near(input.effectiveVoltage(),0,"unpatched counterclockwise gate acts as manual low");
        input.amount=0;near(input.effectiveVoltage(),0,"gate at noon is neutral");
        input.connected=true;
        for(float amount:{.01f,1.f,-.01f,-1.f}) {
            input.amount=amount;input.voltage=1.49f;
            near(input.effectiveVoltage(),amount>0?0:5,"low gate uses logical inversion independent of gain magnitude");
            input.voltage=1.5f;
            near(input.effectiveVoltage(),amount>0?5:0,"gate threshold includes exactly 1.5V");
            input.voltage=-5;
            near(input.effectiveVoltage(),amount>0?0:5,"negative gate voltage counts as low");
        }
        input.voltage=std::numeric_limits<float>::infinity();near(input.effectiveVoltage(),0,"nonfinite patched voltage is ignored");
        input.voltage=1;input.amount=std::numeric_limits<float>::quiet_NaN();near(input.effectiveVoltage(),0,"nonfinite amount is ignored");
    }
    {
        Modulation mod;std::array<ModLane,8> lanes;std::array<AuxInput,3> aux;
        for(int i=0;i<3;++i){aux[i].connected=true;aux[i].amount=1;aux[i].voltage=float(i+1);aux[i].assignment=i;aux[i].destination=ModPitchOffset;}
        mod.render(lanes,.001,aux);
        near(mod.routes[0][ModPitchOffset],4,"X targets track 1, Z targets both");
        near(mod.routes[1][ModPitchOffset],5,"Y targets track 2, Z targets both");
        for(auto& input:aux)input.destination=ModNone;
        for(int i=0;i<2;++i){lanes[i].values.fill(4);lanes[i].destination=ModPitchOffset;}
        aux[0].destination=ModPitchOffset;aux[0].voltage=-5;
        mod.tick(lanes,.125);mod.render(lanes,.001,aux);
        near(mod.routes[0][ModPitchOffset],3,"AUX cancels MOD before the combined destination is clamped");
        near(mod.outputs[0],4,"AUX leaves MOD output voltage untouched");
        aux[0].connected=false;mod.render(lanes,.001,aux);
        near(mod.routes[0][ModPitchOffset],5,"disconnect restores saturated MOD sum");
        lanes[0].mute=true;lanes[1].mute=true;mod.render(lanes,.001,aux);
        near(mod.routes[0][ModPitchOffset],0,"removed sources do not leave stale modulation");
    }
    {
        Sequencer seq;auto& aux=seq.settings.auxInputs[0];aux.connected=true;aux.amount=1;aux.voltage=1.9f;aux.destination=ModOctave;
        tick(seq);near(seq.tracks[0].pitch,1,"AUX octave uses whole volt steps");near(seq.tracks[1].pitch,0,"AUX track isolation");
        aux.voltage=1.f/12;aux.destination=ModPitchPre;seq.reset();tick(seq);near(seq.tracks[0].pitch,0,"AUX pre pitch passes through scale quantizer");
        aux.destination=ModPitchPost;seq.reset();tick(seq);near(seq.tracks[0].pitch,1.f/12,"AUX post pitch adds chromatic semitone");
        aux.destination=ModPitchOffset;aux.voltage=.03f;seq.reset();tick(seq);near(seq.tracks[0].pitch,.03f,"AUX offset preserves fractional semitone");
        aux.voltage=.07f;seq.process(.001,true,true,false,false);near(seq.tracks[0].pitch,.07f,"AUX pitch updates between clock edges");
        aux.connected=false;seq.process(.001,true,true,false,false);near(seq.tracks[0].pitch,0,"unplugging removes continuous pitch offset immediately");
        aux.connected=true;aux.voltage=-5;aux.destination=ModProbability;seq.reset();tick(seq);near(seq.tracks[0].gate,0,"AUX probability can suppress a stage");
        near(seq.settings.tracks[0].stages[0].probability,100,"AUX leaves saved probability unchanged");
        aux.destination=ModGateLength;seq.reset();tick(seq);near(seq.tracks[0].gate,0,"AUX negative gate length reaches zero");
        aux.destination=ModMute;aux.voltage=5;seq.reset();tick(seq);near(seq.tracks[0].gate,0,"AUX high gate mutes track");
        seq.settings.tracks[0].mute=true;seq.reset();tick(seq);near(seq.tracks[0].gate,5,"AUX high gate reverses programmed mute");
        aux.amount=-1;seq.reset();tick(seq);near(seq.tracks[0].gate,0,"inverted high gate restores programmed mute");
        seq.process(.001,false,true,false,false);near(seq.tracks[0].gate,0,"stop still silences output with AUX active");
        near(seq.modulation.routes[0][ModMute],0,"stop clears effective routes");
    }
    // Every supported destination follows the same established destination
    // semantics as MOD, for each input/target and at multiple audio rates.
    for(int rate:{44100,48000,96000})for(int dest=1;dest<ModDestinationCount;++dest)for(int input=0;input<3;++input)for(int target=0;target<3;++target) {
        Sequencer a,b;float voltage=dest==ModMute?5.f:dest==ModProbability?-3.f:1.25f;
        auto& aux=a.settings.auxInputs[input];aux.connected=true;aux.voltage=voltage;aux.amount=1;aux.destination=static_cast<ModDestination>(dest);aux.assignment=target;
        auto& lane=b.settings.modLanes[0];lane.values.fill(voltage);lane.destination=aux.destination;lane.assignment=target;
        for(int sample=0;sample<rate/8;++sample){a.process(1.0/rate,true,false,false,false);b.process(1.0/rate,true,false,false,false);
            if(sample%127==0)for(int track=0;track<2;++track){near(a.tracks[track].pitch,b.tracks[track].pitch,"AUX/MOD destination pitch agrees");near(a.tracks[track].gate,b.tracks[track].gate,"AUX/MOD destination timing agrees");}}
    }
    {
        Sequencer seq;auto& aux=seq.settings.auxInputs[2];aux.connected=true;aux.voltage=5;aux.amount=1;aux.destination=ModRatchet;
        int rises=0;bool last=false;
        for(int i=0;i<6000;++i){seq.process(1.0/48000,true,false,false,false);bool high=seq.tracks[0].gate>0;if(high&&!last)++rises;last=high;}
        check(rises==8,"AUX ratchet produces eight actual gates per pulse");
        check(seq.settings.tracks[0].stages[0].ratchets==1,"AUX ratchet does not overwrite programmed value");
    }
    std::cout<<"PASS: "<<checks<<" AUX voltage, gate, destination, timing and MOD interaction checks\n";
}
