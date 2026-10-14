if(std::getenv("METROPOLIX_ACCUM_ROUTING_REOPEN_QA")){
    if(qaFrame==6){check(m->integer(Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION))==metro::CtrlAccumDir && m->integer(Metropolix::auxParam(0,Metropolix::AUX_DESTINATION))==metro::ModAccumReset,"native save/reopen retains CTRL and AUX accumulator assignments");for(int d=metro::ModAccumulate;d<=metro::ModAccumReset;++d)check(m->integer(Metropolix::modParam(d-metro::ModAccumulate,Metropolix::MOD_DESTINATION))==d,"native save/reopen retains each internal accumulator route");check(m->getPreset(0).occupied,"saved bank retains accumulator routing");check(true,"ACCUM ROUTING REOPEN QA COMPLETE");APP->window->close();}return;
}
if(qaFrame==6){
    using M=Metropolix;m->params[M::RUN].setValue(0);m->expanded.store(false);
    click(find("CTRL 1 assignment"));drag(find("Encoder"),-2000);check(m->integer(M::ctrlParam(0,M::CTRL_DESTINATION))==metro::CtrlAccumDir,"held encoder reaches Accum Dir at end of CTRL list");
    check(find("CTRL 1")->tooltipText().find("exact noon = freeze")!=std::string::npos,"Accum Dir hover explains forward/freeze/reverse");
    m->params[M::ctrlParam(0,M::CTRL_VALUE)].setValue(.5f);press(find("CTRL 1"),GLFW_MOUSE_BUTTON_RIGHT);
    auto* centre=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text=="Centre knob (noon)";}));check(centre!=nullptr,"controller menu offers exact noon for freezing");if(centre){event::Action action;centre->onAction(action);}closeMenus();check(m->params[M::ctrlParam(0,M::CTRL_VALUE)].getValue()==0,"noon menu applies exact zero");APP->history->undo();check(m->params[M::ctrlParam(0,M::CTRL_VALUE)].getValue()==.5f,"centre action restores previous position on undo");
    click(find("AUX X assignment"));m->getParamQuantity(M::auxParam(0,M::AUX_DESTINATION))->setDisplayValueString("Accum Reset");check(m->integer(M::auxParam(0,M::AUX_DESTINATION))==metro::ModAccumReset,"native named entry selects accumulator-only reset from the expanded destination list");
    check(find("AUX X attenuverter")->tooltipText().find("Gates/triggers")!=std::string::npos,"AUX hover explains gate thresholds and inversion");
    for(int d=metro::ModAccumulate;d<=metro::ModAccumReset;++d){int lane=d-metro::ModAccumulate;m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(d);m->selectedLane.store(lane);m->uiMod.store(true);m->uiPage.store(M::STAGE_PAGE);m->uiStageDetail.store(true);m->selectedStage.store(0);click(find("Stage 1 / SLIDE"));check(m->params[M::modParam(lane,M::MOD_VALUE)].getValue()==5,"accumulator MOD stage click toggles a gate rather than dragging a bipolar value");APP->history->undo();check(m->params[M::modParam(lane,M::MOD_VALUE)].getValue()==0,"MOD gate toggle undoes");}
    m->putPreset(0,m->capturePreset());m->uiMod.store(false);m->uiPage.store(M::CTRL_PAGE);m->selectedCtrl.store(0);APP->patch->save(dir+"/accum-routing-roundtrip.vcv");
}
if(qaFrame==8)metroqa::screenshot(dir+"/accum-routing-controls.png");
if(qaFrame==12){check(!qaFailed,"ACCUM ROUTING UI QA COMPLETE");APP->window->close();}
