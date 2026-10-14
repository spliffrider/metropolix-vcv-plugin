// Build interactive components over the 34 HP artwork. Existing parameter and
// port IDs are preserved; AUX IDs are appended.
void MetropolixWidget::buildHardware(Metropolix* m) {
    auto control=[&](const std::string& name, float x,float y,float size,HardwareControl::Look look) {
        auto* c=new HardwareControl;
        c->metroModule=m; c->name=name; c->look=look;
        c->box=Rect(Vec(x-size/2,y-size/2),Vec(size,size));
        addChild(c); return c;
    };
    auto unavailable=[m](const std::string& name) {
        screenPage(m,Metropolix::UNAVAILABLE_PAGE);
        auto* menu=createMenu(); menu->addChild(createMenuLabel(name));
        menu->addChild(createMenuLabel("This hardware function is not implemented yet."));
    };
    auto choice=[](ui::Menu* menu,Metropolix* module,const std::string& label,int id,std::vector<std::string> values,int offset=0) {
        if(!module) return;
        auto* q=module->getParamQuantity(id);
        menu->addChild(createIndexSubmenuItem(label,values,
            [q,offset] { return static_cast<size_t>(std::round(q->getValue())-offset); },
            [q,offset](size_t i) { setWithHistory(q,static_cast<float>(i+offset)); }));
    };
    auto stageEditor=[m] { if(m) { m->uiMod.store(false); m->modEditor.store(false); m->expanded.store(true); } };
    auto modEditor=[m] { if(m) { m->uiMod.store(true); m->modEditor.store(true); m->expanded.store(true); } };
    auto laneMenu=[m,modEditor] {
        if(!m) return;
        auto* menu=createMenu(); menu->addChild(createMenuLabel("MOD lanes"));
        for(int l=0;l<8;++l) menu->addChild(createCheckMenuItem(string::f("MOD %d",l+1),"",
            [m,l] { return m->selectedLane.load()==l; },
            [m,l] { m->selectedLane.store(l); m->uiMod.store(true); screenPage(m,Metropolix::STAGE_PAGE); }));
        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createCheckMenuItem("Mute all MOD lanes","",[m]{return m->integer(Metropolix::MOD_GROUP_MUTE)!=0;},[m]{setWithHistory(m->getParamQuantity(Metropolix::MOD_GROUP_MUTE),1-m->integer(Metropolix::MOD_GROUP_MUTE));}));
        menu->addChild(createMenuItem("Selected lane settings","",[m] { modSettingsMenu(m,m->selectedLane.load()); }));
        menu->addChild(createMenuItem("Open MOD editor","",modEditor));
    };
    // Outputs and inputs retain their original IDs; moving a jack does not
    // change any saved cable's destination.
    const int outIds[]={Metropolix::A_OUT,Metropolix::B_OUT,Metropolix::CLOCK_OUT,Metropolix::PITCH_1,Metropolix::PITCH_2,Metropolix::GATE_1,Metropolix::GATE_2};
    const Vec outPos[]={Vec(24,45),Vec(61,45),Vec(98,45),Vec(171,45),Vec(208,45),Vec(171,96),Vec(208,96)};
    for(int i=0;i<7;++i) addOutput(createOutputCentered<MetalPort>(outPos[i],m,outIds[i]));
    addInput(createInputCentered<MetalPort>(Vec(135.5f,45),m,Metropolix::CLOCK_IN));
    addInput(createInputCentered<MetalPort>(Vec(135.5f,96),m,Metropolix::RESET_IN));
    for(int i=0;i<3;++i) {
        std::string name=std::string("AUX ")+char('X'+i);
        addInput(createInputCentered<MetalPort>(Vec(24+i*37,96),m,Metropolix::AUX_X_IN+i));
        auto open=[m,i] { if(m) { m->selectedAux.store(i);screenPage(m,Metropolix::AUX_PAGE); } };
        auto menu=[m,i](ui::Menu* menu) { appendAuxMenu(menu,m,i); };
        auto detail=[m,i] {
            if(!m)return std::string();
            int dest=m->integer(Metropolix::auxParam(i,Metropolix::AUX_DESTINATION));
            int target=m->integer(Metropolix::auxParam(i,Metropolix::AUX_ASSIGNMENT));
            return Metropolix::modDestinations()[dest]+" / "+(metro::modGlobalDestination(static_cast<metro::ModDestination>(dest))?std::string("Global"):target==2?std::string("Both tracks"):string::f("Track %d",target+1))
                +string::f("\nEffective input: %+.2f V",m->auxVoltage[i].load());
        };
        auto* knob=control(name+" attenuverter",24+i*37,159,20,HardwareControl::BLACK_KNOB);
        knob->binding=[i] { return Metropolix::auxParam(i,Metropolix::AUX_AMOUNT); };
        knob->action=open;knob->dragAction=open;knob->extraMenu=menu;knob->detailHelp=detail;
        knob->help="CV: noon = zero; clockwise adds, counterclockwise inverts. Gates/triggers: clockwise follows >=1.5 V, counterclockwise inverts logic. Unpatched: clockwise = high, counterclockwise = low. Assigned Accumulate at noon pauses pitch accumulation.";
        auto* button=control(name+" assignment",24+i*37,193,15,HardwareControl::DARK_BUTTON);
        button->binding=[m,i] { return m && !m->uiAlt.load()?Metropolix::auxParam(i,Metropolix::AUX_DESTINATION):-1; };
        button->action=[m,open,i] { if(m && m->uiAlt.load())randomizeWorkflow(m,i);else open(); };
        button->dragAction=[m,open] { if(m && !m->uiAlt.load())open(); };
        button->confirmationCapable=true;button->extraMenu=menu;button->detailHelp=detail;
        button->rightAction=[m,i,open] { if(m){open();appendAuxMenu(createMenu(),m,i);} };
        button->help="Click to edit AUX destination; click the encoder to switch to track assignment. Drag this button to change destination. Right-click for named choices. ALT + X randomizes; ALT + Y morphs; ALT + Z restores the pre-ALT state.";
        knob->bind();button->bind();
    }
    for(int i=0;i<2;++i) {
        auto open=[m,i]{if(m){m->selectedCtrl.store(i);screenPage(m,Metropolix::CTRL_PAGE);}};
        auto menu=[m,i](ui::Menu* menu){appendCtrlMenu(menu,m,i);};
        auto detail=[m,i]{
            if(!m)return std::string();
            auto d=static_cast<metro::CtrlDestination>(m->integer(Metropolix::ctrlParam(i,Metropolix::CTRL_DESTINATION)));
            int target=m->integer(Metropolix::ctrlParam(i,Metropolix::CTRL_ASSIGNMENT));
            return Metropolix::ctrlDestinations()[d]+" / "+((d==metro::CtrlTempo || d==metro::CtrlOutputSwap)?std::string("Global"):
                target==2?std::string("Both tracks"):string::f("Track %d",target+1))+
                (d==metro::CtrlAccumDir?"\nClockwise = forward; exact noon = freeze the last accumulated pitch; counterclockwise = reverse.":metro::ctrlDirect(d)?"\nDirect control: changes the panel setting.":"\nNoon = programmed value. Turn left/right for variation.");
        };
        auto* knob=control(string::f("CTRL %d",i+1),31+i*72,252,41,HardwareControl::WHITE_KNOB);
        knob->binding=[i]{return Metropolix::ctrlParam(i,Metropolix::CTRL_VALUE);};
        knob->action=open;knob->extraMenu=menu;knob->detailHelp=detail;
        knob->help="Assignable performance control. The adjacent black button opens its destination and track assignment. Right-click for assignments and exact centre.";
        auto* button=control(string::f("CTRL %d assignment",i+1),67+i*72,229,15,HardwareControl::DARK_BUTTON);
        button->binding=[i]{return Metropolix::ctrlParam(i,Metropolix::CTRL_DESTINATION);};
        button->action=open;button->dragAction=open;button->confirmationCapable=true;button->extraMenu=menu;button->detailHelp=detail;
        button->rightAction=[m,i,open]{if(m){open();appendCtrlMenu(createMenu(),m,i);}};
        button->help="Click to edit destination. Drag to preview; press the encoder to confirm, then press again to switch target tracks. Hold the encoder to clear this assignment. Right-click for direct named choices.";
        knob->bind();button->bind();
    }
    for(int s=0;s<8;++s) {
        const int ids[]={Metropolix::PITCH+s,Metropolix::PULSES+s,Metropolix::GATES+s};
        const float ys[]={38,159,268}, heights[]={83,81,40};
        for(int k=0;k<3;++k) {
            auto* c=new HardwareSlider;
            c->metroModule=m; c->stage=s; c->kind=static_cast<HardwareSlider::Kind>(k);
            int id=ids[k]; c->binding=[id] { return id; };
            c->box=Rect(Vec(233+s*35.7f,ys[k]),Vec(22,heights[k]));
            // At Rack's default sensitivity: 400 px over the pitch range and
            // about 24 px per pulse/gate step, independent of panel height.
            const float speeds[]={2.5f,6.f,14.f};
            c->customSpeed=speeds[k];
            c->help=k==0?"Shared pitch slider; root, scale, and track settings shape the resulting note.":k==1?"Clock pulses spent on this shared stage (1-8).":"Shared gate mode: Rest, Single, Multiple, or Hold.";
            c->bind(); addParam(c);
        }
    }
    auto* display=new HardwareDisplay;
    display->metroModule=m; display->name="Metropolix display"; display->look=HardwareControl::INVISIBLE;
    display->box=Rect(Vec(130,135),Vec(93,49));
    display->binding=[m] { return screenParam(m); };
    display->help="HOME: drag or click to browse four overviews. Editing: drag the active value; click performs the encoder action. Right-click for direct access.";
    display->confirmationCapable=true;display->action=[m] { encoderPress(m); };
    display->extraMenu=[m](ui::Menu* menu) {
        if(m && m->uiPage.load()==Metropolix::ORDER_PAGE)appendOrderMenu(menu,m);
        if(stagePairPage(m))appendStagePairMenu(menu,m);
        if(simpleStagePage(m))appendSimpleStageMenu(menu,m);
        if(timingPage(m))appendTimingMenu(menu,m,m->uiPage.load());
        if(factoryPage(m))appendFactoryMenu(menu,m);
        if(backupPage(m)){if(m->uiPage.load()==Metropolix::BACKUP_PRESET_PAGE)appendBackupPresetMenu(menu,m);else appendBackupMenu(menu,m);}
        appendWorkflowMenu(menu,m);appendModWorkflowMenu(menu,m);
        if(outputPage(m))appendOutputMenu(menu,m);
        if(m && m->uiPage.load()==Metropolix::CLOCK_OUT_PAGE)appendClockMenu(menu,m);
        if(m && (m->uiPage.load()==Metropolix::SETUP_PAGE || m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE || m->uiPage.load()==Metropolix::UI_SETUP_PAGE || m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE || m->uiPage.load()==Metropolix::TUNER_PAGE))appendGlobalMenu(menu,m);
        overrideMenu(menu,m);
        if(cvPage(m) || (m && m->uiPage.load()==Metropolix::CV_TYPE_PAGE))appendCvMenu(menu,m);
        if(m && m->uiPage.load()==Metropolix::SCALE_PAGE)appendScaleMenu(menu,m);
        if(m && m->uiPage.load()==Metropolix::SLIDE_PAGE)appendSlideMenu(menu,m);
        if(m && m->uiPage.load()==Metropolix::LOOPY_PAGE)appendLoopyMenu(menu,m);
        if(m && m->uiPage.load()==Metropolix::AUX_PAGE)appendAuxMenu(menu,m,m->selectedAux.load());
        if(m && m->uiPage.load()==Metropolix::CTRL_PAGE)appendCtrlMenu(menu,m,m->selectedCtrl.load());
        if(m && (presetPage(m) || chainPage(m) || m->uiPage.load()==Metropolix::PRESET_SETUP_PAGE))appendPresetMenu(menu,m);
    };
    display->navigationMode=[m] { return m && (backupPage(m) || factoryPage(m) || (trackMenuPage(m) && !m->uiField.load()) || outputPage(m) || (m->uiPage.load()==Metropolix::MOD_MENU_PAGE && !m->uiField.load()) || (binaryStagePage(m) && m->uiAlt.load()) || (((cvPage(m) && cvGate(m)) || (modStagePage(m) && modStageGates(m))) && m->uiAlt.load()) || (m->uiPage.load()==Metropolix::SCALE_PAGE && m->uiField.load() && !m->scaleEditing.load()) || m->uiPage.load()==Metropolix::HOME || presetPage(m) || chainPage(m) || ((m->uiPage.load()==Metropolix::SETUP_PAGE || m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE || m->uiPage.load()==Metropolix::UI_SETUP_PAGE || m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE) && !m->uiField.load())) && !m->uiChooseFunction.load(); };
    display->navigationTurn=[m](int d) {
        if(!m)return;
        if(trackMenuPage(m))m->trackMenuCursor.store(metro::clamp(m->trackMenuCursor.load()+d,0,TrackMenuCount-1));
        else if(outputPage(m))outputTurn(m,d);
        else if(m->uiPage.load()==Metropolix::MOD_MENU_PAGE)m->modMenuCursor.store(metro::clamp(m->modMenuCursor.load()+d,0,ModMenuCount-1));
        else if(m->uiPage.load()==Metropolix::FACTORY_CONFIRM_PAGE)m->factoryConfirmCursor=metro::clamp(m->factoryConfirmCursor+d,0,1);
        else if(m->uiPage.load()==Metropolix::FACTORY_PAGE)m->factoryCursor.store(metro::clamp(m->factoryCursor.load()+d,0,1));
        else if(m->uiPage.load()==Metropolix::BACKUP_PRESET_PAGE)m->backupPresetCursor.store(metro::clamp(m->backupPresetCursor.load()+d,0,BackupPresetCount-1));
        else if(m->uiPage.load()==Metropolix::BACKUP_PAGE)m->backupCursor.store(metro::clamp(m->backupCursor.load()+d,0,BackupCount-1));
        else if(m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE)m->midiSetupCursor.store(metro::clamp(m->midiSetupCursor.load()+d,0,MidiSetupCount-1));
        else if(modStagePage(m) && modStageGates(m) && m->uiAlt.load())rotateModStages(m,d);
        else if(cvPage(m) && cvGate(m) && m->uiAlt.load())rotateCv(m,d);
        else if(binaryStagePage(m) && m->uiAlt.load())rotateStagePattern(m,d,false);
        else if(m->uiPage.load()==Metropolix::SCALE_PAGE)m->getParamQuantity(Metropolix::SCALE)->setValue(metro::scaleBrowseId(metro::clamp(metro::scaleBrowsePosition(m->integer(Metropolix::SCALE))+d,0,148)));
        else if(chainPage(m))chainTurn(m,d);
        else if(m->uiPage.load()==Metropolix::SETUP_PAGE)m->setupCursor.store(metro::clamp(m->setupCursor.load()+d,0,SetupMenuCount-1));
        else if(m->uiPage.load()==Metropolix::UI_SETUP_PAGE)m->uiSetupCursor.store(metro::clamp(m->uiSetupCursor.load()+d,0,UiSetupCount-1));
        else if(m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE)m->globalCursor.store(metro::clamp(m->globalCursor.load()+d,0,GlobalMenuCount-1));
        else if(presetPage(m)){m->presetBankIndex.store(metro::modulo(m->presetBankIndex.load()+d,8));m->presetNotice.store(0);}
        else m->uiHome.store(metro::modulo(m->uiHome.load()+d,4));
    };
    display->rightAction=[display,stageEditor,laneMenu,m] {
        if(factoryPage(m)){appendFactoryMenu(createMenu(),m);return;}
        if(backupPage(m)){auto* menu=createMenu();if(m->uiPage.load()==Metropolix::BACKUP_PRESET_PAGE)appendBackupPresetMenu(menu,m);else appendBackupMenu(menu,m);return;}
        if(m && (m->uiPage.load()==Metropolix::SETUP_PAGE || m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE || m->uiPage.load()==Metropolix::UI_SETUP_PAGE || m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE) && !m->uiField.load()){appendGlobalMenu(createMenu(),m);return;}
        if(trackMenuPage(m) && !m->uiField.load()){trackSettingsMenu(m);return;}
        if(outputPage(m)){appendOutputMenu(createMenu(),m);return;}
        if(m && m->uiPage.load()==Metropolix::CLOCK_OUT_PAGE && screenParam(m)<0){appendClockMenu(createMenu(),m);return;}
        if(presetPage(m) || chainPage(m))appendPresetMenu(createMenu(),m);
        else if(screenParam(m)>=0) display->createContextMenu();
        else if(m && m->uiMod.load()) laneMenu(); else stageEditor();
    };
    display->bind(); addChild(display);
    auto* encoder=control("Encoder",176,229,42,HardwareControl::ENCODER);
    encoder->binding=[m] { return screenParam(m); };
    encoder->help="Drag up/down to edit. At HOME, drag or click to browse four overviews. Click switches fields or toggles the screen's secondary setting.";
    encoder->action=[m] { encoderPress(m); };
    encoder->extraMenu=display->extraMenu;
    encoder->rightAction=[encoder,m] {
        if(factoryPage(m)){appendFactoryMenu(createMenu(),m);return;}
        if(backupPage(m)){auto* menu=createMenu();if(m->uiPage.load()==Metropolix::BACKUP_PRESET_PAGE)appendBackupPresetMenu(menu,m);else appendBackupMenu(menu,m);return;}
        if(m && (m->uiPage.load()==Metropolix::SETUP_PAGE || m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE || m->uiPage.load()==Metropolix::UI_SETUP_PAGE || m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE) && !m->uiField.load()){appendGlobalMenu(createMenu(),m);return;}
        if(trackMenuPage(m) && !m->uiField.load()){trackSettingsMenu(m);return;}
        if(outputPage(m)){appendOutputMenu(createMenu(),m);return;}
        if(m && m->uiPage.load()==Metropolix::CLOCK_OUT_PAGE && !encoder->getParamQuantity()){appendClockMenu(createMenu(),m);return;}
        if(presetPage(m) || chainPage(m))appendPresetMenu(createMenu(),m);
        else if(encoder->getParamQuantity())encoder->createContextMenu();
        else {auto* menu=createMenu();menu->addChild(createMenuLabel(encoder->help));}
    };
    encoder->confirmationCapable=true;encoder->navigationMode=display->navigationMode;encoder->navigationTurn=display->navigationTurn;
    for(auto* c:{static_cast<HardwareControl*>(display),encoder}) {
        c->detailHelp=[m]{return factoryPage(m)?factoryHelp(m):backupHelp(m);};
        c->groupedParams=[m] {
            if(m && m->uiPage.load()==Metropolix::MX_SETUP_PAGE)return std::vector<int>{Metropolix::MX_MODE,Metropolix::MX_TRACK,Metropolix::MX_LANE};
            if(m && m->uiPage.load()==Metropolix::UI_SETUP_PAGE)return std::vector<int>{uiSetupParam(m)};
            if(m && m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE && midiSetupParam(m)>=0)return std::vector<int>{midiSetupParam(m)};
            if(m && m->uiPage.load()==Metropolix::MOD_MENU_PAGE && modMenuParam(m)>=0)return std::vector<int>{modMenuParam(m)};
            if(m && m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE && globalMenuParam(m)>=0)return std::vector<int>{globalMenuParam(m)};
            if(m && m->uiPage.load()==Metropolix::AUX_PAGE)return std::vector<int>{Metropolix::auxParam(m->selectedAux.load(),Metropolix::AUX_DESTINATION),Metropolix::auxParam(m->selectedAux.load(),Metropolix::AUX_ASSIGNMENT)};
            if(m && m->uiPage.load()==Metropolix::CTRL_PAGE)return std::vector<int>{Metropolix::ctrlParam(m->selectedCtrl.load(),Metropolix::CTRL_DESTINATION),Metropolix::ctrlParam(m->selectedCtrl.load(),Metropolix::CTRL_ASSIGNMENT)};
            if(m && m->uiPage.load()==Metropolix::MOD_ASSIGN_PAGE)return std::vector<int>{Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_DESTINATION),Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_ASSIGNMENT)};
            if(modStagePage(m))return modStageIds(m);
            if(trackMenuPage(m)){int id=trackMenuParam(m);return id<0?trackResetParams(m):std::vector<int>{id};}
            if(overridePage(m))return overrideParams(m);
            if(cvPage(m))return cvParams(m);
            if(outputPage(m))return std::vector<int>{Metropolix::OUTPUT_SOURCE_A+outputIndex(m)};
            if(m && m->uiPage.load()==Metropolix::CLOCK_OUT_PAGE)return std::vector<int>{Metropolix::CLOCK_OUT_DIV,Metropolix::CLOCK_OUT_OFFSET};
            if(m && m->uiPage.load()==Metropolix::ORDER_PAGE && !orderControlled(m))return orderIds(m);
            if(stagePairPage(m))return stagePairIds(m);
            if(simpleStagePage(m))return simpleStageIds(m);
            if(timingPage(m))return timingIds(m,m->uiPage.load());
            std::vector<int> ids;
            if(m && !m->uiMod.load() && m->uiPage.load()==Metropolix::GATE_PAGE)
                for(int id:{Metropolix::trackParam(m->selectedTrack.load(),Metropolix::GATE_LENGTH),Metropolix::gateScaleParam(m->selectedTrack.load())})if(m->ctrlOwner(id)<0)ids.push_back(id);
            if(m && !m->uiMod.load() && m->uiPage.load()==Metropolix::SLIDE_PAGE)
                for(int id:{Metropolix::trackParam(m->selectedTrack.load(),Metropolix::SLIDE_TIME),Metropolix::slideTypeParam(m->selectedTrack.load())})if(m->ctrlOwner(id)<0)ids.push_back(id);
            return ids;
        };
        auto before=std::make_shared<Metropolix::ChainState>();
        c->navigationStart=[m,before]{if(chainPage(m))*before=m->getChainState();};
        // Click/long actions record their own history; only turns need the
        // accumulated bank/repeat edit recorded at the end of a drag.
        c->navigationEnd=[m,c,before]{if(c->moved>=3 && !c->longFired && chainPage(m) && !(before->working==m->getChainState().working))chainHistory(m,*before,"edit chain link");};
        c->navigationValue=[m]{
            if(trackMenuPage(m))return std::string(trackMenuName(m->trackMenuCursor.load()))+"\nDrag to browse; encoder edits or toggles.";
            if(outputPage(m))return metro::outputLabel(shownOutputSource(m))+(m->outputDraft[outputIndex(m)].load()>=0?" ?\nPress encoder to confirm.":"\nDrag to choose; encoder switches fields.");
            if(cvPage(m) && cvGate(m))return std::string("Rotate CV gate pattern; one step per encoder turn.");
            if(chainPage(m)) {
                if(m->chainQuick || m->chainEdit.load()==1)return string::f("Preset bank %d\nPress a stage button to choose its slot.",m->presetBankIndex.load()+1);
                return string::f("Chain link %d: %dx\nDrag up/down to edit the selected field.",selectedChainLink(m)+1,m->getChainState().working.links[std::min(31,selectedChainLink(m))].repeats);
            }
            if(presetPage(m))return string::f("Preset bank %d",m->presetBankIndex.load()+1);
            if(m && m->uiPage.load()==Metropolix::SCALE_PAGE)return metro::scaleName(m->integer(Metropolix::SCALE));
            if(factoryPage(m))return factoryHelp(m);
            if(backupPage(m))return backupHelp(m);
            if(m && m->uiPage.load()==Metropolix::SETUP_PAGE)return std::string("Setup menu: click encoder to open the selected item.");
            return string::f("Home overview %d / 4",m?m->uiHome.load()+1:1);
        };
    }
    encoder->longAction=[m]{
        if(m)m->cancelDraft();
        if(m && (m->uiPage.load()==Metropolix::UI_SETUP_PAGE || (m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE && globalMenuParam(m)>=0))){int id=m->uiPage.load()==Metropolix::UI_SETUP_PAGE?uiSetupParam(m):globalMenuParam(m);auto* q=m->getParamQuantity(id);q->setValue(q->defaultValue);}
        else if(m && m->uiPage.load()==Metropolix::MX_SETUP_PAGE)resetMx(m,false);
        else if(m && m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE && midiSetupParam(m)>=0){auto* q=m->getParamQuantity(midiSetupParam(m));q->setValue(q->defaultValue);}
        else if(m && m->uiPage.load()==Metropolix::MOD_MENU_PAGE && modMenuParam(m)>=0){auto* q=m->getParamQuantity(modMenuParam(m));q->setValue(q->defaultValue);}
        else if(m && m->uiPage.load()==Metropolix::AUX_PAGE){m->cancelDraft();clearWorkflowAux(m,false);}
        else if(m && m->uiPage.load()==Metropolix::CTRL_PAGE)clearCtrlAssignment(m,m->selectedCtrl.load(),false);
        else if(m && m->uiPage.load()==Metropolix::MOD_ASSIGN_PAGE)resetModAssignment(m,false);
        else if(modStagePage(m))resetModStages(m,false);
        else if(trackMenuPage(m))resetTrackMenu(m,trackMenuParam(m)<0,false);
        else if(outputPage(m))resetOutput(m,false);
        else if(m && m->uiPage.load()==Metropolix::CLOCK_OUT_PAGE)resetClockOutput(m,false);
        else if(m && m->uiPage.load()==Metropolix::ORDER_PAGE)resetOrder(m,false);
        else if(stagePairPage(m))resetStagePairs(m,false,false);
        else if(simpleStageLongEnabled(m))resetSimpleStages(m,false,false);
        else if(timingPage(m))resetTiming(m,false);
        else if(m && m->uiPage.load()==Metropolix::SCALE_PAGE)scaleEditPress(m);
        else if(cvPage(m))resetCv(m,false,false);
        else if(m && m->uiPage.load()==Metropolix::CV_TYPE_PAGE)m->getParamQuantity(screenParam(m))->setValue(0);
        else if(m && m->uiPage.load()==Metropolix::SLIDE_PAGE)resetSlide(m,false);
        else if(chainPage(m))chainDelete(m);
        else if(overridePage(m))for(int id:overrideParams(m))m->getParamQuantity(id)->setValue(-1);
        else if(m && !m->uiMod.load() && m->uiPage.load()==Metropolix::GATE_PAGE) {
            for(int id:{Metropolix::trackParam(m->selectedTrack.load(),Metropolix::GATE_LENGTH),Metropolix::gateScaleParam(m->selectedTrack.load())})
                if(m->ctrlOwner(id)<0)m->getParamQuantity(id)->setValue(m->getParamQuantity(id)->defaultValue);
        }
        else if(m && m->uiPage.load()==Metropolix::SWING_PAGE && !m->uiMod.load())m->getParamQuantity(Metropolix::swingParam(m->selectedTrack.load()))->setValue(50.f);
    };
    encoder->longEnabled=[m]{return m && (m->uiPage.load()==Metropolix::MX_SETUP_PAGE || (m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE && midiSetupParam(m)>=0) || (m->uiPage.load()==Metropolix::MOD_MENU_PAGE && modMenuParam(m)>=0) || m->uiPage.load()==Metropolix::UI_SETUP_PAGE || (m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE && globalMenuParam(m)>=0) || m->uiPage.load()==Metropolix::AUX_PAGE || m->uiPage.load()==Metropolix::CTRL_PAGE || m->uiPage.load()==Metropolix::MOD_ASSIGN_PAGE || modStagePage(m) || (trackMenuPage(m) && (trackMenuParam(m)<0 || m->ctrlOwner(trackMenuParam(m))<0)) || outputPage(m) || m->uiPage.load()==Metropolix::CLOCK_OUT_PAGE || (m->uiPage.load()==Metropolix::ORDER_PAGE && !orderControlled(m)) || stagePairPage(m) || simpleStageLongEnabled(m) || (timingPage(m) && !timingIds(m,m->uiPage.load()).empty()) || cvPage(m) || m->uiPage.load()==Metropolix::CV_TYPE_PAGE || scaleEncoderLongEnabled(m) || (m->uiPage.load()==Metropolix::SLIDE_PAGE && !m->uiMod.load()) || overridePage(m) || (!m->uiMod.load() && m->uiPage.load()==Metropolix::GATE_PAGE) || (m->uiPage.load()==Metropolix::SWING_PAGE && !m->uiMod.load() && m->ctrlOwner(Metropolix::swingParam(m->selectedTrack.load()))<0) || (chainPage(m) && !m->chainEnabled.load() && m->chainCursor.load()%2 && m->chainCursor.load()<2*m->getChainState().working.size));};
    auto* exit=control("EXIT",214,229,15,HardwareControl::DARK_BUTTON);
    exit->help="Step back from a submenu or return HOME. Hold for more than one second to sync recalled pitch/CTRL pickup values to their visible panel positions. Right-click for optional editors.";
    exit->action=[m] { if(m) { if(m->chainQuick){m->chainQuickCancel=true;return;}if(chainPage(m) && m->chainEdit.load()){m->chainEdit.store(0);m->chainCreating=false;return;} screenPage(m,parentPage(m)); m->uiEdit.store(false); m->uiAlt.store(false); m->uiChooseFunction.store(false); } };
    exit->longAction=[m]{syncPanel(m);};
    exit->rightAction=[stageEditor,modEditor,m] {
        auto* menu=createMenu();
        menu->addChild(createMenuItem("Open stage editor","",stageEditor));
        menu->addChild(createMenuItem("Open MOD editor","",modEditor));
        menu->addChild(createMenuItem("Close optional editor","",[m] { if(m)m->expanded.store(false); }));
        menu->addChild(createMenuItem("Sync to visible panel positions","",[m]{syncPanel(m);}));
    };
    for(int t=0;t<2;++t) {
        auto* c=control(string::f("TRK %d",t+1),145+t*31,272,16,HardwareControl::BLUE_BUTTON);
        c->dragAction=[m,t]{if(m){workflowTrackPress(m,t);m->heldTrackReset.fetch_or(1u<<t);}};
        c->releaseAction=[m,t]{if(m){m->heldTrackReset.fetch_and(~(1u<<t));if(!m->heldTrackReset.load())m->workflowCopyUsed=false;}};
        c->help="Hold this track while pressing RESET to force its reset even with Global Reset Off. Select this note track. ALT then TRK opens its hardware menu: turn to browse, press to edit/toggle, hold encoder to reset. Right-click for direct settings.";
        c->action=[m,t] { if(m) { if(m->workflowCopyUsed)return; if(m->loopyMuteHeld.load()){m->muteLoopy(t);return;} if(chainPage(m) && (m->chainQuick || m->chainEdit.load())){chainMute(m,t);return;} m->selectedTrack.store(t); m->uiMod.store(false); m->modEditor.store(false); m->uiChooseFunction.store(false); if(m->uiAlt.load()){m->uiAlt.store(false);screenPage(m,Metropolix::TRACK_MENU_PAGE);m->trackMenuCursor.store(0);} } };
        c->active=[m,t] { if(m && m->loopyMuteHeld.load())return !m->integer(Metropolix::trackParam(t,Metropolix::MUTE)) || std::fmod(system::getTime(),.3)<.15; if(chainPage(m) && (m->chainQuick || m->chainEdit.load())) {unsigned mask=m->chainQuick?m->chainQuickMutes:m->getChainState().working.links[selectedChainLink(m)].mutes;return !(mask&(1u<<t)) || std::fmod(system::getTime(),.3)<.15;}return m && !m->uiMod.load() && m->selectedTrack.load()==t; };
        c->activeColor=[m,t]{if(m && m->loopyMuteHeld.load())return m->integer(Metropolix::trackParam(t,Metropolix::MUTE))?nvgRGB(240,55,40):nvgRGB(245,220,60);if(chainPage(m) && (m->chainQuick || m->chainEdit.load())) {unsigned mask=m->chainQuick?m->chainQuickMutes:m->getChainState().working.links[selectedChainLink(m)].mutes;return mask&(1u<<t)?nvgRGB(240,55,40):nvgRGB(245,220,60);}return nvgRGB(135,187,227);};
        c->rightAction=[m,t] { if(m) { m->selectedTrack.store(t); m->uiMod.store(false); m->uiChooseFunction.store(false); trackSettingsMenu(m); } };
    }
    auto* mod=control("MOD",207,272,16,HardwareControl::BLUE_BUTTON);
    mod->help="Select modulation editing. Hold Loopy and click MOD to mute/unmute the whole group without changing individual lane mutes. Right-click for group mute, lane selection or the editor.";
    mod->dragAction=[m]{if(m)m->heldTrackReset.fetch_or(4u);};mod->releaseAction=[m]{if(m)m->heldTrackReset.fetch_and(~4u);};
    mod->action=[m] { if(m) { if(m->loopyMuteHeld.load()){m->muteLoopy(10);return;} if(chainPage(m) && (m->chainQuick || m->chainEdit.load())){chainMute(m,2);return;} if(m->timingShortcutHeld.load()){m->uiMod.store(true);m->modEditor.store(true);return;} if(m->uiAlt.load()){m->uiMod.store(true);m->uiAlt.store(false);screenPage(m,Metropolix::MOD_MENU_PAGE);return;}bool assignment=m->uiMod.load() && m->uiPage.load()==Metropolix::STAGE_PAGE;m->uiMod.store(true);m->modEditor.store(true);m->uiChooseFunction.store(false);screenPage(m,assignment?Metropolix::MOD_ASSIGN_PAGE:Metropolix::STAGE_PAGE);m->uiAlt.store(false); } };
    mod->active=[m] { if(m && m->loopyMuteHeld.load())return !m->integer(Metropolix::MOD_GROUP_MUTE) || std::fmod(system::getTime(),.3)<.15; if(chainPage(m) && (m->chainQuick || m->chainEdit.load())) {unsigned mask=m->chainQuick?m->chainQuickMutes:m->getChainState().working.links[selectedChainLink(m)].mutes;return !(mask&4) || std::fmod(system::getTime(),.3)<.15;}return m && m->uiMod.load(); }; mod->rightAction=laneMenu;
    mod->activeColor=[m]{if(m && m->loopyMuteHeld.load())return m->integer(Metropolix::MOD_GROUP_MUTE)?nvgRGB(240,55,40):nvgRGB(245,220,60);if(chainPage(m) && (m->chainQuick || m->chainEdit.load())) {unsigned mask=m->chainQuick?m->chainQuickMutes:m->getChainState().working.links[selectedChainLink(m)].mutes;return mask&4?nvgRGB(240,55,40):nvgRGB(245,220,60);}return nvgRGB(135,187,227);};
    auto* run=createParam<TransportButton>(Vec(13,298),m,Metropolix::RUN); addParam(run);
    auto* reset=createParam<TransportButton>(Vec(75.4f,298),m,Metropolix::RESET);
    reset->reset=true; reset->momentary=true; addParam(reset);
    auto* loopy=control("Loopy",52.2f,306,16,HardwareControl::BLUE_BUTTON);
    loopy->help="Click to enter Loopy. Hold a stage for a momentary loop or stopped audition. Right-click a stage to latch a range. Hold this button for track/MOD mutes; its menu also offers mouse mute mode.";
    loopy->dragAction=[m]{if(m){m->loopyMuteHeld.store(true);m->loopyMuteUsed.store(false);}};
    loopy->releaseAction=[m]{if(m)m->loopyMuteHeld.store(false);};
    loopy->action=[m]{if(!m || m->loopyMuteUsed.load())return;if(m->uiPage.load()==Metropolix::LOOPY_PAGE){if(m->loopyLatched.load())m->clearLoopy();else if(m->integer(Metropolix::RUN))m->latchLoopy();}else openLoopy(m);};
    loopy->rightAction=[m]{appendLoopyMenu(createMenu(),m);};
    loopy->active=[m]{return m && ((m->loopyCommand.load()&255u) || m->loopyMuteHeld.load());};

    // A short click selects a screen; a held vertical drag edits its primary
    // value directly. Opening a menu never steals the start of a value drag.
    auto valueButton=[&](const std::string& name,float x,float y,int page,std::function<int()> binding) {
        auto* c=control(name,x,y,16,HardwareControl::BUTTON);
        c->binding=binding;
        c->action=[m,page] { screenPage(m,page); };
        c->dragAction=c->action;
        c->active=[m,page] { return m && m->uiPage.load()==page; };
        c->help="Click to show this setting on the display.";
        return c;
    };
    auto trackOrMod=[m](int trackProperty,int modProperty) {
        if(!m) return -1;
        return m->uiMod.load()?Metropolix::modParam(m->selectedLane.load(),modProperty):Metropolix::trackParam(m->selectedTrack.load(),trackProperty);
    };
    auto* setup=valueButton("SETUP",114.6f,306,Metropolix::SETUP_PAGE,[] { return Metropolix::RANGE; });
    setup->look=HardwareControl::CREAM_BUTTON;
    setup->binding=[m]{return chainPage(m)?-1:Metropolix::RANGE;};
    setup->dragAction=[m]{if(!chainPage(m))screenPage(m,Metropolix::SETUP_PAGE);};
    setup->longAction=[m]{if(!m)return;if(chainPage(m))beginQuickChain(m);else openChainPage(m);};
    setup->releaseAction=[m]{if(m)finishQuickChain(m);};
    setup->help="Click for Setup. Hold >1 second for Preset Chains. On the chain screen, hold again for Quick Chain; release enables playback. EXIT before release cancels auto-enable.";
    setup->extraMenu=[m,stageEditor,modEditor](ui::Menu* menu) {
        appendPresetMenu(menu,m);
        appendGlobalMenu(menu,m);
        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createMenuItem("Selected track settings","",[m] { trackSettingsMenu(m); }));
        menu->addChild(createMenuItem("Open stage editor","",stageEditor));
        menu->addChild(createMenuItem("Open MOD editor","",modEditor));
    };
    auto* bpm=valueButton("BPM",145.8f,306,Metropolix::BPM_PAGE,[m] { return m && m->integer(Metropolix::CLOCK_SOURCE)==3?-1:m && m->externalClock.load()?Metropolix::EXT_CLOCK_DIV:Metropolix::TEMPO; });
    auto lastTap=std::make_shared<double>(0.0);
    bpm->action=[m,lastTap] {
        if(!m)return;
        double now=system::getTime(),dt=now-*lastTap;
        if(m->uiPage.load()==Metropolix::BPM_PAGE && !m->externalClock.load() && *lastTap>0 && dt>=.15 && dt<=2.5)
            setWithHistory(m->getParamQuantity(Metropolix::TEMPO),static_cast<float>(60.0/dt));
        *lastTap=now;screenPage(m,Metropolix::BPM_PAGE);
    };
    bpm->doubleClickReset=false;
    bpm->look=HardwareControl::CREAM_BUTTON;
    bpm->help="Click for BPM; repeated clicks tap internal tempo. Drag to edit BPM or external clock division. Hold BPM + stage for 20-160 BPM, or /1, /2, /3, /4, /6, /12, /24, /48 externally.";
    auto* scale=valueButton("SCALE",177,306,Metropolix::SCALE_PAGE,[] { return Metropolix::SCALE; });
    scale->look=HardwareControl::CREAM_BUTTON;
    scale->action=[m] { if(!m || m->scaleShortcutUsed.load())return;if(m->scaleCopyGesture)copyScale(m);else{screenPage(m,Metropolix::SCALE_PAGE);m->uiField.store(1);} };
    scale->releaseAction=[m]{if(m){m->scaleShortcutHeld.store(false);m->scaleCopyGesture=false;}};
    scale->extraMenu=[m](ui::Menu* menu){appendScaleMenu(menu,m);};
    scale->help="Click for Scale / Root. ALT switches fields; encoder click locks/applies/unlocks. ALT + SCALE copies the displayed scale; hold >1 second to paste into a user slot. Keyboard Alt or the right-click menu offers the same copy/paste. Hold encoder to edit user notes; hold SCALE + stage for shortcuts.";
    scale->navigationTurn=[m](int d){if(m && !m->scaleCopyGesture)m->getParamQuantity(Metropolix::SCALE)->setValue(metro::scaleBrowseId(metro::clamp(metro::scaleBrowsePosition(m->integer(Metropolix::SCALE))+d,0,148)));};
    scale->navigationValue=[m]{return m?metro::scaleName(m->integer(Metropolix::SCALE)):std::string();};
    scale->groupedParams=[m] {if(m && m->scaleCopyGesture){if(!metro::userScale(m->integer(Metropolix::SCALE)))return std::vector<int>{};int slot=m->integer(Metropolix::SCALE)-metro::FactoryScaleCount;return std::vector<int>{Metropolix::userScaleParam(slot),Metropolix::userScaleParam(slot,true)};}std::vector<int> ids={Metropolix::SCALE,Metropolix::ROOT,Metropolix::SCALE_LOCK};for(int s=0;s<8;++s)for(bool root:{false,true})ids.push_back(Metropolix::scaleShortcutParam(s,root));for(int s=0;s<100;++s)ids.push_back(Metropolix::userScaleParam(s,true));return ids;};
    auto* alt=control("ALT",208.2f,306,16,HardwareControl::DARK_BUTTON);
    alt->help="Toggle secondary stage-button actions: internal preset LOAD/SAVE, A/B assignment, and lane/accumulator settings. Rack file presets remain in the module's right-click menu.";
    alt->action=[m] { if(m) { m->cancelDraft();if(m->uiPage.load()==Metropolix::SCALE_PAGE)m->uiField.store(1-m->uiField.load());else {if(!m->uiAlt.load())captureWorkflowBaseline(m);m->uiAlt.store(!m->uiAlt.load());} } }; alt->active=[m] { return m && m->uiAlt.load(); };
    auto scaleModifier=[m,alt,scale]{return m && (m->uiAlt.load() || alt->dragging || ((scale->pressMods | APP->window->getMods()) & GLFW_MOD_ALT));};
    scale->binding=[m,scaleModifier]{return m && (m->scaleCopyGesture || scaleModifier())?-1:Metropolix::SCALE;};
    scale->navigationMode=[m,scaleModifier]{return m && !m->scaleCopyGesture && !scaleModifier();};
    scale->dragAction=[m,alt,scaleModifier]{if(!m)return;m->scaleCopyGesture=scaleModifier();m->scaleShortcutHeld.store(!m->scaleCopyGesture);m->scaleShortcutUsed.store(false);if(m->scaleCopyGesture){m->cancelDraft();screenPage(m,Metropolix::SCALE_PAGE);m->scaleEditing.store(false);m->uiField.store(1);m->uiAlt.store(false);if(alt->dragging)alt->moved=3;}};
    scale->longEnabled=[m]{return m && m->scaleCopyGesture;};
    scale->longAction=[m]{pasteScale(m,false);};
    auto* order=valueButton("ORDER",21,340,Metropolix::ORDER_PAGE,[trackOrMod] { return trackOrMod(Metropolix::ORDER,Metropolix::MOD_ORDER); });
    order->dragAction=[m]{if(m){screenPage(m,Metropolix::ORDER_PAGE);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->orderShortcutHeld.store(true);m->orderShortcutUsed.store(false);}};
    order->releaseAction=[m]{if(m)m->orderShortcutHeld.store(false);};
    order->action=[m]{if(m && !m->orderShortcutUsed.load())screenPage(m,Metropolix::ORDER_PAGE);};
    order->extraMenu=[m](ui::Menu* menu){appendOrderMenu(menu,m);};
    order->help="Play order and direction. Hold ORDER + short stage press to recall one of eight global shortcuts; hold the stage >1 second to save the pair. Long encoder resets both fields. Right-click for direction and shortcuts.";
    order->groupedParams=[m] {
        // Target can change while ORDER is held. One gesture receipt covers
        // both note tracks, all MOD lanes and the eight shared shortcut pairs.
        std::vector<int> ids;for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target){auto pair=Metropolix::orderParams(mod,target);if(m && m->ctrlOwner(pair[0])<0 && m->ctrlOwner(pair[1])<0)ids.insert(ids.end(),pair.begin(),pair.end());}
        for(int id=Metropolix::ORDER_SHORTCUTS;id<Metropolix::FORMAT32_NUM_PARAMS;++id)ids.push_back(id);
        return ids;
    };
    auto timingButton=[&](const std::string& name,float x,int page) {
        auto* c=valueButton(name,x,340,page,[m,page]{return timingParam(m,page,m && m->uiPage.load()==page?m->uiField.load():0);});
        c->dragAction=[m,page]{if(m){openTiming(m,page);m->timingShortcutHeld.store(page);m->timingShortcutUsed.store(false);}};
        c->releaseAction=[m,page]{if(m && m->timingShortcutHeld.load()==page)m->timingShortcutHeld.store(0);};
        c->action=[m,page]{if(m && !m->timingShortcutUsed.load())openTiming(m,page);};
        c->groupedParams=[m,page]{return timingIds(m,page,true);};
        c->extraMenu=[m,page](ui::Menu* menu){appendTimingMenu(menu,m,page);};
        c->help=page==Metropolix::LENGTH_PAGE?"Length: encoder switches Stages / Pulses; ALT + encoder edits stage offset. Hold LEN + stage for 1-8 stages or 8-64 pulses. Hold encoder >1 second to reset all three fields. Right-click for shortcuts and fields.":"Division: encoder switches Clock / Pulse Count on note tracks. Hold DIV + stage for clock /1, /2, /3, /4, /6, /8, /16, /32 or pulse count /1-8. MOD has clock division only. Long encoder resets all division fields.";
        return c;
    };
    auto* len=timingButton("LEN",52.2f,Metropolix::LENGTH_PAGE);
    auto* division=timingButton("DIV",83.4f,Metropolix::DIV_PAGE);
    auto* swing=valueButton("SWING",114.6f,340,Metropolix::SWING_PAGE,[m] { return m && !m->uiMod.load()?Metropolix::swingParam(m->selectedTrack.load()):-1; });
    swing->help="Per-track swing: 50% = straight, 78% = maximum. Click to edit; drag up/down or use the encoder. Stage buttons on this screen choose the eight swing shortcuts. Hold encoder >1 second to reset to 50%. MOD lanes stay straight.";
    auto swingMenu=[m](ui::Menu* menu) {
        if(!m)return;
        if(m->uiMod.load()){menu->addChild(createMenuLabel("Swing applies to TRK 1 and TRK 2 only."));return;}
        menu->addChild(new ui::MenuSeparator);
        auto* q=m->getParamQuantity(Metropolix::swingParam(m->selectedTrack.load()));
        for(int s=0;s<8;++s){int percent=metro::swingShortcut(s);menu->addChild(createCheckMenuItem(std::to_string(percent)+"%","",[q,percent]{return q->getValue()==percent;},[q,percent]{setWithHistory(q,percent);}));}
    };
    swing->extraMenu=swingMenu;
    auto extraDisplay=display->extraMenu;
    display->extraMenu=[m,extraDisplay,swingMenu](ui::Menu* menu){extraDisplay(menu);if(m && m->uiPage.load()==Metropolix::SWING_PAGE)swingMenu(menu);};
    encoder->extraMenu=display->extraMenu;
    auto* slideTime=valueButton("SLIDE",145.8f,340,Metropolix::SLIDE_PAGE,[m] { return m && !m->uiMod.load()?Metropolix::trackParam(m->selectedTrack.load(),Metropolix::SLIDE_TIME):-1; });
    slideTime->help="Slide amount. Encoder click cycles Analog / Tempo / Acid; long press resets amount/type. Hold SLIDE + stage 1-8 for amount shortcuts. Right-click for slide types.";
    slideTime->extraMenu=[m](ui::Menu* menu){appendSlideMenu(menu,m);};
    auto* trackGate=valueButton("GATE",177,340,Metropolix::GATE_PAGE,[m] {
        if(!m || m->uiMod.load())return -1;
        return m->uiPage.load()==Metropolix::GATE_PAGE && m->uiField.load()?Metropolix::gateScaleParam(m->selectedTrack.load()):Metropolix::trackParam(m->selectedTrack.load(),Metropolix::GATE_LENGTH);
    });
    trackGate->action=[m]{if(m && m->uiPage.load()!=Metropolix::GATE_PAGE)screenPage(m,Metropolix::GATE_PAGE);};trackGate->dragAction=trackGate->action;
    trackGate->help="Gate Length / Gate Scale. Click encoder to switch. Hold this button and press stage 1-8 for shortcuts. Long encoder press resets both fields.";
    auto gateMenu=[m](ui::Menu* menu) {
        if(!m || m->uiMod.load())return;
        int field=m->uiPage.load()==Metropolix::GATE_PAGE?m->uiField.load():0;
        int id=field?Metropolix::gateScaleParam(m->selectedTrack.load()):Metropolix::trackParam(m->selectedTrack.load(),Metropolix::GATE_LENGTH);
        const int shortcuts[]={1,15,30,45,60,75,90,100};menu->addChild(new ui::MenuSeparator);
        for(int v:shortcuts)menu->addChild(createMenuItem(v==100 && !field?"Hold":std::to_string(v)+"%","",[m,id,v]{setWithHistory(m->getParamQuantity(id),float(v));},m->ctrlOwner(id)>=0));
    };
    trackGate->extraMenu=gateMenu;
    auto displayMenu=display->extraMenu;display->extraMenu=[m,displayMenu,gateMenu](ui::Menu* menu){displayMenu(menu);if(m && m->uiPage.load()==Metropolix::GATE_PAGE)gateMenu(menu);};encoder->extraMenu=display->extraMenu;
    auto* edit=control("EDIT",208.2f,340,16,HardwareControl::DARK_BUTTON);
    edit->help="Choose a stage function: press EDIT, then its printed stage button, then a stage. Right-click opens the grid.";
    edit->action=[m] { if(m){if(!m->workflowEditUsed)m->uiChooseFunction.store(!m->uiChooseFunction.load());m->workflowEditUsed=false;} };
    edit->active=[m] { return m && m->uiChooseFunction.load(); }; edit->rightAction=stageEditor;
    const char* names[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
    const int rows[]={Metropolix::SLIDE,Metropolix::SKIP,Metropolix::PITCH_OVERRIDE,Metropolix::GATE_OVERRIDE,Metropolix::RATCHETS,Metropolix::PROBABILITY,Metropolix::ACCUMULATION,Metropolix::CV};
    for(int s=0;s<8;++s) {
        auto* c=control(std::string("Stage ")+std::to_string(s+1)+" / "+names[s],244+s*35.7f,340,16,HardwareControl::BLUE_BUTTON);
        c->binding=[m,s,trackGate,slideTime,edit,bpm] {
            if(bpm->dragging)return -1;
            if(m && edit->dragging && m->uiMod.load())return -1;
            if(slideTime->dragging && m && m->uiPage.load()==Metropolix::SLIDE_PAGE)return -1;
            if(trackGate->dragging && m && m->uiPage.load()==Metropolix::GATE_PAGE)return -1;
            if(!m || m->timingShortcutHeld.load() || m->orderShortcutHeld.load() || m->scaleShortcutHeld.load() || m->loopyMuteHeld.load() || m->uiPage.load()==Metropolix::LOOPY_PAGE || (m->uiAlt.load() && !overridePage(m)) || m->uiChooseFunction.load() || presetPage(m) || chainPage(m) || m->uiPage.load()==Metropolix::SWING_PAGE) return -1;
            return m->uiMod.load()?Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_VALUE+s):Metropolix::stageParam(m->selectedTrack.load(),s,m->selectedRow.load());
        };
        c->help="EDIT then a function, then select a stage. Hold + drag edits this stage. Slide/Skip clicks toggle. Right-click for values.";
        c->groupedParams=[m]{return m && m->timingShortcutHeld.load()?std::vector<int>{}:stagePairPage(m)?stagePairIds(m):simpleStagePage(m)?simpleStageIds(m):cvPage(m)?cvParams(m):overrideParams(m);};
        c->detailHelp=[m,s] {
            if(m && m->timingShortcutHeld.load())return "Hold the panel button and click for "+timingShortcutLabel(m,m->timingShortcutHeld.load(),s)+".";
            if(m && (m->orderShortcutHeld.load() || m->uiPage.load()==Metropolix::ORDER_PAGE))return "Order shortcut "+orderShortcutLabel(m,s)+". Hold ORDER + click to recall; long stage press saves. Right-click offers both.";
            if(m && m->loopyMuteHeld.load())return std::string("Click to mute/unmute MOD lane ")+std::to_string(s+1)+". Running mutes take effect on a clock pulse.";
            if(m && m->uiPage.load()==Metropolix::LOOPY_PAGE)return std::string("Hold to loop this stage while running, or audition its frozen pitch/CV while stopped. Right-click during playback to latch a two-boundary range.");
            if(m && m->uiPage.load()==Metropolix::SWING_PAGE)return std::string("Swing shortcut: ")+std::to_string(metro::swingShortcut(s))+"%. Click to apply to the selected note track.";
            if(!presetPage(m))return std::string();
            int slot=m->presetBankIndex.load()*8+s;
            return string::f("Preset %d-%d: %s. Click to load/save; right-click for slot actions. Turn encoder to change bank.",slot/8+1,slot%8+1,m->getPreset(slot).occupied?"saved":"empty");
        };
        if(s==6)c->help+=" ALT + this button opens LIM: click the encoder to select Mode, then drag to choose Stage or Track.";
        if(s==7)c->help+=" ALT + this button opens the note track's CV type. On EDIT CV, encoder click cycles Step/sRamp/pRamp; ALT edits all voltages or rotates/inverts CV gates.";
        c->dragAction=[m,s,trackGate,slideTime,edit,bpm] { if(m && (m->timingShortcutHeld.load() || m->orderShortcutHeld.load()))return; if(m && bpm->dragging){if(m->integer(Metropolix::CLOCK_SOURCE)==3){bpm->moved=3;return;}const int divs[]={1,2,3,4,6,12,24,48};int id=m->externalClock.load()?Metropolix::EXT_CLOCK_DIV:Metropolix::TEMPO;m->cancelDraft();m->getParamQuantity(id)->setValue(m->externalClock.load()?divs[s]:20*(s+1));bpm->moved=3;return;} if(m && workflowStagePress(m,s,edit->dragging))return; if(m && (m->scaleShortcutHeld.load() || (slideTime->dragging && m->uiPage.load()==Metropolix::SLIDE_PAGE)))return; if(m && m->uiPage.load()==Metropolix::LOOPY_PAGE && !m->loopyMuteHeld.load()){m->pressLoopyStage(s);return;} if(m && !m->loopyMuteHeld.load() && !m->uiAlt.load() && !m->uiChooseFunction.load() && !presetPage(m) && !chainPage(m) && m->uiPage.load()!=Metropolix::SWING_PAGE && !(trackGate->dragging && m->uiPage.load()==Metropolix::GATE_PAGE)) { m->selectedStage.store(s); m->uiPage.store(Metropolix::STAGE_PAGE); m->uiStageDetail.store(true); } };
        c->releaseAction=[m,s]{if(m){m->releaseLoopyStage(s);workflowStageRelease(m,s);}};
        c->longSeconds=2;c->longAction=[m,s,scale,order]{if(m && m->orderShortcutHeld.load()){orderShortcut(m,s,true);order->moved=3;}else if(m && m->scaleShortcutHeld.load()){scaleShortcut(m,s,true);scale->moved=3;}else if(chainPage(m) && !m->chainEdit.load() && !m->chainQuick)chainSave(m,s);};
        c->longEnabled=[m,c]{c->longSeconds=m && (m->orderShortcutHeld.load() || m->scaleShortcutHeld.load())?1:2;return m && (m->orderShortcutHeld.load() || m->scaleShortcutHeld.load() || (chainPage(m) && !m->chainEnabled.load() && !m->chainEdit.load() && !m->chainQuick));};
        int row=rows[s];
        c->action=[this,m,s,row,unavailable,trackGate,scale,slideTime,bpm,order,len,division] {
            if(bpm->dragging)return;
            if(!m)return;
            if(int page=m->timingShortcutHeld.load()){timingShortcut(m,page,s,false);(page==Metropolix::LENGTH_PAGE?len:division)->moved=3;return;}
            if(m->orderShortcutHeld.load()){orderShortcut(m,s,false);order->moved=3;return;}
            if(m->workflowCopyUsed)return;
            if(slideTime->dragging && m->uiPage.load()==Metropolix::SLIDE_PAGE){int id=Metropolix::trackParam(m->selectedTrack.load(),Metropolix::SLIDE_TIME);if(!m->uiMod.load() && m->ctrlOwner(id)<0){m->getParamQuantity(id)->setValue(metro::slideShortcut(s));slideTime->shortcutEdited=true;}return;}
            if(m->scaleShortcutHeld.load()){scaleShortcut(m,s,false);scale->moved=3;return;}
            if(m->loopyMuteHeld.load()){m->muteLoopy(s+2);return;}
            if(m->uiPage.load()==Metropolix::LOOPY_PAGE)return;
            if(chainPage(m)){chainStage(m,s);return;}
            if(trackGate->dragging && m->uiPage.load()==Metropolix::GATE_PAGE) {
                const int shortcuts[]={1,15,30,45,60,75,90,100};int id=screenParam(m);
                // The held GATE gesture records its own single native undo.
                if(id>=0 && m->ctrlOwner(id)<0){m->getParamQuantity(id)->setValue(float(shortcuts[s]));trackGate->shortcutEdited=true;}
                return;
            }
            if(m->uiAlt.load() && !overridePage(m)) {
                m->uiAlt.store(false);
                if(s==0) openPresetPage(m,false);
                else if(s==1) openPresetPage(m,true);
                else if(s==2)screenPage(m,Metropolix::A_PAGE);
                else if(s==3)screenPage(m,Metropolix::B_PAGE);
                else if(s==4)screenPage(m,Metropolix::CLOCK_OUT_PAGE);
                else if(s==6){m->uiMod.store(false);screenPage(m,Metropolix::ACCUM_PAGE);}
                else if(s==7){if(m->uiMod.load())modSettingsMenu(m,m->selectedLane.load());else screenPage(m,Metropolix::CV_TYPE_PAGE);}
                else openMx(m);
                return;
            }
            if(presetPage(m) && !m->uiChooseFunction.load()) { activatePresetSlot(m,s);return; }
            if(m->uiChooseFunction.load()) {
                m->uiChooseFunction.store(false);
                if(m->uiMod.load()){m->selectedLane.store(s);m->uiStageDetail.store(false);screenPage(m,Metropolix::STAGE_PAGE);}
                else if(row>=0) { m->selectedRow.store(row); m->uiEdit.store(true); m->uiStageDetail.store(false); screenPage(m,Metropolix::STAGE_PAGE); }
                return;
            }
            if(m->uiPage.load()==Metropolix::SWING_PAGE) {
                if(!m->uiMod.load())setWithHistory(m->getParamQuantity(Metropolix::swingParam(m->selectedTrack.load())),metro::swingShortcut(s));
                return;
            }
            m->selectedStage.store(s); m->uiPage.store(Metropolix::STAGE_PAGE); m->uiEdit.store(true); m->uiStageDetail.store(true);
            if(m->uiMod.load()) {
                int l=m->selectedLane.load();
                bool gate=m->integer(Metropolix::modParam(l,Metropolix::MOD_TYPE))==metro::ModGate || metro::modGateDestination(static_cast<metro::ModDestination>(m->integer(Metropolix::modParam(l,Metropolix::MOD_DESTINATION))));
                if(gate){auto* q=m->getParamQuantity(Metropolix::modParam(l,Metropolix::MOD_VALUE+s));setWithHistory(q,q->getValue()>=1.5f?0.f:5.f);}
            }
            int current=m->selectedRow.load();
            if(!m->uiMod.load() && (current==Metropolix::SLIDE || current==Metropolix::SKIP)) {
                auto* q=m->getParamQuantity(Metropolix::stageParam(m->selectedTrack.load(),s,current));
                setWithHistory(q,q->getValue()>.5f?0.f:1.f);
            }
            else if(cvPage(m) && cvGate(m)){auto* q=m->getParamQuantity(Metropolix::stageParam(m->selectedTrack.load(),s,Metropolix::CV));setWithHistory(q,q->getValue()>=1.5f?0.f:5.f);}
        };
        c->active=[m,s,slideTime] {
            if(!m)return false;
            if(slideTime->dragging && m->uiPage.load()==Metropolix::SLIDE_PAGE)return !m->uiMod.load();
            if(m->timingShortcutHeld.load())return m->integer(timingParam(m,m->timingShortcutHeld.load(),m->uiField.load()))==timingShortcutValue(m->timingShortcutHeld.load(),m->uiField.load(),m->uiMod.load(),s);
            if(m->orderShortcutHeld.load() || m->scaleShortcutHeld.load())return true;
            if(m->loopyMuteHeld.load())return !m->integer(Metropolix::modParam(s,Metropolix::MOD_MUTE)) || std::fmod(system::getTime(),.3)<.15;
            if(m->uiPage.load()==Metropolix::LOOPY_PAGE)return true;
            if(chainPage(m)) {
                auto state=m->getChainState();
                if(m->chainQuick || m->chainEdit.load()==1)return m->getPreset(8*m->presetBankIndex.load()+s).occupied;
                if(state.enabled)return m->lastLoadedSlot.load()%8==s;
                if(state.loaded==s)return std::fmod(system::getTime(),.9)<.65;
                return state.saved[s].size>0;
            }
            if(m->uiChooseFunction.load())return true;
            if(m->uiPage.load()==Metropolix::SWING_PAGE)return !m->uiMod.load() && m->integer(Metropolix::swingParam(m->selectedTrack.load()))==metro::swingShortcut(s);
            if(presetPage(m)) {
                int slot=m->presetBankIndex.load()*8+s;
                if(m->pendingSlot.load()==slot)return std::fmod(system::getTime(),.3)<.15;
                if(m->lastLoadedSlot.load()==slot && m->uiPage.load()==Metropolix::LOAD_PAGE)return std::fmod(system::getTime(),.9)<.65;
                return m->getPreset(slot).occupied;
            }
            int row=m->selectedRow.load();
            if(m->uiPage.load()==Metropolix::STAGE_PAGE) {
                if(m->uiStageDetail.load() && m->selectedStage.load()==s && (m->uiMod.load() || (row!=Metropolix::SLIDE && row!=Metropolix::SKIP && !(row==Metropolix::CV && cvGate(m)))))return std::fmod(system::getTime(),.65)<.4;
                if(m->uiMod.load())return std::abs(m->params[Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_VALUE+s)].getValue())>.001f;
                if(row==Metropolix::CV)return m->getParamQuantity(Metropolix::stageParam(m->selectedTrack.load(),s,row))->getValue()!=(0.f);
                int v=m->integer(Metropolix::stageParam(m->selectedTrack.load(),s,row));
                return row>=Metropolix::PITCH_OVERRIDE?v>=0:row==Metropolix::RATCHETS?v>1:row==Metropolix::PROBABILITY?v<100:v!=0;
            }
            return m->playStage[m->selectedTrack.load()].load()==s;
        };
        c->activeColor=[m,s,slideTime] {
            const NVGcolor colors[]={nvgRGB(235,54,46),nvgRGB(249,142,33),nvgRGB(241,220,63),nvgRGB(88,214,121),nvgRGB(76,222,233),nvgRGB(95,137,248),nvgRGB(218,82,222),nvgRGB(213,224,246)};
            if(!m)return colors[7];
            if(slideTime->dragging && m->uiPage.load()==Metropolix::SLIDE_PAGE)return nvgRGB(240,245,248);
            if(m->orderShortcutHeld.load() || m->scaleShortcutHeld.load())return nvgRGB(240,245,248);
            if(m->loopyMuteHeld.load())return m->integer(Metropolix::modParam(s,Metropolix::MOD_MUTE))?nvgRGB(240,55,40):nvgRGB(245,220,60);
            if(m->uiPage.load()==Metropolix::LOOPY_PAGE) {
                if(m->loopyCommand.load()&(1u<<s))return nvgRGB(90,165,248);
                return m->integer(Metropolix::RUN) && std::fmod(system::getTime()*2+s*.13,1.)<.5?nvgRGB(249,135,186):nvgRGB(135,187,227);
            }
            if(chainPage(m) && !m->chainQuick && m->chainEdit.load()!=1)return nvgRGB(240,245,248);
            if(chainPage(m))return m->presetBankIndex.load()==7?nvgRGB(249,135,186):colors[m->presetBankIndex.load()];
            if(m->uiChooseFunction.load())return colors[s];
            if(presetPage(m))return m->presetBankIndex.load()==7?nvgRGB(249,135,186):colors[m->presetBankIndex.load()];
            if(m->uiMod.load())return colors[7];
            if(m->selectedRow.load()==Metropolix::CV)return nvgRGB(249,135,186);
            const int map[]={4,5,6,0,1,7,2,3};return colors[map[m->selectedRow.load()]];
        };
        c->dimActive=[m] { return (presetPage(m) || chainPage(m)) && !m->uiChooseFunction.load(); };
        c->extraMenu=[m](ui::Menu* menu){appendStagePairMenu(menu,m);appendSimpleStageMenu(menu,m);};
        c->rightAction=[c,m,s] {
            if(!m)return;
            if(stagePairPage(m) || simpleStagePage(m)){m->selectedStage.store(s);c->bind();if(c->getParamQuantity())c->createContextMenu();else{auto* menu=createMenu();appendStagePairMenu(menu,m);appendSimpleStageMenu(menu,m);}return;}
            if(timingPage(m)){auto* menu=createMenu();int page=m->uiPage.load(),id=timingParam(m,page,m->uiField.load());menu->addChild(createMenuItem("Set "+timingShortcutLabel(m,page,s),"",[m,page,s]{timingShortcut(m,page,s);},m->ctrlOwner(id)>=0));appendTimingMenu(menu,m,page);return;}
            if(outputPage(m)){appendOutputMenu(createMenu(),m);return;}
            if(m->uiPage.load()==Metropolix::CLOCK_OUT_PAGE || (m->uiAlt.load() && s==4)){appendClockMenu(createMenu(),m);return;}
            if(m->uiPage.load()==Metropolix::SLIDE_PAGE){auto* menu=createMenu();if(!m->uiMod.load())menu->addChild(createMenuItem("Set slide amount: "+std::to_string(metro::slideShortcut(s))+"%","",[m,s]{setWithHistory(m->getParamQuantity(Metropolix::trackParam(m->selectedTrack.load(),Metropolix::SLIDE_TIME)),metro::slideShortcut(s));}));appendSlideMenu(menu,m);return;}
            if(m->uiPage.load()==Metropolix::ORDER_PAGE){auto* menu=createMenu();menu->addChild(createMenuItem("Recall order shortcut "+orderShortcutLabel(m,s),"",[m,s]{orderShortcut(m,s,false);},orderControlled(m)));menu->addChild(createMenuItem("Save order/direction to shortcut "+std::to_string(s+1),"",[m,s]{orderShortcut(m,s,true);}));appendOrderMenu(menu,m);return;}
            if(m->uiPage.load()==Metropolix::SCALE_PAGE){auto* menu=createMenu();menu->addChild(createMenuItem("Recall scale shortcut "+std::to_string(s+1),"",[m,s]{scaleShortcut(m,s,false);}));menu->addChild(createMenuItem("Save scale/root to shortcut "+std::to_string(s+1),"",[m,s]{scaleShortcut(m,s,true);}));appendScaleMenu(menu,m);return;}
            if(m->uiPage.load()==Metropolix::LOOPY_PAGE || m->loopyMuteHeld.load()){appendLoopyMenu(createMenu(),m,s);return;}
            if(chainPage(m)){appendChainMenu(createMenu(),m);return;}
            if(m->uiPage.load()==Metropolix::SWING_PAGE){if(!m->uiMod.load()){m->selectedStage.store(s);auto* menu=createMenu();menu->addChild(createMenuItem(std::to_string(metro::swingShortcut(s))+"% swing","",[m,s]{setWithHistory(m->getParamQuantity(Metropolix::swingParam(m->selectedTrack.load())),metro::swingShortcut(s));}));}return;}
            if(presetPage(m)){presetSlotMenu(m,s);return;}
            m->uiAlt.store(false); m->uiChooseFunction.store(false); m->uiEdit.store(true); m->uiStageDetail.store(true); m->selectedStage.store(s); m->uiPage.store(Metropolix::STAGE_PAGE);
            c->bind(); c->createContextMenu();
        };
        c->extraMenu=[m,s,choice](ui::Menu* menu) {
            if(!m)return;
            appendWorkflowMenu(menu,m);appendModWorkflowMenu(menu,m);
            if(m && (m->uiPage.load()==Metropolix::SETUP_PAGE || m->uiPage.load()==Metropolix::GLOBAL_SETUP_PAGE || m->uiPage.load()==Metropolix::UI_SETUP_PAGE || m->uiPage.load()==Metropolix::MIDI_SETUP_PAGE || m->uiPage.load()==Metropolix::TUNER_PAGE))appendGlobalMenu(menu,m);
        overrideMenu(menu,m);
            if(cvPage(m))appendCvMenu(menu,m);
            menu->addChild(new ui::MenuSeparator);
            if(m->uiMod.load()) {
                choice(menu,m,"Transition",Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_RAMP+s),{"Step","Ramp"});
            } else {
                for(int r=0;r<8;++r) menu->addChild(createCheckMenuItem(rowNames[r],"",[m,r] { return m->selectedRow.load()==r; },[m,r] { m->selectedRow.store(r); }));
                menu->addChild(createMenuItem("Probability / accumulator triggers","",[m,s] { stageSettingsMenu(m,s); }));
            }
        };
    }
}
