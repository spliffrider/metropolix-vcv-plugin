// Desktop-only controls. The existing Loopy command and mute mailbox remain
// the only performance inputs to DSP; keyboard ownership never enters a patch.
struct PerformancePanel : widget::OpaqueWidget {
    Metropolix* m=nullptr;
    std::set<int> keys;
    std::set<int> commands;
    bool armed=false,ownsInput=false,ownsLatch=false,lastRun=false;
    int mouseFirst=-1,mouseLast=-1;
    uint64_t restoreSerial=0;
    ui::Tooltip* tip=nullptr;
    int hovered=-1;double hoverSince=0;

    explicit PerformancePanel(Metropolix* module):m(module) {
        box.size=Vec(EDITOR_WIDTH,380);
        if(m){restoreSerial=m->uiRestoreSerial.load();lastRun=m->integer(Metropolix::RUN);}
        auto* pulses=createParam<ValueCell>(Vec(341,177),m,Metropolix::LOOPY_PULSES);
        pulses->metroModule=m;pulses->box.size=Vec(119,23);pulses->dark=true;
        pulses->shortChoices.push_back("Auto");for(int i=1;i<=64;++i)pulses->shortChoices.push_back(std::to_string(i));addChild(pulses);
    }
    ~PerformancePanel() {clearTip();}
    static int keyStage(int key) {
        if(key>=GLFW_KEY_1 && key<=GLFW_KEY_8)return key-GLFW_KEY_1;
        if(key>=GLFW_KEY_KP_1 && key<=GLFW_KEY_KP_8)return key-GLFW_KEY_KP_1;
        return -1;
    }
    static Rect region(int id) {
        if(id>=20 && id<28)return Rect(Vec(20+(id-20)*55,216),Vec(52,48));
        if(id>=30 && id<38)return Rect(Vec(20+(id-30)*55,312),Vec(48,23));
        switch(id){
            case 0:return Rect(Vec(20,45),Vec(200,25));
            case 1:return Rect(Vec(230,45),Vec(70,25));
            case 2:return Rect(Vec(310,45),Vec(70,25));
            case 3:return Rect(Vec(390,45),Vec(70,25));
            case 4:return Rect(Vec(20,84),Vec(100,23));
            case 5:return Rect(Vec(128,84),Vec(95,23));
            case 6:return Rect(Vec(250,84),Vec(100,23));
            case 7:return Rect(Vec(358,84),Vec(102,23));
            case 8:return Rect(Vec(20,177),Vec(146,23));
            case 9:return Rect(Vec(174,177),Vec(100,23));
            case 10:return Rect(Vec(20,348),Vec(140,23));
            case 11:return Rect(Vec(292,7),Vec(52,21));
            case 12:return Rect(Vec(350,7),Vec(52,21));
            case 13:return Rect(Vec(408,7),Vec(52,21));
            default:return Rect(Vec(-100,-100),Vec(0,0));
        }
    }
    int hit(Vec pos) const {
        for(int i=0;i<38;++i)if((i<14 || (i>=20 && i<28) || i>=30) && region(i).contains(pos))return i;
        return -1;
    }
    static int stageAt(Vec pos) {return metro::clamp(int(std::floor((pos.x-20)/55)),0,7);}
    unsigned keyboardMask() const {
        unsigned mask=0;for(int key:keys){int stage=keyStage(key);if(stage>=0)mask|=1u<<stage;}return mask;
    }
    unsigned heldMask() const {
        unsigned mask=keyboardMask();if(mouseFirst>=0)mask|=(1u<<mouseFirst)|(1u<<mouseLast);return mask;
    }
    void publish(bool releasing=false) {
        if(!m)return;
        unsigned mask=heldMask();
        if(mask){openLoopy(m);m->loopyHeld.store(mask);if(!releasing || !m->loopyLatched.load())m->requestLoopy(mask);ownsInput=true;}
        else if(ownsInput){m->loopyHeld.store(0);if(!m->loopyLatched.load())m->requestLoopy(0);ownsInput=false;}
    }
    void releaseHeld() {keys.clear();mouseFirst=mouseLast=-1;publish(true);}
    void releaseLoop() {
        keys.clear();mouseFirst=mouseLast=-1;ownsInput=ownsLatch=false;if(m)m->clearLoopy();
    }
    void disarm() {armed=false;commands.clear();releaseHeld();}
    void latch() {
        if(!m || !m->integer(Metropolix::RUN))return;
        if(m->loopyLatched.load())releaseLoop();
        else if(m->loopyCommand.load()&255u){m->latchLoopy();ownsLatch=true;}
    }
    void chooseTrack(int track) {
        releaseHeld();m->selectedTrack.store(track);m->uiMod.store(false);
    }
    void run() {
        if(!m)return;
        if(m->integer(Metropolix::RUN))releaseLoop();
        setWithHistory(m->getParamQuantity(Metropolix::RUN),1-m->integer(Metropolix::RUN));
    }
    void action(int id) {
        if(!m)return;
        switch(id){
            case 0:if(armed)disarm();else armed=true;break;
            case 1:run();break;
            case 2:releaseLoop();m->resetFromUi.store(true);break;
            case 3:releaseLoop();break;
            case 4:chooseTrack(0);break;
            case 5:m->muteLoopy(0);break;
            case 6:chooseTrack(1);break;
            case 7:m->muteLoopy(1);break;
            case 8:releaseHeld();setWithHistory(m->getParamQuantity(Metropolix::LOOPY_TARGET),1-m->integer(Metropolix::LOOPY_TARGET));break;
            case 9:latch();break;
            case 10:m->muteLoopy(10);break;
            case 11:disarm();m->openDesktopView(0);break;
            case 12:disarm();m->openDesktopView(1);break;
            case 13:disarm();m->expanded.store(false);break;
            default:if(id>=30 && id<38)m->muteLoopy(id-28);break;
        }
    }
    void onButton(const event::Button& e) override {
        widget::Widget::onButton(e);if(e.isConsumed())return;
        e.stopPropagating();
        if(e.button!=GLFW_MOUSE_BUTTON_LEFT)return;
        e.consume(this);
        if(!m || e.action!=GLFW_PRESS)return;
        int id=hit(e.pos);
        if(id>=20 && id<28){mouseFirst=mouseLast=id-20;publish();}
        else action(id);
    }
    void onDragHover(const event::DragHover& e) override {
        if(e.origin==this && mouseFirst>=0 && e.button==GLFW_MOUSE_BUTTON_LEFT){
            int stage=stageAt(e.pos);if(stage!=mouseLast){mouseLast=stage;publish();}e.consume(this);return;
        }
        widget::OpaqueWidget::onDragHover(e);
    }
    void onDragEnd(const event::DragEnd& e) override {
        if(e.button!=GLFW_MOUSE_BUTTON_LEFT || mouseFirst<0)return;
        if(m && mouseFirst!=mouseLast && m->integer(Metropolix::RUN)){m->latchLoopy();ownsLatch=true;}
        mouseFirst=mouseLast=-1;publish(true);
    }
    void onSelectKey(const event::SelectKey& e) override {
        if(!m)return;
        // Releases must work even if a modifier was pressed after the stage key.
        if(armed && e.action==GLFW_RELEASE && keys.erase(e.key)){publish(true);e.consume(this);return;}
        if(e.action==GLFW_RELEASE){if(armed && commands.erase(e.key))e.consume(this);return;}
        // Rack can open a blocking native file dialog before another step().
        // Release first, then pass modified commands through untouched.
        if(e.action==GLFW_PRESS && (e.mods&RACK_MOD_MASK)!=0){disarm();return;}
        if(!armed)return;
        if((e.mods&RACK_MOD_MASK)!=0)return;
        int stage=keyStage(e.key);
        if(stage>=0){if(e.action==GLFW_PRESS && keys.insert(e.key).second)publish();if(keys.count(e.key))e.consume(this);return;}
        int command=-1;
        switch(e.key){
            case GLFW_KEY_SPACE:command=1;break;
            case GLFW_KEY_L:command=9;break;
            case GLFW_KEY_T:command=4+2*(1-m->selectedTrack.load());break;
            case GLFW_KEY_M:command=5+2*m->selectedTrack.load();break;
            case GLFW_KEY_G:command=10;break;
            case GLFW_KEY_ESCAPE:releaseLoop();disarm();e.consume(this);return;
            default:return;
        }
        if(e.action==GLFW_PRESS){commands.insert(e.key);action(command);e.consume(this);}
        else if(commands.count(e.key))e.consume(this);
    }
    void onDeselect(const event::Deselect&) override {disarm();}
    void onHide(const event::Hide&) override {disarm();clearTip();}
    void onRemove(const event::Remove& e) override {disarm();clearTip();widget::OpaqueWidget::onRemove(e);}
    void clearTip(){if(tip){if(tip->parent)tip->parent->removeChild(tip);delete tip;tip=nullptr;}}
    std::string help(int id) const {
        if(id>=20 && id<28)return "Hold to loop or audition. While running, drag to a second stage and release to latch that range. RELEASE returns to the full sequence.";
        if(id>=30 && id<38)return "Mute/unmute MOD "+std::to_string(id-29)+". Changes wait for the next clock pulse while running; the MOD group mute also applies.";
        switch(id){
            case 0:return "Click to arm keys 1-8 (number row or numpad). T selects the other track, M mutes it, G mutes MOD, Space runs/stops, L latches. Esc releases and disarms. Clicking elsewhere disarms.";
            case 1:return "Start/stop playback. Stopping here releases the performance loop and held audition.";
            case 2:return "Reset position and accumulators; release the performance loop.";
            case 3:return "Release the current loop or audition and return to the full sequence.";
            case 4:case 6:return "Select this track for stage audition and Current Track loops. T switches tracks when keyboard control is armed.";
            case 5:case 7:return "Mute/unmute this track at its next base-clock pulse. A dot means the change is pending. Sequencer/chain clock keeps running.";
            case 8:return "Switch loop target between the selected track and both tracks. Releases held stages first.";
            case 9:return "While running, hold 1-8 and click LATCH (or press L). Click again to release. A mouse drag across stages latches automatically.";
            case 10:return "Mute/unmute the MOD group while preserving individual lane choices. Changes apply on a clock pulse while running.";
            case 11:return "Open the stage value editor. Shift+P over this module returns to Performance.";
            case 12:return "Open the MOD lane editor. Shift+P over this module returns to Performance.";
            case 13:return "Close the optional view. Temporary holds release; an intentional latched loop keeps playing.";
            default:return "";
        }
    }
    void onHover(const event::Hover& e) override {
        int id=hit(e.pos);if(id!=hovered){hovered=id;hoverSince=system::getTime();clearTip();}
        widget::OpaqueWidget::onHover(e);
    }
    void onLeave(const event::Leave&) override {hovered=-1;clearTip();}
    void step() override {
        if(m){
            bool running=m->integer(Metropolix::RUN);
            if(restoreSerial!=m->uiRestoreSerial.load()){
                disarm();ownsLatch=false;restoreSerial=m->uiRestoreSerial.load();
            }
            if(lastRun && !running && (ownsInput || ownsLatch))releaseLoop();
            lastRun=running;
            bool focused=APP->window && APP->window->win && glfwGetWindowAttrib(APP->window->win,GLFW_FOCUSED);
            if(!visible || !focused || (armed && APP->event->getSelectedWidget()!=this))disarm();
            // Rack clears heldKeys when its window loses the pointer/focus.
            // Reconcile lost release events without retriggering on key repeat.
            if(armed){bool changed=false;for(auto it=keys.begin();it!=keys.end();)if(!APP->event->heldKeys.count(*it)){it=keys.erase(it);changed=true;}else ++it;if(changed)publish(true);}
        }
        if(!visible || !settings::tooltips || hovered<0)clearTip();
        else if(!tip && system::getTime()-hoverSince>.5){std::string message=help(hovered);if(!message.empty()){tip=new ui::Tooltip;tip->text=message;APP->scene->addChild(tip);}}
        widget::OpaqueWidget::step();
    }
    void drawButton(NVGcontext* vg,int id,const std::string& label,bool active=false,NVGcolor color=teal()) {
        Rect r=region(id);rect(vg,r.pos.x,r.pos.y,r.size.x,r.size.y,active?color:ink(),3);
        if(id==hovered){nvgBeginPath(vg);nvgRoundedRect(vg,r.pos.x+.5f,r.pos.y+.5f,r.size.x-1,r.size.y-1,3);nvgStrokeColor(vg,nvgRGB(155,182,187));nvgStrokeWidth(vg,1);nvgStroke(vg);}
        text(vg,r.pos.x+r.size.x/2,r.pos.y+r.size.y/2,label,10,paper(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
    }
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg;font(vg);
        rect(vg,0,0,480,380,nvgRGB(30,39,44),0);rect(vg,0,0,480,34,nvgRGB(18,27,32),0);
        text(vg,20,18,"PERFORMANCE",14,nvgRGB(211,228,230));
        drawButton(vg,11,"EDIT");drawButton(vg,12,"MOD");drawButton(vg,13,"CLOSE");
        drawButton(vg,0,armed?"KEYBOARD ARMED - ESC TO RELEASE":"CLICK TO ARM KEYBOARD",armed);
        drawButton(vg,1,m && m->integer(Metropolix::RUN)?"STOP":"RUN",m && m->integer(Metropolix::RUN));
        drawButton(vg,2,"RESET");drawButton(vg,3,"RELEASE");
        if(m){
            unsigned pending=m->loopyMuteCommands.load();
            for(int t=0;t<2;++t){
                float x=t?250:20;bool mutedTrack=m->integer(Metropolix::trackParam(t,Metropolix::MUTE));
                drawButton(vg,4+2*t,"TRACK "+std::to_string(t+1),m->selectedTrack.load()==t,trackColor(t));
                drawButton(vg,5+2*t,std::string(pending&(1u<<t)?"* ":"")+(mutedTrack?"UNMUTE":"MUTE"),mutedTrack,nvgRGB(166,63,66));
                text(vg,x,124,string::f("PITCH %+.3f V",m->playPitch[t].load()),11,paper());
                text(vg,x,140,string::f("PULSE %d    ACCUM %+d deg",m->playPulse[t].load()+1,m->playAccumulation[t].load()),10,nvgRGB(162,187,190));
                text(vg,x+207,124,m->playMuted[t].load()?"MUTED":m->playGate[t].load()>0?"GATE":"--",10,m->playMuted[t].load()?nvgRGB(246,127,127):nvgRGB(121,214,187),NVG_ALIGN_RIGHT|NVG_ALIGN_MIDDLE);
                for(int s=0;s<8;++s)rect(vg,x+s*27.2f,153,24,7,m->playStage[t].load()==s?trackColor(t):nvgRGB(59,76,82),1);
            }
            drawButton(vg,8,m->integer(Metropolix::LOOPY_TARGET)?"TARGET: BOTH":"TARGET: TRACK "+std::to_string(m->selectedTrack.load()+1));
            drawButton(vg,9,m->loopyLatched.load()?"UNLATCH":"LATCH",m->loopyLatched.load(),orange());
            text(vg,285,189,"PULSES",9,nvgRGB(162,187,190));
            text(vg,20,207,"HOLD TO PLAY",9,nvgRGB(162,187,190));text(vg,460,207,"DRAG A RANGE TO LATCH",9,nvgRGB(162,187,190),NVG_ALIGN_RIGHT|NVG_ALIGN_MIDDLE);
            unsigned mask=m->loopyCommand.load()&255u;
            int first=0,last=7;while(first<8 && !(mask&(1u<<first)))++first;while(last>=0 && !(mask&(1u<<last)))--last;
            for(int s=0;s<8;++s){
                Rect r=region(20+s);bool in=mask && s>=first && s<=last;
                rect(vg,r.pos.x,r.pos.y,r.size.x,r.size.y,in?orange():ink(),4);
                text(vg,r.pos.x+26,235,std::to_string(s+1),19,paper(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
                bool sounding=false;for(int t=0;t<2;++t)if((m->integer(Metropolix::LOOPY_TARGET) || m->selectedTrack.load()==t) && m->playStage[t].load()==s)sounding=true;
                rect(vg,r.pos.x+6,253,40,3,sounding?teal():nvgRGB(71,86,90),1);
            }
            std::string status=mask?string::f("%s %d%s",m->integer(Metropolix::RUN)?"LOOP":"AUDITION",first+1,last!=first?string::f(" - %d",last+1).c_str():""):"FULL SEQUENCE";
            if(mask && m->integer(Metropolix::RUN))status+=m->loopyLatched.load()?"  /  LATCHED":"  /  HELD";
            text(vg,20,279,status,11,mask?nvgRGB(250,188,119):paper());
            text(vg,460,279,pending?"* MUTE CHANGE QUEUED":m->uiDraftParam>=0?"EDIT PENDING ON OLED":"",9,nvgRGB(250,188,119),NVG_ALIGN_RIGHT|NVG_ALIGN_MIDDLE);
            text(vg,20,301,"MOD LANES - CLICK TO MUTE",9,nvgRGB(162,187,190));
            for(int lane=0;lane<8;++lane){bool mutedLane=m->integer(Metropolix::modParam(lane,Metropolix::MOD_MUTE));drawButton(vg,30+lane,std::string(pending&(1u<<(lane+2))?"*":"")+std::to_string(lane+1)+(mutedLane?" OFF":" ON"),mutedLane,nvgRGB(166,63,66));}
            bool group=m->integer(Metropolix::MOD_GROUP_MUTE);drawButton(vg,10,std::string(pending&(1u<<10)?"* ":"")+(group?"ALL MOD: MUTED":"ALL MOD: ON"),group,nvgRGB(166,63,66));
        }
        text(vg,175,354,"1-8 stages | SPACE run | L latch | T track",9,nvgRGB(162,187,190));
        text(vg,175,369,"M mute | G MOD | ESC release + disarm",9,nvgRGB(162,187,190));
        widget::OpaqueWidget::draw(args);
    }
};
