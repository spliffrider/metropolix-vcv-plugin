// Production widget callbacks in an isolated, real Rack patch with MX attached.
using M=Metropolix;
static app::ModuleWidget* companion=nullptr;
static Vec anchor;
static int syncHistory=0;
if(std::getenv("METROPOLIX_FIDELITY_REOPEN_QA")){
    if(qaFrame==6){
        check(m->integer(M::cvTypeParam(0))==0 && m->getParamQuantity(M::cvTypeParam(0))->getDisplayValueString()=="Unipolar + (Velocity CV)","saved CV type and effective velocity range survive Rack reopen");
        check(m->getParamQuantity(M::modParam(0,M::MOD_VALUE))->getDisplayValueString()=="+12" && !m->pickupActiveMask.load(),"destination units and synced pickup survive Rack reopen");
        check(m->rightExpander.module && m->rightExpander.module->model==modelMX,"adjacent MX link survives Rack save/reopen");
        check(!qaFailed,"FIDELITY REOPEN QA COMPLETE");APP->window->close();
    }return;
}
widget::EventContext ctx;
event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragMove move;move.context=&ctx;move.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;
if(qaFrame==4){auto state=m->getChainState();state.enabled=false;m->setChainState(state);m->sendChainCommand();}
if(qaFrame==6){
    m->params[M::RUN].setValue(0);m->expanded.store(false);m->selectedTrack.store(0);m->uiMod.store(false);settings::tooltips=true;
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->params[M::ENC_CONFIRM].setValue(1);screenPage(m,M::LENGTH_PAGE);m->params[M::trackParam(0,M::LENGTH)].setValue(8);
    auto* encoder=find("Encoder");int h=APP->history->actionIndex;encoder->onDragStart(start);move.mouseDelta=Vec(0,44);encoder->onDragMove(move);
    check(m->uiDraftValue==6 && m->integer(M::trackParam(0,M::LENGTH))==8,"held encoder previews stage length without applying it");
    check(encoder->adjustmentTip && encoder->adjustmentTip->text.find("6"+encoder->getParamQuantity()->getUnit()+" ?")!=std::string::npos && encoder->adjustmentTip->text.find("Press encoder to apply")!=std::string::npos,"held adjustment readout shows pending value and confirmation hint");
    event::Leave leave;leave.context=&ctx;encoder->onLeave(leave);encoder->step();check(encoder->adjustmentTip && APP->history->actionIndex==h,"pending drag keeps one live readout after leaving control");
    encoder->onDragEnd(end);check(!encoder->adjustmentTip && APP->history->actionIndex==h,"unconfirmed release adds no history");click(encoder);check(m->integer(M::trackParam(0,M::LENGTH))==6 && APP->history->actionIndex==h+1,"pending drag applies with one encoder confirmation");APP->history->undo();
    for(int page:{M::GLOBAL_SETUP_PAGE,M::UI_SETUP_PAGE,M::MIDI_SETUP_PAGE,M::PRESET_SETUP_PAGE}){screenPage(m,page);click(find("EXIT"));check(m->uiPage.load()==M::SETUP_PAGE,"EXIT returns setup subpage to its parent");}
    screenPage(m,M::MIDI_SETUP_PAGE);openBackup(m);click(find("EXIT"));check(m->uiPage.load()==M::MIDI_SETUP_PAGE,"EXIT returns desktop MIDI backup submenu to MIDI Setup");click(find("EXIT"));click(find("EXIT"));check(m->uiPage.load()==M::HOME,"successive EXIT presses reach Home");
    screenPage(m,M::TUNER_PAGE);m->tunerActive.store(true);click(find("EXIT"));check(m->uiPage.load()==M::GLOBAL_SETUP_PAGE && !m->tunerActive.load(),"EXIT stops tuner and returns to Global Setup");
    m->params[M::LOAD_PITCH].setValue(M::PitchThru);m->params[M::LOAD_CTRL].setValue(M::CtrlThru);
    for(int i=0;i<10;++i)m->params[M::panelParam(i)].setValue(i<8?.1f:-.5f);
    auto snapshot=m->capturePreset();for(int i=0;i<10;++i)snapshot.values[M::presetIndex(M::panelParam(i))]=i<8?.8f:.5f;
    m->putPreset(0,snapshot);m->requestPreset(0);m->sendPresetCommand();
}
if(qaFrame==10){
    check(true,string::f("pickup fixture: mask=%u pending=%d loaded=%d pitchPolicy=%d ctrlPolicy=%d chain=%d",m->pickupActiveMask.load(),m->pendingSlot.load(),m->lastLoadedSlot.load(),m->integer(M::LOAD_PITCH),m->integer(M::LOAD_CTRL),m->chainEnabled.load()));
    check(m->pickupActiveMask.load()==1023,"all eight pitches and both CTRL positions await pickup");
    screenPage(m,M::HOME);syncHistory=APP->history->actionIndex;auto* exit=find("EXIT");exit->onDragStart(start);exit->heldSince-=1.1;exit->step();exit->onDragEnd(end);
    check(APP->history->actionIndex==syncHistory+1 && m->workflowMessage=="SYNC TO PANEL","long EXIT creates one panel sync action and display notice");
}
if(qaFrame==13){
    bool values=true;for(int i=0;i<10;++i)values &= m->panelValue(M::panelParam(i))==(i<8?.1f:-.5f);
    check(!m->pickupActiveMask.load() && values,"panel sync releases all ten remembered positions to visible controls");APP->history->undo();
}
if(qaFrame==16){
    bool values=true;for(int i=0;i<10;++i)values &= m->panelValue(M::panelParam(i))==(i<8?.8f:.5f) && m->params[M::panelParam(i)].getValue()==(i<8?.1f:-.5f);
    check(m->pickupActiveMask.load()==1023 && values,"panel sync undo restores all ten pickup targets without moving controls");APP->history->redo();
}
if(qaFrame==19){
    check(!m->pickupActiveMask.load(),"panel sync redo releases all ten controls");int h=APP->history->actionIndex;syncPanel(m);check(APP->history->actionIndex==h,"sync without remembered controls adds no history");
    anchor=box.pos;for(auto* child:APP->scene->rack->getModuleContainer()->children){auto* candidate=dynamic_cast<app::ModuleWidget*>(child);if(candidate->getModel()==modelMX)companion=candidate;}
    check(companion && companion->box.pos==Vec(box.pos.x+box.size.x,box.pos.y),"real MX starts adjacent to compact panel");m->expanded.store(true);m->modEditor.store(true);m->uiMod.store(true);
}
auto adjacency=[&](const char* label){
    check(companion && box.pos==anchor && companion->box.pos==Vec(box.pos.x+box.size.x,box.pos.y),label);
    check(companion && m->rightExpander.module==companion->module && companion->module->leftExpander.module==m,"Rack retains reciprocal MX engine link after resizing");
    bool overlap=false;for(auto* child:APP->scene->rack->getModuleContainer()->children){auto* other=dynamic_cast<app::ModuleWidget*>(child);if(other && other!=this)overlap |= box.intersects(other->box);}
    check(!overlap,"resized main panel does not overlap another Rack module");
};
if(qaFrame==22){
    adjacency("opening optional editor keeps MX attached and main panel anchored");
    m->selectedLane.store(0);m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModPitchPre);m->params[M::modParam(0,M::MOD_VALUE)].setValue(1);m->params[M::modParam(0,M::MOD_TYPE)].setValue(0);
    ValueCell* field=nullptr;for(auto* child:modPanel->children)if(auto* c=dynamic_cast<ValueCell*>(child))if(c->modProperty==M::MOD_VALUE)field=c;
    if(field)field->step();check(field && field->valueText()=="+12 st","optional MOD editor shows destination unit and offset");
    if(field){field->onDragStart(start);move.mouseDelta=Vec(0,-10);field->onDragMove(move);check(field->adjustmentTip && field->adjustmentTip->text.find("st")!=std::string::npos,"optional MOD drag readout uses destination units");field->onDragEnd(end);APP->history->undo();}

}
if(qaFrame==24){metroqa::screenshot(dir+"/mx-expanded-units.png");m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModTempo);m->params[M::modParam(0,M::MOD_VALUE)].setValue(5);}
if(qaFrame==26){auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));OledCanvas c;auto label=display->modValueLabel(c,0,5);check(label.find("BPM")!=std::string::npos && c.width(label)<=40,"maximum BPM OLED label fits without dropping its unit");metroqa::screenshot(dir+"/mx-expanded-tempo.png");m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModPitchPre);m->params[M::modParam(0,M::MOD_VALUE)].setValue(1);m->expanded.store(false);}
if(qaFrame==27){
    adjacency("closing optional editor keeps MX attached and main panel anchored");
    m->uiMod.store(false);m->params[M::outputTrackParam(0,M::VELOCITY_SOURCE)].setValue(3);m->params[M::cvTypeParam(0)].setValue(0);screenPage(m,M::CV_TYPE_PAGE);
    auto* encoder=find("Encoder");encoder->onDragStart(start);move.mouseDelta=Vec(0,-44);encoder->onDragMove(move);
    check(m->integer(M::cvTypeParam(0))==0 && m->uiDraftValue==2 && encoder->adjustmentTip && encoder->adjustmentTip->text.find("Gate Toggle ?")!=std::string::npos,"CV type drag previews Gates while retaining saved Bipolar");encoder->onDragEnd(end);
    auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));auto pixels=display->render();std::ofstream pgm(dir+"/cv-type-draft.pgm",std::ios::binary);pgm<<"P5\n128 64\n255\n";pgm.write(reinterpret_cast<char*>(pixels.pixels),128*64);
}
if(qaFrame==30){
    metroqa::screenshot(dir+"/cv-type-pending.png");click(find("EXIT"));check(m->integer(M::cvTypeParam(0))==0 && m->uiDraftParam<0,"EXIT cancels CV type preview while retaining effective velocity override");
    APP->patch->save(dir+"/fidelity-roundtrip.vcv");
}
if(qaFrame==33){check(!qaFailed,"FIDELITY UI QA COMPLETE");APP->window->close();}
