#pragma once
void workflowNotice(Metropolix* m,const std::string& message);
bool canPasteScale(Metropolix* m) {
    return m && m->scaleClipboardMask && metro::userScale(m->integer(Metropolix::SCALE));
}
void copyScale(Metropolix* m) {
    if(!m)return;
    m->cancelDraft();int shown=m->integer(Metropolix::SCALE);
    m->scaleClipboardMask=metro::scaleMask(m->uiScale(shown));
    m->scaleClipboardRoot=m->integer(m->scaleRootParam(shown));
    workflowNotice(m,"SCALE COPIED");
}
void pasteScale(Metropolix* m,bool record=true) {
    if(!m)return;
    m->cancelDraft();
    if(!m->scaleClipboardMask){workflowNotice(m,"COPY SCALE FIRST");return;}
    if(!metro::userScale(m->integer(Metropolix::SCALE))){workflowNotice(m,"CHOOSE USER SCALE");return;}
    int slot=m->integer(Metropolix::SCALE)-metro::FactoryScaleCount;
    std::vector<int> ids{Metropolix::userScaleParam(slot),Metropolix::userScaleParam(slot,true)};
    std::vector<float> before;
    for(int id:ids)before.push_back(m->params[id].getValue());
    m->getParamQuantity(ids[0])->setValue(float(m->scaleClipboardMask));
    m->getParamQuantity(ids[1])->setValue(float(m->scaleClipboardRoot));
    if(record)parameterHistory(m,ids,before,"paste user scale");
    workflowNotice(m,"SCALE PASTED");
}
void scaleLockPress(Metropolix* m) {
    if(!m)return;
    int shown=m->integer(Metropolix::SCALE),locked=m->integer(Metropolix::SCALE_LOCK);
    setWithHistory(m->getParamQuantity(Metropolix::SCALE_LOCK),locked==shown?-1:shown);
}
bool scaleEncoderLongEnabled(Metropolix* m) {
    return m && m->uiPage.load()==Metropolix::SCALE_PAGE &&
        (m->scaleEditing.load() || (m->uiField.load()==1 && metro::userScale(m->integer(Metropolix::SCALE))));
}
void scaleEditPress(Metropolix* m) {
    if(!m || !metro::userScale(m->integer(Metropolix::SCALE)))return;
    m->cancelDraft();m->scaleEditing.store(!m->scaleEditing.load());m->uiField.store(1);
}
void scaleToggleNote(Metropolix* m,int note=-1) {
    if(!m || !metro::userScale(m->integer(Metropolix::SCALE)))return;
    int id=Metropolix::userScaleParam(m->integer(Metropolix::SCALE)-49);
    if(note<0)note=m->integer(Metropolix::SCALE_NOTE);
    unsigned next=unsigned(m->integer(id))^(1u<<metro::clamp(note,0,11));
    if(next)setWithHistory(m->getParamQuantity(id),float(next));
}
void scaleShortcut(Metropolix* m,int slot,bool save) {
    if(!m)return;
    m->scaleShortcutUsed.store(true);
    int shown=m->integer(Metropolix::SCALE),root=m->integer(m->scaleRootParam());
    std::vector<int> ids;std::vector<float> before;
    if(save)ids={Metropolix::scaleShortcutParam(slot),Metropolix::scaleShortcutParam(slot,true)};
    else {
        shown=m->integer(Metropolix::scaleShortcutParam(slot));root=m->integer(Metropolix::scaleShortcutParam(slot,true));
        ids={Metropolix::SCALE,m->scaleRootParam(shown)};
        if(m->integer(Metropolix::SCALE_LOCK)>=0)ids.push_back(Metropolix::SCALE_LOCK);
    }
    for(int id:ids)before.push_back(m->params[id].getValue());
    m->params[ids[0]].setValue(shown);m->params[ids[1]].setValue(root);
    if(ids.size()>2)m->params[ids[2]].setValue(shown);
    m->scaleEditing.store(false);m->uiPage.store(Metropolix::SCALE_PAGE);m->uiField.store(1);
    if(!m->scaleShortcutHeld.load())parameterHistory(m,ids,before,save?"save scale shortcut":"recall scale shortcut");
}
void appendScaleMenu(ui::Menu* menu,Metropolix* m) {
    if(!m)return;
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuLabel("Scales and chords"));
    menu->addChild(createMenuItem("Copy displayed scale","ALT + SCALE",[m]{copyScale(m);}));
    menu->addChild(createMenuItem("Paste scale into displayed user slot","Hold ALT + SCALE",[m]{pasteScale(m);},!canPasteScale(m)));
    const char* banks[]={"Major scales","Melodic minor","Harmonic minor","Miscellaneous","World scales","Major chords","Minor chords"};
    for(auto bank:banks)menu->addChild(createSubmenuItem(bank,"",[m,bank](ui::Menu* child){
        for(int p=0;p<49;++p){int id=metro::scaleBrowseId(p);if(std::string(metro::factoryScale(id).bank)!=bank)continue;
            child->addChild(createCheckMenuItem(metro::scaleName(id),"",[m,id]{return m->integer(Metropolix::SCALE)==id;},[m,id]{m->scaleEditing.store(false);setWithHistory(m->getParamQuantity(Metropolix::SCALE),id);}));}
    }));
    for(int bank=0;bank<10;++bank)menu->addChild(createSubmenuItem("User (Bank "+std::to_string(bank+1)+")","",[m,bank](ui::Menu* child){
        for(int n=0;n<10;++n){int id=49+bank*10+n;child->addChild(createCheckMenuItem(metro::scaleName(id),"",[m,id]{return m->integer(Metropolix::SCALE)==id;},[m,id]{m->scaleEditing.store(false);setWithHistory(m->getParamQuantity(Metropolix::SCALE),id);}));}
    }));
    menu->addChild(createMenuItem("Lock / apply / unlock displayed scale","",[m]{scaleLockPress(m);}));
    if(metro::userScale(m->integer(Metropolix::SCALE))) {
        menu->addChild(createCheckMenuItem("Edit user scale notes","",[m]{return m->scaleEditing.load();},[m]{scaleEditPress(m);}));
        menu->addChild(createSubmenuItem("Enabled notes (relative to root)","",[m](ui::Menu* child){
            const char* notes[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
            for(int n=0;n<12;++n)child->addChild(createCheckMenuItem(notes[n],"",[m,n]{return m->integer(Metropolix::userScaleParam(m->integer(Metropolix::SCALE)-49))&(1u<<n);},[m,n]{scaleToggleNote(m,n);}));
        }));
        menu->addChild(createSubmenuItem("Copy factory notes to this user scale","",[m](ui::Menu* child){
            for(int p=0;p<49;++p){int id=metro::scaleBrowseId(p);child->addChild(createMenuItem(metro::scaleName(id),"",[m,id]{setWithHistory(m->getParamQuantity(Metropolix::userScaleParam(m->integer(Metropolix::SCALE)-49)),metro::scaleMask(metro::scaleAt(id)));}));}
        }));
    }
    menu->addChild(createSubmenuItem("Root","",[m](ui::Menu* child){
        const char* notes[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
        for(int n=0;n<12;++n)child->addChild(createCheckMenuItem(notes[n],"",[m,n]{return m->integer(m->scaleRootParam())==n;},[m,n]{setWithHistory(m->getParamQuantity(m->scaleRootParam()),n);}));
    }));
    menu->addChild(createSubmenuItem("Scale shortcuts","",[m](ui::Menu* child){
        for(int s=0;s<8;++s){child->addChild(createMenuItem("Recall shortcut "+std::to_string(s+1)+": "+metro::scaleName(m->integer(Metropolix::scaleShortcutParam(s))),"",[m,s]{scaleShortcut(m,s,false);}));child->addChild(createMenuItem("Save current scale/root to "+std::to_string(s+1),"",[m,s]{scaleShortcut(m,s,true);}));}
    }));
}
