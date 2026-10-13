constexpr int SetupMenuCount=5, GlobalMenuCount=6;
const char* setupMenuName(int n){const char* names[]={"Slider Octaves","Presets Setup...","Preset Chains...","Global Setup...","Init Preset..."};return names[metro::clamp(n,0,SetupMenuCount-1)];}
const char* globalMenuName(int n){const char* names[]={"Clock Src","Clk/Rset Out","Reset Input","Bottom Pitch","Run Btn","Tuner..."};return names[metro::clamp(n,0,GlobalMenuCount-1)];}
int globalMenuParam(Metropolix* m,int item=-1){if(!m)return -1;const int ids[]={Metropolix::CLOCK_SOURCE,Metropolix::CLOCK_RUNNING_ONLY,Metropolix::RESET_INPUT_MODE,Metropolix::BOTTOM_PITCH,Metropolix::RUN_MODE,-1};return ids[metro::clamp(item<0?m->globalCursor.load():item,0,GlobalMenuCount-1)];}
void openTuner(Metropolix* m){if(!m)return;m->uiPage.store(Metropolix::TUNER_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->tunerActive.store(true);}
void openGlobalSetup(Metropolix* m){if(!m)return;m->uiPage.store(Metropolix::GLOBAL_SETUP_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->tunerActive.store(false);}
void appendGlobalMenu(ui::Menu* menu,Metropolix* m){
    if(!m)return;
    menu->addChild(createMenuLabel("Global configuration"));
    menu->addChild(createMenuItem("Open Global Setup","",[m]{openGlobalSetup(m);}));
    for(int n=0;n<GlobalMenuCount-1;++n){int id=globalMenuParam(m,n);auto* q=m->getParamQuantity(id);auto* choice=dynamic_cast<SwitchQuantity*>(q);
        menu->addChild(createIndexSubmenuItem(globalMenuName(n),choice->labels,[q]{return size_t(q->getValue());},[q](size_t value){setWithHistory(q,float(value));}));
    }
    menu->addChild(createCheckMenuItem("Tuner","",[m]{return m->tunerActive.load();},[m]{if(m->tunerActive.load()){m->tunerActive.store(false);openGlobalSetup(m);}else openTuner(m);}));
}
void setupEncoderPress(Metropolix* m){
    if(m->uiPage.load()==Metropolix::SETUP_PAGE){
        switch(m->setupCursor.load()){
            case 0:m->uiField.store(1-m->uiField.load());break;
            case 1:m->uiPage.store(Metropolix::PRESET_SETUP_PAGE);m->uiField.store(0);break;
            case 2:openChainPage(m);break;
            case 3:openGlobalSetup(m);break;
            default:m->requestInitialize();break;
        }
    }else if(m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE){
        int id=globalMenuParam(m);if(id<0){openTuner(m);return;}
        auto* q=m->getParamQuantity(id);
        if(q->maxValue-q->minValue==1)setWithHistory(q,1-q->getValue());
        else m->uiField.store(1-m->uiField.load());
    }else if(m->uiPage.load()==Metropolix::TUNER_PAGE){auto* q=m->getParamQuantity(Metropolix::TUNER_GATE);setWithHistory(q,1-q->getValue());}
}
