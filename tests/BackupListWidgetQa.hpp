// Rendering geometry and real navigation; file and maintenance actions follow
// in BackupWidgetQa.hpp, including actual imports, Undo/Redo and disarmed reopen.
APP->engine->bypassModule(m,true);
std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};
auto clean=[&]{closeMenus();m->cancelDraft();m->uiChooseFunction.store(false);m->uiAlt.store(false);m->backupNoticeUntil=m->factoryNoticeUntil=0;for(auto* a:APP->history->actions)delete a;APP->history->actions.clear();APP->history->savedIndex=APP->history->actionIndex=0;};
auto open=[&](int kind){if(kind==0){openBackup(m);m->backupCursor.store(0);}else if(kind==1){openBackupPresets(m);m->backupPresetCursor.store(0);}else if(kind==2){openFactory(m);m->factoryCursor.store(0);}else confirmFactory(m,kind-3);};
auto cursor=[&](int kind){return kind==0?m->backupCursor.load():kind==1?m->backupPresetCursor.load():kind==2?m->factoryCursor.load():m->factoryConfirmCursor;};
const int counts[]={4,10,2,2,2};
auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));auto* encoder=find("Encoder");
bool navigation=true;int navCount=0;
for(auto* control:{encoder,static_cast<HardwareControl*>(display)})for(int kind=0;kind<5;++kind)for(int item=0;item<counts[kind];++item){clean();open(kind);auto before=raw();if(item)drag(control,-22.f*item);control->bind();navigation &= cursor(kind)==item && screenParam(m)<0 && raw()==before && !APP->history->actionIndex && !encoder->longEnabled() && encoder->groupedParams().empty();drag(control,3000);navigation &= cursor(kind)==0 && raw()==before;drag(control,-3000);navigation &= cursor(kind)==counts[kind]-1 && raw()==before && !APP->history->actionIndex;++navCount;}
check(navigation,std::to_string(navCount)+" Backup/maintenance encoder/OLED selection and boundary cases preserve parameters/history and remain unbound");
const char* actions[]={"Export All","Export Settings","Export Presets","Import Backup"};
const char* factoryActions[]={"Factory Reset","Clear All Presets"};
const char* backupNotices[]={"","BACKUP SAVED","IMPORTED / STOPPED","BACKUP FAILED"};
const char* presetNotices[]={"","PRESETS SAVED","BACKUP FAILED"};
const char* factoryNotices[]={"","SETTINGS RESET","PRESETS CLEARED"};
bool pixels=true,spacing=true;int pixelCount=0;
for(int kind=0;kind<5;++kind)for(int item=0;item<counts[kind];++item)for(int notice=0;notice<(kind==0?4:kind<3?3:1);++notice){
    clean();open(kind);if(kind==0)m->backupCursor.store(item);else if(kind==1)m->backupPresetCursor.store(item);else if(kind==2)m->factoryCursor.store(item);else m->factoryConfirmCursor=item;
    OledCanvas expected;auto row=[&](int position,const std::string& text,bool selected){int y=12+12*position;if(selected)expected.fill(0,y-1,128,12);expected.menuText(1,y,text,selected?0:255);spacing &= expected.menuWidth(text)+2<=128;};
    if(kind<2){expected.text(1,1,kind?"PRESET EXPORT":"BACKUP FILES");int start=item<4?0:std::min(item-1,6);for(int n=0;n<4;++n){int current=start+n;std::string label=kind?(current==0?"All Presets":current==1?"Filled Presets":"Bank "+std::to_string(current-1)):actions[current];row(n,label,current==item);}const char* text=kind?presetNotices[notice]:backupNotices[notice];if(notice){m->backupNotice=text;m->backupNoticeUntil=system::getTime()+5;}expected.text(1,59,notice?text:kind?"CLICK TO EXPORT":"JSON BACKUP");}
    else if(kind==2){expected.text(1,1,"LAB "+labVersion());for(int n=0;n<2;++n)row(n,factoryActions[n],n==item);if(notice){m->factoryNotice=factoryNotices[notice];m->factoryNoticeUntil=system::getTime()+5;}expected.text(1,59,notice?factoryNotices[notice]:"CONFIRM / UNDO");}
    else {expected.text(1,1,kind==4?"CLEAR 64 PRESETS?":"RESET SETTINGS?");expected.text(1,14,kind==4?"KEEP LIVE SETTINGS":"KEEP 64 PRESETS");row(2,"Cancel",item==0);row(3,"Apply",item==1);}
    auto before=raw();auto actual=display->render();pixels &= std::memcmp(expected.pixels,actual.pixels,sizeof(expected.pixels))==0 && raw()==before && !APP->history->actionIndex;
    std::ofstream out(dir+"/list-"+std::to_string(kind)+"-item"+std::to_string(item)+"-notice"+std::to_string(notice)+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<char*>(actual.pixels),128*64);++pixelCount;
}
check(pixels,std::to_string(pixelCount)+" Backup/maintenance pixel states retain four compact rows, mixed-case descenders, inverted selection, complete status lines and Cancel/Apply scope");check(spacing,"all Backup/maintenance action labels fit the OLED without abbreviation");
bool expired=true;for(int kind=0;kind<3;++kind){clean();open(kind);auto before=display->render();m->backupNotice=m->factoryNotice="BACKUP FAILED";m->backupNoticeUntil=m->factoryNoticeUntil=system::getTime()-1;auto after=display->render();expired &= std::memcmp(before.pixels,after.pixels,sizeof(before.pixels))==0;}check(expired,"expired Backup/maintenance notices restore their own footer without masking list rows");
bool cancelled=true;for(auto* control:{encoder,static_cast<HardwareControl*>(display)})for(int action=0;action<2;++action){clean();confirmFactory(m,action);auto before=raw();click(control);cancelled &= m->uiPage.load()==M::FACTORY_PAGE && raw()==before && !APP->history->actionIndex;confirmFactory(m,action);drag(control,-22);click(find("EXIT"));cancelled &= m->uiPage.load()==M::FACTORY_PAGE && raw()==before && !APP->history->actionIndex;}
check(cancelled,"both controls retain Cancel-first maintenance and EXIT cancellation from Apply without parameter/history changes");
bool menus=true;const char* menuNames[]={"Import backup…","Bank 8…","Factory Reset…"};for(auto* control:{encoder,static_cast<HardwareControl*>(display)})for(int kind=0;kind<3;++kind){clean();open(kind);auto before=raw();press(control,GLFW_MOUSE_BUTTON_RIGHT);menus &= seek(APP->scene,[&](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text==menuNames[kind];})!=nullptr && raw()==before && !APP->history->actionIndex;closeMenus();}check(menus,"both controls preserve named Backup, bank and maintenance native context menus");
clean();for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
