// Exercise real Rack selection/key/drag callbacks and resulting output jacks.
using M=Metropolix;
auto* p=performance;
auto buttonAt=[&](int id){
    widget::EventContext ctx;event::Button e;e.context=&ctx;e.pos=PerformancePanel::region(id).getCenter();
    e.button=GLFW_MOUSE_BUTTON_LEFT;e.action=GLFW_PRESS;e.mods=0;p->onButton(e);
    if(ctx.target)APP->event->setSelectedWidget(ctx.target);
    return ctx.consumed;
};
auto key=[&](int code,int action=GLFW_PRESS,int mods=0){
    if(action==GLFW_PRESS)APP->event->heldKeys.insert(code);
    if(action==GLFW_RELEASE)APP->event->heldKeys.erase(code);
    widget::EventContext ctx;event::SelectKey e;e.context=&ctx;e.key=code;e.action=action;e.mods=mods;
    p->onSelectKey(e);return ctx.consumed;
};
auto endMouse=[&]{event::DragEnd e;e.button=GLFW_MOUSE_BUTTON_LEFT;p->onDragEnd(e);};
auto rangeTo=[&](int stage){widget::EventContext ctx;event::DragHover e;e.context=&ctx;e.origin=p;e.button=GLFW_MOUSE_BUTTON_LEFT;e.pos=PerformancePanel::region(20+stage).getCenter();p->onDragHover(e);};
auto arm=[&]{if(!p->armed)buttonAt(0);APP->event->setSelectedWidget(p);};
if(std::getenv("METROPOLIX_PERFORMANCE_REOPEN_QA")){
    if(qaFrame==8){
        check(m->performanceEditor.load() && m->expanded.load() && p->visible,"performance view survives actual Rack save/reopen");
        check(!p->armed && p->keys.empty() && !m->loopyCommand.load() && !m->loopyHeld.load() && !m->loopyLatched.load(),"reopening never arms keyboard or retains held/latched stages");
        check(m->params.size()==789 && m->inputs.size()==5 && m->outputs.size()==7,"existing parameter and port identities stay fixed");
        metroqa::screenshot(dir+"/performance-reopened.png");check(!qaFailed,"PERFORMANCE REOPEN QA COMPLETE");APP->window->close();
    }
    return;
}
if(qaFrame==6){
    m->params[M::RUN].setValue(0);m->params[M::ROOT].setValue(0);m->params[M::SCALE].setValue(1);
    m->params[M::trackParam(0,M::TRANSPOSE)].setValue(0);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->params[M::stageParam(0,2,M::PITCH_OVERRIDE)].setValue(24);
    m->params[M::stageParam(0,5,M::PITCH_OVERRIDE)].setValue(12);
    m->params[M::LOOPY_TARGET].setValue(0);m->selectedTrack.store(0);
    m->openDesktopView(2);
    check(!key(GLFW_KEY_3) && !m->loopyCommand.load(),"unarmed performance leaves keyboard input alone");
    key(GLFW_KEY_3,GLFW_RELEASE);
}
if(qaFrame==8){
    check(p->visible && box.size.x==HARDWARE_WIDTH+EDITOR_WIDTH && !editor->visible && !modPanel->visible,"optional performance view uses the existing expansion without stacking editors");
    arm();check(p->armed && key(GLFW_KEY_3) && m->loopyCommand.load()==4,"explicit keyboard arming starts stage three");
    check(!key(GLFW_KEY_L,GLFW_RELEASE),"release of a key pressed before arming is left to Rack");
}
if(qaFrame==10){
    check(m->outputs[M::GATE_1].getVoltage()==5 && m->playStage[0].load()==2 && std::abs(m->playPitch[0].load()-2)<.01,"keyboard stage audition produces actual 5V gate and frozen pitch");
    key(GLFW_KEY_6);check(m->loopyCommand.load()==36,"two keyboard holds publish both loop boundaries");
    key(GLFW_KEY_3,GLFW_RELEASE);check(m->loopyCommand.load()==32,"releasing one boundary keeps the other held");
}
if(qaFrame==12){
    check(m->playStage[0].load()==5 && std::abs(m->playPitch[0].load()-1)<.01,"remaining held stage reaches the audio engine");
    m->params[M::stageParam(0,5,M::PITCH_OVERRIDE)].setValue(36);key(GLFW_KEY_6,GLFW_REPEAT);
}
if(qaFrame==14){
    check(std::abs(m->playPitch[0].load()-1)<.01,"key repeat does not retrigger or resample a frozen audition");
    check(key(GLFW_KEY_6,GLFW_RELEASE,GLFW_MOD_CONTROL) && !m->loopyCommand.load(),"stage release works when modifiers change during a hold");
    check(!key(GLFW_KEY_4,GLFW_PRESS,GLFW_MOD_CONTROL) && !m->loopyCommand.load(),"modified Rack shortcuts do not trigger performance stages");key(GLFW_KEY_4,GLFW_RELEASE,GLFW_MOD_CONTROL);
    arm();key(GLFW_KEY_3);
    check(!key(GLFW_KEY_S,GLFW_PRESS,RACK_MOD_CTRL) && !p->armed && !m->loopyCommand.load(),"Rack save shortcut passes through after releasing holds before a blocking file dialog");key(GLFW_KEY_S,GLFW_RELEASE,RACK_MOD_CTRL);
    arm();key(GLFW_KEY_3);ui::TextField field;APP->event->setSelectedWidget(&field);
    check(!p->armed && !m->loopyCommand.load(),"selecting a text field disarms and releases keyboard stages");APP->event->setSelectedWidget(nullptr);key(GLFW_KEY_3,GLFW_RELEASE);
}
if(qaFrame==16){
    check(m->outputs[M::GATE_1].getVoltage()==0,"focus change drops the actual audition gate");
    arm();key(GLFW_KEY_3);APP->event->handleLeave();p->step();
    check(!m->loopyCommand.load() && p->keys.empty(),"lost key release on window leave cannot stick a stage");
    key(GLFW_KEY_3);key(GLFW_KEY_KP_3);key(GLFW_KEY_KP_3,GLFW_RELEASE);
    check(m->loopyCommand.load()==4,"number-row and numpad aliases release independently");key(GLFW_KEY_3,GLFW_RELEASE);
    for(int code=GLFW_KEY_1;code<=GLFW_KEY_8;++code)key(code);
    check(m->loopyCommand.load()==255,"all eight simultaneous stage keys are accepted");
    for(int code=GLFW_KEY_1;code<=GLFW_KEY_8;++code)key(code,GLFW_RELEASE);
    m->params[M::RUN].setValue(1);buttonAt(21);rangeTo(4);endMouse();
    check(m->loopyLatched.load() && m->loopyCommand.load()==18 && !m->loopyHeld.load(),"mouse drag across stages two-five latches on release");
}
if(qaFrame==18){
    metroqa::screenshot(dir+"/performance-loop.png");
    buttonAt(3);buttonAt(26);rangeTo(1);endMouse();
    check(m->loopyCommand.load()==66 && m->loopyLatched.load(),"reverse mouse drag selects the same inclusive range boundaries");
    buttonAt(3);buttonAt(22);endMouse();check(!m->loopyCommand.load() && !m->loopyLatched.load(),"single pad press remains momentary");
    arm();key(GLFW_KEY_2);key(GLFW_KEY_5);key(GLFW_KEY_L);key(GLFW_KEY_L,GLFW_RELEASE);
    key(GLFW_KEY_2,GLFW_RELEASE);key(GLFW_KEY_5,GLFW_RELEASE);
    check(m->loopyCommand.load()==18 && m->loopyLatched.load(),"keyboard latch keeps both boundaries after key release");
    key(GLFW_KEY_ESCAPE);check(!p->armed && !m->loopyCommand.load(),"Escape releases the loop and disarms keyboard");
    m->params[M::RUN].setValue(0);m->params[M::CLOCK_SOURCE].setValue(2);m->inputs[M::CLOCK_IN].setChannels(1);m->inputs[M::CLOCK_IN].setVoltage(0);
    m->params[M::trackParam(0,M::MUTE)].setValue(0);m->params[M::modParam(2,M::MOD_MUTE)].setValue(0);m->params[M::MOD_GROUP_MUTE].setValue(0);
    m->params[M::RUN].setValue(1);
}
if(qaFrame==20){
    m->params[M::RUN].setValue(1);buttonAt(5);buttonAt(32);buttonAt(10);
    check((m->loopyMuteCommands.load()&1041u)==1041u,"visible mute buttons enqueue track, lane and group changes");
}
if(qaFrame==22){
    check(!m->integer(M::trackParam(0,M::MUTE)) && !m->integer(M::modParam(2,M::MOD_MUTE)) && !m->integer(M::MOD_GROUP_MUTE),"running mutes wait for a base-clock pulse");
    m->inputs[M::CLOCK_IN].setVoltage(5);
}
if(qaFrame==24){
    check(m->integer(M::trackParam(0,M::MUTE)) && m->integer(M::modParam(2,M::MOD_MUTE)) && m->integer(M::MOD_GROUP_MUTE) && !m->loopyMuteCommands.load(),"all visible mutes apply through the existing audio mailbox on a clock pulse");
    m->inputs[M::CLOCK_IN].setVoltage(0);m->params[M::RUN].setValue(0);
    buttonAt(5);buttonAt(10);
}
if(qaFrame==26){
    check(!m->integer(M::trackParam(0,M::MUTE)) && !m->integer(M::MOD_GROUP_MUTE) && m->integer(M::modParam(2,M::MOD_MUTE)),"group unmute preserves individual lane mutes while stopped");
    buttonAt(6);buttonAt(8);check(m->selectedTrack.load()==1 && m->integer(M::LOOPY_TARGET)==1,"visible selectors choose track two and both-track loops");
    arm();key(GLFW_KEY_4);buttonAt(2);check(!m->loopyCommand.load(),"reset button releases held stages");
}
if(qaFrame==28){
    check(!m->resetFromUi.load() && m->params[M::RESET].getValue()==0,"one-shot desktop reset is consumed without leaving the hardware reset held");
    arm();key(GLFW_KEY_3);buttonAt(11);
    check(!p->armed && !m->loopyCommand.load() && !m->performanceEditor.load() && !m->modEditor.load(),"switching to the stage editor releases temporary inputs");
    m->openDesktopView(2);
}
if(qaFrame==30){
    arm();key(GLFW_KEY_3);++m->uiRestoreSerial;p->step();
    check(!p->armed && !m->loopyCommand.load(),"module restore invalidates a held keyboard gesture");
    arm();key(GLFW_KEY_2);m->params[M::RUN].setValue(1);p->lastRun=true;key(GLFW_KEY_L);key(GLFW_KEY_L,GLFW_RELEASE);key(GLFW_KEY_2,GLFW_RELEASE);
    APP->patch->save(dir+"/performance-roundtrip.vcv");metroqa::screenshot(dir+"/performance-saved.png");
}
if(qaFrame==32){
    m->params[M::RUN].setValue(0);p->step();check(!m->loopyCommand.load(),"external transport stop releases a desktop-owned latch instead of holding a Stage Player gate");
    check(!qaFailed,"PERFORMANCE UI QA COMPLETE");APP->window->close();
}
