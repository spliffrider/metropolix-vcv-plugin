#include <memory>

void presetRackTests() {
    using M = Metropolix;
    auto storage = std::unique_ptr<M>(new M);
    M& m = *storage;
    Module::ProcessArgs args; args.sampleRate=48000; args.sampleTime=1.f/48000;
    auto process=[&](int n=1) { for(int i=0;i<n;++i)m.process(args); };
    auto drain=[&] { M::PresetRecall event; while(m.presetRecalls.pop(event)) {} };
    auto submit=[&](int slot) { drain();m.requestPreset(slot);m.sendPresetCommand();process(); };
    auto pulse=[&] {
        m.inputs[M::CLOCK_IN].setVoltage(0);process(100);
        m.inputs[M::CLOCK_IN].setVoltage(5);process();
    };
    for(int k=0;k<2;++k)m.params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m.readSettings();
    auto saved=m.capturePreset();
    saved.values[M::ROOT]=7;
    saved.values[M::PITCH]=.5f;
    saved.values[M::PULSES]=6;
    saved.values[M::GATES]=metro::Single;
    saved.values[M::TEMPO]=200;
    saved.values[M::RANGE]=4;
    saved.values[M::OUTPUT_SOURCE_A]=9;
    saved.values[M::TOTAL_PULSES]=18;
    saved.values[M::auxParam(0,M::AUX_DESTINATION)]=metro::ModPitchPre;
    saved.values[M::auxParam(0,M::AUX_ASSIGNMENT)]=2;
    saved.values[M::auxParam(0,M::AUX_AMOUNT)]=.75f;
    saved.values[M::ctrlParam(0,M::CTRL_DESTINATION)]=metro::CtrlRatchet;
    saved.values[M::ctrlParam(0,M::CTRL_ASSIGNMENT)]=1;
    saved.values[M::ctrlParam(0,M::CTRL_VALUE)]=.75f;
    for(int slot=0;slot<64;++slot) {
        saved.values[M::stageParam(1,0,M::CV)]=float(slot)/10-3;
        m.putPreset(slot,saved);
    }
    m.presetBankIndex.store(7);m.uiPage.store(M::PRESET_SETUP_PAGE);m.uiField.store(6);
    m.params[M::QUEUE_PULSES].setValue(4096);
    json_t* data=m.dataToJson();json_t* params=m.paramsToJson();
    auto restored=std::unique_ptr<M>(new M);
    restored->paramsFromJson(params);restored->dataFromJson(data);
    json_decref(params);
    for(int slot=0;slot<64;++slot) {
        auto a=m.getPreset(slot),b=restored->getPreset(slot);
        require(b.occupied && a.values==b.values,"all 64 internal snapshots survive patch save/load");
    }
    require(restored->integer(M::QUEUE_PULSES)==4096 && restored->presetBankIndex.load()==7 && restored->uiField.load()==6,"global preset settings and selected bank/page persist");
    require(restored->pendingSlot.load()==-1,"reopening patch never re-arms a queued transition");
    json_t* slots=json_object_get(data,"presets");
    json_array_set_new(slots,0,json_array());
    json_array_set_new(json_array_get(slots,1),M::ROOT,json_string("bad"));
    json_array_set_new(json_array_get(slots,2),M::ROOT,json_real(999999));
    restored->dataFromJson(data);
    require(!restored->getPreset(0).occupied && !restored->getPreset(1).occupied,"short or nonnumeric preset rejected without corrupting neighboring slots");
    require(restored->getPreset(2).values[M::ROOT]==11 && restored->getPreset(63).occupied,"untrusted numeric values clamp to safe range");
    json_decref(data);
    json_t* old=json_pack("{s:i}","schemaVersion",9);restored->dataFromJson(old);json_decref(old);
    require(!restored->getPreset(63).occupied && restored->integer(M::TOTAL_PULSES)==0 && restored->integer(M::QUEUE_PULSES)==32,"old Rack file clears old bank contents and restores neutral preset defaults");
    // The JSON path used by the actual UI save action must preserve IDs/values.
    params=m.paramsToJson();auto parsed=m.presetFromParamsJson(params);json_decref(params);
    require(parsed.values==m.capturePreset().values,"UI snapshot parses Rack parameter IDs correctly");
    float pitch=m.params[M::PITCH].getValue();
    submit(0);
    require(m.integer(M::ROOT)==7 && m.integer(M::TOTAL_PULSES)==18,"stopped load recalls scale and per-preset total length");
    require(m.params[M::PITCH].getValue()==pitch && m.integer(M::PULSES)==1,"default load keeps physical pitch and pulse switches");
    require(m.params[M::TEMPO].getValue()==120 && m.integer(M::RANGE)==3 && m.integer(M::OUTPUT_SOURCE_A)==0,"internal presets preserve tempo, slider range and global output routing");
    require(m.integer(M::auxParam(0,M::AUX_DESTINATION))==metro::ModPitchPre && m.params[M::auxParam(0,M::AUX_AMOUNT)].getValue()==0,"AUX Assigns recalls routing but keeps attenuverter");
    require(m.integer(M::ctrlParam(0,M::CTRL_DESTINATION))==metro::CtrlRatchet && m.params[M::ctrlParam(0,M::CTRL_VALUE)].getValue()==0,"CTRL Assigns recalls destination but keeps live knob");
    m.params[M::LOAD_PITCH].setValue(M::PitchJump);m.params[M::LOAD_SWITCHES].setValue(1);m.params[M::LOAD_CTRL].setValue(M::CtrlJump);
    submit(1);
    require(m.params[M::PITCH].getValue()==.5f && m.integer(M::PULSES)==6 && m.integer(M::GATES)==metro::Single,"Jump recalls all shared sliders and switches");
    require(m.params[M::ctrlParam(0,M::CTRL_VALUE)].getValue()==.75f,"CTRL Jump recalls knob position");
    m.params[M::LOAD_CTRL].setValue(0);m.params[M::LOAD_AUX].setValue(0);
    m.params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m.params[M::auxParam(0,M::AUX_DESTINATION)].setValue(metro::ModNone);
    submit(2);
    require(m.integer(M::ctrlParam(0,M::CTRL_DESTINATION))==metro::CtrlNone && m.integer(M::auxParam(0,M::AUX_DESTINATION))==metro::ModNone,"Off retains live CTRL and AUX routing");
    // Use literal external edges to prove the new first note is emitted on the
    // boundary, not one sample/clock later. Track LEN and skipped stages differ.
    m.params[M::TOTAL_PULSES].setValue(0);m.params[M::PITCH].setValue(0);m.params[M::PULSES].setValue(1);
    m.params[M::ROOT].setValue(0);m.params[M::LOAD_DELAY].setValue(metro::LoadQueuePulses);
    m.params[M::QUEUE_PULSES].setValue(4);m.params[M::RUN].setValue(1);
    m.params[M::extraParam(0,M::PULSE_LIMIT)].setValue(2);
    m.params[M::trackParam(1,M::DIVISION)].setValue(3);
    m.params[M::stageParam(0,1,M::SKIP)].setValue(1);
    m.inputs[M::CLOCK_IN].channels=1;process(100);
    saved=m.capturePreset();saved.values[M::ROOT]=5;saved.values[M::TOTAL_PULSES]=3;
    saved.values[M::modParam(0,M::MOD_VALUE)]=2;
    m.putPreset(8,saved);
    pulse();pulse(); // elapsed pulses 0 and 1
    submit(8);
    require(m.pendingSlot.load()==8 && m.integer(M::ROOT)==0,"running queued load leaves current pattern playing");
    pulse();pulse();
    require(m.pendingSlot.load()==8 && m.integer(M::ROOT)==0,"track fixed pulse length and skips cannot fire queued load early");
    pulse();
    require(m.pendingSlot.load()==-1 && m.integer(M::ROOT)==5,"load uses next multiple since reset, not four pulses from request");
    require(m.sequencer.tracks[0].stage==0 && m.sequencer.tracks[1].stage==0 && std::abs(m.outputs[M::PITCH_1].getVoltage()-5.f/12)<1e-5,"both tracks output new first note on exact queue boundary");
    require(m.sequencer.modulation.lanes[0].stage==0 && m.sequencer.modulation.outputs[0]==2,"MOD first value is recalled on same edge");
    M::PresetRecall recall;require(m.presetRecalls.pop(recall) && recall.before.values[M::ROOT]==0 && recall.after.values[M::ROOT]==5,"undo receipt captures actual before/after values at recall time");
    pulse();pulse();pulse();
    require(m.sequencer.tracks[0].stage==0 && m.sequencer.modulation.lanes[0].stage==0,"Total Pulses resets tracks and MOD without another preset");
    // Queue cancellation / replacement and explicit stop/reset.
    submit(8);submit(8);require(m.pendingSlot.load()==-1,"selecting queued preset twice cancels");
    submit(8);m.putPreset(9,M::Snapshot{});submit(9);require(m.pendingSlot.load()==-1,"empty slot cancels pending load");
    m.params[M::QUEUE_PULSES].setValue(0);submit(8);submit(10);
    require(m.pendingSlot.load()==10,"new slot replaces pending load");
    m.params[M::RUN].setValue(0);process();
    require(m.pendingSlot.load()==-1 && m.lastLoadedSlot.load()==10 && m.outputs[M::GATE_1].getVoltage()==0,"stop applies queued preset without sounding a gate");
    m.params[M::RUN].setValue(1);process(100);submit(8);
    for(int i=0;i<8;++i)pulse();require(m.pendingSlot.load()==8,"Queue zero waits despite clocks and automatic Total resets");
    m.params[M::RESET].setValue(1);process();
    require(m.pendingSlot.load()==-1 && m.lastLoadedSlot.load()==8,"manual RESET loads Queue zero preset immediately");
    m.params[M::RESET].setValue(0);process(100);pulse();
    require(m.sequencer.tracks[0].stage==0,"reset load starts on next valid external edge");
    submit(10);m.inputs[M::RESET_IN].channels=1;m.inputs[M::RESET_IN].setVoltage(5);process();
    require(m.pendingSlot.load()==-1 && m.lastLoadedSlot.load()==10,"reset jack also commits queued load");
    m.inputs[M::RESET_IN].setVoltage(0);process(100);
    // Current preset's Total, not incoming Total, selects transition time.
    m.params[M::LOAD_DELAY].setValue(metro::LoadTotalPulses);m.params[M::TOTAL_PULSES].setValue(3);
    m.params[M::RESET].setValue(1);process();m.params[M::RESET].setValue(0);process(100);
    pulse();submit(8);pulse();pulse();require(m.pendingSlot.load()==8,"Total queue waits for complete current phrase");
    pulse();require(m.pendingSlot.load()==-1,"Total queue commits at current phrase boundary");
    // History restores only values changed by that recall; globals stay live.
    M::PresetCommand undo;undo.kind=M::PresetCommand::Restore;undo.snapshot=recall.before;undo.other=recall.after;
    m.params[M::TEMPO].setValue(155);drain();m.presetUiCommands.push_back(undo);m.sendPresetCommand();process();
    require(m.integer(M::ROOT)==0 && m.integer(M::TEMPO)==155,"undo restores recalled parameters without overwriting later global tempo");
    require(m.pendingSlot.load()==-1 && m.lastLoadedSlot.load()==-1,"undo cancels pending recall and clears loaded-slot marker");
    std::cout << "PASS: 64 preset slots, policies, migration, queued clock output, stop/reset, Total Pulses and undo receipts\n";
}
