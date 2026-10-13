void swingRackTests() {
    using M=Metropolix;
    auto holder=std::unique_ptr<M>(new M);auto& m=*holder;
    Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    for(int k=0;k<2;++k)m.params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m.params[M::swingParam(0)].setValue(66);m.params[M::swingParam(1)].setValue(75);m.readSettings();
    require(m.sequencer.settings.tracks[0].swing==66 && m.sequencer.settings.tracks[1].swing==75,"independent track swing reaches audio engine");
    auto saved=m.capturePreset();
    require(saved.values[M::presetIndex(M::swingParam(0))]==66 && saved.values[M::presetIndex(M::swingParam(1))]==75,"internal snapshot stores both swing values");
    m.putPreset(0,saved);
    json_t* data=m.dataToJson(),*params=m.paramsToJson();
    auto other=std::unique_ptr<M>(new M);other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);
    require(other->integer(M::swingParam(0))==66 && other->integer(M::swingParam(1))==75 && other->getPreset(0).values==saved.values,"swing patch and banks persist");
    json_object_set_new(data,"schemaVersion",json_integer(11));json_object_set_new(data,"presetFormat",json_integer(1));
    auto* slot=json_array_get(json_object_get(data,"presets"),0);
    while(json_array_size(slot)>size_t(M::TOTAL_PULSES+1))json_array_remove(slot,json_array_size(slot)-1);
    other->dataFromJson(data);json_decref(data);
    require(other->integer(M::swingParam(0))==50 && other->integer(M::swingParam(1))==50,"2.8 patch migration restores straight timing over existing module");
    require(other->getPreset(0).occupied && other->getPreset(0).values[M::presetIndex(M::swingParam(0))]==50,"old internal banks migrate with straight swing");
    m.params[M::swingParam(0)].setValue(50);m.params[M::swingParam(1)].setValue(50);
    m.requestPreset(0);m.sendPresetCommand();m.process(args);
    require(m.integer(M::swingParam(0))==66 && m.integer(M::swingParam(1))==75,"preset recall restores per-track swing");
    M::PresetRecall receipt;require(m.presetRecalls.pop(receipt),"swing recall produces undo receipt");
    M::PresetCommand undo;undo.kind=M::PresetCommand::Restore;undo.snapshot=receipt.before;undo.other=receipt.after;
    require(m.presetCommands.push(undo),"undo command accepted");m.process(args);
    require(m.integer(M::swingParam(0))==50 && m.integer(M::swingParam(1))==50,"preset undo restores prior swing");
    for(int target=0;target<3;++target)for(float value:{-1.f,0.f,1.f}) {
        m.params[M::swingParam(0)].setValue(51);m.params[M::swingParam(1)].setValue(51);
        m.params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlSwing);
        m.params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(target);
        m.params[M::ctrlParam(0,M::CTRL_VALUE)].setValue(value);m.readSettings();
        for(int t=0;t<2;++t)require(m.integer(M::swingParam(t))==(target==2 || target==t?int(metro::ctrlDirectValue(metro::CtrlSwing,value)):51),"CTRL swing direct range and track assignment");
        require(m.ctrlOwner(M::swingParam(target==2?0:target))==0,"CTRL-owned swing is locked on display/buttons");
    }
    m.params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m.params[M::RUN].setValue(1);m.params[M::TEMPO].setValue(120);
    // Same-edge chain loads must use the new swing on their first long pulse.
    auto chain=m.getChainState();chain.working.insert(0);chain.enabled=true;m.setChainState(chain);m.sendChainCommand();
    m.process(args);require(m.integer(M::swingParam(0))==66 && std::abs(m.sequencer.tracks[0].duration-.165)<.000001,"chain snapshot applies swing before its first note");
}
