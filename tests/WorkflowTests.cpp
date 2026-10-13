#include "../src/Workflows.hpp"
#include <cstdlib>
#include <iostream>
int checks=0;
void require(bool ok,const char* message){++checks;if(!ok){std::cerr<<"FAIL "<<message<<'\n';std::exit(1);}}
int main(){
    metro::RandomSource random;random.state=77;
    for(int options=2;options<=100;++options){metro::WorkflowRange range;range.low=-13;range.high=range.low+options-1;range.toggle=options==2;float base=(range.low+range.high)*.5f;if(range.discrete)base=std::round(base);int changes=0;
        for(int n=0;n<10000;++n){float full=metro::workflowRandom(base,range,random,false),morph=metro::workflowRandom(base,range,random,true);require(full>=range.low && full<=range.high && full==std::round(full),"randomization respects discrete native parameter bounds");require(morph>=range.low && morph<=range.high && morph==std::round(morph),"morph respects bounds and native units");require(std::abs(morph-base)<=std::max(1.f,(range.high-range.low)*.1f),"morph stays within manual step or ten-percent limit");changes+=morph!=base;}
        if(options<=10)require(changes>1700 && changes<2300,"small-option morph has twenty-percent change rate");else require(changes==10000,"large-option morph moves by at least one unit away from an interior value");
    }
    metro::WorkflowRange cv;cv.low=-5;cv.high=5;cv.unit=.001f;cv.discrete=false;
    for(int n=0;n<100000;++n){float v=metro::workflowRandom(0,cv,random,true);require(std::abs(v)>=.000999f && std::abs(v)<=1.00001f,"continuous CV morph uses millivolt minimum and ten-percent range maximum");}
    cv.low=0;cv.high=5;cv.toggle=true;
    for(int n=0;n<10000;++n){float v=metro::workflowRandom(5,cv,random,false);require(v==0 || v==5,"gate randomization emits only logical Off/On endpoints");}
    metro::WorkflowRange eight;eight.low=1;eight.high=8;int counts[8]={};
    for(int n=0;n<80000;++n)++counts[int(metro::workflowRandom(4,eight,random,false))-1];
    for(int n:counts)require(n>9600 && n<10400,"full randomization samples endpoints and interior discrete options equally");
    std::cout<<"PASS: WorkflowTests "<<checks<<" checks\n";
}
