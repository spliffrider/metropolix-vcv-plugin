#include "../src/MidiLogic.hpp"
#include <array>
#include <vector>
#include <iostream>
#include <cstdlib>
unsigned checks=0;
void check(bool ok){++checks;if(!ok){std::cerr<<"MIDI assertion "<<checks<<" failed\n";std::exit(1);}}
int main(){
    using namespace metro;using Packet=std::array<int,4>;std::vector<Packet> messages;
    auto emit=[&](int s,int a,int b,int n){messages.push_back({{s,a,b,n}});};
    for(int ch=0;ch<=16;++ch)for(int key=0;key<128;++key)for(int velocity=0;velocity<128;++velocity){
        MidiVoice v;messages.clear();v.process(true,key,ch,velocity,1,false,emit);
        bool audible=ch && velocity>0;check(messages.size()==size_t(audible?1:0));
        if(audible){check(messages[0][0]==0x90+ch-1 && messages[0][1]==key && messages[0][2]==velocity);}
        v.process(true,key,ch,velocity,1,false,emit);check(messages.size()==size_t(audible?1:0));
        v.silence(emit);check(messages.size()==size_t(audible?2:0));if(audible)check(messages.back()[0]==0x80+ch-1 && messages.back()[1]==key);
        v.silence(emit);check(messages.size()==size_t(audible?2:0));
    }
    {MidiVoice v;messages.clear();v.process(true,60,1,127,1,false,emit);v.process(true,62,1,80,2,true,emit);check(messages.size()==3 && messages[1][0]==0x90 && messages[1][1]==62 && messages[2][0]==0x80 && messages[2][1]==60);v.process(true,62,2,80,3,true,emit);check(messages[3][0]==0x80 && messages[4][0]==0x91);v.process(false,62,2,80,3,false,emit);check(messages.back()[0]==0x81);}
    for(int v=-500;v<=500;++v){check(midiVoltage(float(v)*.01f,true)>=0 && midiVoltage(float(v)*.01f,true)<=127);check(midiVoltage(float(v)*.01f,false)>=0 && midiVoltage(float(v)*.01f,false)<=127);}
    check(midiVoltage(-5,true)==0 && midiVoltage(0,true)==64 && midiVoltage(5,true)==127);check(midiVoltage(0,false)==0 && midiVoltage(5,false)==127);check(midiNote(0)==60 && midiNote(1)==72 && midiNote(-10)==0 && midiNote(20)==127);
    {MidiClockInput input;for(int n=0;n<6000;++n){input.pulse();check(input.edge()==(n%6==0));}input.align();input.pulse();check(input.edge());input.align();for(int n=0;n<60;++n)input.pulse();for(int n=0;n<10;++n)check(input.edge());check(!input.edge());}
    for(int rate:{44100,48000,96000})for(int bpm:{20,60,120,240,400}){
        MidiClockOutput clock;int pulses=0;auto out=[&](int s,int,int,int n){check(s==0xf8 && n==1);++pulses;};
        for(int f=0;f<rate*10;++f)clock.process(1./rate,false,false,15./bpm,true,out);
        check(std::abs(pulses-bpm*4)<=1);int before=pulses;for(int f=0;f<rate;++f)clock.process(1./rate,false,false,15./bpm,false,out);check(pulses==before);
    }
    {MidiClockOutput clock;messages.clear();for(int f=0;f<6000;++f)clock.process(1./48000,f==0,true,.125,true,emit);check(messages.size()==6);for(int f=0;f<12000;++f)clock.process(1./48000,false,true,.125,true,emit);check(messages.size()==6);}
    #include "MidiMxTests.hpp"
    std::cout<<"PASS: MIDI channel/note lifecycle, MX notes/CC cleanup, CC range, clock phase and rates ("<<checks<<" assertions)\n";
}
