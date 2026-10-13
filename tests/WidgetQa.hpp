// Dedicated Rack widget tests; these exercise our actual event callbacks.
// Native operating-system mouse/focus behavior is a separate manual check.
void MetropolixWidget::qaStep() {
    const char* directory=std::getenv("METROPOLIX_QA_DIR");
    auto* m=getModule<Metropolix>(); if(!directory || !m)return;
    ++qaFrame; const std::string dir(directory);
    if(std::getenv("METROPOLIX_VIDEO_QA")) { videoStep(); return; }
    if(std::getenv("METROPOLIX_AUDIO_QA")) {
        if(std::getenv("METROPOLIX_LOOPY_AUDIO_QA")) {
            static bool momentary=false,returned=false;static unsigned range=0;
            if(qaFrame>40 && qaFrame<110)momentary |= m->playStage[1].load()==2;
            if(qaFrame>200 && qaFrame<350){int stage=m->playStage[1].load();if(stage>=0)range |= 1u<<stage;}
            if(qaFrame>400 && qaFrame<480){int stage=m->playStage[1].load();returned |= stage==0 || stage>4;}
            if(qaFrame==30){m->selectedTrack.store(1);m->params[Metropolix::LOOPY_TARGET].setValue(0);m->params[Metropolix::LOOPY_PULSES].setValue(0);m->pressLoopyStage(2);}
            if(qaFrame==120)m->releaseLoopyStage(2);
            if(qaFrame==180){m->params[Metropolix::LOOPY_PULSES].setValue(5);m->requestLoopy(18);m->latchLoopy();}
            if(qaFrame==360)m->clearLoopy();
            // Stages 2/3/4 use 1+2+2 pulses: the five-pulse loop never reaches stage 5.
            if(qaFrame==480){std::ofstream log(dir+"/report.txt",std::ios::app);log<<((momentary && returned && (range&14u)==14u && !(range&~14u))?"PASS ":"FAIL ")<<"LOOPY PERFORMANCE COMPLETE: momentary stage and latched range performed in real stereo patch: momentary="<<momentary<<", returned="<<returned<<", observed stages mask="<<range<<"\n";}
        }
        if(std::getenv("METROPOLIX_CHAIN_AUDIO_QA")) {
            if(qaFrame==2) {
                json_t* json=APP->engine->moduleToJson(m),*data=json_object_get(json,"data");
                for(int s=0;s<4;++s)json_array_set_new(json_array_get(json_object_get(data,"presets"),s),Metropolix::TOTAL_PULSES,json_integer((std::getenv("METROPOLIX_SWING_AUDIO_QA") || std::getenv("METROPOLIX_OVERRIDE_AUDIO_QA"))?8:4));
                json_t* links=json_object_get(json_object_get(data,"chains"),"working");
                for(size_t i=0;i<json_array_size(links);++i)json_object_set_new(json_array_get(links,i),"repeats",json_integer(1));
                APP->engine->moduleFromJson(m,json);json_decref(json);
            }
            static int previous=-1,transitions=0;static bool good=true;
            int link=m->chainPlayingLink.load();
            if(link>=0 && link!=previous) {
                if(previous>=0){good &= link==(previous+1)%7;++transitions;}
                previous=link;
                if(std::getenv("METROPOLIX_SWING_AUDIO_QA")) {
                    const int slots[]={0,1,0,2,3,1,3};const int amounts[]={50,62,66,75};
                    good &= m->integer(Metropolix::swingParam(1))==amounts[slots[link]];
                }
                if(std::getenv("METROPOLIX_OVERRIDE_AUDIO_QA")) {
                    const int slots[]={0,1,0,2,3,1,3},pitches[]={-1,15,22,12},gates[]={-1,88,0,50},scales[]={100,70,50,100};int slot=slots[link];
                    good &= m->integer(Metropolix::stageParam(1,2,Metropolix::PITCH_OVERRIDE))==pitches[slot] &&
                        m->integer(Metropolix::stageParam(1,2,Metropolix::GATE_OVERRIDE))==gates[slot] &&
                        m->integer(Metropolix::gateScaleParam(1))==scales[slot];
                }
                if(transitions==7) {std::ofstream report(dir+"/report.txt",std::ios::app);report<<(good?"PASS ":"FAIL ")<<"CHAIN PERFORMANCE COMPLETE: all seven links and loop observed during real audio playback\n";}
                if(transitions==7 && std::getenv("METROPOLIX_SWING_AUDIO_QA")){std::ofstream report(dir+"/report.txt",std::ios::app);report<<(good?"PASS ":"FAIL ")<<"SWING PERFORMANCE COMPLETE: straight and swung presets observed during real stereo playback\n";}
                if(transitions==7 && std::getenv("METROPOLIX_OVERRIDE_AUDIO_QA")){std::ofstream report(dir+"/report.txt",std::ios::app);report<<(good?"PASS ":"FAIL ")<<"OVERRIDE PERFORMANCE COMPLETE: pitch/gate overrides and Gate Scale observed on all seven links and loop\n";}
            }
        }
        if(std::getenv("METROPOLIX_PRESET_AUDIO_QA")) {
            if(qaFrame==2) {
                // Compress the eight-bar demo to one bar for this short test.
                json_t* json=APP->engine->moduleToJson(m);
                for(int s=0;s<4;++s)json_array_set_new(json_array_get(json_object_get(json_object_get(json,"data"),"presets"),s),Metropolix::TOTAL_PULSES,json_integer(16));
                APP->engine->moduleFromJson(m,json);json_decref(json);
                m->params[Metropolix::TOTAL_PULSES].setValue(16);
                m->requestPreset(1);
            }
            static int completed=0;
            int slot=m->lastLoadedSlot.load();
            if(completed==0 && slot==1){completed=1;m->requestPreset(2);}
            else if(completed==1 && slot==2){completed=2;m->requestPreset(3);}
            else if(completed==2 && slot==3){completed=3;m->requestPreset(0);}
            else if(completed==3 && slot==0) {
                completed=4;
                std::ofstream log(dir+"/report.txt",std::ios::app);
                log<<"PASS PRESET PERFORMANCE COMPLETE: 4 patch transitions reached their pulse boundaries\n";
            }
        }
        if(std::getenv("METROPOLIX_CTRL_QA")) {
            if(qaFrame==90)m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_VALUE)].setValue(2.f/7);
            if(qaFrame==180)m->params[Metropolix::ctrlParam(1,Metropolix::CTRL_VALUE)].setValue(3.f/7);
            if(qaFrame==270) {
                bool good=m->integer(Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION))==metro::CtrlPulseCount
                    && m->integer(Metropolix::ctrlParam(1,Metropolix::CTRL_DESTINATION))==metro::CtrlRatchet
                    && m->params[Metropolix::ctrlParam(0,Metropolix::CTRL_VALUE)].getValue()>0
                    && m->params[Metropolix::ctrlParam(1,Metropolix::CTRL_VALUE)].getValue()>0;
                std::ofstream log(dir+"/report.txt",std::ios::app);log<<(good?"PASS ":"FAIL ")<<"CTRL PERFORMANCE COMPLETE\n";
                for(int k=0;k<2;++k)m->params[Metropolix::ctrlParam(k,Metropolix::CTRL_VALUE)].setValue(0);
            }
        }
        if(std::getenv("METROPOLIX_AUX_QA")) {
            static float minimum[3]={100,100,100},maximum[3]={-100,-100,-100};
            for(int i=0;i<3;++i){float v=m->auxVoltage[i].load();minimum[i]=std::min(minimum[i],v);maximum[i]=std::max(maximum[i],v);}
            if(qaFrame==180) {
                bool good=minimum[0]==-.5f && maximum[0]==.5f && minimum[1]<-.3f && maximum[1]<=0 && minimum[2]==0 && maximum[2]==5;
                std::ofstream log(dir+"/report.txt",std::ios::app);
                for(int i=0;i<3;++i)log<<"AUX "<<char('X'+i)<<" effective range: "<<minimum[i]<<".."<<maximum[i]<<" V\n";
                log<<(good?"PASS ":"FAIL ")<<"AUX WIRING COMPLETE\n";
            }
        }
        if(qaFrame==180)APP->window->screenshot(dir+"/two-voice.png");
        return;
    }
    auto check=[&](bool ok,const std::string& s) {
        std::ofstream log(dir+"/report.txt",std::ios::app);
        log<<(ok?"PASS ":"FAIL ")<<s<<'\n'; qaFailed |= !ok;
    };
    auto find=[&](const std::string& name)->HardwareControl* {
        for(auto* w:children) { auto* c=dynamic_cast<HardwareControl*>(w); if(c && c->name==name)return c; }
        return nullptr;
    };
    auto press=[&](widget::Widget* w,int button) {
        if(!w){check(false,"requested control exists");return;}
        widget::EventContext ctx; event::Button e; e.context=&ctx;
        e.pos=w->box.size.div(2); e.button=button; e.action=GLFW_PRESS; e.mods=0;
        w->onButton(e); check(ctx.consumed && ctx.target==w,"control consumes button press");
    };
    auto drag=[&](widget::Widget* w,float dy) {
        if(!w){check(false,"requested draggable control exists");return;}
        press(w,GLFW_MOUSE_BUTTON_LEFT);
        widget::EventContext ctx;
        event::DragStart start; start.context=&ctx; start.button=GLFW_MOUSE_BUTTON_LEFT; w->onDragStart(start);
        if(dy!=0) { event::DragMove move; move.context=&ctx; move.button=GLFW_MOUSE_BUTTON_LEFT; move.mouseDelta=Vec(0,dy); w->onDragMove(move); }
        event::DragEnd end; end.context=&ctx; end.button=GLFW_MOUSE_BUTTON_LEFT; w->onDragEnd(end);
    };
    auto click=[&](widget::Widget* w) { drag(w,0); };
    std::function<widget::Widget*(widget::Widget*,std::function<bool(widget::Widget*)>)> seek;
    seek=[&](widget::Widget* root,std::function<bool(widget::Widget*)> match)->widget::Widget* {
        if(match(root))return root;
        for(auto* w:root->children)if(auto* hit=seek(w,match))return hit;
        return nullptr;
    };
    auto closeMenus=[] {
        for(auto* w:APP->scene->children)if(w->visible && dynamic_cast<ui::MenuOverlay*>(w))w->requestDelete();
    };
    if(std::getenv("METROPOLIX_POLISH_QA")) {
        #include "UiPolishQa.hpp"
        return;
    }
    if(std::getenv("METROPOLIX_CHAIN_UI_QA")) {
        #include "ChainWidgetQa.hpp"
        return;
    }
    if(std::getenv("METROPOLIX_SWING_UI_QA")) {
        #include "SwingWidgetQa.hpp"
        return;
    }
    if(std::getenv("METROPOLIX_LOOPY_UI_QA")) {
        #include "LoopyWidgetQa.hpp"
        return;
    }
    if(std::getenv("METROPOLIX_OVERRIDE_UI_QA")) {
        #include "OverrideWidgetQa.hpp"
        return;
    }
    if(std::getenv("METROPOLIX_DISPLAY_QA")) {
        #include "DisplayQa.hpp"
        return;
    }
    if(std::getenv("METROPOLIX_PRESET_UI_QA")) {
        #include "PresetWidgetQa.hpp"
        return;
    }
    if(qaFrame==6) {
        click(find("EDIT")); check(m->uiChooseFunction.load(),"EDIT arms function selection (manual p.85)");
        click(find("Stage 1 / SLIDE")); check(!m->uiChooseFunction.load() && m->selectedRow.load()==Metropolix::SLIDE,"EDIT plus SLIDE chooses the stage function");
        click(find("Stage 5 / RATCH")); check(m->integer(Metropolix::stageParam(0,4,Metropolix::SLIDE))==1,"stage press toggles slide after function selection (manual p.87)");
        drag(find("Stage 5 / RATCH"),400); check(m->integer(Metropolix::stageParam(0,4,Metropolix::SLIDE))==0,"held stage-button drag changes value without an extra click toggle");
        m->selectedRow.store(Metropolix::RATCHETS); m->uiEdit.store(false);
        click(find("EDIT")); click(find("BPM"));
        check(!m->uiChooseFunction.load() && screenParam(m)==Metropolix::TEMPO,"settings button leaves function chooser and restores encoder binding");
        click(find("EDIT")); click(find("TRK 1"));
        check(!m->uiChooseFunction.load(),"track selection leaves function chooser");
    }
    if(qaFrame==10) { m->expanded.store(false); m->uiPage.store(Metropolix::HOME); m->uiMod.store(false); }
    if(qaFrame==20) {
        check(box.size.x==510,"hardware panel is 34 HP");
        int counts[3]={0,0,0}; int ports=0;
        for(auto* w:children) { if(auto* f=dynamic_cast<HardwareSlider*>(w))counts[f->kind]++; if(dynamic_cast<MetalPort*>(w))ports++; }
        check(counts[0]==8 && counts[1]==8 && counts[2]==8,"eight interactive pitch, pulse and gate sliders");
        check(ports==12,"nine existing ports plus three patchable AUX inputs");
        for(int input=0;input<3;++input) {
            int found=0;for(auto* w:children)if(auto* p=dynamic_cast<MetalPort*>(w))if(p->type==engine::Port::INPUT && p->portId==Metropolix::AUX_X_IN+input)++found;
            check(found==1,"each AUX input has one real Rack jack");
        }
        APP->window->screenshot(dir+"/compact.png");
        auto* c=find("BPM"); click(c);
        check(m->uiPage.load()==Metropolix::BPM_PAGE,"BPM click selects display page");
        auto* q=m->getParamQuantity(Metropolix::TEMPO); float old=q->getValue();
        drag(c,-100); check(q->getValue()>old,"BPM hold + upward drag increases value");
        float higher=q->getValue(); drag(c,100); check(q->getValue()<higher,"BPM hold + downward drag decreases value");
        APP->history->undo(); check(q->getValue()==higher,"one undo restores complete button drag");
        APP->history->redo(); check(q->getValue()<higher,"redo restores button drag");
        setWithHistory(q,120);
        widget::EventContext ctx; event::Enter enter; enter.context=&ctx; c->onEnter(enter);
        check(c->tip && c->tooltipText().find("BPM")!=std::string::npos && c->tooltipText().find("up/down")!=std::string::npos,"hover text includes current value and drag help");
    }
    if(qaFrame==24) {
        APP->window->screenshot(dir+"/tooltip.png"); find("BPM")->clearTip();
        press(find("BPM"),GLFW_MOUSE_BUTTON_RIGHT);
    }
    if(qaFrame==26) {
        APP->window->screenshot(dir+"/numeric-menu.png");
        auto* overlay=seek(APP->scene,[](widget::Widget* w){return w->visible && dynamic_cast<ui::MenuOverlay*>(w)!=nullptr;});
        // The hidden module browser is also a MenuOverlay. Only search the
        // visible parameter menu for its populated value field.
        auto* field=overlay?dynamic_cast<ui::TextField*>(seek(overlay,[m](widget::Widget* w){auto* f=dynamic_cast<ui::TextField*>(w);return f && f->text==m->getParamQuantity(Metropolix::TEMPO)->getDisplayValueString();})):nullptr;
        check(field!=nullptr,"right-click numeric button opens Rack value entry");
        if(field) {
            field->text="123";
            widget::EventContext ctx; event::SelectKey e; e.context=&ctx; e.key=GLFW_KEY_ENTER; e.action=GLFW_PRESS; e.mods=0;
            field->onSelectKey(e);
            check(std::fabs(m->params[Metropolix::TEMPO].getValue()-123.f)<.001f,"numeric entry applies to the same parameter (actual="+std::to_string(m->params[Metropolix::TEMPO].getValue())+", field="+field->text+", consumed="+std::to_string(ctx.consumed)+")");
            APP->history->undo(); check(m->params[Metropolix::TEMPO].getValue()==120,"numeric entry supports undo");
        }
        closeMenus();
    }
    if(qaFrame==30) { press(find("ORDER"),GLFW_MOUSE_BUTTON_RIGHT); }
    if(qaFrame==32) {
        auto* item=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w) { auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Ping Pong"; }));
        check(item!=nullptr,"ORDER context menu lists named play orders");
        if(item) { event::Action e; item->onAction(e); check(m->integer(Metropolix::trackParam(0,Metropolix::ORDER))==1,"menu choice edits track order"); APP->history->undo(); }
        APP->window->screenshot(dir+"/context-menu.png"); closeMenus();
        m->selectedStage.store(4); m->selectedRow.store(Metropolix::RATCHETS); m->uiEdit.store(true); m->uiPage.store(Metropolix::STAGE_PAGE); m->expanded.store(true);
    }
    if(qaFrame==36) {
        auto* enc=find("Encoder"); ValueCell* grid=nullptr; ValueCell* slide=nullptr;
        for(auto* w:editor->children) { auto* c=dynamic_cast<ValueCell*>(w); if(c && c->stageColumn==4) { if(c->stageRow==Metropolix::RATCHETS)grid=c; if(c->stageRow==Metropolix::SLIDE)slide=c; } }
        check(enc && grid && slide,"hardware encoder and optional stage grid exist");
        if(enc && grid && slide) {
            check(enc->getParamQuantity()==grid->getParamQuantity(),"encoder and grid bind to the identical stage parameter");
            setWithHistory(grid->getParamQuantity(),4);
            drag(enc,-300); check(grid->getParamQuantity()->getValue()>4,"encoder upward drag increments a discrete stage value");
            APP->history->undo(); check(grid->getParamQuantity()->getValue()==4,"encoder drag supports undo");
            click(slide); check(slide->getParamQuantity()->getValue()==1,"grid short click toggles once");
            drag(slide,1500); check(slide->getParamQuantity()->getValue()==0,"grid boolean also accepts downward drag (actual="+std::to_string(slide->getParamQuantity()->getValue())+")");
            APP->history->undo(); check(slide->getParamQuantity()->getValue()==1,"grid drag supports undo");
        }
        check(box.size.x==990 && editor->visible,"optional editor opens at 66 HP total");
        for(auto* w:editor->children)if(auto* c=dynamic_cast<ValueCell*>(w))if(c->trackProperty==Metropolix::GATE_LENGTH) {
            int dest=Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION);
            widget::EventContext ctx;event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;
            event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;
            c->onDragStart(start);
            m->params[dest].setValue(metro::CtrlGateLength);
            c->onDragEnd(end);
            check(!c->dragStarted && !APP->window->isCursorLocked(),"CTRL assignment during grid drag releases cursor capture");
            c->onDragStart(start);check(!c->dragStarted,"owned grid value does not start a gesture");
            m->params[dest].setValue(metro::CtrlNone);c->onDragEnd(end);
            check(!c->dragStarted && !APP->window->isCursorLocked(),"clearing CTRL during blocked drag creates no unfinished gesture");
        }
        bool overlap=false; for(auto* w:parent->children)if(w!=this && dynamic_cast<ModuleWidget*>(w) && box.intersects(w->box))overlap=true;
        check(!overlap,"expanded editor does not overlap other modules");
        click(find("TRK 2"));
    }
    if(qaFrame==40) {
        check(m->selectedTrack.load()==1,"track selection works");
        check(find("Encoder")->paramId==Metropolix::stageParam(1,4,Metropolix::RATCHETS),"encoder rebinds to the selected track");
        check(m->params[Metropolix::stageParam(1,4,Metropolix::RATCHETS)].getValue()==1,"track 2 remains independent");
        click(find("TRK 1"));
        for(auto* w:children) { auto* f=dynamic_cast<HardwareSlider*>(w); if(!f || f->stage!=0)continue;
            auto* q=f->getParamQuantity(); float base=f->kind==HardwareSlider::PITCH_SLIDER?.5f:f->kind==HardwareSlider::PULSE_SWITCH?4.f:1.f;
            setWithHistory(q,base); drag(f,-400); check(q->getValue()>base,"slider upward drag raises pitch/pulse/gate value");
            float raised=q->getValue(); drag(f,400); check(q->getValue()<raised,"slider downward drag lowers pitch/pulse/gate value");
        }
    }
    if(qaFrame==45)APP->window->screenshot(dir+"/expanded.png");
    if(qaFrame==50) { click(find("MOD")); press(find("MOD"),GLFW_MOUSE_BUTTON_RIGHT); }
    if(qaFrame==52) {
        auto* item=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Open MOD editor";}));
        check(item!=nullptr,"MOD right-click exposes optional editor");
        if(item){event::Action e; item->onAction(e);} closeMenus(); m->selectedLane.store(3);
    }
    if(qaFrame==56) {
        check(modPanel->visible && !editor->visible,"MOD editor replaces stage grid");
        for(auto* w:modPanel->children) {
            auto* c=dynamic_cast<ValueCell*>(w); if(!c)continue;
            if(c->modProperty==Metropolix::MOD_VALUE+2) {
                check(c->paramId==Metropolix::modParam(3,Metropolix::MOD_VALUE+2),"MOD value binds to selected lane");
                setWithHistory(c->getParamQuantity(),-2.5f); APP->history->undo(); check(c->getParamQuantity()->getValue()==0,"MOD value undo"); APP->history->redo();
            }
            if(c->modProperty==Metropolix::MOD_RAMP+2) { click(c); check(c->getParamQuantity()->getValue()==1,"MOD transition toggles to ramp"); }
        }
        check(m->params[Metropolix::modParam(0,Metropolix::MOD_VALUE+2)].getValue()==0,"other MOD lanes remain independent");
        setWithHistory(m->getParamQuantity(Metropolix::OUTPUT_SOURCE_A),5);
        m->uiPage.store(Metropolix::STAGE_PAGE); m->uiField.store(1);
        json_t* data=APP->engine->moduleToJson(m); m->uiMod.store(false);m->selectedLane.store(0);m->uiPage.store(0);
        APP->engine->moduleFromJson(m,data);json_decref(data);
        check(m->uiMod.load() && m->selectedLane.load()==3 && m->uiField.load()==1 && m->uiPage.load()==Metropolix::STAGE_PAGE,"hardware screen and selection survive save/load");
        check(m->integer(Metropolix::OUTPUT_SOURCE_A)==5,"output assignment survives save/load");
        APP->patch->save(dir+"/roundtrip.vcv");
    }
    if(qaFrame==60)APP->window->screenshot(dir+"/mod-editor.png");
    if(qaFrame==65) { m->expanded.store(false); click(find("EXIT")); }
    if(qaFrame==70) {
        check(box.size.x==510 && !editor->visible && !modPanel->visible,"optional editors close to hardware width");
        check(m->uiPage.load()==Metropolix::HOME && !m->uiAlt.load(),"EXIT returns to home");
        auto* stub=find("Loopy"); check(stub && stub->tooltipText().find("momentary loop")!=std::string::npos,"Loopy control explains its performance gestures");
        auto* knob=find("AUX X attenuverter");auto* q=m->getParamQuantity(Metropolix::auxParam(0,Metropolix::AUX_AMOUNT));
        drag(knob,-100);check(q->getValue()>0,"AUX knob upward drag increases amount");
        APP->history->undo();check(q->getValue()==0,"AUX knob drag is one undo");
        drag(knob,100);check(q->getValue()<0,"AUX knob downward drag inverts amount");APP->history->undo();
        click(find("AUX X assignment"));check(m->selectedAux.load()==0 && m->uiPage.load()==Metropolix::AUX_PAGE,"AUX button opens correct screen");
        drag(find("AUX X assignment"),-100);check(m->integer(Metropolix::auxParam(0,Metropolix::AUX_DESTINATION))>0,"held AUX assignment button changes destination");APP->history->undo();
        click(find("Encoder"));check(screenParam(m)==Metropolix::auxParam(0,Metropolix::AUX_ASSIGNMENT),"AUX encoder click switches to target tracks");
        drag(find("Encoder"),-100);check(m->integer(Metropolix::auxParam(0,Metropolix::AUX_ASSIGNMENT))>0,"AUX encoder edits target tracks");APP->history->undo();
        check(knob->tooltipText().find("Effective input")!=std::string::npos,"AUX hover includes live voltage and route");
        press(find("AUX Y assignment"),GLFW_MOUSE_BUTTON_RIGHT);
    }
    if(qaFrame==74) {
        auto* item=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Destination";}));
        check(item!=nullptr,"AUX menu offers named destinations");
        if(item) {
            auto* submenu=item->createChildMenu();
            auto* mute=dynamic_cast<ui::MenuItem*>(seek(submenu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Mute";}));
            check(mute!=nullptr,"AUX destination submenu contains Mute");
            if(mute){event::Action e;mute->onAction(e);}
            delete submenu;
            check(m->integer(Metropolix::auxParam(1,Metropolix::AUX_DESTINATION))==metro::ModMute,"AUX menu changes Y destination");
            check(m->integer(Metropolix::auxParam(0,Metropolix::AUX_DESTINATION))==0,"AUX menu leaves X unchanged");
        }
        auto* clear=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Clear assignment";}));
        check(clear!=nullptr,"AUX clear assignment is available");
        if(clear){event::Action e;clear->onAction(e);check(m->integer(Metropolix::auxParam(1,Metropolix::AUX_DESTINATION))==0,"clear removes route");APP->history->undo();check(m->integer(Metropolix::auxParam(1,Metropolix::AUX_DESTINATION))==metro::ModMute,"clear assignment supports undo");}
        closeMenus();
        json_t* data=APP->engine->moduleToJson(m);m->selectedAux.store(0);m->uiPage.store(0);APP->engine->moduleFromJson(m,data);json_decref(data);
        check(m->selectedAux.load()==1 && m->uiPage.load()==Metropolix::AUX_PAGE,"AUX page and input survive patch reload");
        click(find("ALT"));int before=m->integer(Metropolix::auxParam(1,Metropolix::AUX_DESTINATION));
        drag(find("AUX Y assignment"),-100);check(m->integer(Metropolix::auxParam(1,Metropolix::AUX_DESTINATION))==before,"ALT AUX does not accidentally alter assignment");
        check(!qaFailed,"UI QA COMPLETE");APP->window->close();
    }
}
