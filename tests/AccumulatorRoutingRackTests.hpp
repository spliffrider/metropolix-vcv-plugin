void accumulatorRoutingRackTests(){
    using M=Metropolix;using namespace metro;
    auto m=std::unique_ptr<M>(new M);Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    m->params[M::RUN].setValue(1);m->params[M::SCALE].setValue(0);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);
    for(int t=0;t<2;++t){m->params[M::trackParam(t,M::LENGTH)].setValue(1);m->params[M::stageParam(t,0,M::ACCUMULATION)].setValue(3);m->params[M::trackParam(t,M::ACCUM_LIMIT)].setValue(7);m->params[M::accumParam(t,M::NEGATIVE_LIMIT)].setValue(4);}
    m->params[M::accumModeParam(1)].setValue(TrackAccumulation);m->inputs[M::CLOCK_IN].channels=1;
    auto tick=[&]{m->inputs[M::CLOCK_IN].setVoltage(0);for(int f=0;f<6000;++f)m->process(args);m->inputs[M::CLOCK_IN].setVoltage(5);m->process(args);};
    for(int value:{0,3,6,1,4,7,2,5}){tick();for(int t=0;t<2;++t)require(std::abs(m->outputs[M::PITCH_1+2*t].getVoltage()*12-value)<.0001f,"native Stage/Track forward accumulation appears at physical V/oct ports");}
    m->params[M::auxParam(0,M::AUX_DESTINATION)].setValue(ModAccumReverse);m->params[M::auxParam(0,M::AUX_ASSIGNMENT)].setValue(2);m->params[M::auxParam(0,M::AUX_AMOUNT)].setValue(1);m->inputs[M::AUX_X_IN].channels=1;m->inputs[M::AUX_X_IN].setVoltage(5);
    for(int value:{2,7,4,1,6,3}){tick();for(int t=0;t<2;++t)require(std::abs(m->outputs[M::PITCH_1+2*t].getVoltage()*12-value)<.0001f,"native AUX reverse retraces both actual pitch outputs within original polarity");}
    m->params[M::auxParam(0,M::AUX_DESTINATION)].setValue(ModAccumulate);m->inputs[M::AUX_X_IN].setVoltage(0);for(int p=0;p<4;++p){tick();for(int t=0;t<2;++t)require(std::abs(m->outputs[M::PITCH_1+2*t].getVoltage()*12-3)<.0001f && m->outputs[M::GATE_1+2*t].getVoltage()==5,"native Accumulate low freezes pitch while output gates keep pulsing");}
    m->params[M::auxParam(0,M::AUX_DESTINATION)].setValue(ModAccumReset);m->inputs[M::AUX_X_IN].setVoltage(5);tick();require(m->outputs[M::PITCH_1].getVoltage()==0 && m->outputs[M::PITCH_2].getVoltage()==0,"native accumulator trigger resets both banks at clock");tick();require(std::abs(m->outputs[M::PITCH_1].getVoltage()*12-3)<.0001f,"native held reset voltage does not retrigger");
    m->params[M::auxParam(0,M::AUX_DESTINATION)].setValue(ModNone);m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(CtrlAccumDir);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);m->params[M::ctrlParam(0,M::CTRL_VALUE)].setValue(0);
    tick();require(std::abs(m->outputs[M::PITCH_1].getVoltage()*12-3)<.0001f && std::abs(m->outputs[M::PITCH_2].getVoltage()*12-6)<.0001f,"native CTRL noon freezes only its assigned track");
    require(m->getParamQuantity(M::ctrlParam(0,M::CTRL_DESTINATION))->maxValue==29 && m->getParamQuantity(M::auxParam(0,M::AUX_DESTINATION))->maxValue>=18,"native destination metadata retains all accumulator routing IDs");
    for(int d=ModAccumulate;d<=ModAccumReset;++d){m->params[M::modParam(d-ModAccumulate,M::MOD_DESTINATION)].setValue(d);m->params[M::modParam(d-ModAccumulate,M::MOD_ASSIGNMENT)].setValue(d%3);}
    m->putPreset(0,m->capturePreset());json_t* params=m->paramsToJson(),*data=m->dataToJson();auto copy=std::unique_ptr<M>(new M);copy->paramsFromJson(params);copy->dataFromJson(data);json_decref(params);json_decref(data);
    for(int id=0;id<M::NUM_PARAMS;++id)require(m->params[id].getValue()==copy->params[id].getValue(),"new routing assignments survive native patch persistence with stable parameter IDs");
    require(copy->getPreset(0).occupied,"internal preset retains appended destination IDs without changing format7");
}
