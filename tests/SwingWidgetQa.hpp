// Actual production widget callbacks inside the isolated Rack test profile.
if(std::getenv("METROPOLIX_SWING_REOPEN_QA")) {
    if(qaFrame==6) {
        check(m->uiPage.load()==Metropolix::SWING_PAGE && m->integer(Metropolix::swingParam(0))==64 && m->integer(Metropolix::swingParam(1))==58,"native Rack file preserves Swing settings and page");
        check(m->getPreset(0).occupied && m->getPreset(0).values[Metropolix::presetIndex(Metropolix::swingParam(1))]==58,"native Rack file preserves Swing bank values");
        check(true,"SWING REOPEN QA COMPLETE");APP->window->close();
    }
    return;
}
if(qaFrame==6) {
    for(int k=0;k<2;++k)m->params[Metropolix::ctrlParam(k,Metropolix::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    click(find("TRK 1"));click(find("SWING"));
    check(m->uiPage.load()==Metropolix::SWING_PAGE && screenParam(m)==Metropolix::swingParam(0),"SWING opens selected track parameter");
    auto* q=m->getParamQuantity(Metropolix::swingParam(0));
    drag(find("SWING"),-100);check(q->getValue()>50,"button upward drag increases swing");
    APP->history->undo();check(q->getValue()==50,"button swing drag is one undo");APP->history->redo();
    click(find("Stage 5 / RATCH"));check(q->getValue()==66 && m->uiPage.load()==Metropolix::SWING_PAGE,"desktop stage shortcut chooses 66 percent without leaving Swing");
    APP->history->undo();APP->history->redo();check(q->getValue()==66,"shortcut supports undo/redo");
    drag(find("Encoder"),100);check(q->getValue()<66,"encoder downward drag decreases swing");APP->history->undo();
    drag(find("Metropolix display"),-100);check(q->getValue()>66,"OLED upward drag edits swing");APP->history->undo();
    click(find("TRK 2"));check(screenParam(m)==Metropolix::swingParam(1),"Swing display follows TRK 2");
    click(find("Stage 8 / CV"));check(m->integer(Metropolix::swingParam(1))==78 && m->integer(Metropolix::swingParam(0))==66,"shortcut edits only selected track");
    press(find("SWING"),GLFW_MOUSE_BUTTON_RIGHT);
    auto* item=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="58%";}));
    check(item!=nullptr,"Swing context menu contains standard shortcuts");
    if(item){event::Action e;item->onAction(e);}closeMenus();check(m->integer(Metropolix::swingParam(1))==58,"context shortcut edits Swing");
    click(find("MOD"));click(find("SWING"));check(screenParam(m)<0,"MOD lane cannot acquire its own swing");
    click(find("TRK 1"));click(find("SWING"));
}
if(qaFrame==10)metroqa::screenshot(dir+"/swing-panel.png");
if(qaFrame==12) {
    auto* encoder=find("Encoder");press(encoder,GLFW_MOUSE_BUTTON_LEFT);
    widget::EventContext ctx;event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragStart(start);
    encoder->heldSince=system::getTime()-1.1;encoder->step();
    event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragEnd(end);
    check(m->integer(Metropolix::swingParam(0))==50,"encoder long press resets to straight timing");
    APP->history->undo();check(m->integer(Metropolix::swingParam(0))==66,"long reset has one undo");
    APP->history->redo();check(m->integer(Metropolix::swingParam(0))==50,"long reset supports redo");
    m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION)].setValue(metro::CtrlSwing);
    m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_ASSIGNMENT)].setValue(0);
    m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_VALUE)].setValue(0);
}
if(qaFrame==16) {
    auto* encoder=find("Encoder");encoder->bind();find("SWING")->bind();
    check(!encoder->getParamQuantity() && !find("SWING")->getParamQuantity(),"CTRL-owned Swing disables direct editing");
    check(find("SWING")->tooltipText().find("Controlled by CTRL 1")!=std::string::npos,"Swing hover explains ownership");
    check(m->integer(Metropolix::swingParam(0))==64,"CTRL noon maps to 64 percent");
    for(auto* w:editor->children)if(auto* cell=dynamic_cast<ValueCell*>(w))if(cell->swingProperty)check(cell->paramId==Metropolix::swingParam(0) && cell->locked(),"optional Swing field follows track and CTRL ownership");
    drag(find("Stage 1 / SLIDE"),0);check(m->integer(Metropolix::swingParam(0))==64,"stage shortcut respects CTRL ownership");
    check(Metropolix::modDestinations()[metro::ModSwing]=="Swing" && Metropolix::ctrlDestinations()[metro::CtrlSwing]=="Swing","AUX MOD CTRL menus expose Swing");
    m->putPreset(0,m->capturePreset());
    APP->patch->save(dir+"/swing-roundtrip.vcv");
}
if(qaFrame==20)metroqa::screenshot(dir+"/swing-ctrl-owned.png");
if(qaFrame==24) {
    check(m->getPreset(0).values[Metropolix::presetIndex(Metropolix::swingParam(0))]==64,"UI save captures swing in internal preset");
    check(true,"SWING UI QA COMPLETE");APP->window->close();
}
