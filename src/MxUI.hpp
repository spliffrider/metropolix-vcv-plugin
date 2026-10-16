int mxAssignment(Metropolix* m){int mode=m->integer(Metropolix::MX_MODE);return mode==metro::MxStageNum?Metropolix::MX_TRACK:mode==metro::MxModStageNum?Metropolix::MX_LANE:-1;}
int mxField(Metropolix* m){return m->uiAlt.load()?2:metro::clamp(m->uiField.load(),0,2);}
int mxParam(Metropolix* m){int field=mxField(m);return field==2?Metropolix::MIDI_MX_CHANNEL:field?mxAssignment(m):Metropolix::MX_MODE;}
void openMx(Metropolix* m){if(!m)return;m->cancelDraft();m->tunerActive.store(false);m->uiPage.store(Metropolix::MX_SETUP_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);}
std::string mxHelp(Metropolix* m){if(!m)return {};return std::string("MX gates and MIDI output. Click encoder to select mode/assignment; ALT selects MIDI channel. Turn, then press to confirm. Hold encoder to reset assignments/channel.\n")+(m->mxConnected.load()?"Gate expander connected. ":"Place an MX module to the right for gate jacks. ")+"MIDI works without an MX: choose the output device in the MIDI menu. Right-click for modes, channel and CC/Note.";}
void resetMx(Metropolix* m,bool record=true){std::vector<int> ids{Metropolix::MX_MODE,Metropolix::MX_TRACK,Metropolix::MX_LANE,Metropolix::MIDI_MX_CHANNEL};std::vector<float> before;for(int id:ids){before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(m->getParamQuantity(id)->defaultValue);}m->cancelDraft();m->uiField.store(0);if(record)parameterHistory(m,ids,before,"initialize MX assignments");}
void appendMxMenu(ui::Menu* menu,Metropolix* m){if(!m)return;
    menu->addChild(createMenuLabel("MX gate expander"));menu->addChild(createMenuItem("Open MX assignments","",[m]{openMx(m);}));
    appendMidiParam(menu,m,Metropolix::MX_MODE,"MX mode");int id=mxAssignment(m);if(id>=0)appendMidiParam(menu,m,id,id==Metropolix::MX_TRACK?"MX track":"MX MOD lane");
    appendMidiParam(menu,m,Metropolix::MIDI_MX_CHANNEL,"MIDI MX channel");appendMidiParam(menu,m,Metropolix::MIDI_MX_OUTPUT,"MIDI MX output");
    menu->addChild(createMenuItem("Reset MX assignments","",[m]{resetMx(m);}));
    menu->addChild(createMenuLabel(m->mxConnected.load()?"MX connected at the right":"Place the MX module immediately to the right"));
    menu->addChild(createMenuLabel("MIDI MX works without the MX module"));
}
