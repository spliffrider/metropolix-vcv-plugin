#include "../src/Mx.hpp"
void mxRackTests(){
    using M=Metropolix;auto instance=std::unique_ptr<Metropolix>(new Metropolix);auto& main=*instance;Mx child;main.id=820;child.id=821;main.model=modelMetropolix;child.model=modelMX;
    main.rightExpander.module=&child;child.leftExpander.module=&main;Module::ProcessArgs args{};args.sampleRate=48000;args.sampleTime=1.f/48000;
    auto frame=[&]{main.process(args);child.process(args);if(child.leftExpander.messageFlipRequested){std::swap(child.leftExpander.producerMessage,child.leftExpander.consumerMessage);child.leftExpander.messageFlipRequested=false;}};
    auto edge=[&]{main.inputs[M::CLOCK_IN].setVoltage(0);for(int f=0;f<60;++f)frame();main.inputs[M::CLOCK_IN].setVoltage(5);frame();frame();};
    main.inputs[M::CLOCK_IN].channels=1;main.params[M::CLOCK_SOURCE].setValue(2);main.params[M::RUN].setValue(1);main.readSettings();
    for(int f=0;f<60;++f)frame();edge();require(child.outputs[0].getVoltage()==5 && child.outputs[2].getVoltage()==5,"MX transport reaches actual expander output with one-sample latency");
    main.params[M::MX_MODE].setValue(metro::MxStageNum);main.params[M::GATES].setValue(metro::Rest);main.params[M::trackParam(0,M::GATE_LENGTH)].setValue(0);main.params[M::stageParam(0,0,M::PROBABILITY)].setValue(0);main.sequencer.reset();edge();
    require(main.outputs[M::GATE_1].getVoltage()==0 && child.outputs[0].getVoltage()==5,"MX stage gate independent of rest, gate length and note probability");
    main.params[M::trackParam(0,M::MUTE)].setValue(1);for(int f=0;f<64;++f)frame();require(child.outputs[0].getVoltage()==0,"MX track mute silences stage gate");
    main.params[M::trackParam(0,M::MUTE)].setValue(0);main.params[M::RUN].setValue(0);for(int f=0;f<64;++f)frame();for(auto& out:child.outputs)require(out.getVoltage()==0,"MX stopped track has no stage gate");
    main.selectedTrack.store(0);main.loopyCommand.store(1u<<4);for(int f=0;f<64;++f)frame();require(child.outputs[4].getVoltage()==5,"MX stopped Loopy Stage Player follows stage button");
    main.loopyCommand.store(0);for(int f=0;f<64;++f)frame();require(child.outputs[4].getVoltage()==0,"MX stage player release drops gate");
    main.params[M::RUN].setValue(1);main.params[M::trackParam(1,M::OFFSET)].setValue(3);main.inputs[M::AUX_X_IN].channels=1;main.inputs[M::AUX_X_IN].setVoltage(5);main.params[M::auxParam(0,M::AUX_AMOUNT)].setValue(1);main.params[M::auxParam(0,M::AUX_DESTINATION)].setValue(metro::ModOutputSwap);main.sequencer.reset();edge();require(child.outputs[3].getVoltage()==5 && child.outputs[0].getVoltage()==0,"MX physical track assignment follows output swap");
    main.inputs[M::AUX_X_IN].setVoltage(0);main.params[M::auxParam(0,M::AUX_DESTINATION)].setValue(0);for(int f=0;f<64;++f)frame();require(child.outputs[0].getVoltage()==5,"MX swap release restores original track assignment");
    auto index=main.mx.index;main.heldTrackReset.store(1);main.params[M::RESET].setValue(1);for(int f=0;f<64;++f)frame();require(main.mx.index==index,"targeted track reset preserves global MX clock divider phase");main.heldTrackReset.store(0);main.params[M::RESET].setValue(0);for(int f=0;f<64;++f)frame();
    main.params[M::MX_MODE].setValue(metro::MxModStageNum);main.params[M::MX_LANE].setValue(7);main.params[M::RUN].setValue(1);main.sequencer.reset();edge();require(child.outputs[0].getVoltage()==5,"MX selected MOD lane stage gate");
    main.params[M::modParam(7,M::MOD_MUTE)].setValue(1);for(int f=0;f<64;++f)frame();require(child.outputs[0].getVoltage()==0,"MX MOD mute silences stage gate");
    main.params[M::MX_MODE].setValue(metro::MxTransportA);main.params[M::RUN].setValue(0);main.params[M::CV_OUT_DELAY].setValue(2);for(int f=0;f<150;++f)frame();main.params[M::RUN].setValue(1);for(int f=0;f<98;++f){frame();require(child.outputs[0].getVoltage()==(f>=97?5.f:0.f),"MX shared 2ms output delay plus one-sample message latency is exact");}
    main.params[M::MX_TRACK].setValue(100);main.params[M::MX_LANE].setValue(-100);frame();require(std::isfinite(child.outputs[0].getVoltage()),"MX bounds malformed assignment indexes safely");main.params[M::MX_TRACK].setValue(0);main.params[M::MX_LANE].setValue(7);
    main.params[M::MX_MODE].setValue(metro::MxTransportA);main.params[M::CV_OUT_DELAY].setValue(2);for(int f=0;f<150;++f)frame();child.leftExpander.module=nullptr;child.process(args);for(auto& out:child.outputs)require(out.getVoltage()==0,"MX detach immediately silences delayed outputs");
    child.leftExpander.module=&main;main.rightExpander.module=nullptr;child.process(args);for(auto& out:child.outputs)require(out.getVoltage()==0,"MX refuses stale nonreciprocal adjacency");
    child.leftExpander.module=nullptr;
    main.params[M::MX_MODE].setValue(6);main.params[M::MX_LANE].setValue(7);json_t* data=main.dataToJson(),*parameters=main.paramsToJson();auto restoredInstance=std::unique_ptr<Metropolix>(new Metropolix);auto& restored=*restoredInstance;restored.paramsFromJson(parameters);restored.dataFromJson(data);json_decref(data);json_decref(parameters);require(restored.integer(M::MX_MODE)==6 && restored.integer(M::MX_LANE)==7,"MX settings persist in Rack patch");
    json_t* old=json_pack("{s:i}","schemaVersion",27);restored.dataFromJson(old);json_decref(old);require(restored.integer(M::MX_MODE)==0 && restored.integer(M::MX_LANE)==0,"old Rack patch defaults MX safely");
    static_assert(M::FORMAT24_NUM_PARAMS==767 && M::FORMAT31_NUM_PARAMS==770 && M::FORMAT32_NUM_PARAMS==786 && M::FORMAT34_NUM_PARAMS==787 && M::NUM_PARAMS==789 && M::FORMAT9_PARAMS==518 && M::FORMAT10_PARAMS==521 && M::SNAPSHOT_PARAMS==522,"global additions preserve MX identities and musical bank format");
    std::cout<<"PASS: Rack MX double buffer, signals, mutes, Stage Player, persistence and detach\n";
}
