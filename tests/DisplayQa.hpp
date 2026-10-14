// Included in qaStep so this uses the same actual widget callbacks as UI QA.
if(qaFrame==5) {
    m->expanded.store(false);m->uiMod.store(false);m->uiAlt.store(false);m->uiHome.store(0);m->uiPage.store(Metropolix::HOME);
    drag(find("Encoder"),-22);check(m->uiHome.load()==1,"HOME upward encoder drag advances the overview");
    drag(find("Encoder"),22);check(m->uiHome.load()==0,"HOME downward encoder drag reverses the overview");
    click(find("Metropolix display"));check(m->uiHome.load()==1,"display click also browses HOME");
    click(find("ORDER"));int direction=Metropolix::extraParam(0,Metropolix::REVERSED);int before=m->integer(direction);
    click(find("Encoder"));check(m->integer(direction)==1-before,"ORDER encoder press toggles direction (p.70)");
    APP->history->undo();check(m->integer(direction)==before,"direction toggle supports undo");
    click(find("LEN"));click(find("Encoder"));check(screenParam(m)==Metropolix::extraParam(0,Metropolix::PULSE_LIMIT),"LEN press selects pulse length (p.75)");
    click(find("ALT"));check(screenParam(m)==Metropolix::trackParam(0,Metropolix::OFFSET),"LEN ALT selects stage offset");click(find("ALT"));
    click(find("SCALE"));click(find("ALT"));check(screenParam(m)==Metropolix::ROOT && !m->uiAlt.load(),"SCALE ALT switches to root without latching stage actions");
    click(find("MOD"));click(find("EDIT"));click(find("Stage 3 / PITCH"));
    check(m->selectedLane.load()==2 && !m->uiChooseFunction.load(),"MOD EDIT + stage selects that MOD lane (p.137)");
    click(find("Stage 4 / GATE"));int ramp=Metropolix::modParam(2,Metropolix::MOD_RAMP+3);before=m->integer(ramp);
    click(find("Encoder"));check(m->integer(ramp)==1-before,"MOD encoder press toggles Step/Ramp (p.139)");APP->history->undo();
    check(screenParam(m)==Metropolix::modParam(2,Metropolix::MOD_VALUE+3),"MOD turn remains voltage editing after transition toggle");
    click(find("TRK 1"));m->selectedRow.store(Metropolix::PROBABILITY);m->uiPage.store(Metropolix::STAGE_PAGE);
    click(find("Encoder"));check(m->integer(Metropolix::stageOption(0,3,Metropolix::PROB_TARGET))==1,"PROB encoder toggles Stage/Pulse (p.98)");APP->history->undo();
    m->selectedRow.store(Metropolix::ACCUMULATION);click(find("Encoder"));check(m->integer(Metropolix::stageOption(0,3,Metropolix::ACCUM_TRIGGER))==1,"ACCUM encoder cycles triggers (p.100)");APP->history->undo();
    click(find("ALT"));click(find("Stage 7 / ACCUM"));
    check(m->uiPage.load()==Metropolix::ACCUM_PAGE && !m->uiAlt.load() && !m->uiMod.load(),"ALT LIM opens accumulator options");
    click(find("Encoder"));click(find("Encoder"));
    check(screenParam(m)==Metropolix::accumModeParam(0),"LIM third field is Stage/Track mode");
    drag(find("Encoder"),-80);check(m->integer(Metropolix::accumModeParam(0))==1,"upward mode drag selects Track");
    APP->history->undo();check(m->integer(Metropolix::accumModeParam(0))==0,"accumulator mode supports undo");
    drag(find("Metropolix display"),-80);drag(find("Metropolix display"),80);
    check(m->integer(Metropolix::accumModeParam(0))==0,"downward display drag selects Stage");
    click(find("TRK 2"));check(screenParam(m)==Metropolix::accumModeParam(1),"LIM follows selected track");
    drag(find("Encoder"),-80);
    check(m->integer(Metropolix::accumModeParam(0))==0 && m->integer(Metropolix::accumModeParam(1))==1,"LIM modes edit independently");
    press(find("Encoder"),GLFW_MOUSE_BUTTON_RIGHT);
    auto* trackChoice=seek(APP->scene,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text=="Track";});
    check(trackChoice!=nullptr,"mode context menu offers named Track choice");closeMenus();
    for(int i=0;i<4;++i)click(find("Encoder"));
    check(m->uiField.load()==0,"LIM cycles six fields and wraps");
    click(find("TRK 1"));
    m->uiHome.store(3);m->uiStageDetail.store(true);
    json_t* data=APP->engine->moduleToJson(m);m->uiHome.store(0);m->uiStageDetail.store(false);APP->engine->moduleFromJson(m,data);json_decref(data);
    check(m->uiHome.load()==3 && m->uiStageDetail.load(),"home view and stage-detail selection survive patch load");
    m->uiMod.store(false);m->uiEdit.store(false);m->uiHome.store(0);m->uiPage.store(Metropolix::HOME);
    m->selectedRow.store(Metropolix::SLIDE);
}
if(qaFrame==12)metroqa::screenshot(dir+"/panel.png");
if(qaFrame==13) {
    const float pitch[]={0.f,.28f,.58f,.85f,.85f,.65f,.5f,0.f};
    for(int s=0;s<8;++s){m->params[Metropolix::PITCH+s].setValue(pitch[s]);m->params[Metropolix::PULSES+s].setValue(1);m->params[Metropolix::GATES+s].setValue(1);}
    m->uiMod.store(true);m->selectedLane.store(0);m->uiHome.store(0);
}
if(qaFrame==10) {
    int value=Metropolix::ctrlParam(0,Metropolix::CTRL_VALUE),dest=Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION);
    m->params[dest].setValue(metro::CtrlGateLength);m->params[value].setValue(0);
    m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_ASSIGNMENT)].setValue(2);
    click(find("CTRL 1 assignment"));
    check(m->uiPage.load()==Metropolix::CTRL_PAGE && m->selectedCtrl.load()==0 && screenParam(m)==dest,"CTRL 1 assignment opens its destination page");
    drag(find("CTRL 1"),-80);check(m->params[value].getValue()>0,"CTRL upward knob drag increases value");
    APP->history->undo();check(m->params[value].getValue()==0,"CTRL drag has Rack undo");
    drag(find("CTRL 1"),80);check(m->params[value].getValue()<0,"CTRL downward knob drag decreases value");
    APP->history->undo();
    click(find("Encoder"));check(screenParam(m)==Metropolix::ctrlParam(0,Metropolix::CTRL_ASSIGNMENT),"CTRL encoder click selects track assignment");
    drag(find("Encoder"),80);check(m->integer(Metropolix::ctrlParam(0,Metropolix::CTRL_ASSIGNMENT))==2 && pendingConfirmation(m) && m->uiDraftValue==0,"CTRL target previews through encoder without changing confirmed routing");
    click(find("Encoder"));check(m->integer(Metropolix::ctrlParam(0,Metropolix::CTRL_ASSIGNMENT))==0 && !pendingConfirmation(m),"CTRL encoder press confirms target before switching fields");APP->history->undo();
    click(find("CTRL 2 assignment"));check(m->selectedCtrl.load()==1 && screenParam(m)==Metropolix::ctrlParam(1,Metropolix::CTRL_DESTINATION),"CTRL 2 has independent assignment page");
    click(find("TRK 1"));click(find("GATE"));find("Encoder")->bind();
    check(find("Encoder")->getParamQuantity()==nullptr && find("Encoder")->tooltipText().find("Controlled by CTRL 1")!=std::string::npos,"direct destination locks encoder and explains owner");
    press(find("Encoder"),GLFW_MOUSE_BUTTON_RIGHT);
    auto* clear=seek(APP->scene,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text=="Clear assignment";});
    check(clear!=nullptr,"locked field context menu offers controller reassignment");
    if(clear){event::Action action;clear->onAction(action);}
    check(m->integer(dest)==metro::CtrlNone,"clear assignment releases controller ownership");closeMenus();
    APP->history->undo();check(m->integer(dest)==metro::CtrlGateLength,"clearing CTRL assignment supports undo");
    m->params[dest].setValue(metro::CtrlNone);
    find("Encoder")->bind();check(find("Encoder")->getParamQuantity()!=nullptr,"unassigned direct field becomes editable again");
}
if(qaFrame==17)metroqa::screenshot(dir+"/reference-pose.png");
if(qaFrame==18) {
    const int ratchets[]={1,7,4,1,2,1,3,1},prob[]={100,75,58,100,25,100,50,100},accum[]={0,2,7,-3,0,-5,1,4};
    for(int s=0;s<8;++s){m->params[Metropolix::stageParam(0,s,Metropolix::RATCHETS)].setValue(ratchets[s]);m->params[Metropolix::stageParam(0,s,Metropolix::PROBABILITY)].setValue(prob[s]);m->params[Metropolix::stageParam(0,s,Metropolix::ACCUMULATION)].setValue(accum[s]);}
}
// Screen states are rendered by the production component. Full-panel captures
// make visual comparisons possible without relying on Windows capture.
struct DisplayCase { const char* name; int page; int field; int home; int row; bool mod; };
const DisplayCase cases[]={
    {"home-positions",Metropolix::HOME,0,0,0,false},
    {"home-notes",Metropolix::HOME,0,1,0,false},
    {"home-panel",Metropolix::HOME,0,2,0,false},
    {"home-mod",Metropolix::HOME,0,3,0,true},
    {"bpm",Metropolix::BPM_PAGE,0,0,0,false},
    {"scale",Metropolix::SCALE_PAGE,1,0,0,false},
    {"scale-world",Metropolix::SCALE_PAGE,1,0,0,false},
    {"scale-locked",Metropolix::SCALE_PAGE,1,0,0,false},
    {"scale-search",Metropolix::SCALE_PAGE,1,0,0,false},
    {"scale-user",Metropolix::SCALE_PAGE,0,0,0,false},
    {"scale-edit",Metropolix::SCALE_PAGE,1,0,0,false},
    {"order",Metropolix::ORDER_PAGE,0,0,0,false},
    {"order-brownian-reverse",Metropolix::ORDER_PAGE,1,0,0,false},
    {"order-mod-reverse",Metropolix::ORDER_PAGE,1,0,0,true},
    {"order-shortcut-saved",Metropolix::ORDER_PAGE,0,0,0,false},
    {"order-ctrl-blocked",Metropolix::ORDER_PAGE,0,0,0,false},
    {"length",Metropolix::LENGTH_PAGE,0,0,0,false},
    {"division",Metropolix::DIV_PAGE,1,0,0,false},
    {"swing-straight",Metropolix::SWING_PAGE,0,0,Metropolix::SLIDE,false},
    {"swing-max",Metropolix::SWING_PAGE,0,0,Metropolix::SLIDE,false},
    {"swing-mod",Metropolix::SWING_PAGE,0,0,0,true},
    {"slide-time",Metropolix::SLIDE_PAGE,0,0,0,false},
    {"cv-type-bipolar",Metropolix::CV_TYPE_PAGE,0,0,Metropolix::CV,false},
    {"cv-type-unipolar",Metropolix::CV_TYPE_PAGE,0,0,Metropolix::CV,false},
    {"cv-type-gates",Metropolix::CV_TYPE_PAGE,0,0,Metropolix::CV,false},
    {"stage-cv-sramp",Metropolix::STAGE_PAGE,0,0,Metropolix::CV,false},
    {"stage-cv-pramp",Metropolix::STAGE_PAGE,0,0,Metropolix::CV,false},
    {"stage-cv-unipolar",Metropolix::STAGE_PAGE,0,0,Metropolix::CV,false},
    {"stage-cv-gates",Metropolix::STAGE_PAGE,0,0,Metropolix::CV,false},
    {"stage-cv-all",Metropolix::STAGE_PAGE,0,0,Metropolix::CV,false},
    {"slide-analog",Metropolix::SLIDE_PAGE,0,0,0,false},
    {"slide-acid",Metropolix::SLIDE_PAGE,0,0,0,false},
    {"slide-mod",Metropolix::SLIDE_PAGE,0,0,0,true},
    {"gate-length",Metropolix::GATE_PAGE,0,0,0,false},
    {"gate-scale",Metropolix::GATE_PAGE,1,0,0,false},
    {"stage-pitch-override",Metropolix::STAGE_PAGE,0,0,Metropolix::PITCH_OVERRIDE,false},
    {"stage-gate-override",Metropolix::STAGE_PAGE,0,0,Metropolix::GATE_OVERRIDE,false},
    {"stage-slide",Metropolix::STAGE_PAGE,0,0,Metropolix::SLIDE,false},
    {"stage-skip",Metropolix::STAGE_PAGE,0,0,Metropolix::SKIP,false},
    {"stage-ratchet",Metropolix::STAGE_PAGE,0,0,Metropolix::RATCHETS,false},
    {"stage-probability",Metropolix::STAGE_PAGE,0,0,Metropolix::PROBABILITY,false},
    {"stage-accumulator",Metropolix::STAGE_PAGE,0,0,Metropolix::ACCUMULATION,false},
    {"stage-cv",Metropolix::STAGE_PAGE,0,0,Metropolix::CV,false},
    {"mod-voltage",Metropolix::STAGE_PAGE,0,0,0,true},
    {"clock-default",Metropolix::CLOCK_OUT_PAGE,0,0,0,false},
    {"clock-offset",Metropolix::CLOCK_OUT_PAGE,1,0,0,false},
    {"clock-max",Metropolix::CLOCK_OUT_PAGE,1,0,0,false},
    {"track-menu-0",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-1",Metropolix::TRACK_MENU_PAGE,1,0,0,false},
    {"track-menu-2",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-3",Metropolix::TRACK_MENU_PAGE,1,0,0,false},
    {"track-menu-4",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-5",Metropolix::TRACK_MENU_PAGE,1,0,0,false},
    {"track-menu-6",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-7",Metropolix::TRACK_MENU_PAGE,1,0,0,false},
    {"track-menu-8",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-9",Metropolix::TRACK_MENU_PAGE,1,0,0,false},
    {"track-menu-10",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"output-kind-0",Metropolix::A_PAGE,0,0,0,false},
    {"output-kind-1",Metropolix::A_PAGE,1,0,0,false},
    {"output-kind-2",Metropolix::A_PAGE,0,0,0,false},
    {"output-kind-3",Metropolix::A_PAGE,1,0,0,false},
    {"output-kind-4",Metropolix::A_PAGE,0,0,0,false},
    {"output-kind-5",Metropolix::A_PAGE,1,0,0,false},
    {"output-kind-6",Metropolix::A_PAGE,0,0,0,false},
    {"output-kind-7",Metropolix::A_PAGE,1,0,0,false},
    {"output-kind-8",Metropolix::A_PAGE,0,0,0,false},
    {"output-kind-9",Metropolix::A_PAGE,1,0,0,false},
    {"output-kind-10",Metropolix::A_PAGE,0,0,0,false},
    {"output-kind-11",Metropolix::A_PAGE,1,0,0,false},
    {"output-kind-12",Metropolix::A_PAGE,0,0,0,false},
    {"output-kind-13",Metropolix::A_PAGE,1,0,0,false},
    {"output-kind-14",Metropolix::A_PAGE,0,0,0,false},
    {"output-kind-15",Metropolix::A_PAGE,1,0,0,false},
    {"output-a",Metropolix::A_PAGE,0,0,0,false},
    {"output-b",Metropolix::B_PAGE,0,0,0,false},
    {"aux-cv",Metropolix::AUX_PAGE,0,0,Metropolix::SLIDE,false},
    {"aux-global-tempo",Metropolix::AUX_PAGE,0,0,Metropolix::SLIDE,false},
    {"aux-loopy-offset",Metropolix::AUX_PAGE,1,0,Metropolix::SLIDE,false},
    {"aux-track-reset",Metropolix::AUX_PAGE,1,0,Metropolix::SLIDE,false},
    {"mod-extended-gate",Metropolix::STAGE_PAGE,0,0,Metropolix::SLIDE,true},
    {"mod-global-home",Metropolix::HOME,0,3,Metropolix::SLIDE,true},
    {"setup-global",Metropolix::SETUP_PAGE,0,0,Metropolix::SLIDE,false},
    {"global-clock-src",Metropolix::GLOBAL_SETUP_PAGE,0,0,Metropolix::SLIDE,false},
    {"global-reset-run",Metropolix::GLOBAL_SETUP_PAGE,1,0,Metropolix::SLIDE,false},
    {"global-bottom-pitch",Metropolix::GLOBAL_SETUP_PAGE,1,0,Metropolix::SLIDE,false},
    {"global-tuner-item",Metropolix::GLOBAL_SETUP_PAGE,0,0,Metropolix::SLIDE,false},
    {"tuner-low",Metropolix::TUNER_PAGE,0,0,Metropolix::SLIDE,false},
    {"tuner-clock",Metropolix::TUNER_PAGE,0,0,Metropolix::SLIDE,false},
    {"track-global-reset",Metropolix::TRACK_MENU_PAGE,0,0,Metropolix::SLIDE,false},
    {"bpm-modulated",Metropolix::BPM_PAGE,0,0,Metropolix::SLIDE,false},
    {"aux-gate",Metropolix::AUX_PAGE,1,0,Metropolix::SLIDE,false},
    {"accum-stage",Metropolix::ACCUM_PAGE,2,0,Metropolix::ACCUMULATION,false},
    {"accum-track",Metropolix::ACCUM_PAGE,2,0,Metropolix::ACCUMULATION,false},
    {"accum-options",Metropolix::ACCUM_PAGE,4,0,Metropolix::ACCUMULATION,false},
    {"ctrl-gate",Metropolix::CTRL_PAGE,0,0,Metropolix::SLIDE,false},
    {"ctrl-accum-dir",Metropolix::CTRL_PAGE,0,0,Metropolix::SLIDE,false},
    {"aux-accum-reset",Metropolix::AUX_PAGE,0,0,Metropolix::SLIDE,false},
    {"aux-accumulate",Metropolix::AUX_PAGE,0,0,Metropolix::SLIDE,false},
    {"mod-accum-gate",Metropolix::STAGE_PAGE,0,0,Metropolix::SLIDE,true},
    {"ctrl-pulse-count",Metropolix::CTRL_PAGE,1,0,Metropolix::SLIDE,false},
    {"ctrl-owned-accum",Metropolix::ACCUM_PAGE,2,0,Metropolix::ACCUMULATION,false},
    {"preset-load",Metropolix::LOAD_PAGE,0,0,0,false},
    {"preset-save",Metropolix::SAVE_PAGE,0,0,0,false},
    {"preset-timing",Metropolix::PRESET_SETUP_PAGE,1,0,0,false},
    {"preset-policies",Metropolix::PRESET_SETUP_PAGE,5,0,0,false},
    {"loopy-auto",Metropolix::LOOPY_PAGE,0,0,0,false},
    {"loopy-both",Metropolix::LOOPY_PAGE,1,0,0,false},
    {"loopy-latched",Metropolix::LOOPY_PAGE,0,0,0,false},
    {"chain-empty",Metropolix::CHAIN_PAGE,0,0,0,false},
    {"chain-edit",Metropolix::CHAIN_PAGE,0,0,0,false},
    {"chain-end",Metropolix::CHAIN_PAGE,0,0,0,false},
    {"setup-init",Metropolix::SETUP_PAGE,0,0,0,false},
    {"preset-init",Metropolix::PRESET_SETUP_PAGE,7,0,0,false},
    {"workflow-stage-copy",Metropolix::STAGE_PAGE,0,0,Metropolix::PROBABILITY,false},
    {"workflow-lane-copy",Metropolix::STAGE_PAGE,0,0,0,true},
    {"workflow-restore",Metropolix::LENGTH_PAGE,0,0,0,false},
    {"ui-setup-top",Metropolix::UI_SETUP_PAGE,0,0,0,false},
    {"ui-setup-confirm",Metropolix::UI_SETUP_PAGE,0,0,0,false},
    {"ui-setup-delay",Metropolix::UI_SETUP_PAGE,1,0,0,false},
    {"aux-pending",Metropolix::AUX_PAGE,0,0,0,false},
    {"mod-pending",Metropolix::MOD_ASSIGN_PAGE,0,0,0,true},
    {"length-pending",Metropolix::LENGTH_PAGE,0,0,0,false},
    {"mod-menu",Metropolix::MOD_MENU_PAGE,0,0,0,true},
    {"stage-position-off",Metropolix::STAGE_PAGE,0,0,Metropolix::PROBABILITY,false},
    {"bpm-divider",Metropolix::BPM_PAGE,0,0,0,false},
    {"home-pickup",Metropolix::HOME,0,2,0,false},
    {"preset-pitch-thru",Metropolix::PRESET_SETUP_PAGE,3,0,0,false},
    {"preset-ctrl-thru",Metropolix::PRESET_SETUP_PAGE,5,0,0,false},
    {"preset-ctrl-jump",Metropolix::PRESET_SETUP_PAGE,5,0,0,false},
    {"track-menu-11",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-12",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-13",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-14",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"track-menu-15",Metropolix::TRACK_MENU_PAGE,0,0,0,false},
    {"backup-0",Metropolix::BACKUP_PAGE,0,0,0,false},
    {"backup-1",Metropolix::BACKUP_PAGE,0,0,0,false},
    {"backup-2",Metropolix::BACKUP_PAGE,0,0,0,false},
    {"backup-3",Metropolix::BACKUP_PAGE,0,0,0,false},
    {"backup-notice",Metropolix::BACKUP_PAGE,0,0,0,false},
    {"backup-presets-all",Metropolix::BACKUP_PRESET_PAGE,0,0,0,false},
    {"backup-presets-filled",Metropolix::BACKUP_PRESET_PAGE,0,0,0,false},
    {"backup-presets-bank-1",Metropolix::BACKUP_PRESET_PAGE,0,0,0,false},
    {"backup-presets-bank-8",Metropolix::BACKUP_PRESET_PAGE,0,0,0,false},
    {"backup-presets-notice",Metropolix::BACKUP_PRESET_PAGE,0,0,0,false},
    {"setup-factory",Metropolix::SETUP_PAGE,0,0,0,false},
    {"factory-reset",Metropolix::FACTORY_PAGE,0,0,0,false},
    {"factory-clear",Metropolix::FACTORY_PAGE,0,0,0,false},
    {"factory-reset-cancel",Metropolix::FACTORY_CONFIRM_PAGE,0,0,0,false},
    {"factory-clear-apply",Metropolix::FACTORY_CONFIRM_PAGE,0,0,0,false},
    {"midi-setup-5",Metropolix::MIDI_SETUP_PAGE,0,0,0,false},
    {"mx-mode-0",Metropolix::MX_SETUP_PAGE,0,0,0,false},
{"mx-mode-1",Metropolix::MX_SETUP_PAGE,0,0,0,false},
{"mx-mode-2",Metropolix::MX_SETUP_PAGE,0,0,0,false},
{"mx-mode-3",Metropolix::MX_SETUP_PAGE,0,0,0,false},
{"mx-mode-4",Metropolix::MX_SETUP_PAGE,0,0,0,false},
{"mx-mode-5",Metropolix::MX_SETUP_PAGE,0,0,0,false},
{"mx-mode-6",Metropolix::MX_SETUP_PAGE,0,0,0,false},
    {"mx-track",Metropolix::MX_SETUP_PAGE,1,0,0,false},
    {"mx-lane",Metropolix::MX_SETUP_PAGE,1,0,0,false},
    {"mx-draft",Metropolix::MX_SETUP_PAGE,0,0,0,false},
    {"mx-lane-draft",Metropolix::MX_SETUP_PAGE,1,0,0,false},
    {"midi-setup-0",Metropolix::MIDI_SETUP_PAGE,1,0,0,false},
    {"midi-setup-1",Metropolix::MIDI_SETUP_PAGE,1,0,0,false},
    {"midi-setup-2",Metropolix::MIDI_SETUP_PAGE,1,0,0,false},
    {"midi-setup-3",Metropolix::MIDI_SETUP_PAGE,1,0,0,false},
    {"midi-setup-4",Metropolix::MIDI_SETUP_PAGE,1,0,0,false},
    {"mod-midi-channel",Metropolix::MOD_MENU_PAGE,1,0,0,true},
    {"mod-midi-cc",Metropolix::MOD_MENU_PAGE,1,0,0,true},
    {"bpm-midi",Metropolix::BPM_PAGE,0,0,0,false},
    {"cv-velocity",Metropolix::STAGE_PAGE,0,0,Metropolix::CV,false},
    {"cv-type-velocity",Metropolix::CV_TYPE_PAGE,0,0,Metropolix::CV,false},
    {"cv-type-pending",Metropolix::CV_TYPE_PAGE,0,0,Metropolix::CV,false},
    {"sync-to-panel",Metropolix::HOME,0,2,0,false},
    {"mod-units-pitch",Metropolix::STAGE_PAGE,0,0,0,true},
    {"mod-units-tempo",Metropolix::STAGE_PAGE,0,0,0,true},
    {"mod-units-probability",Metropolix::STAGE_PAGE,0,0,0,true},
    {"mod-units-offset",Metropolix::STAGE_PAGE,0,0,0,true},
    {"mod-units-octave",Metropolix::STAGE_PAGE,0,0,0,true},
    {"mod-all-vertices",Metropolix::STAGE_PAGE,0,0,0,true},
    {"mod-gate-midi",Metropolix::STAGE_PAGE,0,0,0,true},

};
const int count=sizeof(cases)/sizeof(cases[0]);
int index=(qaFrame-20)/6;
if(qaFrame>=20 && index<count) {
    const auto& test=cases[index];
    if((qaFrame-20)%6==0) {
        m->uiPage.store(test.page);m->uiField.store(test.field);m->uiHome.store(test.home);m->selectedRow.store(test.row);m->uiMod.store(test.mod);
        m->uiStageDetail.store(true);m->selectedStage.store(2);m->selectedLane.store(1);
        m->uiAlt.store(false);m->clearWorkflows();m->cancelDraft();m->params[Metropolix::SHOW_POSITION].setValue(1);m->params[Metropolix::SCREEN_LEVEL].setValue(4);m->clearPickup();
        m->orderShortcutHeld.store(false);m->orderShortcutUsed.store(false);
        m->params[Metropolix::outputTrackParam(0,Metropolix::VELOCITY_SOURCE)].setValue(0);
        if(std::string(test.name)=="setup-init")m->setupCursor.store(Metropolix::SetupInit);
        if(std::string(test.name)=="preset-init")m->params[Metropolix::INIT_PRESET].setValue(64);
        if(std::string(test.name)=="workflow-stage-copy" || std::string(test.name)=="workflow-lane-copy"){m->workflowStageHeld=1;m->workflowStageSource=0;m->workflowLaneCopy=test.mod;}
        if(std::string(test.name)=="workflow-restore"){m->workflowMessage="ALT STATE RESTORED";m->workflowUntil=system::getTime()+2;}
        if(!test.mod && (test.row==Metropolix::CV || test.page==Metropolix::CV_TYPE_PAGE)) {
            std::string name=test.name;int t=m->selectedTrack.load();
            int type=name.find("unipolar")!=std::string::npos?1:name.find("gates")!=std::string::npos?2:0;
            m->params[Metropolix::cvTypeParam(t)].setValue(type);
            const float values[]={0,1.5f,4,-2,-2,0,3,1};
            for(int s=0;s<8;++s){m->params[Metropolix::stageParam(t,s,Metropolix::CV)].setValue(type==2?(s==2 || s==5?5:0):values[s]);m->params[Metropolix::PULSES+s].setValue(4);m->params[Metropolix::cvRampParam(t,s)].setValue(name=="stage-cv"?0:s%3);}
            m->params[Metropolix::cvRampParam(t,2)].setValue(name=="stage-cv-sramp"?1:name=="stage-cv"?0:2);
            m->uiAlt.store(name=="stage-cv-all");
        }
        if(test.row>=Metropolix::PITCH_OVERRIDE && !test.mod) {
            const int pitches[]={-1,12,28,60,7,-1,40,55},gates[]={-1,0,82,100,13,-1,88,1};
            for(int s=0;s<8;++s)m->params[Metropolix::stageParam(m->selectedTrack.load(),s,test.row)].setValue(test.row==Metropolix::PITCH_OVERRIDE?pitches[s]:gates[s]);
        }
        if(test.page==Metropolix::A_PAGE || test.page==Metropolix::B_PAGE){
            int source=0;std::string name=test.name;
            if(name.find("output-kind-")==0){int kind=std::stoi(name.substr(12));source=metro::outputSource(kind,metro::outputIsMod(kind)?7:1);}
            m->params[Metropolix::OUTPUT_SOURCE_A+(test.page==Metropolix::B_PAGE)].setValue(source);m->outputDraft[0].store(-1);m->outputDraft[1].store(-1);
        }
        if(test.page==Metropolix::TRACK_MENU_PAGE){
            m->trackMenuCursor.store(std::string(test.name)=="track-global-reset"?8:std::stoi(std::string(test.name).substr(11)));
            m->params[Metropolix::trackBehaviorParam(m->selectedTrack.load(),Metropolix::GATE_SOURCE)].setValue(6);
            m->params[Metropolix::trackBehaviorParam(m->selectedTrack.load(),Metropolix::REST_PITCH)].setValue(1);
        }
        m->clearLoopy();
        if(test.page==Metropolix::CLOCK_OUT_PAGE){std::string name=test.name;m->params[Metropolix::CLOCK_OUT_DIV].setValue(name=="clock-default"?1:name=="clock-max"?256:4);m->params[Metropolix::CLOCK_OUT_OFFSET].setValue(name=="clock-max"?255:name=="clock-default"?0:2);}
        if(test.page==Metropolix::SLIDE_PAGE)m->params[Metropolix::slideTypeParam(m->selectedTrack.load())].setValue(std::string(test.name)=="slide-analog"?1:std::string(test.name)=="slide-acid"?2:0);
        m->scaleEditing.store(false);m->params[Metropolix::SCALE_LOCK].setValue(-1);m->params[Metropolix::SCALE].setValue(1);
        if(test.page==Metropolix::SCALE_PAGE) {
            std::string name=test.name;
            if(name=="scale-world")m->params[Metropolix::SCALE].setValue(28);
            if(name=="scale-locked" || name=="scale-search")m->params[Metropolix::SCALE_LOCK].setValue(1);
            if(name=="scale-search")m->params[Metropolix::SCALE].setValue(7);
            if(name=="scale-user" || name=="scale-edit"){m->params[Metropolix::SCALE].setValue(148);m->params[Metropolix::SCALE_LOCK].setValue(148);m->params[Metropolix::userScaleParam(99)].setValue(0x91);m->params[Metropolix::userScaleParam(99,true)].setValue(9);m->params[Metropolix::SCALE_NOTE].setValue(4);m->scaleEditing.store(name=="scale-edit");}
        }
        if(test.page==Metropolix::LOOPY_PAGE){m->params[Metropolix::LOOPY_PULSES].setValue(test.field?64:0);m->params[Metropolix::LOOPY_TARGET].setValue(test.field);if(std::string(test.name)=="loopy-latched"){m->requestLoopy(18);m->latchLoopy();}}
        if(test.page==Metropolix::SWING_PAGE)m->params[Metropolix::swingParam(m->selectedTrack.load())].setValue(std::string(test.name)=="swing-max"?78:50);
        if(test.page==Metropolix::CHAIN_PAGE) {
            Metropolix::ChainState state;
            if(std::string(test.name)!="chain-empty")for(int i=0;i<32;++i) {metro::ChainLink l;l.slot=i==31?55:i;l.repeats=i==31?64:i+1;l.mutes=i%8;state.working.insert(i,l);}
            m->setChainState(state);m->chainCursor.store(std::string(test.name)=="chain-end"?66:63);m->chainEdit.store(std::string(test.name)=="chain-edit"?2:0);
            if(!state.working.size)m->chainCursor.store(0);
        }
        if(test.page==Metropolix::ACCUM_PAGE)m->params[Metropolix::accumModeParam(0)].setValue(std::string(test.name)=="accum-stage"?0:1);
        if(test.page==Metropolix::CTRL_PAGE) {
            m->selectedCtrl.store(test.field);
            m->params[Metropolix::ctrlParam(test.field,Metropolix::CTRL_DESTINATION)].setValue(test.field?metro::CtrlPulseCount:metro::CtrlGateLength);
            m->params[Metropolix::ctrlParam(test.field,Metropolix::CTRL_VALUE)].setValue(test.field?.5f:.2f);
            m->params[Metropolix::ctrlParam(test.field,Metropolix::CTRL_ASSIGNMENT)].setValue(test.field?1:2);
            if(std::string(test.name)=="ctrl-accum-dir")m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION)].setValue(metro::CtrlAccumDir);
        }
        if(std::string(test.name)=="ctrl-owned-accum")m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION)].setValue(metro::CtrlAccumMode);
        if(test.page==Metropolix::AUX_PAGE) {
            m->selectedAux.store(test.field);
            m->params[Metropolix::auxParam(test.field,Metropolix::AUX_AMOUNT)].setValue(1);
            m->params[Metropolix::auxParam(test.field,Metropolix::AUX_DESTINATION)].setValue(test.field?metro::ModMute:metro::ModPitchPre);
            m->params[Metropolix::auxParam(test.field,Metropolix::AUX_ASSIGNMENT)].setValue(test.field?2:0);
            if(std::string(test.name)=="aux-accum-reset" || std::string(test.name)=="aux-accumulate")m->params[Metropolix::auxParam(0,Metropolix::AUX_DESTINATION)].setValue(std::string(test.name)=="aux-accum-reset"?metro::ModAccumReset:metro::ModAccumulate);
        }
        if(std::string(test.name)=="mod-accum-gate")m->params[Metropolix::modParam(1,Metropolix::MOD_DESTINATION)].setValue(metro::ModAccumReset);
        if(std::string(test.name)=="mod-extended-gate")m->params[Metropolix::modParam(1,Metropolix::MOD_DESTINATION)].setValue(metro::ModSlide);
        if(std::string(test.name)=="mod-global-home")m->params[Metropolix::modParam(1,Metropolix::MOD_DESTINATION)].setValue(metro::ModLoopy);
        if(std::string(test.name)=="setup-global")m->setupCursor.store(Metropolix::SetupClock);
        if(std::string(test.name)=="global-clock-src"){m->globalCursor.store(0);m->params[Metropolix::CLOCK_SOURCE].setValue(1);}
        if(std::string(test.name)=="global-reset-run"){m->globalCursor.store(2);m->params[Metropolix::RESET_INPUT_MODE].setValue(2);}
        if(std::string(test.name)=="global-bottom-pitch"){m->globalCursor.store(3);m->params[Metropolix::BOTTOM_PITCH].setValue(7);}
        if(std::string(test.name)=="global-tuner-item")m->globalCursor.store(5);
        if(test.page==Metropolix::TUNER_PAGE){m->params[Metropolix::TUNER_OCTAVE].setValue(std::string(test.name)=="tuner-low"?-5:5);m->params[Metropolix::TUNER_GATE].setValue(std::string(test.name)=="tuner-clock");}
        if(std::string(test.name)=="track-global-reset")m->params[Metropolix::TRACK_GLOBAL_RESET_1].setValue(0);
        if(std::string(test.name)=="aux-global-tempo")m->params[Metropolix::auxParam(0,Metropolix::AUX_DESTINATION)].setValue(metro::ModTempo);
        if(std::string(test.name)=="aux-loopy-offset")m->params[Metropolix::auxParam(1,Metropolix::AUX_DESTINATION)].setValue(metro::ModLoopyOffset);
        if(std::string(test.name)=="aux-track-reset")m->params[Metropolix::auxParam(1,Metropolix::AUX_DESTINATION)].setValue(metro::ModReset);
        std::string setupName=test.name;
        if(test.page==Metropolix::ORDER_PAGE){auto ids=Metropolix::orderParams(test.mod,test.mod?m->selectedLane.load():m->selectedTrack.load());m->params[ids[0]].setValue(setupName=="order-brownian-reverse"?17:setupName=="order-mod-reverse"?19:0);m->params[ids[1]].setValue(test.field==1);}
        if(setupName=="order-shortcut-saved"){m->workflowMessage="SHORTCUT 8 SAVED";m->workflowUntil=system::getTime()+5;}
        if(setupName=="order-ctrl-blocked"){m->workflowMessage="ORDER OWNED BY CTRL";m->workflowUntil=system::getTime()+5;}
        if(setupName=="home-pickup"){m->params[Metropolix::LOAD_PITCH].setValue(Metropolix::PitchThru);m->params[Metropolix::LOAD_CTRL].setValue(Metropolix::CtrlThru);for(int i=0;i<10;++i){int id=Metropolix::panelParam(i);m->panelPickup[i].recall(i<8?.1f:-.8f,i<8?(.9f-i*.08f):.5f);m->params[id].setValue(i<8?.1f:-.8f);}m->publishPickup();}
        if(setupName=="preset-pitch-thru")m->params[Metropolix::LOAD_PITCH].setValue(Metropolix::PitchThru);
        if(setupName=="preset-ctrl-thru" || setupName=="preset-ctrl-jump")m->params[Metropolix::LOAD_CTRL].setValue(setupName=="preset-ctrl-thru"?Metropolix::CtrlThru:Metropolix::CtrlJump);

        if(test.page==Metropolix::UI_SETUP_PAGE){m->uiSetupCursor.store(setupName=="ui-setup-top"?0:setupName=="ui-setup-confirm"?2:7);m->params[Metropolix::ENC_CONFIRM].setValue(1);m->params[Metropolix::CV_OUT_DELAY].setValue(21);}
        if(setupName=="aux-pending" || setupName=="mod-pending" || setupName=="length-pending"){int id=screenParam(m);m->uiDraftParam=id;m->uiDraftBase=m->params[id].getValue();m->uiDraftOrigin=m->getParamQuantity(id)->getValue();m->uiDraftValue=setupName=="length-pending"?4:metro::ModTempo;}
        if(setupName=="mod-menu")m->modMenuCursor.store(3);
        if(test.page==Metropolix::MX_SETUP_PAGE){int mode=setupName.find("mx-mode-")==0?std::stoi(setupName.substr(8)):setupName=="mx-track"?5:6;m->params[Metropolix::MX_MODE].setValue(mode);m->params[Metropolix::MX_TRACK].setValue(1);m->params[Metropolix::MX_LANE].setValue(7);if(setupName=="mx-draft" || setupName=="mx-lane-draft"){int id=setupName=="mx-draft"?Metropolix::MX_MODE:Metropolix::MX_LANE;m->uiDraftParam=id;m->uiDraftBase=m->params[id].getValue();m->uiDraftValue=setupName=="mx-draft"?0:3;}}
        if(test.page==Metropolix::BACKUP_PAGE){m->backupCursor.store(setupName=="backup-notice"?3:std::stoi(setupName.substr(7)));m->backupNotice="IMPORTED / STOPPED";m->backupNoticeUntil=setupName=="backup-notice"?system::getTime()+5:0;}
        if(test.page==Metropolix::BACKUP_PRESET_PAGE){m->backupPresetCursor.store(setupName=="backup-presets-all"?0:setupName=="backup-presets-filled"?1:setupName=="backup-presets-bank-1"?2:9);m->backupNotice="PRESETS SAVED";m->backupNoticeUntil=setupName=="backup-presets-notice"?system::getTime()+5:0;}
        if(setupName=="setup-factory")m->setupCursor.store(Metropolix::SetupVersion);
        if(factoryPage(m)){m->factoryCursor.store(setupName.find("factory-clear")==0?1:0);m->factoryConfirmCursor=setupName=="factory-clear-apply"?1:0;m->backupNoticeUntil=0;}
        if(test.page==Metropolix::MIDI_SETUP_PAGE)m->midiSetupCursor.store(std::stoi(setupName.substr(11)));
        if(setupName=="mod-midi-channel")m->modMenuCursor.store(0);
        if(setupName=="mod-midi-cc"){m->modMenuCursor.store(1);m->params[Metropolix::midiModParam(1,Metropolix::MIDI_MOD_CC)].setValue(128);}
        if(setupName=="bpm-midi"){m->params[Metropolix::CLOCK_SOURCE].setValue(3);m->externalClock.store(true);m->displayBpm.store(120);}
        if(setupName=="cv-velocity"){m->params[Metropolix::outputTrackParam(0,Metropolix::VELOCITY_SOURCE)].setValue(3);m->params[Metropolix::stageParam(0,2,Metropolix::CV)].setValue(2.5);}
        if(setupName=="cv-type-velocity")m->params[Metropolix::outputTrackParam(0,Metropolix::VELOCITY_SOURCE)].setValue(3);
        if(setupName=="cv-type-pending"){int id=Metropolix::cvTypeParam(0);m->uiDraftParam=id;m->uiDraftBase=m->params[id].getValue();m->uiDraftValue=2;}
        if(setupName=="sync-to-panel"){m->workflowMessage="SYNC TO PANEL";m->workflowUntil=system::getTime()+2;}
        if(setupName.find("mod-units-")==0 || setupName=="mod-all-vertices" || setupName=="mod-gate-midi"){
            int destination=setupName=="mod-units-tempo"?metro::ModTempo:setupName=="mod-units-probability"?metro::ModProbability:setupName=="mod-units-offset"?metro::ModPitchOffset:setupName=="mod-units-octave"?metro::ModOctave:setupName=="mod-gate-midi"?metro::ModMute:metro::ModPitchPre;
            m->params[Metropolix::modParam(1,Metropolix::MOD_DESTINATION)].setValue(destination);m->params[Metropolix::modParam(1,Metropolix::MOD_TYPE)].setValue(0);m->params[Metropolix::modParam(1,Metropolix::MOD_VALUE+2)].setValue(setupName=="mod-gate-midi"?2:setupName=="mod-units-offset"?.125f:setupName=="mod-units-octave"?-1.9f:1);
            m->uiAlt.store(setupName=="mod-all-vertices");m->params[Metropolix::midiModParam(1,Metropolix::MIDI_MOD_CC)].setValue(setupName=="mod-gate-midi"?21:0);
        }

        if(setupName=="stage-position-off")m->params[Metropolix::SHOW_POSITION].setValue(0);
        if(setupName=="bpm-divider"){m->params[Metropolix::CLOCK_SOURCE].setValue(2);m->externalClock.store(true);m->displayBpm.store(120);m->params[Metropolix::EXT_CLOCK_DIV].setValue(6);}

    }
    if((qaFrame-20)%6==3) {
        metroqa::screenshot(dir+"/"+test.name+".png");
        auto* screen=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));
        if(std::string(test.name)=="bpm-modulated")m->displayBpm.store(240);
        if(test.page==Metropolix::AUX_PAGE && !test.field)m->auxVoltage[0].store(2.5f);
        OledCanvas c=screen->render();int lit=0;for(auto& row:c.pixels)for(uint8_t v:row)lit+=v>0;
        check(lit>40 && lit<6000,std::string(test.name)+" renders a populated display");
        // Native pixel map for compact contact sheets / regression inspection.
        std::ofstream pgm(dir+"/"+test.name+".pgm",std::ios::binary);pgm<<"P5\n128 64\n255\n";pgm.write(reinterpret_cast<char*>(c.pixels),128*64);
    }
}
if(qaFrame==20+count*6+3){check(!qaFailed,"DISPLAY QA COMPLETE");APP->window->close();}
