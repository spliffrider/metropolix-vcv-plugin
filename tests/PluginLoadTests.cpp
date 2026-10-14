#include <dlfcn.h>
#include <rack.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

void require(bool ok,const char* message){if(!ok){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
int main(int argc,char** argv){
    require(argc==2,"production binary path supplied");
    void* binary=dlopen(argv[1],RTLD_NOW|RTLD_LOCAL);if(!binary){std::cerr<<dlerror()<<'\n';return 1;}
    auto init=reinterpret_cast<void(*)(rack::plugin::Plugin*)>(dlsym(binary,"init"));require(init,"production init exported");
    rack::Context context;rack::contextSet(&context);context.engine=new rack::engine::Engine;
    // Keep binary loaded until process exit: registered factory vtables belong
    // to it. All modules are destroyed before the host/plugin is released.
    rack::plugin::Plugin plugin;plugin.slug="MetropolixLab";init(&plugin);
    require(plugin.models.size()==2,"production registers exactly two public models");
    bool mainFound=false,mxFound=false;
    for(auto* model:plugin.models){
        auto* module=model->createModule();require(module,"production factory constructs module");
        if(model->slug=="Metropolix"){
            mainFound=true;require(module->params.size()==770 && module->outputs.size()==7,"main production parameter/port identities");
            module->params[1].setValue(1);rack::engine::Module::ProcessArgs args{};args.sampleRate=48000;args.sampleTime=1.f/48000;bool gate=false;
            for(int frame=0;frame<48000;++frame){args.frame=frame;module->process(args);for(auto& out:module->outputs)require(std::isfinite(out.getVoltage()),"finite production output");gate |= module->outputs[1].getVoltage()>=1.5f;}
            require(gate,"actual production sequencer generates a gate signal");
        }else if(model->slug=="MX"){
            mxFound=true;require(module->outputs.size()==8,"MX production output identities");rack::engine::Module::ProcessArgs args{};args.sampleRate=48000;args.sampleTime=1.f/48000;module->process(args);for(auto& out:module->outputs)require(out.getVoltage()==0,"unlinked production MX is low");
        }else require(false,"only public production model slugs");
        json_t* saved=module->toJson();require(saved && json_is_array(json_object_get(saved,"params")),"production module serializes");module->fromJson(saved);json_decref(saved);delete module;
    }
    require(mainFound && mxFound,"both production models tested");
    plugin.models.clear();delete context.engine;context.engine=nullptr;rack::contextSet(nullptr);
    std::cout<<"PASS: stripped production binary loads, registers both models, generates finite gates and roundtrips state\n";
}
