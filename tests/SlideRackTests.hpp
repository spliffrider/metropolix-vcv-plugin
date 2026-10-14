void slideRackTests(){
    using M=Metropolix;auto m=std::unique_ptr<M>(new M),other=std::unique_ptr<M>(new M);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->params[M::slideTypeParam(0)].setValue(metro::AnalogSlide);m->params[M::slideTypeParam(1)].setValue(metro::AcidSlide);m->readSettings();
    require(m->sequencer.settings.tracks[0].slideType==metro::AnalogSlide && m->sequencer.settings.tracks[1].slideType==metro::AcidSlide,"per-track slide types reach actual engine");
    require(M::FORMAT4_PARAMS==460 && M::presetParam(458)==685 && M::presetParam(459)==686 && M::presetIndex(M::SCALE_LOCK)>=M::FORMAT9_PARAMS,"format 4 prefix still contains only the two added slide types");
    auto preset=m->capturePreset();m->putPreset(0,preset);auto* params=m->paramsToJson();auto* data=m->dataToJson();other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);
    require(other->integer(M::slideTypeParam(0))==1 && other->integer(M::slideTypeParam(1))==2 && other->getPreset(0).values==preset.values,"slide types and format 4 preset survive Rack roundtrip");
    auto* slot=json_array_get(json_object_get(data,"presets"),0);while(json_array_size(slot)>458)json_array_remove(slot,json_array_size(slot)-1);
    json_object_set_new(data,"schemaVersion",json_integer(15));json_object_set_new(data,"presetFormat",json_integer(3));other->dataFromJson(data);
    require(other->integer(M::slideTypeParam(0))==0 && other->integer(M::slideTypeParam(1))==0 && other->getPreset(0).occupied && other->getPreset(0).values[458]==0 && other->getPreset(0).values[459]==0,"old schema/format 3 patch preserves Tempo without rejecting older preset");json_decref(data);
    require(m->getParamQuantity(M::trackParam(0,M::SLIDE_TIME))->getUnit()=="%" && m->getParamQuantity(M::trackParam(0,M::SLIDE_TIME))->getDescription().find("ms per octave")!=std::string::npos,"Analog readout has correct units and useful rate description");
    m->params[M::slideTypeParam(0)].setValue(0);require(m->getParamQuantity(M::trackParam(0,M::SLIDE_TIME))->getUnit()=="% of a pulse","Tempo readout retains pulse units");
    m->params[M::slideTypeParam(0)].setValue(2);m->params[M::stageParam(0,0,M::SLIDE)].setValue(1);m->params[M::GATES].setValue(metro::Rest);m->params[M::stageParam(0,0,M::PITCH_OVERRIDE)].setValue(12);m->params[M::RUN].setValue(1);m->readSettings();
    Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;m->process(args);
    require(m->outputs[M::GATE_1].getVoltage()==5 && m->outputs[M::PITCH_1].getVoltage()==1,"Acid behaves as physical Hold even with original Rest switch at actual ports");
    m->params[M::stageParam(0,0,M::GATE_OVERRIDE)].setValue(0);m->readSettings();m->process(args);require(m->outputs[M::GATE_1].getVoltage()==0,"explicit stage Rest overrides Acid held gate");
}
