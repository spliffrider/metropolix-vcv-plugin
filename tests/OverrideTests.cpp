#include "../src/Sequencer.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

int checks=0;
void check(bool ok,const std::string& message) {++checks;if(!ok){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
void near(float a,float b,const std::string& message){check(std::abs(a-b)<.0001f,message);}
void tick(metro::Sequencer& s){s.process(.00001,true,true,true,false);}
void render(metro::Sequencer& s){s.process(0,true,true,false,false);}
int main() {
    // Manual p83's four waveform rows, sampled independently of the timing code.
    for(int row=0;row<4;++row)for(int stage=0;stage<4;++stage) {
        metro::Sequencer s;auto& track=s.settings.tracks[0];
        track.gateLength=row==3?.75f:.5f;track.gateScale=row>=2?.5f:1.f;
        track.stages[1].ratchets=2;
        if(row>0){track.stages[2].gateOverride=13;track.stages[3].gateOverride=88;}
        tick(s);auto& state=s.tracks[0];state.stage=stage;state.duration=1.;state.allowed=true;
        float length=stage==2 && row>0?.13f:stage==3 && row>0?.88f:row==3?.75f:.5f;
        if(row>=2)length*=.5f;
        for(int sample=0;sample<1000;++sample) {
            double phase=(sample+.37)/1000.;state.age=phase;render(s);
            double ratchetPhase=stage==1?std::fmod(phase*2.,1.):phase;
            check((state.gate>0)==(ratchetPhase<length),"p83 track/override/ratchet/scale waveform");
        }
    }
    for(int trigger=0;trigger<3;++trigger) {
        // p91: Rest and Hold overrides cannot alter any cumulative transposition.
        metro::Sequencer normal,rest,hold;
        for(auto* s:{&normal,&rest,&hold}) {
            s->settings.stages[0].pulses=3;s->settings.tracks[0].length=2;
            s->settings.tracks[0].stages[0].ratchets=3;
            s->settings.tracks[0].stages[0].accumulation=1;
            s->settings.tracks[0].stages[0].accumTrigger=static_cast<metro::AccumTrigger>(trigger);
        }
        rest.settings.tracks[0].stages[0].gateOverride=0;hold.settings.tracks[0].stages[0].gateOverride=100;
        for(int cycle=0;cycle<6;++cycle) {
            for(auto* s:{&normal,&rest,&hold})tick(*s);
            for(int sample=0;sample<12000;++sample) {
                for(auto* s:{&normal,&rest,&hold})s->process(1./48000,true,true,false,false);
                near(normal.tracks[0].pitch,rest.tracks[0].pitch,"Rest override preserves accumulation/pitch");
                near(normal.tracks[0].pitch,hold.tracks[0].pitch,"Hold override preserves accumulation/pitch");
                if(rest.tracks[0].stage==0)near(rest.tracks[0].gate,0,"override Rest is silent");
            }
        }
    }
    for(int scale=0;scale<7;++scale)for(int note=0;note<=60;++note) {
        metro::Sequencer s;s.settings.scale=scale;s.settings.stages[0].pitch=.13f;
        s.settings.tracks[0].stages[0].pitchOverride=note;tick(s);
        int base=metro::nearestDegree(float(note),scale);
        near(s.tracks[0].pitch,metro::degreeToSemitone(base,scale)/12.f,"override is quantized in the current scale");
        s.settings.stages[0].pitch=.91f;s.settings.octaves=1;render(s);
        near(s.tracks[0].pitch,metro::degreeToSemitone(base,scale)/12.f,"override ignores slider/range changes");
        s.tracks[0].activeAccumulation=2;render(s);
        near(s.tracks[0].pitch,metro::degreeToSemitone(base+2,scale)/12.f,"override accumulates in scale degrees");
        s.settings.root=3;s.settings.tracks[0].transpose=12;render(s);
        near(s.tracks[0].pitch,(metro::degreeToSemitone(base+2,scale)+15)/12.f,"override retains root and track transposition");
    }
    {
        metro::Sequencer s;s.settings.tracks[0].stages[0].pitchOverride=24;s.settings.tracks[1].stages[0].pitchOverride=7;tick(s);
        near(s.tracks[0].pitch,2,"track 1 override");near(s.tracks[1].pitch,7.f/12,"track 2 override");
        s.settings.auxInputs[0].connected=true;s.settings.auxInputs[0].amount=1;s.settings.auxInputs[0].voltage=1.f/12;s.settings.auxInputs[0].destination=metro::ModPitchPre;s.settings.scale=0;render(s);near(s.tracks[0].pitch,25.f/12,"pitch pre applies before override quantization");
        s.settings.auxInputs[1].connected=true;s.settings.auxInputs[1].amount=1;s.settings.auxInputs[1].voltage=1.f/12;s.settings.auxInputs[1].destination=metro::ModPitchPost;render(s);near(s.tracks[0].pitch,26.f/12,"pitch post applies after quantization");
        s.settings.tracks[0].stages[0].pitchOverride=-1;render(s);near(s.tracks[0].pitch,2.f/12,"removing override restores slider");
    }
    {
        metro::Sequencer s;tick(s);auto& state=s.tracks[0];state.stage=0;state.allowed=true;state.duration=1;
        auto& track=s.settings.tracks[0];track.stages[0].gateOverride=100;s.settings.stages[0].pulses=3;
        for(int pulse=0;pulse<3;++pulse)for(double phase:{0.,.5,.999999}){state.pulse=pulse;state.age=phase;render(s);near(state.gate,5,"100 percent stays high for entire stage");}
        track.gateScale=.5f;state.pulse=0;state.age=.49;render(s);near(state.gate,5,"Gate Scale scales Hold override");
        state.age=.51;render(s);near(state.gate,0,"scaled Hold returns to pulse gating");
        s.settings.stages[0].gate=metro::Rest;state.age=0;render(s);near(state.gate,0,"override does not bypass physical Rest switch");
        s.settings.stages[0].gate=metro::Multi;track.stages[0].gateOverride=-1;track.gateScale=1;track.gateLength=1;state.age=.999999;render(s);near(state.gate,5,"track Hold endpoint");
        track.mute=true;render(s);near(state.gate,0,"mute suppresses held gates");
        track.mute=false;track.gateScale=0;render(s);near(state.gate,0,"zero Gate Scale silences length Hold");
    }
    {
        metro::Sequencer s;s.settings.tracks[0].stages[0].skip=true;s.settings.tracks[0].stages[0].pitchOverride=60;s.settings.tracks[0].stages[1].pitchOverride=12;
        tick(s);check(s.tracks[0].stage==1,"skipped override consumes no clock");near(s.tracks[0].pitch,1,"skipped pitch override never sounds");
        s.reset();tick(s);near(s.tracks[0].pitch,1,"reset retains programmed overrides");
    }
    {
        metro::Sequencer s;auto& track=s.settings.tracks[0];track.gateLength=.01f;
        auto& aux=s.settings.auxInputs[0];aux.connected=true;aux.amount=1;aux.voltage=-.025f;aux.destination=metro::ModGateLength;
        tick(s);s.tracks[0].duration=1;s.tracks[0].age=.008;render(s);near(s.tracks[0].gate,5,"old patches preserve positive modulation minimum");
        track.gateScale=.5f;s.tracks[0].age=.008;render(s);near(s.tracks[0].gate,0,"scaling occurs after inherited minimum");
        track.gateScale=1;track.gateLength=1;aux.voltage=-1;s.tracks[0].age=.9;render(s);near(s.tracks[0].gate,0,"negative modulation reduces track length Hold");
        track.stages[0].gateOverride=100;render(s);near(s.tracks[0].gate,5,"assigned Hold overrides inherited length modulation");
    }
    std::cout<<"PASS: "<<checks<<" stage override checks\n";
}
