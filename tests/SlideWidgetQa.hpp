if(std::getenv("METROPOLIX_SLIDE_REOPEN_QA")) {
    if(qaFrame==6){check(m->integer(Metropolix::slideTypeParam(0))==metro::AnalogSlide && m->integer(Metropolix::slideTypeParam(1))==metro::AcidSlide && m->getPreset(0).occupied && m->getPreset(0).values[458]==1 && m->getPreset(0).values[459]==2,"native patch and preset preserve both slide types");check(true,"SLIDE REOPEN QA COMPLETE");APP->window->close();}return;
}
auto begin=[&](HardwareControl* c){widget::EventContext context;event::DragStart e;e.context=&context;e.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragStart(e);};
auto end=[&](HardwareControl* c){widget::EventContext context;event::DragEnd e;e.context=&context;e.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragEnd(e);};
if(qaFrame==6){
    using M=Metropolix;for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::RUN].setValue(0);click(find("TRK 1"));click(find("SLIDE"));
    check(m->uiPage.load()==M::SLIDE_PAGE && screenParam(m)==M::trackParam(0,M::SLIDE_TIME),"SLIDE enters per-track amount page");
    click(find("Encoder"));check(m->integer(M::slideTypeParam(0))==2,"encoder cycles Tempo to Acid");click(find("Encoder"));check(m->integer(M::slideTypeParam(0))==1,"encoder cycles Acid to Analog");APP->history->undo();check(m->integer(M::slideTypeParam(0))==2,"slide type has Rack undo");APP->history->redo();
    float before=m->params[M::trackParam(0,M::SLIDE_TIME)].getValue();drag(find("Encoder"),-60);check(m->params[M::trackParam(0,M::SLIDE_TIME)].getValue()>before,"amount increases with upward drag");APP->history->undo();check(m->params[M::trackParam(0,M::SLIDE_TIME)].getValue()==before,"amount drag undoes");
    auto* button=find("SLIDE");begin(button);click(find("Stage 7 / ACCUM"));end(button);check(m->uiPage.load()==M::SLIDE_PAGE && m->integer(M::trackParam(0,M::SLIDE_TIME))==85,"held SLIDE shortcut keeps slide page");APP->history->undo();check(m->params[M::trackParam(0,M::SLIDE_TIME)].getValue()==before,"held slide shortcut has one undo");
    m->params[M::trackParam(0,M::SLIDE_TIME)].setValue(70);m->params[M::slideTypeParam(0)].setValue(2);auto* encoder=find("Encoder");begin(encoder);encoder->heldSince=system::getTime()-1.1;encoder->step();end(encoder);
    check(m->integer(M::trackParam(0,M::SLIDE_TIME))==25 && m->integer(M::slideTypeParam(0))==0,"long encoder resets amount/type");APP->history->undo();check(m->integer(M::trackParam(0,M::SLIDE_TIME))==70 && m->integer(M::slideTypeParam(0))==2,"long reset groups both into one undo");
    m->params[M::ctrlParam(1,M::CTRL_DESTINATION)].setValue(metro::CtrlSlideAmount);m->params[M::ctrlParam(1,M::CTRL_ASSIGNMENT)].setValue(0);m->params[M::ctrlParam(1,M::CTRL_VALUE)].setValue(.5);m->params[M::slideTypeParam(0)].setValue(1);m->readSettings();
    begin(encoder);encoder->heldSince=system::getTime()-1.1;encoder->step();end(encoder);check(m->integer(M::trackParam(0,M::SLIDE_TIME))==75 && m->integer(M::slideTypeParam(0))==0,"reset respects CTRL-owned amount");APP->history->undo();check(m->integer(M::slideTypeParam(0))==1,"type-only reset has undo");m->params[M::ctrlParam(1,M::CTRL_DESTINATION)].setValue(0);
    click(find("TRK 2"));click(find("SLIDE"));click(find("Encoder"));check(m->integer(M::slideTypeParam(1))==2 && m->integer(M::slideTypeParam(0))==1,"types independent across tracks");
    press(find("SLIDE"),GLFW_MOUSE_BUTTON_RIGHT);check(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Analog";})!=nullptr,"context menu offers slide types");closeMenus();
    click(find("MOD"));click(find("SLIDE"));click(find("Encoder"));check(m->integer(M::slideTypeParam(0))==1 && m->integer(M::slideTypeParam(1))==2 && screenParam(m)<0,"MOD cannot edit note track slide types");click(find("TRK 2"));click(find("SLIDE"));
    m->putPreset(0,m->capturePreset());APP->patch->save(dir+"/slide-roundtrip.vcv");
}
if(qaFrame==8)metroqa::screenshot(dir+"/slide-acid-display.png");
if(qaFrame==12){check(!qaFailed,"SLIDE UI QA COMPLETE");APP->window->close();}
