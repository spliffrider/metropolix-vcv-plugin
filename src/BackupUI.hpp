#include "BackupFile.hpp"
#include "BackupRestore.hpp"
#include <osdialog.h>
constexpr int BackupCount=4;
const char* backupName(int n){const char* names[]={"Export All","Export Settings","Export Presets","Import Backup"};return names[metro::clamp(n,0,3)];}
void openBackup(Metropolix* m){if(!m)return;m->cancelDraft();m->tunerActive.store(false);m->uiPage.store(Metropolix::BACKUP_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);}
bool exportBackup(Metropolix* m,const std::string& path,metrobackup::Scope scope,std::string& error){
    metrobackup::Json module(APP->engine->moduleToJson(m));metrobackup::Json document(metrobackup::capture(module.get(),scope));
    metrobackup::Scope checked=metrobackup::All;if(!metrobackup::validate(m,document.get(),checked,error))return false;
    return metrobackup::write(path,document.get(),error);
}
bool importBackup(Metropolix* m,const std::string& path,std::string& error){
    metrobackup::Json document(metrobackup::read(path,error));if(!document)return false;
    metrobackup::Json before,after;if(!metrobackup::restore(m,document.get(),before,after,error))return false;
    auto* change=new history::ModuleChange;change->name="restore Metropolix backup";change->moduleId=m->id;change->oldModuleJ=before.release();change->newModuleJ=after.release();APP->history->push(change);
    return true;
}
void backupChoose(Metropolix* m,int action){
    if(!m)return;
    auto* filters=osdialog_filters_parse("Metropolix Lab backup:json");
    char* chosen=osdialog_file(action==3?OSDIALOG_OPEN:OSDIALOG_SAVE,nullptr,action==3?nullptr:"MetropolixLab-backup.json",filters);osdialog_filters_free(filters);
    if(!chosen)return;
    std::string path(chosen);std::free(chosen);std::string error;
    bool good=action==3?importBackup(m,path,error):exportBackup(m,path,static_cast<metrobackup::Scope>(action),error);
    if(good){openBackup(m);m->backupCursor.store(action);m->backupNotice=action==3?"IMPORTED / STOPPED":"BACKUP SAVED";}
    else{m->backupNotice="BACKUP FAILED";osdialog_message(OSDIALOG_WARNING,OSDIALOG_OK,error.c_str());}
    m->backupNoticeUntil=system::getTime()+5;
}
void appendBackupMenu(ui::Menu* menu,Metropolix* m){if(!m)return;
    menu->addChild(createMenuLabel("Portable backup files"));menu->addChild(createMenuItem("Open Backup files","",[m]{openBackup(m);}));
    const char* names[]={"Export all settings, presets and chains…","Export settings/state…","Export presets and chains…","Import backup…"};
    for(int n=0;n<BackupCount;++n)menu->addChild(createMenuItem(names[n],"",[m,n]{backupChoose(m,n);}));
    menu->addChild(createMenuLabel("Metropolix Lab JSON; Rack undo restores an import"));
}
