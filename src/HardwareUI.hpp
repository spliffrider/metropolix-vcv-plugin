// Included inside Metropolix.cpp's private UI namespace.
// Coordinates follow the supplied front-on panel photograph at 34 HP / 510 px.
constexpr float HARDWARE_WIDTH = 510.f;
constexpr float EDITOR_WIDTH = 480.f;
void chainEncoderPress(Metropolix* m);
void openChainPage(Metropolix* m);
void appendChainMenu(ui::Menu* menu,Metropolix* m);
void stroke(NVGcontext* vg,float x,float y,float x2,float y2,NVGcolor color,float width=1.f) {
    nvgBeginPath(vg);nvgMoveTo(vg,x,y);nvgLineTo(vg,x2,y2);nvgStrokeColor(vg,color);nvgStrokeWidth(vg,width);nvgStroke(vg);
}

void disc(NVGcontext* vg, float x, float y, float r, NVGcolor color) {
    nvgBeginPath(vg); nvgCircle(vg, x, y, r); nvgFillColor(vg, color); nvgFill(vg);
}
void metalDisc(NVGcontext* vg, float x, float y, float r, NVGcolor top, NVGcolor bottom) {
    nvgBeginPath(vg); nvgCircle(vg, x, y, r);
    nvgFillPaint(vg, nvgLinearGradient(vg, x-r, y-r, x+r, y+r, top, bottom)); nvgFill(vg);
}
void socketDrawing(NVGcontext* vg, float x, float y) {
    disc(vg,x+.5f,y+1,11.9f,nvgRGBA(0,0,0,60));
    nvgBeginPath(vg);
    for(int i=0;i<6;++i) {
        float a=i*M_PI/3.f + M_PI/6.f;
        float px=x+11.5f*std::cos(a), py=y+11.5f*std::sin(a);
        if(i) nvgLineTo(vg,px,py); else nvgMoveTo(vg,px,py);
    }
    nvgClosePath(vg); nvgFillPaint(vg,nvgLinearGradient(vg,x-10,y-10,x+9,y+9,nvgRGB(233,226,205),nvgRGB(110,103,85))); nvgFill(vg);
    metalDisc(vg,x,y,10.6f,nvgRGB(250,248,231),nvgRGB(119,118,112));
    disc(vg,x,y,9.8f,nvgRGB(61,57,46));
    metalDisc(vg,x,y,9.3f,nvgRGB(243,237,215),nvgRGB(90,84,68));
    // Narrow alternating reflections model the turned metal lip around the bore.
    for(int i=0;i<48;++i) {
        float a=i*2*M_PI/48.f,b=a+2*M_PI/48.f+.01f;
        float shine=.5f+.5f*std::sin(a*2.0f+.8f);int v=110+static_cast<int>(125*shine);
        nvgBeginPath(vg);nvgArc(vg,x,y,8.65f,a,b,NVG_CW);
        nvgStrokeWidth(vg,.8f);nvgStrokeColor(vg,nvgRGB(v,std::max(0,v-8),std::max(0,v-27)));nvgStroke(vg);
    }
    disc(vg,x,y,7.8f,nvgRGB(36,33,25));
    metalDisc(vg,x,y,7.2f,nvgRGB(171,156,117),nvgRGB(235,224,189));
    disc(vg,x,y-.1f,6.45f,nvgRGB(8,10,9));
    nvgBeginPath(vg);nvgArc(vg,x,y,10.1f,-2.8f,-.5f,NVG_CW);
    nvgStrokeWidth(vg,.55f);nvgStrokeColor(vg,nvgRGB(255,252,238));nvgStroke(vg);
}
struct MetalPort : app::PortWidget {
    MetalPort() { box.size=Vec(24,24); }
    void draw(const DrawArgs& args) override { socketDrawing(args.vg,12,12); }
};

int screenParam(Metropolix* m) {
    if(!m) return -1;
    int t=m->selectedTrack.load(), l=m->selectedLane.load(), f=m->uiField.load();
    bool mod=m->uiMod.load();
    if(m->uiChooseFunction.load())return -1;
    switch(m->uiPage.load()) {
        case Metropolix::LOOPY_PAGE: return Metropolix::LOOPY_PULSES;
        case Metropolix::BPM_PAGE: return m->externalClock.load()?-1:Metropolix::TEMPO;
        case Metropolix::SCALE_PAGE: return f ? Metropolix::SCALE : Metropolix::ROOT;
        case Metropolix::ORDER_PAGE: return mod ? Metropolix::modParam(l,Metropolix::MOD_ORDER) : Metropolix::trackParam(t,Metropolix::ORDER);
        case Metropolix::LENGTH_PAGE:
            if(m->uiAlt.load())return mod?Metropolix::modParam(l,Metropolix::MOD_OFFSET):Metropolix::trackParam(t,Metropolix::OFFSET);
            return mod?Metropolix::modParam(l,f?Metropolix::MOD_PULSE_LIMIT:Metropolix::MOD_LENGTH):(f?Metropolix::extraParam(t,Metropolix::PULSE_LIMIT):Metropolix::trackParam(t,Metropolix::LENGTH));
        case Metropolix::DIV_PAGE: return mod ? Metropolix::modParam(l,Metropolix::MOD_DIVISION) : (f?Metropolix::extraParam(t,Metropolix::PULSE_DIV):Metropolix::trackParam(t,Metropolix::DIVISION));
        case Metropolix::SWING_PAGE: return mod ? -1 : Metropolix::swingParam(t);
        case Metropolix::SLIDE_PAGE: return mod ? -1 : Metropolix::trackParam(t,Metropolix::SLIDE_TIME);
        case Metropolix::GATE_PAGE: return mod ? -1 : f?Metropolix::gateScaleParam(t):Metropolix::trackParam(t,Metropolix::GATE_LENGTH);
        case Metropolix::STAGE_PAGE: return mod ? Metropolix::modParam(l,Metropolix::MOD_VALUE+m->selectedStage.load()) : Metropolix::stageParam(t,m->selectedStage.load(),m->selectedRow.load());
        case Metropolix::A_PAGE: return Metropolix::OUTPUT_SOURCE_A;
        case Metropolix::B_PAGE: return Metropolix::OUTPUT_SOURCE_B;
        case Metropolix::AUX_PAGE: return Metropolix::auxParam(m->selectedAux.load(),f?Metropolix::AUX_ASSIGNMENT:Metropolix::AUX_DESTINATION);
        case Metropolix::CTRL_PAGE: return Metropolix::ctrlParam(m->selectedCtrl.load(),f?Metropolix::CTRL_ASSIGNMENT:Metropolix::CTRL_DESTINATION);
        case Metropolix::SETUP_PAGE: return f==3?Metropolix::RANGE:-1;
        case Metropolix::PRESET_SETUP_PAGE: return Metropolix::TOTAL_PULSES + metro::clamp(f, 0, 6);
        case Metropolix::ACCUM_PAGE: return Metropolix::accumScreenParam(t,f);
        default: return -1;
    }
}
void screenPage(Metropolix* m, int page) {
    if(!m) return;
    if(m->uiPage.load()==Metropolix::LOOPY_PAGE && page!=Metropolix::LOOPY_PAGE && m->loopyHeld.load() && m->integer(Metropolix::RUN))m->latchLoopy();
    m->loopyMuteHeld.store(false);
    m->uiPage.store(page); m->uiField.store(0); m->uiChooseFunction.store(false);
}
void encoderPress(Metropolix* m) {
    if(!m) return;
    int p=m->uiPage.load();
    if(p==Metropolix::LOOPY_PAGE) {
        if(m->loopyLatched.load())m->clearLoopy();
        else setWithHistory(m->getParamQuantity(Metropolix::LOOPY_TARGET),1-m->integer(Metropolix::LOOPY_TARGET));
        return;
    }
    if(p==Metropolix::CHAIN_PAGE) {chainEncoderPress(m);return;}
    if(p==Metropolix::SETUP_PAGE) {
        int f=m->uiField.load();
        if(f==1)screenPage(m,Metropolix::PRESET_SETUP_PAGE);
        else if(f==2)openChainPage(m);
        else m->uiField.store(f==3?0:3);
        return;
    }
    if(p==Metropolix::HOME) { m->uiHome.store((m->uiHome.load()+1)%4); return; }
    if(p==Metropolix::PRESET_SETUP_PAGE) { m->uiField.store((m->uiField.load()+1)%7); return; }
    if(p==Metropolix::LOAD_PAGE) { setWithHistory(m->getParamQuantity(Metropolix::LOAD_DELAY), (m->integer(Metropolix::LOAD_DELAY)+1)%3); return; }
    if(p==Metropolix::SAVE_PAGE) { m->presetSaveAction.store((m->presetSaveAction.load()+1)%3); return; }
    if(p==Metropolix::ACCUM_PAGE) { m->uiField.store((m->uiField.load()+1)%6); return; }
    if(p==Metropolix::ORDER_PAGE) {
        int id=m->uiMod.load()?Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_REVERSED):Metropolix::extraParam(m->selectedTrack.load(),Metropolix::REVERSED);
        auto* q=m->getParamQuantity(id);setWithHistory(q,1-q->getValue());return;
    }
    if(p==Metropolix::STAGE_PAGE) {
        if(overridePage(m) && m->selectedRow.load()==Metropolix::PITCH_OVERRIDE){syncPitchOverrides(m);return;}
        int id=-1, count=2, t=m->selectedTrack.load(),s=m->selectedStage.load();
        if(m->uiMod.load())id=Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_RAMP+s);
        else if(m->selectedRow.load()==Metropolix::PROBABILITY)id=Metropolix::stageOption(t,s,Metropolix::PROB_TARGET);
        else if(m->selectedRow.load()==Metropolix::ACCUMULATION){id=Metropolix::stageOption(t,s,Metropolix::ACCUM_TRIGGER);count=3;}
        if(id>=0){auto* q=m->getParamQuantity(id);setWithHistory(q,(static_cast<int>(q->getValue())+1)%count);}return;
    }
    if(p==Metropolix::SCALE_PAGE || p==Metropolix::LENGTH_PAGE || p==Metropolix::AUX_PAGE || p==Metropolix::CTRL_PAGE || (p==Metropolix::GATE_PAGE && !m->uiMod.load()) ||
       (p==Metropolix::DIV_PAGE && !m->uiMod.load()))
        m->uiField.store(1-m->uiField.load());
}


void openLoopy(Metropolix* m) {
    if(!m)return;
    if(m->uiMod.load()){m->selectedTrack.store(0);m->uiMod.store(false);}
    screenPage(m,Metropolix::LOOPY_PAGE);m->uiAlt.store(false);m->uiEdit.store(false);
}
void loopRange(Metropolix* m,int first,int last) {
    if(!m)return;
    if(!m->integer(Metropolix::RUN))return;
    openLoopy(m);m->loopyHeld.store(0);m->requestLoopy((1u<<first)|(1u<<last));m->latchLoopy();
}
void appendLoopyMenu(ui::Menu* menu,Metropolix* m,int start=-1) {
    if(!m)return;
    menu->addChild(createMenuLabel("Loopy performance"));
    menu->addChild(createMenuItem("Open Loopy","",[m]{openLoopy(m);}));
    menu->addChild(createSubmenuItem("Loop pulses","",[m](ui::Menu* child){
        for(int n=0;n<=64;++n)child->addChild(createCheckMenuItem(n?std::to_string(n):"Auto","",[m,n]{return m->integer(Metropolix::LOOPY_PULSES)==n;},[m,n]{setWithHistory(m->getParamQuantity(Metropolix::LOOPY_PULSES),n);}));
    }));
    menu->addChild(createCheckMenuItem("Target both tracks","",[m]{return m->integer(Metropolix::LOOPY_TARGET)!=0;},[m]{setWithHistory(m->getParamQuantity(Metropolix::LOOPY_TARGET),1-m->integer(Metropolix::LOOPY_TARGET));}));
    if(start>=0 && m->integer(Metropolix::RUN))menu->addChild(createSubmenuItem("Latch range from stage "+std::to_string(start+1),"",[m,start](ui::Menu* child){
        for(int end=0;end<8;++end)child->addChild(createMenuItem("Through stage "+std::to_string(end+1),"",[m,start,end]{loopRange(m,start,end);}));
    }));
    menu->addChild(createMenuItem("Latch current loop","",[m]{if(m->integer(Metropolix::RUN))m->latchLoopy();}));
    menu->addChild(createMenuItem("Release loop / audition","",[m]{m->clearLoopy();}));
    menu->addChild(createCheckMenuItem("Mute tracks / MOD lanes","",[m]{return m->loopyMuteHeld.load();},[m]{m->loopyMuteHeld.store(!m->loopyMuteHeld.load());m->loopyMuteUsed.store(true);}));
}

// An actual Rack Knob under every continuous/discrete value control. This
// preserves fine adjustment, snapping, numeric entry, and one undo per drag.
struct HardwareTooltip : ui::Tooltip {
    widget::Widget* owner=nullptr;
    void step() override {
        ui::Tooltip::step();
        if(owner && parent) box.pos=owner->getAbsoluteOffset(owner->box.size).round();
        if(parent) box=box.nudge(parent->box.zeroPos());
    }
};
struct HardwareControl : ValueKnob {
    enum Look { BUTTON, DARK_BUTTON, WHITE_KNOB, BLACK_KNOB, ENCODER, INVISIBLE, SOCKET, BLUE_BUTTON, CREAM_BUTTON };
    Look look=BUTTON;
    Metropolix* metroModule=nullptr;
    std::string name, help;
    std::function<int()> binding;
    std::function<void()> action, rightAction, dragAction;
    std::function<void()> longAction, releaseAction, navigationStart, navigationEnd;
    std::function<bool()> longEnabled;
    std::function<std::string()> navigationValue;
    std::function<bool()> navigationMode;
    std::function<void(int)> navigationTurn;
    std::function<void(ui::Menu*)> extraMenu;
    std::function<bool()> active;
    std::function<bool()> dimActive;
    std::function<std::string()> detailHelp;
    std::function<NVGcolor()> activeColor;
    NVGcolor lightColor=nvgRGB(135,187,227);
    ui::Tooltip* tip=nullptr;
    bool dragging=false;
    bool navigating=false;
    float navigationDelta=0;
    float moved=0;
    float customSpeed=0;
    bool doubleClickReset=true;
    bool longFired=false;
    double heldSince=0, longSeconds=1;
    int controllerOwner=-1;
    std::function<std::vector<int>()> groupedParams;
    std::vector<int> gestureIds;
    std::vector<float> gestureBefore;
    HardwareControl() { smooth=false; forceLinear=true; speed=.55f; box.size=Vec(16,16); }
    ~HardwareControl() { clearTip(); }
    void bind() {
        if(dragging) return;
        int id=binding ? binding() : -1;
        controllerOwner=metroModule?metroModule->ctrlOwner(id):-1;
        module=(metroModule && id>=0 && controllerOwner<0) ? metroModule : nullptr;
        paramId=id>=0 ? id : 0;
        if(auto* q=getParamQuantity()) speed=customSpeed>0?customSpeed:(q->snapEnabled?std::max(.7f,30.f/q->getRange()):.7f);
    }
    std::string tooltipText() {
        auto* q=getParamQuantity();
        std::string s=q ? q->getString() : name;
        if(controllerOwner>=0 && metroModule) s=metroModule->getParamQuantity(paramId)->getString()+string::f("\nControlled by CTRL %d. Turn that knob; right-click to change its assignment.",controllerOwner+1);
        if(!help.empty()) s += "\n"+help;
        if(detailHelp) s += "\n"+detailHelp();
        if(q) s += "\nHold + drag up/down to increase/decrease. Ctrl = fine. Right-click = values/reset.";
        return s;
    }
    void clearTip() {
        if(!tip) return;
        if(tip->parent) tip->parent->removeChild(tip);
        delete tip; tip=nullptr;
    }
    void onEnter(const event::Enter&) override {
        bind();
        if(!settings::tooltips || tip || dragging) return;
        auto* tooltip=new HardwareTooltip; tooltip->owner=this; tip=tooltip;
        tip->text=tooltipText(); APP->scene->addChild(tip);
    }
    void onLeave(const event::Leave& e) override { clearTip(); ValueKnob::onLeave(e); }
    void step() override {
        bind(); ValueKnob::step();
        if(tip) { tip->text=tooltipText(); tip->box.pos=getAbsoluteOffset(box.size); }
        if(dragging && moved<3 && !longFired && longAction && (!longEnabled || longEnabled()) && system::getTime()-heldSince>=longSeconds) {longFired=true;longAction();}
        if(dragging && navigating && moved>0 && navigationValue && settings::tooltips) {
            if(!tip){auto* t=new HardwareTooltip;t->owner=this;tip=t;APP->scene->addChild(t);}
            tip->text=navigationValue();
        }
    }
    void onHover(const event::Hover& e) override { app::ParamWidget::onHover(e); }
    void onButton(const event::Button& e) override {
        bind();
        if(e.button==GLFW_MOUSE_BUTTON_RIGHT && e.action==GLFW_PRESS) {
            clearTip();
            if(controllerOwner>=0)appendCtrlMenu(createMenu(),metroModule,controllerOwner);
            else if(rightAction) rightAction();
            else if(getParamQuantity()) createContextMenu();
            else { auto* menu=createMenu(); menu->addChild(createMenuLabel(name)); menu->addChild(createMenuLabel(help)); }
            e.consume(this); return;
        }
        app::ParamWidget::onButton(e);
    }
    void onDragStart(const event::DragStart& e) override {
        if(e.button!=GLFW_MOUSE_BUTTON_LEFT)return;
        bind(); dragging=true; moved=0; clearTip();heldSince=system::getTime();longFired=false;
        navigating=navigationMode && navigationMode(); navigationDelta=0;
        if(dragAction) dragAction();
        if(navigating && navigationStart)navigationStart();
        ValueKnob::onDragStart(e);
        gestureIds=groupedParams?groupedParams():std::vector<int>{};gestureBefore.clear();
        for(int id:gestureIds)gestureBefore.push_back(metroModule->params[id].getValue());
    }
    void onDragMove(const event::DragMove& e) override {
        moved+=e.mouseDelta.norm();
        if(navigating && navigationTurn) {
            navigationDelta-=e.mouseDelta.y;
            int turns=static_cast<int>(navigationDelta/22.f);
            if(turns){navigationTurn(turns);navigationDelta-=22.f*turns;}
        } else {
            ValueKnob::onDragMove(e);
            if(gestureIds.size()>1 && overridePage(metroModule)) {
                auto it=std::find(gestureIds.begin(),gestureIds.end(),paramId);
                if(it!=gestureIds.end()) {
                    int index=int(it-gestureIds.begin()),scale=metroModule->integer(Metropolix::SCALE);
                    bool pitch=metroModule->selectedRow.load()==Metropolix::PITCH_OVERRIDE;
                    auto degree=[&](float v){return v<0?-1:metro::nearestDegree(v,scale);};
                    float current=metroModule->params[paramId].getValue();
                    float delta=pitch?float(degree(current)-degree(gestureBefore[index])):current-gestureBefore[index];
                    for(size_t i=0;i<gestureIds.size();++i)if(gestureIds[i]!=paramId) {
                        float v=gestureBefore[i]+delta;
                        if(pitch){int d=metro::clamp(degree(gestureBefore[i])+int(delta),-1,metro::scaleAt(scale).count*5);v=d<0?-1.f:float(metro::degreeToSemitone(d,scale));}
                        metroModule->getParamQuantity(gestureIds[i])->setValue(v);
                    }
                }
            }
        }
    }
    void onDragEnd(const event::DragEnd& e) override {
        if(e.button!=GLFW_MOUSE_BUTTON_LEFT)return;
        bool batch=!gestureIds.empty() && (gestureIds.size()>1 || !getParamQuantity()) && (moved>=3 || longFired);
        std::vector<float> after;
        if(batch) {
            for(int id:gestureIds)after.push_back(metroModule->params[id].getValue());
            // Finish Rack's cursor/drag state with no native parameter change;
            // then record all affected values as one normal Rack undo action.
            auto it=std::find(gestureIds.begin(),gestureIds.end(),paramId);
            if(it!=gestureIds.end())metroModule->params[paramId].setValue(gestureBefore[it-gestureIds.begin()]);
        }
        ValueKnob::onDragEnd(e);
        if(batch){for(size_t i=0;i<gestureIds.size();++i)metroModule->params[gestureIds[i]].setValue(after[i]);parameterHistory(metroModule,gestureIds,gestureBefore,"edit gate or stage overrides");}
        gestureIds.clear();gestureBefore.clear();
        if(navigating && navigationEnd)navigationEnd();
        if(releaseAction)releaseAction();
        dragging=false;clearTip();
    }
    void onAction(const event::Action&) override { if(moved<3.f && !longFired && action) action(); }
    void onDoubleClick(const event::DoubleClick& e) override { if(doubleClickReset && getParamQuantity()) app::Knob::onDoubleClick(e); }
    void appendContextMenu(ui::Menu* menu) override { if(extraMenu) extraMenu(menu); }
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg; float x=box.size.x/2, y=box.size.y/2, r=std::min(x,y)-.7f;
        if(look==INVISIBLE) return;
        if(look==SOCKET) { socketDrawing(vg,x,y); return; }
        bool on=active && active();
        disc(vg,x+.4f,y+.9f,r+.4f,nvgRGBA(0,0,0,65));
        metalDisc(vg,x,y,r,nvgRGB(102,104,106),nvgRGB(212,213,211));
        if(look==BUTTON || look==DARK_BUTTON || look==BLUE_BUTTON || look==CREAM_BUTTON) {
            NVGcolor top=look==DARK_BUTTON?nvgRGB(51,54,53):nvgRGB(249,249,248);
            NVGcolor bottom=look==DARK_BUTTON?nvgRGB(17,20,20):nvgRGB(198,202,209);
            if(look==BLUE_BUTTON){top=nvgRGB(199,209,228);bottom=nvgRGB(137,155,184);}
            if(look==CREAM_BUTTON){top=nvgRGB(249,247,234);bottom=nvgRGB(205,205,193);}
            if(on) { top=activeColor?activeColor():lightColor; bottom=color::mult(top,.77f); }
            else if(dimActive && dimActive()) { top=color::mult(activeColor?activeColor():lightColor,.42f);bottom=color::mult(top,.77f); }
            disc(vg,x,y,r-.7f,nvgRGB(232,234,231));metalDisc(vg,x,y,r-1.5f,top,bottom);
            nvgBeginPath(vg);nvgArc(vg,x,y,r-2.1f,-2.8f,-.45f,NVG_CW);nvgStrokeWidth(vg,.45f);nvgStrokeColor(vg,nvgRGBA(255,255,255,145));nvgStroke(vg);
            return;
        }
        metalDisc(vg,x,y,r-1,nvgRGB(42,43,45),nvgRGB(93,95,98));
        for(int i=0;i<60;++i) {
            float a=i*2*M_PI/60.f;
            stroke(vg,x+std::cos(a)*(r-3.3f),y+std::sin(a)*(r-3.3f),x+std::cos(a)*(r-1),y+std::sin(a)*(r-1),nvgRGBA(179,182,182,90),.45f);
        }
        bool white=look==WHITE_KNOB;
        float cap=look==ENCODER?r-6.2f:white?r-5.5f:r-2.5f;
        metalDisc(vg,x,y,cap+1.3f,nvgRGB(165,168,169),nvgRGB(25,27,29));
        metalDisc(vg,x,y,cap,white?nvgRGB(253,253,251):nvgRGB(60,63,65),white?nvgRGB(213,216,216):nvgRGB(32,34,36));
        if(look!=ENCODER) {
            auto* q=getParamQuantity();
            float v=q ? q->getScaledValue() : .025f;
            float a=(-.83f + v*1.66f)*M_PI;
            stroke(vg,x+std::sin(a)*(cap*.45f),y-std::cos(a)*(cap*.45f),x+std::sin(a)*(cap-.5f),y-std::cos(a)*(cap-.5f),white?ink():nvgRGB(244,247,245),white?2.3f:1.8f);
        }
    }
};

// The lens keeps its rectangular hardware shape; Rack handles the emissive
// layer and halo, including View > Bloom and dark-room brightness.
struct PitchSliderLight : app::LightWidget {
    PitchSliderLight() { box.size=Vec(4,10.2f); }
    void drawBackground(const DrawArgs&) override {}
    void drawLight(const DrawArgs& args) override {
        rect(args.vg,0,0,box.size.x,box.size.y,color,.5f);
        stroke(args.vg,1.2f,.6f,1.2f,box.size.y-.9f,color::mult(color,1.5f),.65f);
    }
};

struct HardwareSlider : HardwareControl {
    enum Kind { PITCH_SLIDER, PULSE_SWITCH, GATE_SWITCH };
    Kind kind=PITCH_SLIDER;
    int stage=0;
    PitchSliderLight* pitchLight=nullptr;
    HardwareSlider() { box.size=Vec(22,82);pitchLight=new PitchSliderLight;addChild(pitchLight); }
    float handleHeight() const { return kind==PULSE_SWITCH?25.f:16.f; }
    float handleCenter() {
        auto* q=getParamQuantity();float v=q?q->getScaledValue():.35f;
        float handle=handleHeight();
        return handle/2+2+(1-v)*(box.size.y-handle-4);
    }
    void step() override {
        HardwareControl::step();
        pitchLight->visible=kind==PITCH_SLIDER;
        pitchLight->box.pos=Vec(box.size.x/2-2,handleCenter()-5.1f);
        float brightness=metroModule?std::max(metroModule->lights[Metropolix::POSITION_1+stage].getBrightness(),
            metroModule->lights[Metropolix::POSITION_2+stage].getBrightness()):0.f;
        pitchLight->color=color::mult(nvgRGB(255,51,13),.45f+.55f*brightness);
    }
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg;
        float x=box.size.x/2, h=box.size.y;
        float handle=handleHeight(),cy=handleCenter();
        float half=kind==PITCH_SLIDER?4.3f:6.5f;
        rect(vg,x-half-.5f,0,half*2+1,h,nvgRGB(111,108,94),.7f);
        rect(vg,x-half,1,half*2,h-2,kind==PULSE_SWITCH?nvgRGB(105,65,26):nvgRGB(7,10,8),.5f);
        stroke(vg,x-half+.8f,2,x-half+.8f,h-2,nvgRGB(170,147,101),.6f);
        stroke(vg,x+half-.6f,2,x+half-.6f,h-2,nvgRGB(44,50,43),.6f);
        if(kind==PULSE_SWITCH) {stroke(vg,x-3,3,x-3,h-3,nvgRGB(146,100,54),.6f);stroke(vg,x+2,3,x+2,h-3,nvgRGB(154,117,71),.7f);}
        else if(kind==PITCH_SLIDER) {
            rect(vg,x-half+1,3,half*2-2,2,nvgRGB(56,92,70),0);
            rect(vg,x-half+1,h-5,half*2-2,2,nvgRGB(56,92,70),0);
            for(float y:{h*.27f,h*.65f})stroke(vg,x-half+1,y,x+half-1,y,nvgRGB(56,48,31),.5f);
        }
        rect(vg,x-half+.1f,cy-handle/2,half*2-.2f,handle,nvgRGB(13,16,17),.6f);
        if(kind==PITCH_SLIDER) {
            nvgBeginPath(vg);nvgRect(vg,x-4.5f,cy-8,9,16);nvgFillPaint(vg,nvgLinearGradient(vg,x-4.5f,cy,x+4.5f,cy,nvgRGB(84,86,81),nvgRGB(214,215,208)));nvgFill(vg);
            rect(vg,x-3.1f,cy-6.1f,6.2f,12.2f,nvgRGB(31,24,17),.3f);
            for(float yy:{cy-7,cy-5.9f,cy+6,cy+7.1f})stroke(vg,x-4.1f,yy,x+4.1f,yy,nvgRGB(222,227,223),.5f);
        } else {
            rect(vg,x-5.6f,cy-handle/2+1,11.2f,handle-2,nvgRGB(61,66,71),.5f);
            for(float yy=cy-handle/2+2;yy<cy+handle/2-1;yy+=2.2f) {
                stroke(vg,x-5.5f,yy,x+5.5f,yy,nvgRGB(186,192,195),.7f);
                stroke(vg,x-5.5f,yy+.9f,x+5.5f,yy+.9f,nvgRGB(23,27,29),.7f);
            }
        }
    }
};

#include "HardwareDisplay.hpp"

struct HardwarePanel : widget::Widget {
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg;
        nvgBeginPath(vg); nvgRect(vg,0,0,HARDWARE_WIDTH,380);
        nvgFillPaint(vg,nvgLinearGradient(vg,0,0,510,370,nvgRGB(242,243,241),nvgRGB(221,223,222))); nvgFill(vg);
        // Fine horizontal grain, deterministic and independent of audio state.
        for(int y=0;y<760;++y) {
            int a=(y*37+11)%13;
            stroke(vg,0,y*.5f,510,y*.5f,nvgRGBA(y%2?255:67,y%2?255:70,y%2?255:72,a),.35f);
        }
        font(vg);
        auto bold=APP->window->loadFont(asset::plugin(pluginInstance,"res/Roboto-Bold.ttf"));
        if(bold) nvgFontFaceId(vg,bold->handle);
        auto label=[&](float x,float y,const std::string& s,float size=7.3f) { text(vg,x,y,s,size*1.2f,ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE); };
        label(255,10,"Metropolix",15.5f);
        for(float x:{22.f,488.f}) for(float y:{9.f,371.f}) rect(vg,x-9,y-3.8f,18,7.6f,nvgRGB(8,9,8),4);
        label(171,26,"TRK 1",8.2f); label(208,26,"TRK 2",8.2f);
        const float px[]={24,61,98,135.5f,171,208};
        const char* top[]={"A","B","CLK","CLK","PITCH","PITCH"};
        const char* bottom[]={"X","Y","Z","RESET","GATE","GATE"};
        for(int i=0;i<6;++i) {
            bool output=i!=3;
            if(output) rect(vg,px[i]-14,33,28,34,nvgRGB(39,41,42),.4f);
            if(i>=4) rect(vg,px[i]-14,84,28,34,nvgRGB(39,41,42),.4f);
            text(vg,px[i],62,top[i],9.8f,output?nvgRGB(244,244,239):ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
            text(vg,px[i],113,bottom[i],9.8f,i>=4?nvgRGB(244,244,239):ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
            metalDisc(vg,px[i],75,4.5f,nvgRGB(182,186,181),nvgRGB(233,234,229));
        }
        auto bracket=[&](float y,const std::string& title,float half) {
            stroke(vg,244,y,371-half,y,ink(),.8f); stroke(vg,371+half,y,494,y,ink(),.8f);
            stroke(vg,244,y,244,y+5,ink(),.8f); stroke(vg,494,y,494,y+5,ink(),.8f); label(371,y,title,8.4f);
        };
        bracket(30,"PITCH",18); bracket(150,"PULSE COUNT",35); bracket(259,"GATE TYPE",29);
        for(int s=0;s<8;++s) {
            float x=244+s*35.7f;
            label(x,136,std::to_string(s+1),7.7f);
            label(x,248,std::to_string(s+1),7.7f);
            if(s<7) for(int n=1;n<=8;++n) label(x+17.5f,229-(n-1)*8.8f,std::to_string(n),7.2f);
            float symbols=x+11.5f;
            for(int g=0;s<7 && g<4;++g) {
                float y=276+g*7.5f;
                if(g==0) rect(vg,symbols,y-1.2f,11.5f,2.5f,ink(),0);
                if(g==1) for(int p=0;p<3;++p) rect(vg,symbols+p*4,y-1.2f,2.7f,2.5f,ink(),0);
                if(g>=2) {
                    nvgBeginPath(vg); nvgRect(vg,symbols,y-1.2f,11.5f,2.5f); nvgStrokeColor(vg,ink()); nvgStrokeWidth(vg,.5f); nvgStroke(vg);
                    if(g==2) rect(vg,symbols,y-1.2f,3.5f,2.5f,ink(),0);
                }
            }
        }
        for(float x:{24.f,61.f,98.f}) {
            line(vg,x,121,x,147,ink()); label(x-8,140,"−",7); label(x+8,140,"+",7);
            for(float y=173;y<185;y+=2.3f) disc(vg,x,y,.45f,ink());
        }
        stroke(vg,135,232,120,245,ink(),.7f); stroke(vg,175.5f,184,175.5f,206,ink(),.7f); stroke(vg,68,228,43,244,ink(),.7f);
        for(float x=198;x<208;x+=2)disc(vg,x,229,.35f,ink());
        stroke(vg,145,259,145,263,ink(),.7f);stroke(vg,152,255,207,255,ink(),.8f);stroke(vg,176,255,176,263,ink(),.7f);stroke(vg,207,255,207,263,ink(),.7f);
        nvgBeginPath(vg);nvgRect(vg,140.5f,251,9,8);nvgStrokeColor(vg,ink());nvgStrokeWidth(vg,.45f);nvgStroke(vg);
        for(int i=0;i<3;++i)stroke(vg,142,253+i*2,148,253+i*2,ink(),.5f);
        label(31,278,"CTRL 1",8); label(103,278,"CTRL 2",8); label(213.5f,244,"EXIT",7.7f);
        label(145,287,"TRK 1",7.5f); label(176,287,"TRK 2",7.5f); label(207,287,"MOD",7.5f);
        const char* upper[]={"RUN","∞","RESET","SETUP","BPM","SCALE","ALT"};
        const char* lower[]={"ORDER","LEN","DIV","SWING","SLIDE","GATE","EDIT"};
        for(int i=0;i<7;++i) { float x=21+i*31.2f; label(x,321,upper[i],7.5f); label(x,355,lower[i],7.7f); }
        nvgBeginPath(vg);nvgRect(vg,200.8f,317,14.8f,8);nvgStrokeWidth(vg,.45f);nvgStrokeColor(vg,ink());nvgStroke(vg);
        const char* stage[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
        const char* alt[]={"LOAD","SAVE","A","B","CLK","MX","LIM","TYPE"};
        const float labelWidths[]={20,21,8,8,16,13,15,19};
        for(int i=0;i<8;++i) {
            float x=244+i*35.7f,w=labelWidths[i];label(x,355,stage[i],7.6f);
            if(i>=2 && i<=5)rect(vg,x-w/2,317,w,8,ink(),0);
            else {nvgBeginPath(vg);nvgRect(vg,x-w/2,317,w,8);nvgStrokeWidth(vg,.45f);nvgStrokeColor(vg,ink());nvgStroke(vg);}
            text(vg,x,321,alt[i],8.1f,i>=2&&i<=5?nvgRGB(243,243,238):ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
            if(i>=6){nvgBeginPath(vg);nvgMoveTo(vg,x,327);nvgLineTo(vg,x-1.8f,330);nvgLineTo(vg,x+1.8f,330);nvgClosePath(vg);nvgFillColor(vg,ink());nvgFill(vg);}
        }
        for(float x=215;x<237;x+=2)disc(vg,x,340,.4f,ink());
        stroke(vg,244,340,494,340,ink(),.65f);
        auto logo=APP->window->loadSvg(asset::plugin(pluginInstance,"res/sensiburner-intellijel.svg"));
        if(logo) {
            float scale=13.8f/logo->getSize().y;
            nvgSave(vg);nvgTranslate(vg,(HARDWARE_WIDTH-logo->getSize().x*scale)/2,363.5f);nvgScale(vg,scale,scale);
            logo->draw(vg);nvgRestore(vg);
        }
        for(float y:{138.f,324.f}) {
            metalDisc(vg,475,y,4.6f,nvgRGB(228,226,213),nvgRGB(93,91,85));
            nvgBeginPath(vg);
            for(int i=0;i<12;++i){float a=i*M_PI/6.f,r=i%2?1.6f:2.7f;float x=475+std::sin(a)*r,yy=y+std::cos(a)*r;if(i)nvgLineTo(vg,x,yy);else nvgMoveTo(vg,x,yy);}
            nvgClosePath(vg);nvgFillColor(vg,nvgRGB(65,62,55));nvgFill(vg);nvgStrokeColor(vg,nvgRGB(235,230,211));nvgStrokeWidth(vg,.55f);nvgStroke(vg);
        }
    }
};
