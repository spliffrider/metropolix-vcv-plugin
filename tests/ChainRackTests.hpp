void chainRackTests() {
    using M=Metropolix;
    auto storage=std::unique_ptr<M>(new M);M& m=*storage;
    Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    auto process=[&](int n=1){for(int i=0;i<n;++i)m.process(args);};
    auto pulse=[&]{m.inputs[M::CLOCK_IN].setVoltage(0);process(100);m.inputs[M::CLOCK_IN].setVoltage(5);process();};
    auto upload=[&](const M::ChainState& s){m.setChainState(s);m.sendChainCommand();process();};
    for(int k=0;k<2;++k)m.params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m.params[M::LOAD_PITCH].setValue(M::PitchJump);m.params[M::LOAD_SWITCHES].setValue(1);
    m.params[M::SCALE].setValue(0);m.params[M::RANGE].setValue(1);
    auto a=m.capturePreset();a.values[M::TOTAL_PULSES]=3;a.values[M::PITCH]=0;
    a.values[M::modParam(0,M::MOD_VALUE)]=2;a.values[M::modParam(0,M::MOD_DESTINATION)]=metro::ModPitchPost;
    a.values[M::modParam(0,M::MOD_ASSIGNMENT)]=1;a.values[M::stageParam(0,1,M::SKIP)]=1;
    a.values[M::stageParam(0,0,M::ACCUMULATION)]=1;
    auto b=a;b.values[M::TOTAL_PULSES]=2;b.values[M::PITCH]=1;b.values[M::modParam(0,M::MOD_VALUE)]=3;
    m.putPreset(0,a);m.putPreset(63,b);
    M::ChainState state;metro::ChainLink link;link.slot=0;link.repeats=2;state.working.insert(0,link);
    link.slot=63;link.repeats=1;link.mutes=5;state.working.insert(1,link);state.enabled=true;
    m.inputs[M::CLOCK_IN].channels=1;m.params[M::RUN].setValue(1);upload(state);
    for(int edge=0;edge<80;++edge) {
        pulse();int phase=edge%8;
        require(m.chainPlayingLink.load()==(phase<6?0:1),"Total x repeats uses base clock independent of skip");
        require(m.chainPlayingRepeat.load()==(phase<3?1:phase<6?2:1),"displayed repeat matches current phrase");
        if(phase==0 || phase==3)require(m.outputs[M::PITCH_1].getVoltage()==0,"accumulator resets at each Total boundary");
        if(phase==6) {
            require(m.sequencer.tracks[0].stage==0 && m.outputs[M::GATE_1].getVoltage()==0 && m.outputs[M::GATE_2].getVoltage()==5,"new link first note and link mutes occur on due edge");
            require(m.outputs[M::PITCH_2].getVoltage()==1 && m.sequencer.modulation.outputs[0]==0,"MOD mute suppresses outputs and internal routes");
        }
    }
    M::PresetRecall receipt;require(!m.presetRecalls.pop(receipt),"automatic links do not flood undo receipts when UI is absent");
    m.requestPreset(63);require(m.presetUiCommands.size()==1 && m.presetUiCommands.front().kind==M::PresetCommand::Cancel,"manual loads blocked in chain mode");
    require(m.presetSaveBlocked(),"manual saves blocked in chain mode");
    m.params[M::RUN].setValue(0);process();m.params[M::RUN].setValue(1);process();pulse();
    require(m.chainPlayingLink.load()==0 && m.chainPulse.load()==1,"Run/Stop restarts Link 1");
    m.params[M::RUN_MODE].setValue(1);pulse();unsigned held=m.chainPulse.load();
    m.params[M::RUN].setValue(0);process(100);require(m.outputs[M::GATE_1].getVoltage()==0,"pause silences gate");
    m.params[M::RUN].setValue(1);process(100);require(m.chainPulse.load()==held,"pause preserves pulse position without clock");
    pulse();require(m.chainPulse.load()==held+1,"pause resumes at next external edge");
    m.inputs[M::RESET_IN].channels=1;m.inputs[M::RESET_IN].setVoltage(5);process();
    require(m.chainPlayingLink.load()==0 && m.chainPosition.fresh,"reset jack returns Link 1 immediately");
    m.inputs[M::RESET_IN].setVoltage(0);process(100);pulse();require(m.chainPulse.load()==1,"reset ignore window does not discard next valid edge");
    state=m.getChainState();state.saved[0]=state.working;state.saved[7]=state.working;state.loaded=7;upload(state);
    json_t* data=m.dataToJson(),*params=m.paramsToJson();
    auto restored=std::unique_ptr<M>(new M);restored->paramsFromJson(params);restored->dataFromJson(data);json_decref(params);
    auto round=restored->getChainState();require(round.working==state.working && round.saved[7]==state.working && round.loaded==7 && round.enabled,"working and saved chains plus enable state persist");
    restored->process(args);require(restored->chainPlayingLink.load()==0 && restored->chainPulse.load()==1,"enabled patch runs without UI and restarts Link 1");
    json_t* links=json_object_get(json_object_get(data,"chains"),"working");
    json_array_set_new(links,0,json_pack("{s:I,s:I,s:I}","slot",json_int_t(9223372036854775807LL),"repeats",json_int_t(9223372036854775807LL),"mutes",json_int_t(-999)));
    json_array_append_new(links,json_string("invalid"));restored->dataFromJson(data);
    require(restored->getChainState().working.links[0].slot==63 && restored->getChainState().working.links[0].repeats==64 && restored->getChainState().working.links[0].mutes==0 && restored->getChainState().working.size==2,"malformed links rejected and 64-bit integers safely clamped");
    json_decref(data);
    json_t* old=json_pack("{s:i}","schemaVersion",10);restored->dataFromJson(old);json_decref(old);
    require(!restored->chainEnabled.load() && restored->getChainState().working.size==0 && restored->integer(M::RUN_MODE)==0,"2.7 patches remain chain-free and keep original transport");
    // Queue fallback, disabled timing, and missing preset retention.
    state.enabled=false;upload(state);
    a.values[M::TOTAL_PULSES]=0;m.putPreset(0,a);
    m.params[M::QUEUE_PULSES].setValue(2);state.working.links[0].repeats=1;state.working.links[1].slot=1;state.enabled=true;upload(state);
    pulse();pulse();pulse();require(m.chainPlayingLink.load()==1 && m.integer(M::TOTAL_PULSES)==0 && m.params[M::PITCH].getValue()==0,"empty preset retains current settings and Queue fallback changes links");
    m.params[M::QUEUE_PULSES].setValue(0);process();
    for(int i=0;i<100;++i)pulse();require(m.chainPlayingLink.load()==1,"Total Off and Queue Off hold current link indefinitely");
    state.enabled=false;upload(state);require(m.sequencer.chainMutes==0 && !m.sequencer.settings.modLanes[0].mute,"disabling chain removes transient mutes");
    m.requestPreset(63);m.sendPresetCommand();process();m.sendPresetCommand();process();
    require(m.lastLoadedSlot.load()==63,"manual preset loading resumes after disabling chain");
    std::cout<<"PASS: Rack chains, Total/Queue timing, empty links, skip/accumulation, MOD mutes, transport and persistence\n";
}
