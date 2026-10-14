using M=Metropolix;
if(std::getenv("METROPOLIX_BACKUP_REOPEN_QA")){
    if(qaFrame==6){check(m->uiPage.load()==M::BACKUP_PRESET_PAGE && m->backupPresetCursor.load()==9 && m->backupCursor.load()==2,"selective submenu and Bank 8 selection survive actual Rack reopen");check(m->integer(M::RUN)==0 && m->getPreset(63).occupied && !m->getPreset(57).occupied && m->getPreset(12).occupied,"selectively restored and untouched slots persist with transport stopped");check(true,"BACKUP REOPEN QA COMPLETE");APP->window->close();}return;
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
    openBackup(m);m->backupCursor.store(2);click(find("Encoder"));check(m->uiPage.load()==M::BACKUP_PRESET_PAGE && screenParam(m)<0,"Export Presets opens selection submenu without a numeric parameter");
    check(!encoder->longEnabled() && encoder->groupedParams().empty(),"selective submenu cannot reset any musical parameter on a long hold");
    drag(find("Encoder"),-3000);check(m->backupPresetCursor.load()==9,"encoder clamps at Bank 8");drag(find("Metropolix display"),3000);check(m->backupPresetCursor.load()==0,"display clamps at All Presets");
    drag(find("Encoder"),-3000);check(encoder->tooltipText().find("Bank 8")!=std::string::npos,"hover identifies selected bank export");
    auto* selectedMenu=createMenu();appendBackupPresetMenu(selectedMenu,m);check(seek(selectedMenu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Filled Presets…";}) && seek(selectedMenu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Bank 8…";}),"right-click selective menu exposes Filled and all eight banks");closeMenus();
    click(find("EXIT"));check(m->uiPage.load()==M::BACKUP_PAGE && m->backupCursor.load()==2,"short EXIT returns selective export to its parent");
    for(int n=0;n<64;++n)m->putPreset(n,M::Snapshot{});auto selected=m->capturePreset();selected.values[M::PITCH]=.35f;for(int n:{7,56,63})m->putPreset(n,selected);
    const std::string filledFile=dir+"/filled-é-音.json",bankFile=dir+"/bank-8.json";
    check(exportPresetBackup(m,filledFile,1,error) && exportPresetBackup(m,bankFile,9,error),"actual selective actions export Filled and Bank 8 files");
    metrobackup::Json bank(metrobackup::read(bankFile,error));check(bank && json_array_size(json_object_get(json_object_get(bank.get(),"bank"),"slots"))==8,"bank file contains its explicit occupied and empty slots");
    auto local=m->capturePreset();local.values[M::PITCH]=.8f;for(int n:{0,12,57,63})m->putPreset(n,local);m->putPreset(56,M::Snapshot{});m->params[M::TEMPO].setValue(187.5f);m->params[M::RUN].setValue(1);
    history=APP->history->actionIndex;metrobackup::Json beforeFilled(APP->engine->moduleToJson(m));check(importBackup(m,filledFile,error) && APP->history->actionIndex==history+1,"Filled import creates one undo action");
    check(m->getPreset(56).occupied && m->getPreset(63).values[M::PITCH]==.35f && m->getPreset(12).values[M::PITCH]==.8f && m->getPreset(57).occupied && m->params[M::TEMPO].getValue()==187.5f && m->integer(M::RUN)==0,"Filled merges occupied slots and preserves other slots and live settings");
    metrobackup::Json afterFilled(APP->engine->moduleToJson(m));APP->history->undo();metrobackup::Json undoneFilled(APP->engine->moduleToJson(m));check(json_equal(beforeFilled.get(),undoneFilled.get()),"actual Rack undo restores exact pre-Filled state");APP->history->redo();metrobackup::Json redoneFilled(APP->engine->moduleToJson(m));check(json_equal(afterFilled.get(),redoneFilled.get()),"actual Rack redo restores exact selective result");
    auto* filledData=json_object_get(afterFilled.get(),"data");history=APP->history->actionIndex;check(importBackup(m,bankFile,error) && APP->history->actionIndex==history+1,"Bank import creates one undo action");metrobackup::Json afterBank(APP->engine->moduleToJson(m));
    check(!m->getPreset(57).occupied && m->getPreset(63).occupied && m->getPreset(12).occupied && m->getPreset(7).occupied && json_equal(json_object_get(filledData,"chains"),json_object_get(json_object_get(afterBank.get(),"data"),"chains")),"Bank 8 clears its empty slots while preserving other banks and chains");
    check(m->id==id && cables==APP->scene->rack->getCableContainer()->children.size(),"selective imports preserve module identity and cable count");
    openBackupPresets(m);m->backupCursor.store(2);m->backupPresetCursor.store(9);m->backupNotice="PRESETS SAVED";m->backupNoticeUntil=system::getTime()+5;APP->patch->save(dir+"/backup-roundtrip.vcv");
}
if(qaFrame==9)metroqa::screenshot(dir+"/backup-files.png");
if(qaFrame==12){check(!qaFailed,"BACKUP UI QA COMPLETE");APP->window->close();}
