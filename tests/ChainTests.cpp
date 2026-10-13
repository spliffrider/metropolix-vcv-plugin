#include "../src/Chains.hpp"
#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
int checks=0;
void require(bool ok,const char* message) {++checks;if(!ok){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
int main() {
    metro::PresetChain chain;
    for(int i=0;i<32;++i){metro::ChainLink l;l.slot=63-i;l.repeats=64;l.mutes=i%8;require(chain.insert(i,l),"all 32 links available");}
    require(!chain.insert(32),"33rd link rejected");
    require(chain.erase(0) && chain.links[0].slot==62 && chain.links[30].slot==32,"delete shifts following links in order");
    require(!chain.erase(-1) && !chain.erase(31),"out of bounds deletion rejected");
    metro::ChainLink l;l.slot=0;require(chain.insert(0,l) && chain.links[1].slot==62,"insert preserves later links");
    for(int repeats:{1,2,3,64})for(int total:{0,1,3,18,4096})for(int queue:{0,1,32,4096}) {
        chain=metro::PresetChain();l.repeats=repeats;chain.insert(0,l);chain.insert(1,l);
        metro::ChainPosition position;
        int duration=total?total:queue;
        const int limit=duration?duration*repeats*2+2:10000;
        for(int edge=0;edge<limit;++edge) {
            bool changed=position.advance(chain,total,queue);
            require(changed==(duration && edge>0 && edge%(duration*repeats)==0),"changes only after full link duration");
            require(position.link==(duration?(edge/(duration*repeats))%2:0),"loops at end of chain");
            require(position.repeat==(duration?(edge/duration)%repeats+1:1),"repeat count starts at one");
            position.tick();
        }
        position.reset();require(position.fresh && position.link==0 && position.repeat==1 && position.pulse==0,"reset returns Link 1");
    }
    // Fractional sample period: chain resets must preserve phase of the base
    // clock, even with skips, track divisions and accumulation triggers.
    for(int rate:{44100,48000,96000}) {
        metro::Sequencer s,reference;s.settings.bpm=reference.settings.bpm=137;
        chain=metro::PresetChain();chain.insert(0);l.repeats=2;chain.insert(1,l);
        metro::ChainPosition position;int edges=0;
        s.settings.tracks[0].division=3;s.settings.tracks[1].division=5;
        for(int stage=0;stage<8;++stage)s.settings.tracks[0].stages[stage].skip=stage%2;
        for(int sample=0;sample<rate*5;++sample) {
            reference.process(1.0/rate,true,false,false,false);
            s.process(1.0/rate,true,false,false,false,[&]{
                if(position.fresh || position.advance(chain,3,32))s.restartAtClock();
                position.tick();++edges;
            });
            require(s.clock==reference.clock,"chaining neither adds nor loses base-clock edges");
        }
        require(edges>40,"clock running over many transitions");
    }
    // Run/Pause resumes accumulated state; default Run/Stop still restarts.
    metro::Sequencer s;s.resumeOnRun=true;
    s.process(.01,true,true,true,false);s.process(.01,true,true,true,false);
    int stage=s.tracks[0].stage;
    s.process(.01,false,true,false,false);s.process(.01,true,true,false,false);
    require(s.tracks[0].stage==stage,"pause preserves position");
    s.process(.01,true,true,true,false);require(s.tracks[0].stage==stage+1,"next clock continues paused sequence");
    s.chainMutes=1;s.process(.01,true,true,true,false);require(s.tracks[0].gate==0 && s.tracks[1].gate==5,"per-link mute gates only assigned track");
    std::cout<<"PASS: "<<checks<<" chain assertions\n";
}
