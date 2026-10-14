#include "BackupFile.hpp"
#include "BackupRestore.hpp"
#include <osdialog.h>
constexpr int BackupCount=4;
constexpr int BackupPresetCount=10;
const char* backupName(int n){const char* names[]={"Export All","Export Settings","Export Presets","Import Backup"};return names[metro::clamp(n,0,3)];}
std::string backupPresetName(int n){return n==0?"All Presets":n==1?"Filled Presets":string::f("Bank %d",metro::clamp(n-1,1,8));}
bool backupPage(Metropolix* m){return m && (m->uiPage.load()==Metropolix::BACKUP_PAGE || m->uiPage.load()==Metropolix::BACKUP_PRESET_PAGE);}
std::string backupHelp(Metropolix* m){
    if(!backupPage(m))return {};
    if(m->uiPage.load()==Metropolix::BACKUP_PRESET_PAGE)return backupPresetName(m->backupPresetCursor.load())+"\nClick encoder to export this selection.";
    return std::string(backupName(m->backupCursor.load()))+(m->backupCursor.load()==2?"\nClick encoder for All, Filled or one bank.":"\nClick encoder to choose a file.");
}
void openBackup(Metropolix* m){if(!m)return;m->cancelDraft();m->tunerActive.store(false);m->uiPage.store(Metropolix::BACKUP_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);}
void openBackupPresets(Metropolix* m){openBackup(m);if(m){m->backupCursor.store(2);m->uiPage.store(Metropolix::BACKUP_PRESET_PAGE);}}
bool exportBackup(Metropolix* m,const std::string& path,metrobackup::Scope scope,std::string& error){
    metrobackup::Json module(APP->engine->moduleToJson(m));metrobackup::Json document(metrobackup::capture(module.get(),scope));
    metrobackup::Scope checked=metrobackup::All;if(!metrobackup::validate(m,document.get(),checked,error))return false;
    return metrobackup::write(path,document.get(),error);
}
bool exportPresetBackup(Metropolix* m,const std::string& path,int selection,std::string& error){
    if(selection==0)return exportBackup(m,path,metrobackup::Presets,error);
    if(selection<1 || selection>=BackupPresetCount){error="Invalid preset selection.";return false;}
    metrobackup::Json module(APP->engine->moduleToJson(m));metrobackup::Json document(metrobackup::captureSelected(module.get(),selection-2));
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
    if(action==2){openBackupPresets(m);return;}
    auto* filters=osdialog_filters_parse("Metropolix Lab backup:json");
    char* chosen=osdialog_file(action==3?OSDIALOG_OPEN:OSDIALOG_SAVE,nullptr,action==3?nullptr:"MetropolixLab-backup.json",filters);osdialog_filters_free(filters);
    if(!chosen)return;
    std::string path(chosen);std::free(chosen);std::string error;
    bool good=action==3?importBackup(m,path,error):exportBackup(m,path,static_cast<metrobackup::Scope>(action),error);
    if(good){openBackup(m);m->backupCursor.store(action);m->backupNotice=action==3?"IMPORTED / STOPPED":"BACKUP SAVED";}
    else{m->backupNotice="BACKUP FAILED";osdialog_message(OSDIALOG_WARNING,OSDIALOG_OK,error.c_str());}
    m->backupNoticeUntil=system::getTime()+5;
}
void backupPresetChoose(Metropolix* m,int selection){
    if(!m)return;
    auto* filters=osdialog_filters_parse("Metropolix Lab backup:json");
    std::string filename=selection==0?"MetropolixLab-presets.json":selection==1?"MetropolixLab-filled.json":string::f("MetropolixLab-bank-%d.json",selection-1);
    char* chosen=osdialog_file(OSDIALOG_SAVE,nullptr,filename.c_str(),filters);osdialog_filters_free(filters);if(!chosen)return;
    std::string path(chosen);std::free(chosen);std::string error;
    if(exportPresetBackup(m,path,selection,error)){openBackupPresets(m);m->backupPresetCursor.store(selection);m->backupNotice="PRESETS SAVED";}
    else{m->backupNotice="BACKUP FAILED";osdialog_message(OSDIALOG_WARNING,OSDIALOG_OK,error.c_str());}
    m->backupNoticeUntil=system::getTime()+5;
}
void appendBackupPresetMenu(ui::Menu* menu,Metropolix* m){
    if(!m)return;
    menu->addChild(createMenuLabel("Preset export selection"));
    for(int n=0;n<BackupPresetCount;++n)menu->addChild(createMenuItem(backupPresetName(n)+"…","",[m,n]{backupPresetChoose(m,n);}));
    menu->addChild(createMenuLabel("Filled merges slots; Bank replaces only its eight slots"));
}
void appendBackupMenu(ui::Menu* menu,Metropolix* m){if(!m)return;
    menu->addChild(createMenuLabel("Portable backup files"));menu->addChild(createMenuItem("Open Backup files","",[m]{openBackup(m);}));
    const char* names[]={"Export all settings, presets and chains…","Export settings/state…","Export presets and chains…","Import backup…"};
    for(int n=0;n<BackupCount;++n){
        if(n==2)menu->addChild(createSubmenuItem("Export presets…","",[m](ui::Menu* child){appendBackupPresetMenu(child,m);}));
        else menu->addChild(createMenuItem(names[n],"",[m,n]{backupChoose(m,n);}));
    }
    menu->addChild(createMenuLabel("Metropolix Lab JSON; Rack undo restores an import"));
}
