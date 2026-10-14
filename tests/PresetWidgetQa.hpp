// Included in qaStep(), using real panel callbacks with Rack's audio engine.
if(std::getenv("METROPOLIX_PRESET_REOPEN_QA")) {
    if(qaFrame==10) {
        check(m->getPreset(0).occupied && m->getPreset(1).occupied && !m->getPreset(2).occupied,"reopened Rack file preserves saved/empty slot layout");
        check(m->getPreset(0).values[Metropolix::ROOT]==7 && m->getPreset(1).values[Metropolix::ROOT]==7,"reopened Rack file preserves stored pattern values");
        check(m->integer(Metropolix::LOAD_DELAY)==metro::LoadQueuePulses && m->integer(Metropolix::QUEUE_PULSES)==0 && m->pendingSlot.load()==-1,"reopened Rack file preserves preferences without queued load");
        check(!qaFailed,"PRESET REOPEN QA COMPLETE");APP->window->close();
    }
    return;
}
if(qaFrame==5) {
    m->params[Metropolix::RUN].setValue(0);
    click(find("EXIT"));click(find("ALT"));click(find("Stage 2 / SKIP"));
    check(m->uiPage.load()==Metropolix::SAVE_PAGE && !m->uiAlt.load(),"ALT SAVE opens internal bank page");
    drag(find("Encoder"),-22);check(m->presetBankIndex.load()==1,"encoder up selects next bank");
    drag(find("Metropolix display"),22);check(m->presetBankIndex.load()==0,"OLED down selects previous bank");
    click(find("Encoder"));check(m->presetSaveAction.load()==1,"SAVE encoder selects Save without exiting");
    m->params[Metropolix::ROOT].setValue(3);
    click(find("Stage 1 / SLIDE"));
    check(m->getPreset(0).occupied && m->getPreset(0).values[Metropolix::ROOT]==3 && m->uiPage.load()==Metropolix::SAVE_PAGE,"empty slot saves current pattern and stays on Save page");
    APP->history->undo();check(!m->getPreset(0).occupied,"saving a bank slot supports undo");
    APP->history->redo();check(m->getPreset(0).occupied,"saving a bank slot supports redo");
    m->params[Metropolix::ROOT].setValue(7);
    click(find("Stage 1 / SLIDE"));
    check(m->getPreset(0).values[Metropolix::ROOT]==3,"occupied save does not overwrite before confirmation");
    auto* confirm=seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Confirm overwrite";});
    check(confirm!=nullptr,"occupied slot offers overwrite confirmation");
    if(confirm){event::Action e;confirm->onAction(e);}
    check(m->getPreset(0).values[Metropolix::ROOT]==7,"confirmation overwrites selected slot");closeMenus();
    drag(find("Stage 3 / PITCH"),-80);
    check(m->uiPage.load()==Metropolix::SAVE_PAGE && !m->getPreset(2).occupied,"dragging a preset button does not edit musical stage or save accidentally");
    click(find("Encoder"));check(m->presetSaveAction.load()==2,"SAVE encoder selects Clear");
    click(find("Stage 1 / SLIDE"));
    auto* clear=seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Confirm clear";});
    check(clear && m->getPreset(0).occupied,"clear requires explicit confirmation");
    if(clear){event::Action e;clear->onAction(e);}
    check(!m->getPreset(0).occupied,"confirmed clear empties slot");closeMenus();
    APP->history->undo();check(m->getPreset(0).occupied,"clearing slot supports undo");
    click(find("ALT"));click(find("Stage 1 / SLIDE"));
    check(m->uiPage.load()==Metropolix::LOAD_PAGE,"ALT LOAD opens internal bank page");
    press(find("Encoder"),GLFW_MOUSE_BUTTON_RIGHT);
    auto* settings=seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Preset settings";});
    check(settings!=nullptr,"LOAD encoder right-click offers preset settings");
    if(settings){event::Action e;settings->onAction(e);}closeMenus();
    check(screenParam(m)<0 && m->uiField.load()==1 && !m->presetSetupEditing.load(),"preset settings starts browsing Queue Pulses");
    click(find("Encoder"));check(screenParam(m)==Metropolix::QUEUE_PULSES,"click enters Queue Pulses editing");
    drag(find("Encoder"),-80);check(m->integer(Metropolix::QUEUE_PULSES)>32,"upward drag increases pulse count");APP->history->undo();
    check(m->integer(Metropolix::QUEUE_PULSES)==32,"pulse count drag supports undo");
    openPresetPage(m,false);
    m->params[Metropolix::ROOT].setValue(0);
    click(find("Stage 1 / SLIDE"));
}
if(qaFrame==10) {
    check(m->integer(Metropolix::ROOT)==7 && m->lastLoadedSlot.load()==0,"stopped load reaches audio engine and recalls saved pattern");
    APP->history->undo();
}
if(qaFrame==15) {check(m->integer(Metropolix::ROOT)==0,"preset load undo restores pre-load values");APP->history->redo();}
if(qaFrame==20) {
    check(m->integer(Metropolix::ROOT)==7,"preset load redo restores saved pattern");
    json_t* data=APP->engine->moduleToJson(m);
    m->putPreset(0,Metropolix::Snapshot{});
    APP->engine->moduleFromJson(m,data);json_decref(data);
    check(m->getPreset(0).occupied,"actual Rack module JSON restores bank contents");
    m->params[Metropolix::LOAD_DELAY].setValue(metro::LoadQueuePulses);
    m->params[Metropolix::QUEUE_PULSES].setValue(0);
    m->params[Metropolix::RUN].setValue(1);
    openPresetPage(m,false);click(find("Stage 1 / SLIDE"));
}
if(qaFrame==25) {
    check(m->pendingSlot.load()==0,"running load queues on reset-only setting");
    openPresetPage(m,true);click(find("Stage 2 / SKIP"));
    check(!m->getPreset(1).occupied && m->presetNotice.load()==5,"saving blocked while preset load queued");
    openPresetPage(m,false);click(find("Stage 2 / SKIP"));
}
if(qaFrame==30) {
    check(m->pendingSlot.load()==-1,"clicking empty slot cancels queued load");
    openPresetPage(m,true);click(find("Stage 2 / SKIP"));
    check(m->getPreset(1).occupied && m->uiPage.load()==Metropolix::HOME,"Save + Exit stores pattern and returns home");
    APP->patch->save(dir+"/preset-roundtrip.vcv");
    check(!qaFailed,"PRESET UI QA COMPLETE");APP->window->close();
}
