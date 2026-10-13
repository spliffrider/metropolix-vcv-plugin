void overrideRackTests() {
    using M=Metropolix;
    auto holder=std::unique_ptr<M>(new M);auto& m=*holder;
    Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    for(int k=0;k<2;++k)m.params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    int pitch=M::stageParam(0,2,M::PITCH_OVERRIDE),gate=M::stageParam(1,3,M::GATE_OVERRIDE);
    m.params[pitch].setValue(24);m.params[gate].setValue(0);m.params[M::gateScaleParam(0)].setValue(37);
    m.readSettings();require(m.sequencer.settings.tracks[0].stages[2].pitchOverride==24 && m.sequencer.settings.tracks[1].stages[3].gateOverride==0,"independent stage overrides reach engine");
    auto saved=m.capturePreset();m.putPreset(0,saved);
    json_t* data=m.dataToJson(),*params=m.paramsToJson();auto other=std::unique_ptr<M>(new M);
    other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);
    require(other->getPreset(0).values==saved.values && other->integer(pitch)==24 && other->integer(gate)==0 && other->integer(M::gateScaleParam(0))==37,"overrides and scale roundtrip in patch/bank");
    auto* slot=json_array_get(json_object_get(data,"presets"),0);
    while(json_array_size(slot)>size_t(M::TOTAL_PULSES+3))json_array_remove(slot,json_array_size(slot)-1);
    json_object_set_new(data,"schemaVersion",json_integer(12));json_object_set_new(data,"presetFormat",json_integer(2));
    other->dataFromJson(data);
    require(other->integer(pitch)==-1 && other->integer(gate)==-1 && other->integer(M::gateScaleParam(0))==100,"2.9 patch clears overrides over existing module");
    auto legacy=other->getPreset(0);require(legacy.occupied && legacy.values[M::presetIndex(pitch)]==-1 && legacy.values[M::presetIndex(M::gateScaleParam(0))]==100,"format 2 banks migrate with neutral overrides");
    json_object_set_new(data,"presetFormat",json_integer(1));while(json_array_size(slot)>size_t(M::TOTAL_PULSES+1))json_array_remove(slot,json_array_size(slot)-1);
    other->dataFromJson(data);json_decref(data);require(other->getPreset(0).occupied && other->getPreset(0).values[M::presetIndex(pitch)]==-1,"format 1 banks still migrate");
    other->params[M::SCALE].setValue(2);other->params[pitch].setValue(28);
    params=other->paramsToJson();other->paramsFromJson(params);json_decref(params);
    require(other->integer(pitch)==28 && other->getParamQuantity(pitch)->getDisplayValueString()=="D#2","Rack roundtrip retains chromatic anchor after changing scale");
    m.params[pitch].setValue(-1);m.params[gate].setValue(-1);m.params[M::gateScaleParam(0)].setValue(100);
    m.requestPreset(0);m.sendPresetCommand();m.process(args);
    require(m.integer(pitch)==24 && m.integer(gate)==0 && m.integer(M::gateScaleParam(0))==37,"recall applies overrides even when shared slider/switch load is Off");
    M::PresetRecall event;require(m.presetRecalls.pop(event),"override recall produces undo receipt");
    M::PresetCommand undo;undo.kind=M::PresetCommand::Restore;undo.snapshot=event.before;undo.other=event.after;
    m.presetCommands.push(undo);m.process(args);require(m.integer(pitch)==-1 && m.integer(gate)==-1 && m.integer(M::gateScaleParam(0))==100,"recall undo restores neutral overrides");
    auto* pq=m.getParamQuantity(pitch);pq->setDisplayValueString("E2");require(m.integer(pitch)==28 && pq->getDisplayValueString()=="E2","native numeric entry accepts note names");
    pq->setDisplayValueString("D#2");require(m.integer(pitch)==26,"numeric note editing quantizes to current scale");
    pq->setDisplayValueString("--");require(m.integer(pitch)==-1,"numeric entry removes pitch override");
    auto* gq=m.getParamQuantity(gate);gq->setDisplayValueString("Rest");require(m.integer(gate)==0 && gq->getDisplayValueString()=="Rest","numeric entry accepts Rest");gq->setDisplayValueString("hold");require(m.integer(gate)==100,"numeric entry accepts Hold");
    require(M::presetIndex(M::RUN_MODE)<0 && M::presetIndex(M::LOAD_PITCH)<0 && M::presetParam(M::FORMAT3_PARAMS-1)==M::LOOPY_PULSES-1,"format 3 prefix excludes preferences and retains all override fields");
    // A same-edge chain load must read its first stage's new overrides before output.
    saved=m.capturePreset();saved.values[M::presetIndex(M::stageParam(0,0,M::PITCH_OVERRIDE))]=12;saved.values[M::presetIndex(M::stageParam(0,0,M::GATE_OVERRIDE))]=0;
    m.putPreset(1,saved);auto state=m.getChainState();state.working.insert(0,metro::ChainLink{});state.working.links[0].slot=1;state.enabled=true;m.setChainState(state);m.sendChainCommand();m.params[M::RUN].setValue(1);m.process(args);
    require(m.outputs[M::PITCH_1].getVoltage()==1 && m.outputs[M::GATE_1].getVoltage()==0,"chain first pulse uses pitch/Rest overrides on load edge");
}
