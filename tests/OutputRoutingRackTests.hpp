void outputRoutingRackTests(){
    using M=Metropolix;auto m=std::unique_ptr<M>(new M),other=std::unique_ptr<M>(new M);
    static_assert(M::OUTPUT_TRACK_OPTIONS==707 && M::FORMAT19_NUM_PARAMS==719 && M::FORMAT5_PARAMS==478 && M::FORMAT6_PARAMS==484,"routing appends track settings without moving legacy fields");
    require(m->integer(M::OUTPUT_SOURCE_A)==0 && m->integer(M::OUTPUT_SOURCE_B)==14,"new modules use manual CV/First Pulse defaults");
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);
    m->params[M::ctrlParam(0,M::CTRL_VALUE)].setValue(.4f);m->params[M::ctrlParam(1,M::CTRL_VALUE)].setValue(-.6f);
    m->params[M::ROOT].setValue(7);m->params[M::PITCH].setValue(.3f);m->params[M::PITCH+3].setValue(.8f);
    m->params[M::trackParam(1,M::OFFSET)].setValue(3);m->params[M::extraParam(0,M::PULSE_LIMIT)].setValue(4);m->params[M::extraParam(1,M::PULSE_LIMIT)].setValue(1);
    m->params[M::stageParam(0,0,M::CV)].setValue(2);m->params[M::stageParam(1,3,M::CV)].setValue(3);
    m->params[M::outputTrackParam(0,M::VELOCITY_SOURCE)].setValue(1);m->params[M::outputTrackParam(1,M::VELOCITY_SOURCE)].setValue(3);
    m->params[M::outputTrackParam(0,M::SLIDER_OCTAVES)].setValue(3);m->params[M::outputTrackParam(1,M::SLIDER_OCTAVES)].setValue(4);m->params[M::outputTrackParam(1,M::SLIDER_DIRECTION)].setValue(1);
    for(int lane=0;lane<8;++lane)m->params[M::modParam(lane,M::MOD_VALUE)].setValue(lane-4);
    m->params[M::RUN].setValue(1);m->readSettings();Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    const float expected[]={2,3,-4,-3,-2,-1,0,1,2,3,2,3.5f,-3,1,5,5,0,5,5,5,5,5,5,5,5,5,5,5,5,7.f/12,7.f/12,5,1.5f,4,.9f,.8f,3.75f,3};
    for(int source=0;source<38;++source){m->params[M::OUTPUT_SOURCE_A].setValue(source);m->params[M::OUTPUT_SOURCE_B].setValue(source);m->readSettings();m->process(args);
        require(std::abs(m->outputs[M::A_OUT].getVoltage()-expected[source])<1e-5f,"every source reaches real output A with its specified voltage");require(std::abs(m->outputs[M::B_OUT].getVoltage()-expected[source])<1e-5f,"every source reaches real output B with its specified voltage");}
    // Internal presets store new track options but keep cable function choices global.
    m->params[M::RUN].setValue(0);m->params[M::CLOCK_OUT_DIV].setValue(8);m->params[M::CLOCK_OUT_OFFSET].setValue(3);
    m->putPreset(0,m->capturePreset());m->params[M::outputTrackParam(0,M::SLIDER_OCTAVES)].setValue(1);m->params[M::outputTrackParam(1,M::VELOCITY_SOURCE)].setValue(0);
    m->params[M::OUTPUT_SOURCE_A].setValue(29);m->params[M::OUTPUT_SOURCE_B].setValue(25);m->requestPreset(0);m->sendPresetCommand();m->process(args);
    require(m->integer(M::outputTrackParam(0,M::SLIDER_OCTAVES))==3 && m->integer(M::outputTrackParam(1,M::VELOCITY_SOURCE))==3,"preset recalls velocity/range/direction");
    require(m->integer(M::OUTPUT_SOURCE_A)==29 && m->integer(M::OUTPUT_SOURCE_B)==25 && m->integer(M::CLOCK_OUT_DIV)==8,"preset retains global output functions and clock settings");
    json_t* params=m->paramsToJson(),*data=m->dataToJson();other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);
    require(other->integer(M::OUTPUT_SOURCE_A)==29 && other->integer(M::OUTPUT_SOURCE_B)==25 && other->getPreset(0).occupied && other->getPreset(0).values[478]==1 && other->getPreset(0).values[482]==4,"format6 and patch preserve all routing options");
    auto* old=json_array_get(json_object_get(data,"presets"),0);while(json_array_size(old)>478)json_array_remove(old,json_array_size(old)-1);
    json_object_set_new(data,"presetFormat",json_integer(5));json_object_set_new(data,"schemaVersion",json_integer(18));other->dataFromJson(data);json_decref(data);
    require(other->getPreset(0).occupied && other->getPreset(0).values[478]==2 && other->getPreset(0).values[479]==0 && other->getPreset(0).values[483]==0,"format5 retains its full prefix with default100% velocity and shared range");
    require(other->integer(M::OUTPUT_SOURCE_A)==0 && other->integer(M::OUTPUT_SOURCE_B)==1,"pre-routing schema supplies legacy defaults for absent new sources");
    // New controller destinations own the actual existing gate/range parameters.
    m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlSliderRange);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);m->params[M::ctrlParam(0,M::CTRL_VALUE)].setValue(1);
    m->params[M::ctrlParam(1,M::CTRL_DESTINATION)].setValue(metro::CtrlGateScale);m->params[M::ctrlParam(1,M::CTRL_ASSIGNMENT)].setValue(1);m->params[M::ctrlParam(1,M::CTRL_VALUE)].setValue(-1);m->readSettings();
    require(m->integer(M::outputTrackParam(0,M::SLIDER_OCTAVES))==4 && m->integer(M::gateScaleParam(1))==0 && m->ctrlOwner(M::gateScaleParam(1))==1,"CTRL Slider Range/Gate Scale reach owned parameters");
}
