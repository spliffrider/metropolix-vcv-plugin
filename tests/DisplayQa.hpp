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
if(qaFrame==12)APP->window->screenshot(dir+"/panel.png");
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
    drag(find("Encoder"),80);check(m->integer(Metropolix::ctrlParam(0,Metropolix::CTRL_ASSIGNMENT))<2,"CTRL target edits through encoder");APP->history->undo();
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
if(qaFrame==17)APP->window->screenshot(dir+"/reference-pose.png");
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
    {"length",Metropolix::LENGTH_PAGE,0,0,0,false},
    {"division",Metropolix::DIV_PAGE,1,0,0,false},
    {"swing-straight",Metropolix::SWING_PAGE,0,0,Metropolix::SLIDE,false},
    {"swing-max",Metropolix::SWING_PAGE,0,0,Metropolix::SLIDE,false},
    {"swing-mod",Metropolix::SWING_PAGE,0,0,0,true},
    {"slide-time",Metropolix::SLIDE_PAGE,0,0,0,false},
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
    {"output-a",Metropolix::A_PAGE,0,0,0,false},
    {"output-b",Metropolix::B_PAGE,0,0,0,false},
    {"aux-cv",Metropolix::AUX_PAGE,0,0,Metropolix::SLIDE,false},
    {"aux-gate",Metropolix::AUX_PAGE,1,0,Metropolix::SLIDE,false},
    {"accum-stage",Metropolix::ACCUM_PAGE,2,0,Metropolix::ACCUMULATION,false},
    {"accum-track",Metropolix::ACCUM_PAGE,2,0,Metropolix::ACCUMULATION,false},
    {"accum-options",Metropolix::ACCUM_PAGE,4,0,Metropolix::ACCUMULATION,false},
    {"ctrl-gate",Metropolix::CTRL_PAGE,0,0,Metropolix::SLIDE,false},
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
};
const int count=sizeof(cases)/sizeof(cases[0]);
int index=(qaFrame-20)/6;
if(qaFrame>=20 && index<count) {
    const auto& test=cases[index];
    if((qaFrame-20)%6==0) {
        m->uiPage.store(test.page);m->uiField.store(test.field);m->uiHome.store(test.home);m->selectedRow.store(test.row);m->uiMod.store(test.mod);
        m->uiStageDetail.store(true);m->selectedStage.store(2);m->selectedLane.store(1);
        if(test.row>=Metropolix::PITCH_OVERRIDE && !test.mod) {
            const int pitches[]={-1,12,28,60,7,-1,40,55},gates[]={-1,0,82,100,13,-1,88,1};
            for(int s=0;s<8;++s)m->params[Metropolix::stageParam(m->selectedTrack.load(),s,test.row)].setValue(test.row==Metropolix::PITCH_OVERRIDE?pitches[s]:gates[s]);
        }
        m->clearLoopy();
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
        }
        if(std::string(test.name)=="ctrl-owned-accum")m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION)].setValue(metro::CtrlAccumMode);
        if(test.page==Metropolix::AUX_PAGE) {
            m->selectedAux.store(test.field);
            m->params[Metropolix::auxParam(test.field,Metropolix::AUX_AMOUNT)].setValue(1);
            m->params[Metropolix::auxParam(test.field,Metropolix::AUX_DESTINATION)].setValue(test.field?metro::ModMute:metro::ModPitchPre);
            m->params[Metropolix::auxParam(test.field,Metropolix::AUX_ASSIGNMENT)].setValue(test.field?2:0);
        }
    }
    if((qaFrame-20)%6==3) {
        APP->window->screenshot(dir+"/"+test.name+".png");
        auto* screen=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));
        if(test.page==Metropolix::AUX_PAGE && !test.field)m->auxVoltage[0].store(2.5f);
        OledCanvas c=screen->render();int lit=0;for(auto& row:c.pixels)for(uint8_t v:row)lit+=v>0;
        check(lit>40 && lit<6000,std::string(test.name)+" renders a populated display");
        // Native pixel map for compact contact sheets / regression inspection.
        std::ofstream pgm(dir+"/"+test.name+".pgm",std::ios::binary);pgm<<"P5\n128 64\n255\n";pgm.write(reinterpret_cast<char*>(c.pixels),128*64);
    }
}
if(qaFrame==20+count*6+3){check(!qaFailed,"DISPLAY QA COMPLETE");APP->window->close();}
