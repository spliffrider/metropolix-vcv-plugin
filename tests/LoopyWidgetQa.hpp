if(std::getenv("METROPOLIX_LOOPY_REOPEN_QA")) {
    if(qaFrame==6){check(m->integer(Metropolix::LOOPY_PULSES)==5 && m->integer(Metropolix::LOOPY_TARGET)==1 && m->uiPage.load()==Metropolix::LOOPY_PAGE,"native Rack file preserves Loopy settings/page");check(m->loopyCommand.load()==0 && !m->loopyLatched.load(),"native reopen clears latched/held gestures");check(m->getPreset(0).occupied,"native reopen preserves existing preset banks");check(!qaFailed,"LOOPY REOPEN QA COMPLETE");APP->window->close();}return;
}
static HardwareControl* held=nullptr;
auto begin=[&](HardwareControl* c){press(c,GLFW_MOUSE_BUTTON_LEFT);widget::EventContext context;event::DragStart e;e.context=&context;e.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragStart(e);};
auto end=[&](HardwareControl* c){widget::EventContext context;event::DragEnd e;e.context=&context;e.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragEnd(e);};
if(qaFrame==6){
    using M=Metropolix;m->params[M::RUN].setValue(0);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    click(find("MOD"));click(find("Loopy"));check(m->uiPage.load()==M::LOOPY_PAGE && !m->uiMod.load() && m->selectedTrack.load()==0,"Loopy enters settings from MOD on track 1");
    drag(find("Encoder"),-100);check(m->integer(M::LOOPY_PULSES)>0,"Loopy encoder drag increases pulse count");APP->history->undo();check(m->integer(M::LOOPY_PULSES)==0,"Loopy pulse drag uses Rack undo");
    click(find("Encoder"));check(m->integer(M::LOOPY_TARGET)==1,"encoder click toggles both-track target");APP->history->undo();check(m->integer(M::LOOPY_TARGET)==0,"Loopy target supports undo");
    m->params[M::stageParam(0,2,M::PITCH_OVERRIDE)].setValue(24);m->params[M::GATES+2].setValue(metro::Rest);
    held=find("Stage 3 / PITCH");begin(held);check((m->loopyCommand.load()&255u)==4,"pressing stage creates momentary performance request");
}
if(qaFrame==8){check(m->playStage[0].load()==2 && m->playPitch[0].load()==2 && m->outputs[Metropolix::GATE_1].getVoltage()==5,"stopped stage audition reaches actual engine/output");metroqa::screenshot(dir+"/loopy-stage-player.png");end(held);check(m->loopyCommand.load()%256==0,"release clears held audition");}
if(qaFrame==10){
    using M=Metropolix;m->params[M::RUN].setValue(1);m->params[M::LOOPY_PULSES].setValue(5);
    begin(held);click(find("BPM"));check(m->loopyLatched.load(),"navigating away while holding latches loop");end(held);check(m->loopyCommand.load()%256==4,"latched loop survives release on another page");
    click(find("Loopy"));check(m->uiPage.load()==M::LOOPY_PAGE && m->loopyLatched.load(),"returning to Loopy preserves latch");click(find("Encoder"));check(!m->loopyLatched.load() && m->loopyCommand.load()%256==0,"encoder unlatches current loop");
    press(find("Stage 2 / SKIP"),GLFW_MOUSE_BUTTON_RIGHT);
    auto* parent=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Latch range from stage 2";}));check(parent!=nullptr,"stage menu offers mouse range selection");
    if(parent){auto* menu=parent->createChildMenu();auto* item=seek(menu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Through stage 5";});if(item){event::Action e;item->onAction(e);}delete menu;}closeMenus();
    check(m->loopyLatched.load() && m->loopyCommand.load()%256==18,"mouse range captures both boundaries and latches");
}
if(qaFrame==12){metroqa::screenshot(dir+"/loopy-latched-range.png");
    auto* d=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));auto c=d->render();std::ofstream pgm(dir+"/loopy-latched.pgm",std::ios::binary);pgm<<"P5\n128 64\n255\n";pgm.write(reinterpret_cast<char*>(c.pixels),128*64);
    m->clearLoopy();m->params[Metropolix::RUN].setValue(0);begin(find("Loopy"));click(find("TRK 1"));click(find("Stage 4 / GATE"));check(m->loopyMuteHeld.load() && m->loopyMuteUsed.load(),"Loopy hold enables track/lane mute gestures");end(find("Loopy"));
}
if(qaFrame==14){check(m->integer(Metropolix::trackParam(0,Metropolix::MUTE))==1 && m->integer(Metropolix::modParam(3,Metropolix::MOD_MUTE))==1,"held Loopy gestures mute selected track/lane");
    m->params[Metropolix::LOOPY_TARGET].setValue(1);m->putPreset(0,m->capturePreset());loopRange(m,1,4);m->params[Metropolix::RUN].setValue(1);loopRange(m,1,4);APP->patch->save(dir+"/loopy-roundtrip.vcv");
}
if(qaFrame==18){check(!qaFailed,"LOOPY UI QA COMPLETE");APP->window->close();}
