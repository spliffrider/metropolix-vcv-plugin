void workflowRackTests(){
    using M=Metropolix;auto m=std::unique_ptr<M>(new M);Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    static_assert(M::INIT_PRESET==727 && M::FORMAT21_NUM_PARAMS==728 && M::FORMAT8_PARAMS==492,"Init setting is global and preserves format8 snapshot");
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);
    m->params[M::PITCH].setValue(.77f);m->params[M::CLOCK_SOURCE].setValue(1);m->params[M::BOTTOM_PITCH].setValue(7);m->params[M::trackParam(0,M::TRANSPOSE)].setValue(45);m->params[M::stageParam(1,3,M::RATCHETS)].setValue(8);m->params[M::TRACK_GLOBAL_RESET_1].setValue(0);
    m->requestInitialize();m->sendPresetCommand();for(int f=0;f<64;++f)m->process(args);
    require(m->integer(M::trackParam(0,M::TRANSPOSE))==0 && m->integer(M::stageParam(1,3,M::RATCHETS))==1 && m->integer(M::TRACK_GLOBAL_RESET_1)==1,"default Init restores track/stage settings and per-track reset defaults");
    require(m->params[M::PITCH].getValue()==.77f && m->integer(M::CLOCK_SOURCE)==1 && m->integer(M::BOTTOM_PITCH)==7,"Init respects pitch recall policy and preserves global setup");
    M::PresetRecall receipt;require(m->presetRecalls.pop(receipt) && receipt.slot==-2 && receipt.before.values[M::presetIndex(M::trackParam(0,M::TRANSPOSE))]==45,"Init produces normal native undo receipt");
    m->params[M::trackParam(0,M::TRANSPOSE)].setValue(12);m->putPreset(15,m->capturePreset());m->params[M::INIT_PRESET].setValue(16);m->params[M::trackParam(0,M::TRANSPOSE)].setValue(33);m->requestInitialize();m->sendPresetCommand();for(int f=0;f<64;++f)m->process(args);require(m->integer(M::trackParam(0,M::TRANSPOSE))==12,"selected Init preset loads its musical settings");
    m->params[M::INIT_PRESET].setValue(64);m->requestInitialize();m->sendPresetCommand();for(int f=0;f<64;++f)m->process(args);require(m->integer(M::trackParam(0,M::TRANSPOSE))==0,"empty custom Init falls back to initialized pattern");
    json_t* params=m->paramsToJson(),*data=m->dataToJson();auto copy=std::unique_ptr<M>(new M);copy->paramsFromJson(params);copy->dataFromJson(data);require(copy->integer(M::INIT_PRESET)==64 && copy->getPreset(15).occupied && copy->workflowClipboard.empty(),"global Init preference/bank persist while clipboard remains transient");json_object_set_new(data,"schemaVersion",json_integer(23));copy->paramsFromJson(params);copy->dataFromJson(data);require(copy->integer(M::INIT_PRESET)==0,"older patches migrate with default Init preference");json_decref(params);json_decref(data);
}
