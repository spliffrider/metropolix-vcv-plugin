#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
using namespace metro;
int checks=0;
void require(bool ok,const char* message){++checks;if(!ok){std::cerr<<"FAIL "<<message<<'\n';std::exit(1);}}
void tick(Sequencer& s,bool run=true){s.process(.002,run,true,false,false);s.process(.002,run,true,true,false);}
int main(){
    for(int rate:{44100,48000,96000})for(bool always:{false,true}){
        Sequencer s;s.settings.clockRunningOnly=!always;int clocks=0;bool high=false;
        for(int f=0;f<rate*2;++f){s.process(1.0/rate,false,false,false,false);bool now=s.clock>0;if(now && !high)++clocks;high=now;require(s.tracks[0].stage==-1 && s.tracks[1].stage==-1 && s.modulation.lanes[0].stage==-1,"stopped clock never plays tracks or MOD");}
        require(clocks==(always?16:0),"Always clocks continue at the selected tempo while stopped");
    }
    for(int div=1;div<=256;++div)for(int shift:{0,div-1}){
        Sequencer s;s.settings.clockRunningOnly=false;s.settings.clockOutDivision=div;s.settings.clockOutOffset=shift;
        for(int p=0;p<520;++p){tick(s,false);require((s.clock>0)==(p%div==shift),"stopped external Always clock honors division and offset");require(s.tracks[0].stage==-1,"clock-only operation does not create notes");}
    }
    for(int order=0;order<21;++order)for(int protect=0;protect<2;++protect){
        Sequencer s;s.seed(45);s.settings.tracks[protect].globalReset=false;
        for(int t=0;t<2;++t){s.settings.tracks[t].order=Order(order);s.settings.tracks[t].division=t+1;for(int st=0;st<8;++st){s.settings.stages[st].pitch=st/7.f;s.settings.tracks[t].stages[st].accumulation=1;}}
        for(int p=0;p<17;++p)tick(s);
        Sequencer reference=s;s.process(.002,true,true,false,true);reference.process(.002,true,true,false,false);
        require(s.tracks[protect].stage==reference.tracks[protect].stage && s.tracks[protect].pitch==reference.tracks[protect].pitch,"Global Reset Off retains current stage and pitch");
        require(s.tracks[1-protect].stage==-1,"Global Reset On resets the other track");
        for(int p=0;p<32;++p){tick(s);tick(reference);require(s.tracks[protect].stage==reference.tracks[protect].stage && s.tracks[protect].pulse==reference.tracks[protect].pulse && s.tracks[protect].pitch==reference.tracks[protect].pitch,"protected track preserves traversal, divider and accumulator after reset");}
        s.manualResetForce=1u<<protect;s.process(.002,true,true,false,true);require(s.tracks[protect].stage==-1,"held track force bypasses Global Reset Off");
    }
    for(bool only:{false,true}){Sequencer s;s.settings.resetRunningOnly=only;s.settings.outputSources[0]=outputSource(OutReset);s.process(.002,false,true,false,true);require((s.aux[0]>0)==!only,"global reset output honors stopped output policy");}
    {Sequencer s;s.settings.tracks[0].globalReset=false;auto& input=s.settings.auxInputs[0];input.connected=true;input.amount=1;input.destination=ModReset;input.voltage=5;tick(s);for(int p=0;p<5;++p)tick(s);int stage=s.tracks[0].stage;s.process(.002,true,true,false,true);require(s.tracks[0].stage==stage,"protected global reset does not rearm an already-held targeted reset gate");}
    {Sequencer s;s.settings.bottomPitch=0;s.settings.scale=0;auto& input=s.settings.auxInputs[0];input.connected=true;input.amount=1;input.destination=ModRoot;input.voltage=1;tick(s);require(s.tracks[0].pitch==1,"fixed bottom anchor does not cancel Root performance transposition");}
    for(int scale=0;scale<49;++scale)for(int root=0;root<12;++root)for(int bottom=0;bottom<12;++bottom)for(int octaves=1;octaves<=4;++octaves){
        Sequencer s;s.settings.scale=scale;s.settings.root=root;s.settings.bottomPitch=bottom;s.settings.octaves=octaves;s.settings.tracks[0].length=1;
        auto notes=scaleAt(scale);int wanted=bottom;bool found=false;for(;wanted<bottom+12;++wanted){for(int i=0;i<notes.count;++i)if(modulo(wanted-root,12)==notes.notes[i])found=true;if(found)break;}
        tick(s);require(std::abs(s.tracks[0].pitch-wanted/12.f)<1e-5,"fixed bottom pitch chooses first in-scale note at or above requested note");
        s.settings.stages[0].pitch=1;tick(s);require(std::abs(s.tracks[0].pitch-(wanted+12*octaves)/12.f)<1e-5,"fixed bottom pitch preserves octave range at top slider endpoint");
        s.settings.bottomPitch=12;s.settings.stages[0].pitch=0;tick(s);require(std::abs(s.tracks[0].pitch-root/12.f)<1e-5,"Root restores original bottom pitch behavior");
    }
    std::cout<<"PASS: ClockConfigTests "<<checks<<" checks\n";
}
