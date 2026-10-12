// Included inside Metropolix.cpp's private UI namespace.
// Coordinates follow the supplied front-on panel photograph at 34 HP / 510 px.
constexpr float HARDWARE_WIDTH = 510.f;
constexpr float EDITOR_WIDTH = 480.f;
NVGcolor oledInk() { return nvgRGB(163, 213, 233); }

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
    metalDisc(vg,x,y,10,nvgRGB(250,248,231),nvgRGB(119,118,112));
    disc(vg,x,y,8.7f,nvgRGB(37,35,27));
    metalDisc(vg,x,y,7.8f,nvgRGB(207,186,140),nvgRGB(82,69,49));
    disc(vg,x,y,6.6f,nvgRGB(11,12,12));
    nvgBeginPath(vg); nvgArc(vg,x,y,7,-2.8f,-.5f,NVG_CW);
    nvgStrokeWidth(vg,.7f); nvgStrokeColor(vg,nvgRGB(253,248,231)); nvgStroke(vg);
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
        case Metropolix::BPM_PAGE: return Metropolix::TEMPO;
        case Metropolix::SCALE_PAGE: return f ? Metropolix::SCALE : Metropolix::ROOT;
        case Metropolix::ORDER_PAGE: return mod ? Metropolix::modParam(l,Metropolix::MOD_ORDER) : Metropolix::trackParam(t,Metropolix::ORDER);
        case Metropolix::LENGTH_PAGE: return mod ? Metropolix::modParam(l,f?Metropolix::MOD_OFFSET:Metropolix::MOD_LENGTH) : Metropolix::trackParam(t,f?Metropolix::OFFSET:Metropolix::LENGTH);
        case Metropolix::DIV_PAGE: return mod ? Metropolix::modParam(l,Metropolix::MOD_DIVISION) : (f?Metropolix::extraParam(t,Metropolix::PULSE_DIV):Metropolix::trackParam(t,Metropolix::DIVISION));
        case Metropolix::SLIDE_PAGE: return mod ? -1 : Metropolix::trackParam(t,Metropolix::SLIDE_TIME);
        case Metropolix::GATE_PAGE: return mod ? -1 : Metropolix::trackParam(t,Metropolix::GATE_LENGTH);
        case Metropolix::STAGE_PAGE: return mod ? Metropolix::modParam(l,(f?Metropolix::MOD_RAMP:Metropolix::MOD_VALUE)+m->selectedStage.load()) : Metropolix::stageParam(t,m->selectedStage.load(),m->selectedRow.load());
        case Metropolix::A_PAGE: return Metropolix::OUTPUT_SOURCE_A;
        case Metropolix::B_PAGE: return Metropolix::OUTPUT_SOURCE_B;
        case Metropolix::SETUP_PAGE: return Metropolix::RANGE;
        default: return -1;
    }
}
void screenPage(Metropolix* m, int page) {
    if(!m) return;
    m->uiPage.store(page); m->uiField.store(0); m->uiChooseFunction.store(false);
}
void encoderPress(Metropolix* m) {
    if(!m) return;
    int p=m->uiPage.load();
    if(p==Metropolix::HOME) { screenPage(m,Metropolix::BPM_PAGE); return; }
    if(p==Metropolix::SCALE_PAGE || p==Metropolix::LENGTH_PAGE ||
       (p==Metropolix::DIV_PAGE && !m->uiMod.load()) || (p==Metropolix::STAGE_PAGE && m->uiMod.load()))
        m->uiField.store(1-m->uiField.load());
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
struct HardwareControl : app::Knob {
    enum Look { BUTTON, DARK_BUTTON, WHITE_KNOB, BLACK_KNOB, ENCODER, INVISIBLE, SOCKET };
    Look look=BUTTON;
    Metropolix* metroModule=nullptr;
    std::string name, help;
    std::function<int()> binding;
    std::function<void()> action, rightAction, dragAction;
    std::function<void(ui::Menu*)> extraMenu;
    std::function<bool()> active;
    std::function<NVGcolor()> activeColor;
    NVGcolor lightColor=nvgRGB(135,187,227);
    ui::Tooltip* tip=nullptr;
    bool dragging=false;
    float moved=0;
    float customSpeed=0;
    HardwareControl() { smooth=false; forceLinear=true; speed=.55f; box.size=Vec(16,16); }
    ~HardwareControl() { clearTip(); }
    void bind() {
        if(dragging) return;
        int id=binding ? binding() : -1;
        module=(metroModule && id>=0) ? metroModule : nullptr;
        paramId=id>=0 ? id : 0;
        if(auto* q=getParamQuantity()) speed=customSpeed>0?customSpeed:(q->snapEnabled?std::max(.7f,30.f/q->getRange()):.7f);
    }
    std::string tooltipText() {
        auto* q=getParamQuantity();
        std::string s=q ? q->getString() : name;
        if(!help.empty()) s += "\n"+help;
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
        if(!settings::tooltips || tip) return;
        auto* tooltip=new HardwareTooltip; tooltip->owner=this; tip=tooltip;
        tip->text=tooltipText(); APP->scene->addChild(tip);
    }
    void onLeave(const event::Leave& e) override { clearTip(); app::Knob::onLeave(e); }
    void step() override {
        bind(); app::Knob::step();
        if(tip) { tip->text=tooltipText(); tip->box.pos=getAbsoluteOffset(box.size); }
    }
    void onHover(const event::Hover& e) override { app::ParamWidget::onHover(e); }
    void onButton(const event::Button& e) override {
        bind();
        if(e.button==GLFW_MOUSE_BUTTON_RIGHT && e.action==GLFW_PRESS) {
            clearTip();
            if(rightAction) rightAction();
            else if(getParamQuantity()) createContextMenu();
            else { auto* menu=createMenu(); menu->addChild(createMenuLabel(name)); menu->addChild(createMenuLabel(help)); }
            e.consume(this); return;
        }
        app::ParamWidget::onButton(e);
    }
    void onDragStart(const event::DragStart& e) override {
        bind(); dragging=true; moved=0; clearTip();
        if(dragAction) dragAction();
        app::Knob::onDragStart(e);
    }
    void onDragMove(const event::DragMove& e) override { moved+=e.mouseDelta.norm(); app::Knob::onDragMove(e); }
    void onDragEnd(const event::DragEnd& e) override { app::Knob::onDragEnd(e); dragging=false; }
    void onAction(const event::Action&) override { if(moved<3.f && action) action(); }
    void onDoubleClick(const event::DoubleClick& e) override { if(getParamQuantity()) app::Knob::onDoubleClick(e); }
    void appendContextMenu(ui::Menu* menu) override { if(extraMenu) extraMenu(menu); }
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg; float x=box.size.x/2, y=box.size.y/2, r=std::min(x,y)-.7f;
        if(look==INVISIBLE) return;
        if(look==SOCKET) { socketDrawing(vg,x,y); return; }
        bool on=active && active();
        disc(vg,x+.4f,y+.9f,r+.4f,nvgRGBA(0,0,0,65));
        metalDisc(vg,x,y,r,nvgRGB(102,104,106),nvgRGB(212,213,211));
        if(look==BUTTON || look==DARK_BUTTON) {
            NVGcolor top=look==DARK_BUTTON?nvgRGB(51,54,53):nvgRGB(249,249,248);
            NVGcolor bottom=look==DARK_BUTTON?nvgRGB(17,20,20):nvgRGB(198,202,209);
            if(on) { top=activeColor?activeColor():lightColor; bottom=color::mult(top,.77f); }
            metalDisc(vg,x,y,r-1,top,bottom);
            return;
        }
        metalDisc(vg,x,y,r-1,nvgRGB(77,78,76),nvgRGB(25,28,29));
        for(int i=0;i<28;++i) {
            float a=i*2*M_PI/28.f;
            line(vg,x+std::cos(a)*(r-3),y+std::sin(a)*(r-3),x+std::cos(a)*(r-1),y+std::sin(a)*(r-1),nvgRGBA(178,180,180,70));
        }
        bool white=look==WHITE_KNOB;
        metalDisc(vg,x,y,r-4,white?nvgRGB(251,251,250):nvgRGB(65,67,66),white?nvgRGB(192,196,196):nvgRGB(30,33,34));
        if(look!=ENCODER) {
            auto* q=getParamQuantity();
            float v=q ? q->getScaledValue() : .12f;
            float a=(-.83f + v*1.66f)*M_PI;
            nvgStrokeWidth(vg,2);
            line(vg,x+std::sin(a)*(r-8),y-std::cos(a)*(r-8),x+std::sin(a)*(r-3),y-std::cos(a)*(r-3),white?ink():nvgRGB(234,237,235));
        }
    }
};

struct HardwareSlider : HardwareControl {
    enum Kind { PITCH_SLIDER, PULSE_SWITCH, GATE_SWITCH };
    Kind kind=PITCH_SLIDER;
    int stage=0;
    HardwareSlider() { box.size=Vec(22,82); }
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg;
        float x=box.size.x/2, h=box.size.y;
        auto* q=getParamQuantity(); float v=q?q->getScaledValue():.35f;
        float handle=kind==PULSE_SWITCH?17.f:kind==GATE_SWITCH?12.f:11.f;
        float cy=handle/2+2+(1-v)*(h-handle-4);
        rect(vg,x-6.6f,0,13.2f,h,nvgRGB(122,117,99),.8f);
        rect(vg,x-5.5f,1,11,h-2,kind==PULSE_SWITCH?nvgRGB(75,53,28):nvgRGB(7,10,9),.5f);
        line(vg,x-4.5f,2,x-4.5f,h-2,nvgRGB(158,142,110));
        line(vg,x+4.5f,2,x+4.5f,h-2,nvgRGB(50,57,53));
        if(kind==PULSE_SWITCH) line(vg,x,2,x,h-2,nvgRGB(137,106,60));
        rect(vg,x-6,cy-handle/2,12,handle,nvgRGB(13,16,17),.6f);
        if(kind==PITCH_SLIDER) {
            rect(vg,x-3.5f,cy-5,7,10,nvgRGB(191,196,191),.4f);
            bool playing=metroModule && (metroModule->playStage[0].load()==stage || metroModule->playStage[1].load()==stage);
            rect(vg,x-1.6f,cy-4.1f,3.2f,8.2f,playing?nvgRGB(255,55,22):nvgRGB(165,27,11),.4f);
            line(vg,x-.6f,cy-3,x-.6f,cy+3,nvgRGB(255,164,106));
        } else {
            rect(vg,x-5.6f,cy-handle/2+1,11.2f,handle-2,nvgRGB(155,161,161),.5f);
            for(float yy=cy-handle/2+2;yy<cy+handle/2-1;yy+=2)
                line(vg,x-5.5f,yy,x+5.5f,yy,nvgRGB(45,50,52));
        }
    }
};

struct HardwareDisplay : HardwareControl {
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg;
        rect(vg,0,0,box.size.x,box.size.y,nvgRGB(39,43,43),1);
        rect(vg,1,1,box.size.x-2,box.size.y-2,nvgRGB(1,7,10),.5f);
        auto f=APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));
        if(f) nvgFontFaceId(vg,f->handle);
        nvgSave(vg); nvgScissor(vg,3,3,box.size.x-6,box.size.y-6);
        auto* m=metroModule;
        int page=m?m->uiPage.load():Metropolix::HOME;
        int t=m?m->selectedTrack.load():0, lane=m?m->selectedLane.load():0;
        bool mod=m && m->uiMod.load();
        std::string target=mod?string::f("MOD %d",lane+1):string::f("TRK %d",t+1);
        if(m && m->uiChooseFunction.load()) {
            text(vg,5,9,"EDIT FUNCTION",7,oledInk());
            text(vg,5,22,"SLD SKP PIT GAT",6.5f,oledInk());
            text(vg,5,31,"RAT PRB ACC CV",6.5f,oledInk());
            text(vg,5,42,"PRESS STAGE BUTTON",5.3f,oledInk());
        } else if(page==Metropolix::HOME) {
            line(vg,5,4,22,4,oledInk());
            for(int row=0;row<3;++row) {
                int pos=m?(row<2?m->playStage[row].load():m->modStage[lane].load()):-1;
                for(int s=0;s<8;++s) {
                    float x=6+s*10.1f, y=8+row*10;
                    nvgBeginPath(vg); nvgRect(vg,x,y,6,6);
                    nvgStrokeColor(vg,oledInk()); nvgStrokeWidth(vg,.8f); nvgStroke(vg);
                    if(pos==s) { nvgFillColor(vg,oledInk()); nvgFill(vg); }
                    if(m && row<2 && m->integer(Metropolix::stageParam(row,s,Metropolix::SKIP))) {
                        line(vg,x,y,x+6,y+6,oledInk()); line(vg,x+6,y,x,y+6,oledInk());
                    }
                }
            }
            text(vg,5,42,target+(mod?" - "+Metropolix::modDestinations()[m->integer(Metropolix::modParam(lane,Metropolix::MOD_DESTINATION))]:"  "+std::string(rowNames[m?m->selectedRow.load():0])),5.4f,oledInk());
        } else if(page==Metropolix::UNAVAILABLE_PAGE) {
            text(vg,5,11,"NOT IMPLEMENTED",7,oledInk());
            text(vg,5,24,"Hover for details",6,oledInk());
            text(vg,5,41,"EXIT: return home",5.5f,oledInk());
        } else {
            auto* q=getParamQuantity();
            const char* titles[]={"HOME","BPM","SCALE","ORDER","LENGTH","DIVISION","SLIDE TIME","GATE LENGTH","STAGE","OUT A","OUT B","PITCH RANGE"};
            std::string title=titles[page];
            if(page==Metropolix::STAGE_PAGE) title=mod?(m->uiField.load()?"STEP / RAMP":"MOD VOLTAGE"):rowNames[m->selectedRow.load()];
            if(page==Metropolix::SCALE_PAGE) title=m->uiField.load()?"SCALE":"ROOT";
            if(page==Metropolix::LENGTH_PAGE) title=m->uiField.load()?"OFFSET":"STAGES";
            if(page==Metropolix::DIV_PAGE && !mod && m->uiField.load()) title="PULSE COUNT DIV";
            text(vg,5,9,title,6.3f,oledInk());
            std::string v=q?q->getDisplayValueString()+q->getUnit():"N/A";
            float size=v.size()>17?6.f:v.size()>10?8.f:12.f;
            text(vg,box.size.x/2,25,v,size,oledInk(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
            text(vg,5,42,page==Metropolix::STAGE_PAGE?target+string::f(" / STAGE %d",m->selectedStage.load()+1):target+" / TURN TO EDIT",5.5f,oledInk());
        }
        nvgRestore(vg);
    }
};

struct HardwarePanel : widget::Widget {
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg;
        nvgBeginPath(vg); nvgRect(vg,0,0,HARDWARE_WIDTH,380);
        nvgFillPaint(vg,nvgLinearGradient(vg,0,0,510,370,nvgRGB(239,240,237),nvgRGB(193,196,197))); nvgFill(vg);
        // Fine horizontal grain, deterministic and independent of audio state.
        for(int y=0;y<760;++y) {
            int a=(y*37+11)%17;
            line(vg,0,y*.5f,510,y*.5f,nvgRGBA(y%2?255:67,y%2?255:70,y%2?255:72,a));
        }
        font(vg);
        auto bold=APP->window->loadFont(asset::system("res/fonts/Nunito-Bold.ttf"));
        if(bold) nvgFontFaceId(vg,bold->handle);
        auto label=[&](float x,float y,const std::string& s,float size=7.3f) { text(vg,x,y,s,size,ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE); };
        label(255,10,"Metropolix",14);
        for(float x:{22.f,488.f}) for(float y:{9.f,371.f}) rect(vg,x-9,y-3.8f,18,7.6f,nvgRGB(8,9,8),4);
        label(171,26,"TRK 1",8); label(208,26,"TRK 2",8);
        const float px[]={24,61,98,135.5f,171,208};
        const char* top[]={"A","B","CLK","CLK","PITCH","PITCH"};
        const char* bottom[]={"X","Y","Z","RESET","GATE","GATE"};
        for(int i=0;i<6;++i) {
            bool output=i!=3;
            if(output) rect(vg,px[i]-14,33,28,34,nvgRGB(39,41,42),.4f);
            if(i>=4) rect(vg,px[i]-14,84,28,34,nvgRGB(39,41,42),.4f);
            text(vg,px[i],62,top[i],7.4f,output?nvgRGB(244,244,239):ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
            text(vg,px[i],113,bottom[i],7.4f,i>=4?nvgRGB(244,244,239):ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
            metalDisc(vg,px[i],75,4.5f,nvgRGB(182,186,181),nvgRGB(233,234,229));
        }
        auto bracket=[&](float y,const std::string& title,float half) {
            line(vg,244,y,371-half,y,ink()); line(vg,371+half,y,494,y,ink());
            line(vg,244,y,244,y+5,ink()); line(vg,494,y,494,y+5,ink()); label(371,y,title,8);
        };
        bracket(30,"PITCH",18); bracket(150,"PULSE COUNT",35); bracket(259,"GATE TYPE",29);
        for(int s=0;s<8;++s) {
            float x=244+s*35.7f;
            label(x,136,std::to_string(s+1),7);
            label(x,248,std::to_string(s+1),7);
            if(s<7) for(int n=1;n<=8;++n) label(x+17.5f,229-(n-1)*8.8f,std::to_string(n),6.6f);
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
        line(vg,135,228,166,211,ink()); line(vg,175.5f,184,175.5f,206,ink()); line(vg,68,228,43,244,ink());
        line(vg,145,259,145,263,ink()); line(vg,145,255,207,255,ink()); line(vg,176,255,176,263,ink()); line(vg,207,255,207,263,ink());
        label(31,278,"CTRL 1",7.5f); label(103,278,"CTRL 2",7.5f); label(213.5f,244,"EXIT",7.2f);
        label(145,287,"TRK 1",6.8f); label(176,287,"TRK 2",6.8f); label(207,287,"MOD",6.8f);
        const char* upper[]={"RUN","∞","RESET","SETUP","BPM","SCALE","ALT"};
        const char* lower[]={"ORDER","LEN","DIV","SWING","SLIDE","GATE","EDIT"};
        for(int i=0;i<7;++i) { float x=21+i*31.2f; label(x,321,upper[i],7); label(x,355,lower[i],7); }
        const char* stage[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
        const char* alt[]={"LOAD","SAVE","A","B","CLK","MX","LIM","TYPE"};
        for(int i=0;i<8;++i) { float x=244+i*35.7f; label(x,355,stage[i],6.8f); label(x,321,alt[i],6.2f); }
        line(vg,214,340,236,340,ink());
        label(255,370,"intellijel",14);
        for(float y:{138.f,324.f}) {
            metalDisc(vg,475,y,4.6f,nvgRGB(228,226,213),nvgRGB(93,91,85));
            line(vg,472.8f,y-2,477.2f,y+2,nvgRGB(83,82,77)); line(vg,473,y+2.2f,477,y-2.2f,nvgRGB(83,82,77));
        }
    }
};
