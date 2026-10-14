if(std::getenv("METROPOLIX_CLOCK_REOPEN_QA")){
    if(qaFrame==6){check(m->integer(Metropolix::CLOCK_OUT_DIV)==16 && m->integer(Metropolix::CLOCK_OUT_OFFSET)==7 && m->uiPage.load()==Metropolix::CLOCK_OUT_PAGE && m->uiField.load()==1,"native patch preserves clock settings and page");check(true,"CLOCK REOPEN QA COMPLETE");APP->window->close();}return;
}
if(qaFrame==6){
    using M=Metropolix;m->params[M::RUN].setValue(0);click(find("ALT"));click(find("Stage 5 / RATCH"));
    check(m->uiPage.load()==M::CLOCK_OUT_PAGE && screenParam(m)==M::CLOCK_OUT_DIV,"ALT CLK opens global clock division page");
    drag(find("Encoder"),-75);check(m->integer(M::CLOCK_OUT_DIV)>1,"upward clock division drag increases division");APP->history->undo();check(m->integer(M::CLOCK_OUT_DIV)==1,"division drag has one undo");
    m->params[M::CLOCK_OUT_DIV].setValue(16);click(find("Encoder"));check(screenParam(m)==M::CLOCK_OUT_OFFSET,"encoder press selects offset");
    drag(find("Metropolix display"),-65);check(m->integer(M::CLOCK_OUT_OFFSET)>0 && m->integer(M::CLOCK_OUT_OFFSET)<16,"display upward drag edits offset within divisor");APP->history->undo();check(m->integer(M::CLOCK_OUT_OFFSET)==0,"offset drag has one undo");
    m->params[M::CLOCK_OUT_OFFSET].setValue(7);auto* encoder=find("Encoder");
    int historyBefore=APP->history->actionIndex;
    widget::EventContext context;event::DragStart begin;begin.context=&context;begin.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragStart(begin);encoder->heldSince=system::getTime()-1.1;encoder->step();event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragEnd(end);
    check(APP->history->actionIndex==historyBefore+1,"long reset creates exactly one history action");
    check(m->integer(M::CLOCK_OUT_DIV)==1 && m->integer(M::CLOCK_OUT_OFFSET)==0,"long encoder resets both fields");APP->history->undo();check(m->integer(M::CLOCK_OUT_DIV)==16 && m->integer(M::CLOCK_OUT_OFFSET)==7,"paired clock reset has one undo");
    m->params[M::CLOCK_OUT_DIV].setValue(1);m->uiField.store(1);encoder->bind();check(!encoder->getParamQuantity() && std::isfinite(encoder->speed),"/1 offset field safely handles zero editable range");drag(encoder,-100);check(m->getParamQuantity(M::CLOCK_OUT_OFFSET)->getValue()==0,"/1 offset stays zero");
    press(encoder,GLFW_MOUSE_BUTTON_RIGHT);check(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Reset CLK output";})!=nullptr,"disabled offset context menu retains clock controls");closeMenus();
    m->params[M::CLOCK_OUT_DIV].setValue(16);m->params[M::CLOCK_OUT_OFFSET].setValue(7);m->uiField.store(1);m->putPreset(0,m->capturePreset());APP->patch->save(dir+"/clock-roundtrip.vcv");
}
if(qaFrame==8)metroqa::screenshot(dir+"/clock-output-display.png");
if(qaFrame==12){check(!qaFailed,"CLOCK UI QA COMPLETE");APP->window->close();}
