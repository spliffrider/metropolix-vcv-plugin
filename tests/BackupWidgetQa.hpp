using M=Metropolix;
if(std::getenv("METROPOLIX_BACKUP_REOPEN_QA")){
    if(qaFrame==6){check(m->uiPage.load()==M::BACKUP_PAGE && m->backupCursor.load()==3,"backup menu selection survives actual Rack reopen");check(m->integer(M::RUN)==0 && m->getPreset(63).occupied,"backup-restored bank persists with transport stopped");check(true,"BACKUP REOPEN QA COMPLETE");APP->window->close();}return;
}
if(qaFrame==6){
    m->params[M::RUN].setValue(0);m->params[M::RUN_ON_BOOT].setValue(2);openMidiSetup(m);m->midiSetupCursor.store(0);drag(find("Encoder"),-3000);
    check(m->midiSetupCursor.load()==5 && screenParam(m)<0,"MIDI Setup appends backup entry without a numeric parameter");
    auto* encoder=dynamic_cast<HardwareControl*>(find("Encoder"));check(!encoder->longEnabled() && encoder->groupedParams().empty(),"backup entry cannot reset an invalid parameter on a long hold");
    click(find("Encoder"));check(m->uiPage.load()==M::BACKUP_PAGE && screenParam(m)<0,"MIDI entry opens backup page");
    drag(find("Metropolix display"),-3000);check(m->backupCursor.load()==3,"display drag selects import and clamps");drag(find("Encoder"),3000);check(m->backupCursor.load()==0,"encoder drag browses back to Export All");
    auto* menu=createMenu();appendBackupMenu(menu,m);check(seek(menu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Import backup…";})!=nullptr,"backup right-click menu exposes native file chooser actions");closeMenus();
    m->params[M::TEMPO].setValue(143.25f);savePresetSlot(m,63,false,false);std::string error;const std::string file=dir+"/test-backup-é-音.json";
    check(exportBackup(m,file,metrobackup::All,error),"actual export action writes a full backup with Unicode filename");
    metrobackup::Json exported(metrobackup::read(file,error));metrobackup::Scope scope=metrobackup::All;check(exported && metrobackup::validate(m,exported.get(),scope,error),"exported file validates and reloads");
    m->params[M::TEMPO].setValue(201);m->params[M::RUN_ON_BOOT].setValue(1);m->params[M::RUN].setValue(1);m->putPreset(63,M::Snapshot{});
    int history=APP->history->actionIndex;auto id=m->id;auto cables=APP->scene->rack->getCableContainer()->children.size();metrobackup::Json devices(APP->engine->moduleToJson(m));
    check(importBackup(m,file,error) && APP->history->actionIndex==history+1,"actual import action restores one undoable module change");
    check(m->params[M::TEMPO].getValue()==143.25f && m->integer(M::RUN)==0 && m->getPreset(63).occupied,"import restores settings and bank while stopping playback");
    metrobackup::Json after(APP->engine->moduleToJson(m));check(m->id==id && cables==APP->scene->rack->getCableContainer()->children.size() && json_equal(json_object_get(json_object_get(devices.get(),"data"),"midiOutput"),json_object_get(json_object_get(after.get(),"data"),"midiOutput")),"import keeps module, cables and local MIDI port choices");
    APP->history->undo();check(m->params[M::TEMPO].getValue()==201 && m->integer(M::RUN)==1 && !m->getPreset(63).occupied,"Rack undo restores previous running state and bank with Run on Boot On");
    APP->history->redo();check(m->params[M::TEMPO].getValue()==143.25f && m->integer(M::RUN)==0 && m->getPreset(63).occupied,"Rack redo restores the stopped imported state");
    json_object_set_new(exported.get(),"format",json_integer(999));check(metrobackup::write(dir+"/invalid.json",exported.get(),error),"invalid-version fixture saved");history=APP->history->actionIndex;metrobackup::Json beforeBad(APP->engine->moduleToJson(m));check(!importBackup(m,dir+"/invalid.json",error) && APP->history->actionIndex==history,"invalid import adds no history action");metrobackup::Json afterBad(APP->engine->moduleToJson(m));check(json_equal(beforeBad.get(),afterBad.get()),"invalid import leaves the complete module unchanged");
    openBackup(m);m->backupCursor.store(3);APP->patch->save(dir+"/backup-roundtrip.vcv");
}
if(qaFrame==9)metroqa::screenshot(dir+"/backup-files.png");
if(qaFrame==12){check(!qaFailed,"BACKUP UI QA COMPLETE");APP->window->close();}
