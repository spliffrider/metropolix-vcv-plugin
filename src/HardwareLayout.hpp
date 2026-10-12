// Build interactive components over the 34 HP artwork. The musical engine and
// parameter/port IDs remain the same as the accepted 2.1.0 build.
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
    auto placeholder=[&](const std::string& name,float x,float y,float size,HardwareControl::Look look) {
        auto* c=control(name,x,y,size,look);
        c->help="This hardware function is not implemented yet.";
        c->action=[unavailable,name] { unavailable(name); }; c->rightAction=c->action;
        return c;
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
        placeholder(name+" input",24+i*37,96,24,HardwareControl::SOCKET);
        placeholder(name+" attenuverter",24+i*37,159,20,HardwareControl::BLACK_KNOB);
        placeholder(name+" assignment",24+i*37,193,15,HardwareControl::DARK_BUTTON);
    }
    for(int i=0;i<2;++i) {
        placeholder(string::f("CTRL %d",i+1),31+i*72,252,41,HardwareControl::WHITE_KNOB);
        placeholder(string::f("CTRL %d assignment",i+1),67+i*72,229,15,HardwareControl::DARK_BUTTON);
    }
    for(int s=0;s<8;++s) {
        const int ids[]={Metropolix::PITCH+s,Metropolix::PULSES+s,Metropolix::GATES+s};
        const float ys[]={38,159,268}, heights[]={83,81,40};
        for(int k=0;k<3;++k) {
            auto* c=new HardwareSlider;
            c->metroModule=m; c->stage=s; c->kind=static_cast<HardwareSlider::Kind>(k);
            int id=ids[k]; c->binding=[id] { return id; };
            c->box=Rect(Vec(233+s*35.7f,ys[k]),Vec(22,heights[k]));
            c->customSpeed=1000.f/(heights[k]-18.f);
            c->help=k==0?"Shared pitch slider; root, scale, and track settings shape the resulting note.":k==1?"Clock pulses spent on this shared stage (1-8).":"Shared gate mode: Rest, Single, Multiple, or Hold.";
            c->bind(); addParam(c);
        }
    }
    auto* display=new HardwareDisplay;
    display->metroModule=m; display->name="Metropolix display"; display->look=HardwareControl::INVISIBLE;
    display->box=Rect(Vec(130,135),Vec(93,49));
    display->binding=[m] { return screenParam(m); };
    display->help="Shows the selected track, lane, and value. Drag the displayed value to edit. Click to select the next field.";
    display->action=[m] { encoderPress(m); };
    display->rightAction=[display,stageEditor,laneMenu,m] {
        if(screenParam(m)>=0) display->createContextMenu();
        else if(m && m->uiMod.load()) laneMenu(); else stageEditor();
    };
    display->bind(); addChild(display);
    auto* encoder=control("Encoder",176,229,42,HardwareControl::ENCODER);
    encoder->binding=[m] { return screenParam(m); };
    encoder->help="Drag up/down to edit the screen value. Click to switch fields. EXIT returns home.";
    encoder->action=[m] { encoderPress(m); };
    auto* exit=control("EXIT",214,229,15,HardwareControl::DARK_BUTTON);
    exit->help="Return to the home overview. Right-click for optional editors.";
    exit->action=[m] { if(m) { screenPage(m,Metropolix::HOME); m->uiEdit.store(false); m->uiAlt.store(false); m->uiChooseFunction.store(false); } };
    exit->rightAction=[stageEditor,modEditor,m] {
        auto* menu=createMenu();
        menu->addChild(createMenuItem("Open stage editor","",stageEditor));
        menu->addChild(createMenuItem("Open MOD editor","",modEditor));
        menu->addChild(createMenuItem("Close optional editor","",[m] { if(m)m->expanded.store(false); }));
    };
    for(int t=0;t<2;++t) {
        auto* c=control(string::f("TRK %d",t+1),145+t*31,272,16,HardwareControl::BUTTON);
        c->help="Select this note track. Right-click for its settings and the stage editor.";
        c->action=[m,t] { if(m) { m->selectedTrack.store(t); m->uiMod.store(false); m->modEditor.store(false); m->uiChooseFunction.store(false); } };
        c->active=[m,t] { return m && !m->uiMod.load() && m->selectedTrack.load()==t; };
        c->rightAction=[m,t] { if(m) { m->selectedTrack.store(t); m->uiMod.store(false); m->uiChooseFunction.store(false); trackSettingsMenu(m); } };
    }
    auto* mod=control("MOD",207,272,16,HardwareControl::BUTTON);
    mod->help="Select modulation editing. Stage buttons select stages; encoder edits volts. Right-click to choose a lane or open the grid.";
    mod->action=[m] { if(m) { m->uiMod.store(true); m->modEditor.store(true); screenPage(m,Metropolix::STAGE_PAGE); } };
    mod->active=[m] { return m && m->uiMod.load(); }; mod->rightAction=laneMenu;
    auto* run=createParam<TransportButton>(Vec(13,298),m,Metropolix::RUN); addParam(run);
    auto* reset=createParam<TransportButton>(Vec(75.4f,298),m,Metropolix::RESET);
    reset->reset=true; reset->momentary=true; addParam(reset);
    placeholder("Loopy",52.2f,306,16,HardwareControl::BUTTON);

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
    setup->extraMenu=[m,stageEditor,modEditor](ui::Menu* menu) {
        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createMenuItem("Selected track settings","",[m] { trackSettingsMenu(m); }));
        menu->addChild(createMenuItem("Open stage editor","",stageEditor));
        menu->addChild(createMenuItem("Open MOD editor","",modEditor));
    };
    valueButton("BPM",145.8f,306,Metropolix::BPM_PAGE,[] { return Metropolix::TEMPO; });
    auto* scale=valueButton("SCALE",177,306,Metropolix::SCALE_PAGE,[] { return Metropolix::SCALE; });
    scale->action=[m] { screenPage(m,Metropolix::SCALE_PAGE); if(m)m->uiField.store(1); }; scale->dragAction=scale->action;
    scale->extraMenu=[choice,m](ui::Menu* menu) { menu->addChild(new ui::MenuSeparator); choice(menu,m,"Root",Metropolix::ROOT,{"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"}); };
    auto* alt=control("ALT",208.2f,306,16,HardwareControl::DARK_BUTTON);
    alt->help="Toggle secondary stage-button actions: Rack preset load/save, A/B assignment, and lane/accumulator settings.";
    alt->action=[m] { if(m)m->uiAlt.store(!m->uiAlt.load()); }; alt->active=[m] { return m && m->uiAlt.load(); };
    valueButton("ORDER",21,340,Metropolix::ORDER_PAGE,[trackOrMod] { return trackOrMod(Metropolix::ORDER,Metropolix::MOD_ORDER); });
    auto* len=valueButton("LEN",52.2f,340,Metropolix::LENGTH_PAGE,[trackOrMod] { return trackOrMod(Metropolix::LENGTH,Metropolix::MOD_LENGTH); });
    len->extraMenu=[choice,m,trackOrMod](ui::Menu* menu) {
        std::vector<std::string> offsets; for(int i=0;i<8;++i)offsets.push_back(std::to_string(i));
        menu->addChild(new ui::MenuSeparator); choice(menu,m,"Stage offset",trackOrMod(Metropolix::OFFSET,Metropolix::MOD_OFFSET),offsets);
    };
    valueButton("DIV",83.4f,340,Metropolix::DIV_PAGE,[trackOrMod] { return trackOrMod(Metropolix::DIVISION,Metropolix::MOD_DIVISION); });
    placeholder("SWING",114.6f,340,16,HardwareControl::BUTTON);
    valueButton("SLIDE",145.8f,340,Metropolix::SLIDE_PAGE,[m] { return m && !m->uiMod.load()?Metropolix::trackParam(m->selectedTrack.load(),Metropolix::SLIDE_TIME):-1; });
    valueButton("GATE",177,340,Metropolix::GATE_PAGE,[m] { return m && !m->uiMod.load()?Metropolix::trackParam(m->selectedTrack.load(),Metropolix::GATE_LENGTH):-1; });
    auto* edit=control("EDIT",208.2f,340,16,HardwareControl::DARK_BUTTON);
    edit->help="Choose a stage function: press EDIT, then its printed stage button, then a stage. Right-click opens the grid.";
    edit->action=[m] { if(m) { if(m->uiMod.load())screenPage(m,Metropolix::STAGE_PAGE); else m->uiChooseFunction.store(!m->uiChooseFunction.load()); } };
    edit->active=[m] { return m && m->uiChooseFunction.load(); }; edit->rightAction=stageEditor;
    const char* names[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
    const int rows[]={Metropolix::SLIDE,Metropolix::SKIP,-1,-1,Metropolix::RATCHETS,Metropolix::PROBABILITY,Metropolix::ACCUMULATION,Metropolix::CV};
    for(int s=0;s<8;++s) {
        auto* c=control(std::string("Stage ")+std::to_string(s+1)+" / "+names[s],244+s*35.7f,340,16,HardwareControl::BUTTON);
        c->binding=[m,s] {
            if(!m || m->uiAlt.load() || m->uiChooseFunction.load()) return -1;
            return m->uiMod.load()?Metropolix::modParam(m->selectedLane.load(),(m->uiField.load()?Metropolix::MOD_RAMP:Metropolix::MOD_VALUE)+s):Metropolix::stageParam(m->selectedTrack.load(),s,m->selectedRow.load());
        };
        c->help="EDIT then a function, then select a stage. Hold + drag edits this stage. Slide/Skip clicks toggle. Right-click for values.";
        c->dragAction=[m,s] { if(m && !m->uiAlt.load() && !m->uiChooseFunction.load()) { m->selectedStage.store(s); m->uiPage.store(Metropolix::STAGE_PAGE); } };
        int row=rows[s];
        c->action=[this,m,s,row,unavailable] {
            if(!m)return;
            if(m->uiAlt.load()) {
                m->uiAlt.store(false);
                if(s==0) loadDialog();
                else if(s==1) saveDialog();
                else if(s==2)screenPage(m,Metropolix::A_PAGE);
                else if(s==3)screenPage(m,Metropolix::B_PAGE);
                else if(s==6)trackSettingsMenu(m);
                else if(s==7)modSettingsMenu(m,m->selectedLane.load());
                else unavailable(s==4?"Clock output configuration":"MX expander");
                return;
            }
            if(m->uiChooseFunction.load()) {
                m->uiChooseFunction.store(false);
                if(row>=0) { m->selectedRow.store(row); m->uiEdit.store(true); screenPage(m,Metropolix::STAGE_PAGE); }
                else unavailable(s==2?"Per-stage pitch override":"Per-stage gate-length override");
                return;
            }
            m->selectedStage.store(s); m->uiPage.store(Metropolix::STAGE_PAGE); m->uiEdit.store(true);
            int current=m->selectedRow.load();
            if(!m->uiMod.load() && (current==Metropolix::SLIDE || current==Metropolix::SKIP)) {
                auto* q=m->getParamQuantity(Metropolix::stageParam(m->selectedTrack.load(),s,current));
                setWithHistory(q,q->getValue()>.5f?0.f:1.f);
            }
        };
        c->active=[m,s] {
            if(!m)return false;
            if(m->uiChooseFunction.load())return true;
            int row=m->selectedRow.load();
            if(!m->uiMod.load() && m->uiEdit.load() && (row==Metropolix::SLIDE || row==Metropolix::SKIP))
                return m->integer(Metropolix::stageParam(m->selectedTrack.load(),s,row))!=0;
            if(m->uiEdit.load() || m->uiMod.load())return m->selectedStage.load()==s;
            return m->playStage[m->selectedTrack.load()].load()==s;
        };
        c->activeColor=[m] {
            if(m && !m->uiMod.load() && m->uiEdit.load() && !m->uiChooseFunction.load()) {
                if(m->selectedRow.load()==Metropolix::SLIDE)return nvgRGB(236,66,58);
                if(m->selectedRow.load()==Metropolix::SKIP)return nvgRGB(247,152,54);
            }
            return nvgRGB(135,187,227);
        };
        c->rightAction=[c,m,s] {
            if(!m)return;
            m->uiAlt.store(false); m->uiChooseFunction.store(false); m->uiEdit.store(true); m->selectedStage.store(s); m->uiPage.store(Metropolix::STAGE_PAGE);
            c->bind(); c->createContextMenu();
        };
        c->extraMenu=[m,s,choice](ui::Menu* menu) {
            if(!m)return;
            menu->addChild(new ui::MenuSeparator);
            if(m->uiMod.load()) {
                choice(menu,m,"Transition",Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_RAMP+s),{"Step","Ramp"});
            } else {
                for(int r=0;r<6;++r) menu->addChild(createCheckMenuItem(rowNames[r],"",[m,r] { return m->selectedRow.load()==r; },[m,r] { m->selectedRow.store(r); }));
                menu->addChild(createMenuItem("Probability / accumulator triggers","",[m,s] { stageSettingsMenu(m,s); }));
            }
        };
    }
}
