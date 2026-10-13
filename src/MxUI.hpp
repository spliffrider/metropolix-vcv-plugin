int mxAssignment(Metropolix* m){int mode=m->integer(Metropolix::MX_MODE);return mode==metro::MxStageNum?Metropolix::MX_TRACK:mode==metro::MxModStageNum?Metropolix::MX_LANE:-1;}
int mxParam(Metropolix* m){return m->uiField.load()?mxAssignment(m):Metropolix::MX_MODE;}
void openMx(Metropolix* m){if(!m)return;m->cancelDraft();m->tunerActive.store(false);m->uiPage.store(Metropolix::MX_SETUP_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);}
void resetMx(Metropolix* m,bool record=true){std::vector<int> ids{Metropolix::MX_MODE,Metropolix::MX_TRACK,Metropolix::MX_LANE};std::vector<float> before;for(int id:ids){before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(m->getParamQuantity(id)->defaultValue);}m->cancelDraft();m->uiField.store(0);if(record)parameterHistory(m,ids,before,"initialize MX assignments");}
void appendMxMenu(ui::Menu* menu,Metropolix* m){if(!m)return;
    menu->addChild(createMenuLabel("MX gate expander"));menu->addChild(createMenuItem("Open MX assignments","",[m]{openMx(m);}));
    appendMidiParam(menu,m,Metropolix::MX_MODE,"MX mode");int id=mxAssignment(m);if(id>=0)appendMidiParam(menu,m,id,id==Metropolix::MX_TRACK?"MX track":"MX MOD lane");
    menu->addChild(createMenuItem("Reset MX assignments","",[m]{resetMx(m);}));
    menu->addChild(createMenuLabel(m->mxConnected.load()?"MX connected at the right":"Place the MX module immediately to the right"));
}
