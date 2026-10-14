bool timingPage(Metropolix* m) {
    return m && (m->uiPage.load()==Metropolix::LENGTH_PAGE || m->uiPage.load()==Metropolix::DIV_PAGE);
}
bool modTimingLanePage(Metropolix* m) {
    if(!m || !m->uiMod.load() || m->uiAlt.load() || m->uiChooseFunction.load()
        || m->timingShortcutHeld.load() || m->orderShortcutHeld.load()
        || m->scaleShortcutHeld.load() || m->loopyMuteHeld.load())return false;
    int page=m->uiPage.load();
    return page==Metropolix::ORDER_PAGE || page==Metropolix::LENGTH_PAGE || page==Metropolix::DIV_PAGE;
}
void selectModTimingLane(Metropolix* m,int lane) {
    if(!modTimingLanePage(m) || lane<0 || lane>=8)return;
    // A different lane cannot inherit the old lane's pending parameter edit.
    if(m->selectedLane.load()!=lane){m->cancelDraft();m->selectedLane.store(lane);}
}
void modTimingLaneMenu(Metropolix* m,int lane) {
    if(!modTimingLanePage(m) || lane<0 || lane>=8)return;
    selectModTimingLane(m,lane);
    auto* menu=createMenu();menu->addChild(createMenuLabel(string::f("MOD lane %d",lane+1)));
    menu->addChild(createMenuItem("Lane settings","",[m,lane]{modSettingsMenu(m,lane);}));
    menu->addChild(createMenuItem("Edit stage values","",[m,lane]{m->uiMod.store(true);m->uiAlt.store(false);m->selectedLane.store(lane);screenPage(m,Metropolix::STAGE_PAGE);}));
}
int timingParam(Metropolix* m,int page,int field) {
    if(!m)return -1;
    bool mod=m->uiMod.load();int target=mod?m->selectedLane.load():m->selectedTrack.load();
    if(page==Metropolix::LENGTH_PAGE) {
        if(field==2)return mod?Metropolix::modParam(target,Metropolix::MOD_STAGE_OFFSET):Metropolix::trackParam(target,Metropolix::OFFSET);
        return mod?Metropolix::modParam(target,field?Metropolix::MOD_PULSE_LIMIT:Metropolix::MOD_LENGTH):field?Metropolix::extraParam(target,Metropolix::PULSE_LIMIT):Metropolix::trackParam(target,Metropolix::LENGTH);
    }
    return mod?Metropolix::modParam(target,Metropolix::MOD_DIVISION):field?Metropolix::extraParam(target,Metropolix::PULSE_DIV):Metropolix::trackParam(target,Metropolix::DIVISION);
}
std::vector<int> timingIds(Metropolix* m,int page,bool allTargets=false) {
    if(!m)return {};
    std::vector<int> ids;
    auto append=[&](bool mod,int target) {
        std::vector<int> next;
        if(page==Metropolix::LENGTH_PAGE)next=mod?std::vector<int>{Metropolix::modParam(target,Metropolix::MOD_LENGTH),Metropolix::modParam(target,Metropolix::MOD_STAGE_OFFSET),Metropolix::modParam(target,Metropolix::MOD_PULSE_LIMIT)}:std::vector<int>{Metropolix::trackParam(target,Metropolix::LENGTH),Metropolix::trackParam(target,Metropolix::OFFSET),Metropolix::extraParam(target,Metropolix::PULSE_LIMIT)};
        else next=mod?std::vector<int>{Metropolix::modParam(target,Metropolix::MOD_DIVISION)}:std::vector<int>{Metropolix::trackParam(target,Metropolix::DIVISION),Metropolix::extraParam(target,Metropolix::PULSE_DIV)};
        for(int id:next)if(m->ctrlOwner(id)<0)ids.push_back(id);
    };
    if(allTargets)for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target)append(mod,target);
    else append(m->uiMod.load(),m->uiMod.load()?m->selectedLane.load():m->selectedTrack.load());
    return ids;
}
void openTiming(Metropolix* m,int page) {
    if(!m)return;
    // Re-pressing a panel button must retain the field selected by the encoder.
    if(m->uiPage.load()!=page)screenPage(m,page);else m->cancelDraft();
    m->uiAlt.store(false);m->uiChooseFunction.store(false);
}
int timingShortcutValue(int page,int field,bool mod,int slot) {
    const int clocks[]={1,2,3,4,6,8,16,32};
    return page==Metropolix::LENGTH_PAGE?(field?8*(slot+1):slot+1):field && !mod?slot+1:clocks[slot];
}
std::string timingShortcutLabel(Metropolix* m,int page,int slot) {
    int field=m && m->uiPage.load()==page?m->uiField.load():0;
    bool mod=m && m->uiMod.load();int value=timingShortcutValue(page,field,mod,slot);
    return page==Metropolix::LENGTH_PAGE?std::to_string(value)+(field?" pulses":" stages"):(field && !mod?"Pulse count /":"Clock /")+std::to_string(value);
}
void timingShortcut(Metropolix* m,int page,int slot,bool record=true) {
    if(!m || slot<0 || slot>7)return;
    int field=m->uiPage.load()==page?m->uiField.load():0;
    openTiming(m,page);m->timingShortcutUsed.store(true);
    int id=timingParam(m,page,field);
    if(m->ctrlOwner(id)>=0){workflowNotice(m,"TIMING OWNED BY CTRL");return;}
    float value=float(timingShortcutValue(page,field,m->uiMod.load(),slot));
    if(record)setWithHistory(m->getParamQuantity(id),value);else m->getParamQuantity(id)->setValue(value);
}
void resetTiming(Metropolix* m,bool record=true) {
    if(!timingPage(m))return;
    m->cancelDraft();auto ids=timingIds(m,m->uiPage.load());std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(m->getParamQuantity(id)->defaultValue);}
    if(record)parameterHistory(m,ids,before,"reset timing screen");
}
void appendTimingMenu(ui::Menu* menu,Metropolix* m,int page) {
    if(!m)return;
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuLabel(page==Metropolix::LENGTH_PAGE?"Sequence length":"Track division"));
    int fields=page==Metropolix::LENGTH_PAGE?3:m->uiMod.load()?1:2;
    for(int field=0;field<fields;++field) {
        const char* label=page==Metropolix::LENGTH_PAGE?(field==0?"Edit stages":field==1?"Edit pulses":"Edit stage offset"):field==0?"Edit clock division":"Edit pulse count division";
        menu->addChild(createMenuItem(label,"",[m,page,field]{openTiming(m,page);m->uiField.store(field==2?0:field);m->uiAlt.store(field==2);}));
    }
    if(page==Metropolix::LENGTH_PAGE) {
        int id=timingParam(m,page,2);
        menu->addChild(createIndexSubmenuItem("Stage offset",{"0","1","2","3","4","5","6","7"},[m,id]{return size_t(m->integer(id));},[m,id](size_t value){if(m->ctrlOwner(id)<0)setWithHistory(m->getParamQuantity(id),float(value));},m->ctrlOwner(id)>=0));
    }
    int field=m->uiPage.load()==page?m->uiField.load():0,id=timingParam(m,page,field);
    for(int slot=0;slot<8;++slot)menu->addChild(createMenuItem("Set "+timingShortcutLabel(m,page,slot),"",[m,page,slot]{timingShortcut(m,page,slot);},m->ctrlOwner(id)>=0));
    menu->addChild(createMenuItem(page==Metropolix::LENGTH_PAGE?"Reset all length fields":"Reset all division fields","Hold encoder",[m,page]{openTiming(m,page);resetTiming(m);},timingIds(m,page).empty()));
}
