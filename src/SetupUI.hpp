constexpr int SetupMenuCount=Metropolix::SetupItemCount, GlobalMenuCount=6, UiSetupCount=8;
const char* setupMenuName(int n){const char* names[]={"Clock","Clock Mode","Run Btn","Reset Input","Bottom Pitch","Tuner...","Total Pulses","Init Preset...","Preset Chains...","MIDI Clock Out","MIDI Transport","MIDI Root Ch","MIDI PC Ch","MIDI MX Ch","MIDI MX Output","Backup Files...","Presets Setup...","UI Setup...","Version / Factory"};return names[metro::clamp(n,0,SetupMenuCount-1)];}
int setupMenuParam(Metropolix* m,int item=-1){if(!m)return -1;const int ids[]={Metropolix::CLOCK_SOURCE,Metropolix::CLOCK_RUNNING_ONLY,Metropolix::RUN_MODE,Metropolix::RESET_INPUT_MODE,Metropolix::BOTTOM_PITCH,-1,Metropolix::TOTAL_PULSES,-1,-1,Metropolix::MIDI_CLOCK_OUT,Metropolix::MIDI_TRANSPORT,Metropolix::MIDI_ROOT_CHANNEL,Metropolix::MIDI_PC_CHANNEL,Metropolix::MIDI_MX_CHANNEL,Metropolix::MIDI_MX_OUTPUT,-1,-1,-1,-1};return ids[metro::clamp(item<0?m->setupCursor.load():item,0,SetupMenuCount-1)];}
std::string setupMenuValue(Metropolix* m,int id){
    if(!m || id<0)return {};
    int value=int(std::round(m->uiValue(id)));std::string text;
    if(id==Metropolix::CLOCK_SOURCE){const char* names[]={"Auto","Internal","External","MIDI"};text=names[metro::clamp(value,0,3)];}
    else if(id==Metropolix::CLOCK_RUNNING_ONLY){const char* names[]={"Always","Running","Legacy"};text=names[metro::clamp(value,0,2)];}
    else if(id==Metropolix::RUN_MODE)text=value?"Run/Pause":"Run/Stop";
    else if(id==Metropolix::RESET_INPUT_MODE){const char* names[]={"Reset","Run","Run-C"};text=names[metro::clamp(value,0,2)];}
    else if(id==Metropolix::RUN_ON_BOOT){const char* names[]={"Off","On","Patch"};text=names[metro::clamp(value,0,2)];}
    else if(id==Metropolix::CV_OUT_DELAY)text=std::to_string(value)+" ms";
    else if(id==Metropolix::TOTAL_PULSES || id==Metropolix::MIDI_ROOT_CHANNEL || id==Metropolix::MIDI_PC_CHANNEL || id==Metropolix::MIDI_MX_CHANNEL)text=value?std::to_string(value):"--";
    else text=previewDisplayString(m,id);
    if(m->uiDraftParam==id && text.find('?')==std::string::npos)text+=" ?";
    return text;
}
// Nonselectable category rows join the four-line list when it scrolls.
int setupDisplayRow(int item){return item+(item>=Metropolix::SetupPulses)+(item>=Metropolix::SetupMidiClock)+(item>=Metropolix::SetupPresets);}
int setupDisplayItem(int row){for(int item=0;item<SetupMenuCount;++item)if(setupDisplayRow(item)==row)return item;return -1;}
const char* setupCategoryName(int row){return row==6?"PRESET":row==10?"MIDI":row==18?"ADVANCED":"";}
std::string setupHelp(Metropolix* m){
    if(!m || m->uiPage.load()!=Metropolix::SETUP_PAGE)return {};
    std::string text=setupMenuName(m->setupCursor.load());int id=setupMenuParam(m);
    if(id>=0){text+=" = "+setupMenuValue(m,id);auto* q=m->getParamQuantity(id);text+=q->getRange()==1?"\nClick to toggle. Hold encoder to restore default.":"\nClick to edit, drag up/down, then click to apply. Hold encoder to restore default.";}
    else text+="\nClick to open. Drag to browse SETUP.";
    return text;
}
const char* uiSetupName(int n){const char* names[]={"Run on Boot","Run Btn","Enc. Confirm","Show Pos.","Button LED Level","Slider LED Level","Screen Level","CV Out Delay"};return names[metro::clamp(n,0,UiSetupCount-1)];}
int uiSetupParam(Metropolix* m,int item=-1){const int ids[]={Metropolix::RUN_ON_BOOT,Metropolix::RUN_MODE,Metropolix::ENC_CONFIRM,Metropolix::SHOW_POSITION,Metropolix::BUTTON_LEVEL,Metropolix::SLIDER_LEVEL,Metropolix::SCREEN_LEVEL,Metropolix::CV_OUT_DELAY};return ids[metro::clamp(item<0?m->uiSetupCursor.load():item,0,UiSetupCount-1)];}
const char* globalMenuName(int n){const char* names[]={"Clock Src","Clk/Rset Out","Reset Input","Bottom Pitch","Run Btn","Tuner..."};return names[metro::clamp(n,0,GlobalMenuCount-1)];}
int globalMenuParam(Metropolix* m,int item=-1){if(!m)return -1;const int ids[]={Metropolix::CLOCK_SOURCE,Metropolix::CLOCK_RUNNING_ONLY,Metropolix::RESET_INPUT_MODE,Metropolix::BOTTOM_PITCH,Metropolix::RUN_MODE,-1};return ids[metro::clamp(item<0?m->globalCursor.load():item,0,GlobalMenuCount-1)];}
void openTuner(Metropolix* m){if(!m)return;m->setupDirectChild.store(m->uiPage.load()==Metropolix::SETUP_PAGE);m->cancelDraft();m->uiPage.store(Metropolix::TUNER_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->tunerActive.store(true);}
void openGlobalSetup(Metropolix* m){if(!m)return;m->uiPage.store(Metropolix::GLOBAL_SETUP_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->tunerActive.store(false);}
void appendGlobalMenu(ui::Menu* menu,Metropolix* m){
    if(!m)return;
    menu->addChild(createMenuLabel("Global configuration"));
    auto* range=m->getParamQuantity(Metropolix::RANGE);std::vector<std::string> ranges;
    for(int n=int(range->getMinValue());n<=int(range->getMaxValue());++n)ranges.push_back(std::to_string(n)+(n==1?" octave":" octaves"));
    menu->addChild(createIndexSubmenuItem("Slider Octaves",ranges,[range]{return size_t(range->getValue()-range->getMinValue());},[range](size_t value){setWithHistory(range,float(value)+range->getMinValue());}));
    appendMidiMenu(menu,m);appendMxMenu(menu,m);
    menu->addChild(createSubmenuItem("Version / Factory…","",[m](ui::Menu* child){appendFactoryMenu(child,m);}));
    menu->addChild(createMenuItem("Open UI Setup","",[m]{m->cancelDraft();m->uiPage.store(Metropolix::UI_SETUP_PAGE);m->uiField.store(0);m->uiAlt.store(false);}));
    for(int n=0;n<UiSetupCount;++n){int id=uiSetupParam(m,n);auto* q=m->getParamQuantity(id);std::vector<std::string> labels;if(auto* choice=dynamic_cast<SwitchQuantity*>(q))labels=choice->labels;else for(int i=int(q->getMinValue());i<=int(q->getMaxValue());++i)labels.push_back(std::to_string(i)+(id==Metropolix::CV_OUT_DELAY?" ms":""));int offset=int(q->getMinValue());menu->addChild(createIndexSubmenuItem(uiSetupName(n),labels,[q,offset]{return size_t(q->getValue()-offset);},[q,offset](size_t value){setWithHistory(q,float(value)+offset);}));}
    menu->addChild(createMenuItem("Open Global Setup","",[m]{openGlobalSetup(m);}));
    for(int n=0;n<GlobalMenuCount-1;++n){int id=globalMenuParam(m,n);auto* q=m->getParamQuantity(id);auto* choice=dynamic_cast<SwitchQuantity*>(q);
        menu->addChild(createIndexSubmenuItem(globalMenuName(n),choice->labels,[q]{return size_t(q->getValue());},[q](size_t value){setWithHistory(q,float(value));}));
    }
    menu->addChild(createCheckMenuItem("Tuner","",[m]{return m->tunerActive.load();},[m]{if(m->tunerActive.load()){m->tunerActive.store(false);openGlobalSetup(m);}else openTuner(m);}));
}
void setupEncoderPress(Metropolix* m){
    if(m->uiPage.load()==Metropolix::SETUP_PAGE){
        int id=setupMenuParam(m);
        if(id>=0){auto* q=m->getParamQuantity(id);if(q->getRange()==1){m->cancelDraft();setWithHistory(q,q->getValue()==q->getMinValue()?q->getMaxValue():q->getMinValue());}else {m->cancelDraft();m->uiField.store(1-m->uiField.load());}return;}
        m->cancelDraft();
        switch(m->setupCursor.load()){
            case Metropolix::SetupTuner:openTuner(m);break;
            case Metropolix::SetupInit:m->requestInitialize();break;
            case Metropolix::SetupChains:openChainPage(m);break;
            case Metropolix::SetupBackup:openBackup(m);break;
            case Metropolix::SetupPresets:openPresetSetup(m);break;
            case Metropolix::SetupUi:m->uiPage.store(Metropolix::UI_SETUP_PAGE);m->uiField.store(0);break;
            case Metropolix::SetupVersion:openFactory(m);break;
            default:break;
        }
    }else if(m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE){
        int id=globalMenuParam(m);if(id<0){openTuner(m);return;}
        auto* q=m->getParamQuantity(id);
        if(q->maxValue-q->minValue==1)setWithHistory(q,1-q->getValue());
        else m->uiField.store(1-m->uiField.load());
    }else if(m->uiPage.load()==Metropolix::UI_SETUP_PAGE){int id=uiSetupParam(m);auto* q=m->getParamQuantity(id);if(q->getRange()==1)setWithHistory(q,1-q->getValue());else m->uiField.store(1-m->uiField.load());}
    else if(m->uiPage.load()==Metropolix::TUNER_PAGE){auto* q=m->getParamQuantity(Metropolix::TUNER_GATE);setWithHistory(q,1-q->getValue());}
}
