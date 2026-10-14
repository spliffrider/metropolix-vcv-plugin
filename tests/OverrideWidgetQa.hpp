if(std::getenv("METROPOLIX_OVERRIDE_REOPEN_QA")) {
    if(qaFrame==6) {
        check(m->selectedRow.load()==Metropolix::GATE_OVERRIDE && m->integer(Metropolix::stageParam(0,2,Metropolix::PITCH_OVERRIDE))==28 && m->integer(Metropolix::stageParam(1,3,Metropolix::GATE_OVERRIDE))==0,"native Rack file preserves override parameters and page");
        check(m->getPreset(0).occupied && m->getPreset(0).values[Metropolix::presetIndex(Metropolix::gateScaleParam(0))]==63,"native Rack file preserves overrides and Gate Scale in bank");
        check(true,"OVERRIDE REOPEN QA COMPLETE");APP->window->close();
    }
    return;
}
if(qaFrame==6) {
    using M=Metropolix;
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->params[M::SCALE].setValue(1);m->params[M::RANGE].setValue(3);
    const int defaults[]={0,4,7,12,16,19,14,7};for(int s=0;s<8;++s)m->params[M::PITCH+s].setValue(defaults[s]/36.f);
    auto longPress=[&] {
        auto* encoder=find("Encoder");press(encoder,GLFW_MOUSE_BUTTON_LEFT);widget::EventContext ctx;
        event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragStart(start);
        encoder->heldSince=system::getTime()-1.1;encoder->step();
        event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragEnd(end);
    };
    click(find("TRK 1"));click(find("EDIT"));click(find("Stage 3 / PITCH"));
    check(m->uiPage.load()==M::STAGE_PAGE && m->selectedRow.load()==M::PITCH_OVERRIDE,"EDIT PITCH opens real override page");
    click(find("Stage 3 / PITCH"));int pitch=M::stageParam(0,2,M::PITCH_OVERRIDE);
    check(screenParam(m)==pitch,"stage selection binds pitch encoder");
    drag(find("Encoder"),-100);check(m->integer(pitch)>=0,"pitch encoder creates override");APP->history->undo();check(m->integer(pitch)==-1,"single pitch drag is one undo");
    int index=APP->history->actionIndex;click(find("Encoder"));
    check(m->integer(pitch)==7,"encoder sync copies selected physical slider note");
    check(APP->history->actionIndex==index+1,"single sync records one history entry");APP->history->undo();check(m->integer(pitch)==-1,"sync undo restores no override");APP->history->redo();
    click(find("ALT"));index=APP->history->actionIndex;click(find("Encoder"));
    const int notes[]={0,4,7,12,16,19,14,7};
    for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::PITCH_OVERRIDE))==notes[s],"ALT sync copies every corresponding slider");
    check(APP->history->actionIndex==index+1,"all-stage sync is one undo action");
    drag(find("Encoder"),-100);
    for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::PITCH_OVERRIDE))>notes[s],"ALT pitch drag raises every stage");
    APP->history->undo();for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::PITCH_OVERRIDE))==notes[s],"one undo restores all pitch overrides");
    APP->history->redo();for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::PITCH_OVERRIDE))>notes[s],"batch pitch drag supports redo");APP->history->undo();
    index=APP->history->actionIndex;longPress();for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::PITCH_OVERRIDE))==-1,"ALT long press clears every pitch override");
    check(APP->history->actionIndex==index+1,"all-stage clear creates one undo action");APP->history->undo();
    for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::PITCH_OVERRIDE))==notes[s],"all-stage clear undo restores distinct pitches");
    click(find("ALT"));longPress();check(m->integer(pitch)==-1 && m->integer(M::stageParam(0,1,M::PITCH_OVERRIDE))==4,"single long clear leaves other stages");APP->history->undo();
    click(find("EDIT"));click(find("Stage 4 / GATE"));click(find("Stage 4 / GATE"));int gate=M::stageParam(0,3,M::GATE_OVERRIDE);
    check(screenParam(m)==gate,"EDIT GATE opens stage gate override");
    m->params[gate].setValue(88);drag(find("Metropolix display"),100);check(m->integer(gate)<88,"OLED drag edits stage gate override");APP->history->undo();check(m->integer(gate)==88,"OLED override drag is one undo");
    for(int s=0;s<8;++s)m->params[M::stageParam(0,s,M::GATE_OVERRIDE)].setValue(s*10);
    click(find("ALT"));drag(find("Encoder"),-100);for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::GATE_OVERRIDE))>s*10,"ALT gate drag raises every stage");
    APP->history->undo();for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::GATE_OVERRIDE))==s*10,"gate batch undo restores distinct values");
    longPress();for(int s=0;s<8;++s)check(m->integer(M::stageParam(0,s,M::GATE_OVERRIDE))==-1,"ALT clears all gate overrides");APP->history->undo();click(find("ALT"));
    click(find("GATE"));click(find("Encoder"));check(screenParam(m)==M::gateScaleParam(0),"track GATE encoder switches to Gate Scale");
    drag(find("Encoder"),100);check(m->integer(M::gateScaleParam(0))<100,"Gate Scale edits from encoder");APP->history->undo();
    m->params[M::trackParam(0,M::GATE_LENGTH)].setValue(75);m->params[M::gateScaleParam(0)].setValue(63);
    index=APP->history->actionIndex;longPress();check(m->integer(M::trackParam(0,M::GATE_LENGTH))==50 && m->integer(M::gateScaleParam(0))==100,"long press resets both track gate fields");
    check(APP->history->actionIndex==index+1,"track gate reset is one undo");APP->history->undo();check(m->integer(M::trackParam(0,M::GATE_LENGTH))==75 && m->integer(M::gateScaleParam(0))==63,"gate reset undo restores both fields");
    auto* held=find("GATE");press(held,GLFW_MOUSE_BUTTON_LEFT);widget::EventContext ctx;
    event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;held->onDragStart(start);
    index=APP->history->actionIndex;click(find("Stage 2 / SKIP"));
    event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;held->onDragEnd(end);
    check(m->integer(M::gateScaleParam(0))==15 && m->uiPage.load()==M::GATE_PAGE,"held GATE stage shortcut edits active Scale field");
    check(APP->history->actionIndex==index+1,"held gate shortcut is one undo");APP->history->undo();check(m->integer(M::gateScaleParam(0))==63,"gate shortcut undo");
    click(find("Encoder"));
    // Keep the processing thread's direct CTRL value at the 75% fixture.
    // A noon knob would legitimately write 50% while this hold is checked.
    m->params[M::ctrlParam(0,M::CTRL_VALUE)].setValue((75.f-1.f)/49.f-1.f);
    m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlGateLength);
    index=APP->history->actionIndex;longPress();check(m->integer(M::trackParam(0,M::GATE_LENGTH))==75 && m->integer(M::gateScaleParam(0))==100,"gate reset respects CTRL length ownership");
    check(APP->history->actionIndex==index+1,"reset from locked length still records Scale undo");APP->history->undo();check(m->integer(M::gateScaleParam(0))==63,"owned reset Scale undo");
    m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    click(find("EDIT"));click(find("Stage 3 / PITCH"));m->params[pitch].setValue(28);click(find("Stage 3 / PITCH"));
    check(find("Stage 3 / PITCH")->activeColor().r>.8f && find("Stage 3 / PITCH")->activeColor().g>.8f,"pitch editing uses yellow stage lights");
    click(find("TRK 2"));click(find("EDIT"));click(find("Stage 4 / GATE"));click(find("Stage 4 / GATE"));
    m->params[M::stageParam(1,3,M::GATE_OVERRIDE)].setValue(0);
    check(find("Stage 4 / GATE")->activeColor().g>.8f && find("Stage 4 / GATE")->activeColor().r<.5f,"gate editing uses green stage lights");
    check(m->getParamQuantity(M::stageParam(1,3,M::GATE_OVERRIDE))->getString().find("Rest")!=std::string::npos,"override tooltip exposes special state");
    press(find("Encoder"),GLFW_MOUSE_BUTTON_RIGHT);
    auto* remove=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text=="Remove this stage override";}));
    check(remove!=nullptr,"override context menu exposes direct removal");if(remove){event::Action action;remove->onAction(action);}closeMenus();
    check(m->integer(M::stageParam(1,3,M::GATE_OVERRIDE))==-1,"context removal follows inherited length");APP->history->undo();check(m->integer(M::stageParam(1,3,M::GATE_OVERRIDE))==0,"menu removal is undoable");
    m->putPreset(0,m->capturePreset());APP->patch->save(dir+"/override-roundtrip.vcv");
}
if(qaFrame==10)metroqa::screenshot(dir+"/override-gate-panel.png");
if(qaFrame==14){check(!qaFailed,"OVERRIDE UI QA COMPLETE");APP->window->close();}
