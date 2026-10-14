void loopyRackTests(){
    using M=Metropolix;auto ptr=std::unique_ptr<M>(new M);auto& m=*ptr;
    Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    for(int k=0;k<2;++k)m.params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m.params[M::LOOPY_PULSES].setValue(5);m.params[M::LOOPY_TARGET].setValue(1);
    require(M::FORMAT3_PARAMS==458 && M::presetIndex(M::LOOPY_PULSES)==M::FORMAT9_PARAMS+1 && M::presetIndex(M::LOOPY_TARGET)==M::FORMAT9_PARAMS+2,"Loopy settings append without moving the old snapshot prefix");
    auto snapshot=m.capturePreset();m.params[M::LOOPY_PULSES].setValue(7);m.putPreset(0,snapshot);m.requestPreset(0);m.sendPresetCommand();m.process(args);
    require(m.integer(M::LOOPY_PULSES)==5 && m.integer(M::LOOPY_TARGET)==1,"preset recall restores both stored Loopy settings");
    m.params[M::stageParam(0,4,M::PITCH_OVERRIDE)].setValue(24);m.params[M::stageParam(0,4,M::CV)].setValue(2);
    m.readSettings();m.pressLoopyStage(4);m.process(args);require(m.outputs[M::GATE_1].getVoltage()==5 && m.outputs[M::PITCH_1].getVoltage()==2 && m.outputs[M::A_OUT].getVoltage()==2,"stopped audition reaches actual pitch/gate/A cables");
    m.releaseLoopyStage(4);m.process(args);require(m.outputs[M::GATE_1].getVoltage()==0 && m.outputs[M::A_OUT].getVoltage()==0,"mouse release drops actual output gates/CV");
    m.params[M::RUN].setValue(1);m.pressLoopyStage(2);m.process(args);
    require(m.sequencer.sounding(0).stage==2 && m.sequencer.sounding(1).stage==2,"both-track request starts on clock");m.latchLoopy();m.releaseLoopyStage(2);
    require((m.loopyCommand.load()&255u)==4u,"latched loop survives releasing held stage");
    m.pressLoopyStage(5);m.releaseLoopyStage(5);require(m.loopyLatched.load() && (m.loopyCommand.load()&255u)==32u,"changing a latched loop preserves its latch");
    auto other=std::unique_ptr<M>(new M);json_t* params=m.paramsToJson(),*data=m.dataToJson();other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);json_decref(data);
    require(other->integer(M::LOOPY_PULSES)==5 && other->integer(M::LOOPY_TARGET)==1 && other->loopyCommand.load()==0 && !other->loopyLatched.load(),"global settings persist while gestures/latches clear on reopen");
    m.clearLoopy();m.process(args);require(m.sequencer.sounding(0).stage==m.sequencer.tracks[0].stage,"release restores underlying track output");
    m.muteLoopy(0);m.process(args);require(m.integer(M::trackParam(0,M::MUTE))==0,"running mute waits for clock boundary");
    for(int i=0;i<7000;++i)m.process(args);require(m.integer(M::trackParam(0,M::MUTE))==1,"performance mute is applied on next pulse");
    m.params[M::RUN].setValue(0);m.muteLoopy(0);m.muteLoopy(5);m.process(args);require(m.integer(M::trackParam(0,M::MUTE))==0 && m.integer(M::modParam(3,M::MOD_MUTE))==1,"stopped track/lane mutes apply immediately");
    auto saved=m.capturePreset();require(saved.values[M::presetIndex(M::modParam(3,M::MOD_MUTE))]==1,"performance mutes are stored in presets");
    data=json_pack("{s:i}","schemaVersion",13);other->dataFromJson(data);json_decref(data);require(other->integer(M::LOOPY_PULSES)==0 && other->integer(M::LOOPY_TARGET)==0,"old patches reset added global preferences neutrally");
    other->getParamQuantity(M::LOOPY_PULSES)->setDisplayValueString("Auto");require(other->getParamQuantity(M::LOOPY_PULSES)->getDisplayValueString()=="Auto","native popup accepts Auto");
}
