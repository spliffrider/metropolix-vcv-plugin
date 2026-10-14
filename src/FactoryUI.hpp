#include "FactoryRestore.hpp"
std::string labVersion(){return pluginInstance?pluginInstance->version:"Dev";}
bool factoryPage(Metropolix* m){return m && (m->uiPage.load()==Metropolix::FACTORY_PAGE || m->uiPage.load()==Metropolix::FACTORY_CONFIRM_PAGE);}
const char* factoryName(int action){return action==0?"Factory Reset":"Clear All Presets";}
void openFactory(Metropolix* m){if(!m)return;m->cancelDraft();m->tunerActive.store(false);m->uiPage.store(Metropolix::FACTORY_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->setupCursor.store(7);m->factoryConfirmCursor=0;}
void confirmFactory(Metropolix* m,int action){openFactory(m);if(!m)return;m->factoryCursor.store(metro::clamp(action,0,1));m->uiPage.store(Metropolix::FACTORY_CONFIRM_PAGE);}
std::string factoryHelp(Metropolix* m){if(!factoryPage(m))return {};
    if(m->uiPage.load()==Metropolix::FACTORY_CONFIRM_PAGE)return std::string(factoryName(m->factoryCursor.load()))+(m->factoryConfirmCursor?"\nClick to apply one undoable change.":"\nCancel selected; click or EXIT to return.");
    return std::string(factoryName(m->factoryCursor.load()))+(m->factoryCursor.load()?"\nErase 64 saved slots; preserve live settings and chain links.":"\nReset plugin settings; preserve all saved presets and chains.");
}
bool applyFactory(Metropolix* m,int action,std::string& error){
    if(!m)return false;metrobackup::Json before,after;
    if(!metrofactory::restore(m,static_cast<metrofactory::Action>(action),before,after,error))return false;
    auto* change=new history::ModuleChange;change->name=action==0?"reset Metropolix settings":"clear Metropolix preset bank";change->moduleId=m->id;change->oldModuleJ=before.release();change->newModuleJ=after.release();APP->history->push(change);
    m->factoryNotice=action==0?"SETTINGS RESET":"PRESETS CLEARED";m->factoryNoticeUntil=system::getTime()+5;return true;
}
void factoryEncoderPress(Metropolix* m){
    if(m->uiPage.load()==Metropolix::FACTORY_PAGE){confirmFactory(m,m->factoryCursor.load());return;}
    if(!m->factoryConfirmCursor){openFactory(m);return;}
    std::string error;if(!applyFactory(m,m->factoryCursor.load(),error))osdialog_message(OSDIALOG_WARNING,OSDIALOG_OK,error.c_str());
}
void appendFactoryMenu(ui::Menu* menu,Metropolix* m){if(!m)return;
    menu->addChild(createMenuLabel("Metropolix Lab "+labVersion()));
    menu->addChild(createMenuItem("Open maintenance","",[m]{openFactory(m);}));
    for(int action=0;action<2;++action)menu->addChild(createMenuItem(std::string(factoryName(action))+"…","",[m,action]{confirmFactory(m,action);}));
    menu->addChild(createMenuLabel("Confirmation required; one Rack Undo restores either action"));
}
