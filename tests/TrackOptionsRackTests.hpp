void trackOptionsRackTests(){
    using M=Metropolix;auto m=std::unique_ptr<M>(new M),other=std::unique_ptr<M>(new M);
    static_assert(M::TRACK_BEHAVIOR==713 && M::FORMAT19_NUM_PARAMS==719 && M::FORMAT6_PARAMS==484 && M::FORMAT7_PARAMS==490,"new options append after frozen routing snapshot");
    require(m->getParamQuantity(M::trackParam(0,M::TRANSPOSE))->minValue==-60 && m->getParamQuantity(M::trackParam(0,M::TRANSPOSE))->maxValue==60,"native transpose covers hardware full range");
    m->params[M::RUN].setValue(1);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);
    m->params[M::trackBehaviorParam(0,M::PULSE_SOURCE)].setValue(3);m->params[M::trackBehaviorParam(0,M::GATE_SOURCE)].setValue(1);m->params[M::GATES].setValue(0);m->params[M::PULSES].setValue(8);
    m->inputs[M::CLOCK_IN].channels=1;Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    auto tick=[&]{m->inputs[M::CLOCK_IN].setVoltage(0);for(int f=0;f<100;++f)m->process(args);m->inputs[M::CLOCK_IN].setVoltage(5);m->process(args);};
    tick();require(m->outputs[M::GATE_1].getVoltage()==5 && m->outputs[M::GATE_2].getVoltage()==0,"one track overrides physical Rest while the other retains it");tick();tick();require(m->sequencer.tracks[0].stage==0,"uniform three-pulse source holds its stage");tick();require(m->sequencer.tracks[0].stage==1 && m->sequencer.tracks[1].stage==0 && m->integer(M::PULSES)==8,"track pulse sources are independent and leave switches intact");
    m->params[M::trackBehaviorParam(1,M::REST_PITCH)].setValue(1);m->putPreset(0,m->capturePreset());m->params[M::trackBehaviorParam(0,M::PULSE_SOURCE)].setValue(1);m->params[M::RUN].setValue(0);m->requestPreset(0);m->sendPresetCommand();m->process(args);
    require(m->integer(M::trackBehaviorParam(0,M::PULSE_SOURCE))==3 && m->integer(M::trackBehaviorParam(1,M::REST_PITCH))==1,"preset recalls appended track options");
    json_t* params=m->paramsToJson(),*data=m->dataToJson();other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);
    require(other->integer(M::trackBehaviorParam(0,M::PULSE_SOURCE))==3 && other->getPreset(0).values[489]==1,"native patch/bank preserves format7 options");
    json_t* slot=json_array_get(json_object_get(data,"presets"),0);while(json_array_size(slot)>484)json_array_remove(slot,json_array_size(slot)-1);
    json_object_set_new(data,"schemaVersion",json_integer(19));json_object_set_new(data,"presetFormat",json_integer(6));other->dataFromJson(data);json_decref(data);
    require(other->getPreset(0).occupied && other->getPreset(0).values[478]==2 && other->getPreset(0).values[484]==0 && other->getPreset(0).values[489]==0,"format6 prefix and default Switch/Hold behavior migrate intact");
}
