constexpr int MidiSetupCount=8,ModMenuCount=4;
const char* midiSetupName(int n){const char* names[]={"MIDI Clock Out","MIDI Transport","MIDI Root Ch","MIDI PC Ch","MIDI MX Ch","MIDI MX Output","Note at 0V","Backup files…"};return names[metro::clamp(n,0,MidiSetupCount-1)];}
int midiSetupParam(Metropolix* m,int item=-1){const int ids[]={Metropolix::MIDI_CLOCK_OUT,Metropolix::MIDI_TRANSPORT,Metropolix::MIDI_ROOT_CHANNEL,Metropolix::MIDI_PC_CHANNEL,Metropolix::MIDI_MX_CHANNEL,Metropolix::MIDI_MX_OUTPUT,Metropolix::MIDI_ZERO_NOTE};int n=metro::clamp(item<0?m->midiSetupCursor.load():item,0,MidiSetupCount-1);return n==7?-1:ids[n];}
const char* modMenuName(int n){const char* names[]={"MIDI Channel","MIDI Mod to CC","Init Lane Defaults","Init All Lanes"};return names[metro::clamp(n,0,ModMenuCount-1)];}
int modMenuParam(Metropolix* m,int item=-1){int n=item<0?m->modMenuCursor.load():item;return n<2?Metropolix::midiModParam(m->selectedLane.load(),n):-1;}
void openMidiSetup(Metropolix* m){if(!m)return;m->cancelDraft();m->uiPage.store(Metropolix::MIDI_SETUP_PAGE);m->uiField.store(0);m->uiAlt.store(false);}
void midiEncoderPress(Metropolix* m){int id=m->uiPage.load()==Metropolix::MOD_MENU_PAGE?modMenuParam(m):midiSetupParam(m);if(id<0){if(m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE)openBackup(m);else initModLane(m,m->modMenuCursor.load()==3);return;}auto* q=m->getParamQuantity(id);if(q->getRange()==1)setWithHistory(q,1-q->getValue());else m->uiField.store(1-m->uiField.load());}
void appendMidiParam(ui::Menu* menu,Metropolix* m,int id,const std::string& label){auto* q=m->getParamQuantity(id);if(auto* sw=dynamic_cast<SwitchQuantity*>(q))menu->addChild(createIndexSubmenuItem(label,sw->labels,[q]{return size_t(q->getValue());},[q](size_t n){setWithHistory(q,float(n));}));else menu->addChild(createSubmenuItem(label,"",[q](ui::Menu* child){for(int n=0;n<128;++n)child->addChild(createCheckMenuItem(std::to_string(n),"",[q,n]{return q->getValue()==n;},[q,n]{setWithHistory(q,float(n));}));}));}
void appendMidiMenu(ui::Menu* menu,Metropolix* m){
    if(!m)return;
    menu->addChild(createMenuLabel("MIDI"));menu->addChild(createMenuItem("Open MIDI Setup","",[m]{openMidiSetup(m);}));
    menu->addChild(createSubmenuItem("MIDI input device","",[m](ui::Menu* child){app::appendMidiMenu(child,&m->midiInput);}));
    menu->addChild(createSubmenuItem("MIDI output device","",[m](ui::Menu* child){app::appendMidiMenu(child,&m->midiOutput);}));
    menu->addChild(createMenuItem("Release MIDI notes","",[m]{m->midiPanicRequest.store(true);}));
    for(int n=0;n<MidiSetupCount;++n){int id=midiSetupParam(m,n);if(id>=0)appendMidiParam(menu,m,id,midiSetupName(n));else menu->addChild(createMenuItem("Backup files…","",[m]{openBackup(m);}));}
    menu->addChild(createSubmenuItem("Track MIDI settings","",[m](ui::Menu* child){int t=m->selectedTrack.load();const char* names[]={"Channel","CV to CC","Slide Toggle CC","Slide Amount CC","Slide Amount"};for(int n=0;n<Metropolix::MIDI_TRACK_STRIDE;++n)appendMidiParam(child,m,Metropolix::midiTrackParam(t,n),names[n]);}));
    menu->addChild(createSubmenuItem("MOD MIDI settings","",[m](ui::Menu* child){int l=m->selectedLane.load();appendMidiParam(child,m,Metropolix::midiModParam(l,0),"Channel");appendMidiParam(child,m,Metropolix::midiModParam(l,1),"MOD to CC");}));
}
