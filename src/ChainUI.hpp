struct ChainChange : history::ModuleAction {
    Metropolix::ChainState before, after;
    void set(const Metropolix::ChainState& s) {
        if(auto* m=dynamic_cast<Metropolix*>(APP->engine->getModule(moduleId)))m->setChainState(s);
    }
    void undo() override { set(before); }
    void redo() override { set(after); }
};
void chainHistory(Metropolix* m,const Metropolix::ChainState& before,const char* name) {
    auto* h=new ChainChange;h->moduleId=m->id;h->before=before;h->after=m->getChainState();h->name=name;APP->history->push(h);
}
void changeChain(Metropolix* m,const Metropolix::ChainState& state,const char* name) {
    auto before=m->getChainState();m->setChainState(state);chainHistory(m,before,name);
}
bool chainPage(Metropolix* m) { return m && m->uiPage.load()==Metropolix::CHAIN_PAGE; }
void openChainPage(Metropolix* m) {
    if(!m)return;
    screenPage(m,Metropolix::CHAIN_PAGE);m->uiAlt.store(false);m->chainEdit.store(0);
}
int selectedChainLink(Metropolix* m) { return m->chainCursor.load()/2; }
void toggleChain(Metropolix* m) {
    auto s=m->getChainState();if(!s.working.size)return;
    s.enabled=!s.enabled;changeChain(m,s,s.enabled?"enable preset chain":"disable preset chain");m->chainEdit.store(0);
    if(s.enabled) {m->chainCursor.store(1);m->chainFollow.store(true);}
}
void chainEncoderPress(Metropolix* m) {
    auto s=m->getChainState();int cursor=m->chainCursor.load(),edit=m->chainEdit.load();
    if(cursor==2*s.working.size+1) {toggleChain(m);return;}
    if(s.enabled) {if(m->chainFollow.load() && m->chainPlayingLink.load()>=0)m->chainCursor.store(2*m->chainPlayingLink.load()+1);m->chainFollow.store(!m->chainFollow.load());return;}
    if(cursor==2*s.working.size+2) {
        s.working=metro::PresetChain();s.loaded=-1;changeChain(m,s,"clear preset chain");m->chainCursor.store(0);m->chainEdit.store(0);return;
    }
    if(edit==0) {
        int i=cursor/2;
        if(cursor%2==0) {
            metro::ChainLink l;l.slot=m->presetBankIndex.load()*8;
            if(!s.working.insert(i,l))return;
            s.loaded=-1;changeChain(m,s,"insert chain link");m->chainCursor.store(2*i+1);m->chainCreating=(i==s.working.size-1);
        } else m->chainCreating=false;
        m->chainEdit.store(1);m->presetBankIndex.store(s.working.links[i].slot/8);
    } else if(edit==1)m->chainEdit.store(2);
    else {
        m->chainEdit.store(0);
        if(m->chainCreating) {m->chainCursor.store(2*s.working.size);chainEncoderPress(m);}
    }
}
void chainTurn(Metropolix* m,int delta) {
    auto s=m->getChainState();int edit=m->chainEdit.load(),i=selectedChainLink(m);
    if(m->chainQuick || edit==1) {
        int bank=metro::modulo(m->presetBankIndex.load()+delta,8);m->presetBankIndex.store(bank);
        if(!m->chainQuick && i<s.working.size) {s.working.links[i].slot=8*bank+s.working.links[i].slot%8;m->setChainState(s);}
    } else if(edit==2 && i<s.working.size && !s.enabled) {
        s.working.links[i].repeats=metro::clamp(s.working.links[i].repeats+delta,1,64);m->setChainState(s);
    } else {
        if(s.enabled && m->chainFollow.load() && m->chainPlayingLink.load()>=0)m->chainCursor.store(2*m->chainPlayingLink.load()+1);
        m->chainFollow.store(false);m->chainCursor.store(metro::clamp(m->chainCursor.load()+delta,0,2*s.working.size+2));
    }
}
void chainMute(Metropolix* m,int track) {
    if(m->chainQuick){m->chainQuickMutes^=1u<<track;return;}
    auto s=m->getChainState();int i=selectedChainLink(m);
    if(!s.enabled && m->chainEdit.load() && i<s.working.size) {s.working.links[i].mutes^=1u<<track;changeChain(m,s,"change chain link mutes");}
}
void chainDelete(Metropolix* m) {
    auto s=m->getChainState();if(s.enabled || m->chainCursor.load()%2==0)return;
    if(s.working.erase(selectedChainLink(m))) {s.loaded=-1;changeChain(m,s,"delete chain link");m->chainEdit.store(0);m->chainCursor.store(std::min(m->chainCursor.load(),2*s.working.size));}
}
void chainSave(Metropolix* m,int slot,bool confirm=false) {
    auto s=m->getChainState();if(s.enabled || !s.working.size)return;
    if(s.saved[slot].size && !confirm) {
        auto* menu=createMenu();menu->addChild(createMenuLabel(string::f("Chain %d",slot+1)));
        menu->addChild(createMenuItem("Confirm overwrite","",[m,slot]{chainSave(m,slot,true);}));menu->addChild(createMenuItem("Cancel","",[]{}));return;
    }
    s.saved[slot]=s.working;s.loaded=slot;changeChain(m,s,"save preset chain");
}
void chainStage(Metropolix* m,int stage) {
    auto s=m->getChainState();if(s.enabled)return;
    if(m->chainQuick) {
        metro::ChainLink l;l.slot=8*m->presetBankIndex.load()+stage;l.mutes=m->chainQuickMutes;
        if(s.working.size && s.working.links[s.working.size-1].slot==l.slot && s.working.links[s.working.size-1].mutes==l.mutes)
            s.working.links[s.working.size-1].repeats=std::min(64,s.working.links[s.working.size-1].repeats+1);
        else if(!s.working.insert(s.working.size,l))return;
        s.loaded=-1;changeChain(m,s,"add quick chain link");m->chainCursor.store(2*s.working.size);return;
    }
    if(m->chainEdit.load()==1) {
        int i=selectedChainLink(m);s.working.links[i].slot=8*m->presetBankIndex.load()+stage;changeChain(m,s,"assign chain preset");
    } else if(!m->chainEdit.load() && s.saved[stage].size) {
        s.working=s.saved[stage];s.loaded=stage;changeChain(m,s,"load preset chain");m->chainCursor.store(0);
    }
}
void beginQuickChain(Metropolix* m) {
    if(m->chainEnabled.load())return;
    auto s=m->getChainState();s.working=metro::PresetChain();s.loaded=-1;changeChain(m,s,"new quick chain");
    m->chainQuick=true;m->chainQuickCancel=false;m->chainQuickMutes=0;m->chainEdit.store(0);m->chainCursor.store(0);
}
void finishQuickChain(Metropolix* m) {
    if(!m->chainQuick)return;
    m->chainQuick=false;
    if(!m->chainQuickCancel && m->getChainState().working.size)toggleChain(m);
}
void appendChainMenu(ui::Menu* menu,Metropolix* m) {
    if(!m)return;
    menu->addChild(new ui::MenuSeparator);menu->addChild(createMenuLabel("Preset chains: 32 links / 8 storage slots"));
    menu->addChild(createMenuItem("Open Preset Chains","Hold SETUP",[m]{openChainPage(m);}));
    menu->addChild(createMenuItem(m->chainEnabled.load()?"Disable chain":"Enable chain","",[m]{toggleChain(m);},!m->getChainState().working.size));
    auto s=m->getChainState();bool locked=s.enabled;
    int i=selectedChainLink(m);
    if(chainPage(m) && m->chainCursor.load()%2 && i<s.working.size && m->chainCursor.load()<2*s.working.size) {
        menu->addChild(createMenuLabel(string::f("Link %d",i+1)));
        menu->addChild(createSubmenuItem("Preset","",[m,i](ui::Menu* banks) {
            const char* names[]={"Red","Orange","Yellow","Green","Cyan","Blue","Magenta","Pink"};
            for(int b=0;b<8;++b)banks->addChild(createSubmenuItem(names[b],"",[m,i,b](ui::Menu* slots) {
                for(int p=0;p<8;++p)slots->addChild(createMenuItem(string::f("Preset %d%s",p+1,m->getPreset(8*b+p).occupied?"":" (empty)"),"",[m,i,b,p] {
                    auto state=m->getChainState();if(state.enabled || i>=state.working.size)return;state.working.links[i].slot=8*b+p;changeChain(m,state,"assign chain preset");
                }));
            }));
        },locked));
        std::vector<std::string> counts;for(int n=1;n<=64;++n)counts.push_back(string::f("%dx",n));
        menu->addChild(createIndexSubmenuItem("Repeats",counts,[m,i]{return size_t(m->getChainState().working.links[i].repeats-1);},[m,i](size_t n){auto state=m->getChainState();if(state.enabled || i>=state.working.size)return;state.working.links[i].repeats=int(n)+1;changeChain(m,state,"change chain repeats");},locked));
        for(int t=0;t<3;++t)menu->addChild(createCheckMenuItem(t==2?"Mute MOD lanes":string::f("Mute Track %d",t+1),"",[m,i,t]{return bool(m->getChainState().working.links[i].mutes&(1u<<t));},[m,i,t]{auto state=m->getChainState();if(state.enabled || i>=state.working.size)return;state.working.links[i].mutes^=1u<<t;changeChain(m,state,"change chain mutes");},locked));
        for(int d:{-1,1})menu->addChild(createMenuItem(d<0?"Move link up":"Move link down","",[m,i,d]{auto state=m->getChainState();if(state.enabled || i>=state.working.size || i+d<0 || i+d>=state.working.size)return;std::swap(state.working.links[i],state.working.links[i+d]);changeChain(m,state,"move chain link");m->chainCursor.store(2*(i+d)+1);},locked || i+d<0 || i+d>=s.working.size));
        menu->addChild(createMenuItem("Delete link","Hold encoder",[m]{chainDelete(m);},locked));
    }
    menu->addChild(createMenuItem("Insert link here","",[m]{m->chainEdit.store(0);m->chainCursor.store(2*selectedChainLink(m));chainEncoderPress(m);},locked || s.working.size==32));
    menu->addChild(createMenuItem("New Quick Chain","",[m]{openChainPage(m);beginQuickChain(m);},locked));
    if(m->chainQuick)menu->addChild(createMenuItem("Finish Quick Chain","",[m]{finishQuickChain(m);}));
    for(int slot=0;slot<8;++slot)menu->addChild(createSubmenuItem(string::f("Chain slot %d%s",slot+1,s.saved[slot].size?" (saved)":" (empty)"),"",[m,slot](ui::Menu* actions) {
        actions->addChild(createMenuItem("Load","",[m,slot]{auto state=m->getChainState();if(state.enabled)return;state.working=state.saved[slot];state.loaded=slot;changeChain(m,state,"load preset chain");m->chainCursor.store(0);m->chainEdit.store(0);},m->chainEnabled.load() || !m->getChainState().saved[slot].size));
        actions->addChild(createMenuItem("Save current chain","Hold stage button",[m,slot]{chainSave(m,slot);},m->chainEnabled.load() || !m->getChainState().working.size));
        actions->addChild(createMenuItem("Clear saved chain","",[m,slot]{auto state=m->getChainState();if(state.enabled)return;state.saved[slot]=metro::PresetChain();if(state.loaded==slot)state.loaded=-1;changeChain(m,state,"clear saved chain");},m->chainEnabled.load()));
    }));
    menu->addChild(createIndexSubmenuItem("Run button",{"Run/Stop: restart at Link 1","Run/Pause: resume"},[m]{return size_t(m->integer(Metropolix::RUN_MODE));},[m](size_t n){setWithHistory(m->getParamQuantity(Metropolix::RUN_MODE),float(n));}));
    menu->addChild(createMenuLabel("Save the Rack patch to keep chains and preset banks."));
}
