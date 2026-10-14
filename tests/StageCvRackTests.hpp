void stageCvRackTests(){
    using M=Metropolix;auto m=std::unique_ptr<M>(new M),other=std::unique_ptr<M>(new M);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);
    m->params[M::cvTypeParam(0)].setValue(metro::UnipolarCv);m->params[M::cvTypeParam(1)].setValue(metro::GateCv);
    for(int s=0;s<8;++s){m->params[M::cvRampParam(0,s)].setValue(s%3);m->params[M::cvRampParam(1,s)].setValue((s+1)%3);}
    m->readSettings();require(m->sequencer.settings.tracks[0].cvType==metro::UnipolarCv && m->sequencer.settings.tracks[1].cvType==metro::GateCv && m->sequencer.settings.tracks[0].stages[5].cvRamp==metro::CvPulseRamp,"CV type/transition reach actual engine independently");
    require(M::FORMAT5_PARAMS==478 && M::presetParam(460)==687 && M::presetParam(477)==704 && M::presetIndex(704)==477 && M::presetIndex(M::LOOPY_PULSES)>=M::FORMAT9_PARAMS,"format 5 prefix still contains only CV types and sixteen transitions");
    auto preset=m->capturePreset();m->putPreset(0,preset);json_t* data=m->dataToJson();json_t* params=m->paramsToJson();other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);
    require(other->integer(M::cvTypeParam(0))==1 && other->integer(M::cvTypeParam(1))==2 && other->getPreset(0).values==preset.values,"CV models and format 5 preset roundtrip");
    auto* slot=json_array_get(json_object_get(data,"presets"),0);while(json_array_size(slot)>460)json_array_remove(slot,json_array_size(slot)-1);
    json_object_set_new(data,"presetFormat",json_integer(4));json_object_set_new(data,"schemaVersion",json_integer(16));other->dataFromJson(data);json_decref(data);
    require(other->integer(M::cvTypeParam(0))==0 && other->integer(M::cvRampParam(1,7))==0 && other->getPreset(0).occupied,"legacy schema/format 4 retains Bipolar Step while accepting slide presets");
    for(int i=460;i<478;++i)require(other->getPreset(0).values[i]==0,"old preset CV fields have neutral defaults");
    auto* cv=m->getParamQuantity(M::stageParam(0,0,M::CV));m->params[M::stageParam(0,0,M::CV)].setValue(-4);require(cv->getValue()==0 && cv->getMinValue()==0 && m->params[M::stageParam(0,0,M::CV)].getValue()==-4,"unipolar editing displays effective zero and preserves stored negative voltage");
    m->params[M::cvTypeParam(0)].setValue(0);require(cv->getValue()==-4,"return to Bipolar restores underlying voltage");
    m->params[M::cvTypeParam(0)].setValue(2);cv->setDisplayValueString("On");require(cv->getValue()==5 && cv->getDisplayValueString()=="On" && cv->getUnit().empty(),"gate numeric entry and active readout use On/Off");
    cv->setDisplayValueString("Off");require(cv->getValue()==0,"gate Off entry");
    m->params[M::RUN].setValue(1);m->params[M::cvTypeParam(0)].setValue(0);m->params[M::OUTPUT_SOURCE_A].setValue(0);m->params[M::OUTPUT_SOURCE_B].setValue(1);
    m->params[M::stageParam(0,0,M::CV)].setValue(-3);m->params[M::stageParam(1,0,M::CV)].setValue(3);m->readSettings();
    Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;m->process(args);
    require(m->outputs[M::A_OUT].getVoltage()==-3 && m->outputs[M::B_OUT].getVoltage()==5,"typed track CV reaches both real assignable output ports");
    m->params[M::stageParam(0,0,M::CV)].setValue(3);m->params[M::cvRampParam(0,0)].setValue(1);m->sequencer.reset();m->readSettings();
    for(int i=0;i<3000;++i)m->process(args);require(std::abs(m->outputs[M::A_OUT].getVoltage()-1.5f)<.002,"whole-stage ramp traverses actual Rack output");
}
