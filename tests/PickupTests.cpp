#include "../src/Pickup.hpp"
#include <iostream>
#include <cstdlib>
using namespace metro;
int checks=0;
void require(bool ok,const char* message){++checks;if(!ok){std::cerr<<"FAIL "<<message<<'\n';std::exit(1);}}
int main(){
    for(int start=0;start<=100;++start)for(int target=0;target<=100;++target){
        float p=start*.01f,s=target*.01f;Pickup pickup;pickup.recall(p,s);
        for(int i=0;i<20;++i)require(pickup.process(p,true,true)==(pickup.active?s:p),"stationary controls never cancel recall");
        if(start!=target){int dir=start<target?1:-1;for(int n=start+dir;n!=target+dir;n+=dir){float raw=n*.01f,out=pickup.process(raw,true,true);require(std::abs(out-(n==target?raw:s))<1e-6,"approach retains saved value until exact pickup");}require(!pickup.active,"reaching saved value releases pickup");}
        require(pickup.process(.37f,true,true)==.37f,"picked-up control follows arbitrary movements");
        pickup.recall(p,s);require(pickup.process(p,true,false)==p && !pickup.active,"Off releases recalled value to physical control");
        pickup.recall(p,s);require(pickup.process(p,false,true)==(pickup.active?s:p),"changing to Jump retains sound until movement");
        float jump=p==1?0:1;require(pickup.process(jump,false,true)==jump && !pickup.active,"Jump releases on any movement");
    }
    for(float min:{0.f,-1.f})for(int target=1;target<100;++target){float s=min+target*.01f;Pickup p;p.recall(min,s);require(p.process(min+.002f,true,true)==s,"subtarget movement retains saved value");require(p.process(min+1,true,true)==min+1 && !p.active,"overshooting target still picks up");p.recall(min+1,s);require(p.process(min,true,true)==min && !p.active,"reverse overshoot picks up");p.recall(min,s);p.recall(min+1,min+.4f);require(p.process(min+.5f,true,true)==min+.4f && p.active,"another recall replaces earlier target and direction");}
    std::cout<<"PASS: PickupTests "<<checks<<" checks\n";
}
