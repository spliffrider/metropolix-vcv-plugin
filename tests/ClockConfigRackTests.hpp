void clockConfigRackTests(){
    using M=Metropolix;using namespace metro;
    static_assert(M::CLOCK_SOURCE==719 && M::FORMAT20_NUM_PARAMS==727 && M::FORMAT7_PARAMS==490 && M::FORMAT8_PARAMS==492,"configuration appends parameters and freezes format7");
    Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    auto make=[](){auto m=std::unique_ptr<M>(new M);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);return m;};
    auto frames=[&](M& m,int n){for(int f=0;f<n;++f)m.process(args);};
    for(int mode=1;mode<=2;++mode){
        auto m=make();m->params[M::RESET_INPUT_MODE].setValue(mode);m->params[M::RUN_MODE].setValue(1);m->params[M::CLOCK_SOURCE].setValue(2);m->inputs[M::CLOCK_IN].channels=1;m->inputs[M::RESET_IN].channels=1;
        frames(*m,64);require(!m->sequencer.running,"gate-run low overrides saved Run button");
        auto tick=[&]{m->inputs[M::CLOCK_IN].setVoltage(0);frames(*m,100);m->inputs[M::CLOCK_IN].setVoltage(5);m->process(args);};
        m->inputs[M::RESET_IN].setVoltage(5);frames(*m,100);for(int p=0;p<4;++p)tick();require(m->sequencer.tracks[0].stage==3 && m->transportRunning.load(),"Reset jack high starts sequencer and native run light");
        m->inputs[M::RESET_IN].setVoltage(0);frames(*m,100);tick();require(m->sequencer.tracks[0].stage==3 && m->outputs[M::GATE_1].getVoltage()==0,"gate-run low pauses musical traversal and gates");
        m->inputs[M::RESET_IN].setVoltage(5);frames(*m,100);tick();require(m->sequencer.tracks[0].stage==(mode==1?0:4),"Run restarts whereas Run-C continues independently of Run button setting");
        m->params[M::RESET].setValue(1);frames(*m,64);require(m->sequencer.tracks[0].stage==-1,"panel Reset remains available in both gate-run modes");
    }
    for(int source=0;source<3;++source){auto m=make();m->params[M::CLOCK_SOURCE].setValue(source);m->params[M::RUN].setValue(1);m->inputs[M::CLOCK_IN].channels=1;frames(*m,13000);require((m->sequencer.tracks[0].stage>=0)==(source==1),"explicit Internal ignores patched external jack; Auto/External wait for edges");}
    {auto m=make();m->params[M::CLOCK_SOURCE].setValue(2);m->params[M::RUN].setValue(1);frames(*m,13000);require(m->sequencer.tracks[0].stage==-1,"explicit External does not fall back when unpatched");}
    {auto m=make();m->params[M::CLOCK_SOURCE].setValue(2);m->params[M::RUN].setValue(1);m->params[M::TRACK_GLOBAL_RESET_1].setValue(0);m->inputs[M::CLOCK_IN].channels=1;frames(*m,64);
        for(int p=0;p<5;++p){m->inputs[M::CLOCK_IN].setVoltage(0);frames(*m,100);m->inputs[M::CLOCK_IN].setVoltage(5);m->process(args);}
        int stage=m->sequencer.tracks[0].stage;m->params[M::RESET].setValue(1);frames(*m,64);require(m->sequencer.tracks[0].stage==stage && m->sequencer.tracks[1].stage==-1,"native button respects Global Reset Off");m->params[M::RESET].setValue(0);frames(*m,64);m->heldTrackReset.store(1);m->params[M::RESET].setValue(1);frames(*m,64);require(m->sequencer.tracks[0].stage==-1,"native held-track gesture forces protected reset");
    }
    for(int octave=-5;octave<=5;++octave){auto m=make();m->tunerActive.store(true);m->params[M::TUNER_OCTAVE].setValue(octave);frames(*m,64);for(int t=0;t<2;++t)require(m->outputs[M::PITCH_1+t*2].getVoltage()==octave && m->outputs[M::GATE_1+t*2].getVoltage()==5,"tuner drives both physical pitch/gate pairs at selected octave");m->params[M::TUNER_GATE].setValue(1);frames(*m,64);require(m->outputs[M::GATE_1].getVoltage()==0,"Clock tuner stays low when stopped even in Always mode");}
    {auto m=make();m->params[M::RUN].setValue(1);m->params[M::CLOCK_OUT_DIV].setValue(16);m->params[M::TUNER_GATE].setValue(1);m->tunerActive.store(true);int pulses=0;bool last=false;for(int f=0;f<96000;++f){m->process(args);bool high=m->outputs[M::GATE_1].getVoltage()>0;if(high && !last)++pulses;last=high;}require(pulses==16,"Clock tuner follows undivided sequence clock independently of CLK output division");}
    {auto m=make();m->tunerActive.store(true);m->uiPage.store(M::TUNER_PAGE);json_t* data=m->dataToJson();auto copy=make();copy->dataFromJson(data);json_decref(data);require(!copy->tunerActive.load() && copy->uiPage.load()==M::GLOBAL_SETUP_PAGE && copy->globalCursor.load()==5,"patch saved in Tuner reopens its inactive menu entry instead of a misleading active screen");}
    {auto m=make();m->params[M::TRACK_GLOBAL_RESET_1].setValue(0);m->params[M::CLOCK_SOURCE].setValue(1);m->params[M::BOTTOM_PITCH].setValue(7);m->putPreset(0,m->capturePreset());m->params[M::TRACK_GLOBAL_RESET_1].setValue(1);m->params[M::CLOCK_SOURCE].setValue(2);m->requestPreset(0);m->sendPresetCommand();frames(*m,64);require(m->integer(M::TRACK_GLOBAL_RESET_1)==0 && m->integer(M::CLOCK_SOURCE)==2,"format8 recalls per-track Global Reset without changing global clock source");
        json_t* params=m->paramsToJson(),*data=m->dataToJson();auto copy=make();copy->paramsFromJson(params);copy->dataFromJson(data);require(copy->getPreset(0).occupied && copy->getPreset(0).values[490]==0 && copy->integer(M::BOTTOM_PITCH)==7 && !copy->tunerActive.load(),"configuration and format8 bank persist without reopening tuner");
        json_object_set_new(data,"schemaVersion",json_integer(22));json_object_set_new(data,"presetFormat",json_integer(7));json_t* values=json_array_get(json_object_get(data,"presets"),0);while(json_array_size(values)>490)json_array_remove(values,json_array_size(values)-1);copy->paramsFromJson(params);copy->dataFromJson(data);require(copy->integer(M::TRACK_GLOBAL_RESET_1)==1 && copy->integer(M::CLOCK_SOURCE)==0 && copy->integer(M::CLOCK_RUNNING_ONLY)==2 && copy->getPreset(0).occupied && copy->getPreset(0).values[490]==1,"legacy schema/format7 gains stable compatible defaults");json_decref(params);json_decref(data);
    }
}
