// Actual Rack widget gestures and front-buffer proofs for the feedback fixes.
auto slider=[&](int kind)->HardwareSlider* {
    for(auto* w:children)if(auto* s=dynamic_cast<HardwareSlider*>(w))if(s->stage==0 && s->kind==kind)return s;
    return nullptr;
};
widget::EventContext ctx;
event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragMove move;move.context=&ctx;move.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;
event::Leave leave;leave.context=&ctx;
if(qaFrame==5) {
    m->params[Metropolix::RUN].setValue(0);
    m->expanded.store(false);m->uiMod.store(false);m->selectedTrack.store(0);
    for(int k=0;k<2;++k)m->params[Metropolix::ctrlParam(k,Metropolix::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    settings::knobMode=settings::KNOB_MODE_LINEAR;settings::knobLinearSensitivity=.001f;
    settings::tooltips=true;settings::knobScroll=true;
    for(int kind=0;kind<3;++kind) {
        auto* s=slider(kind);s->bind();auto* q=s->getParamQuantity();
        float base=kind==0?.5f:kind==1?4.f:1.f;q->setValue(base);
        int history=APP->history->actionIndex;
        s->onDragStart(start);move.mouseDelta=Vec(0,-10);s->onDragMove(move);
        check(kind==0?std::fabs(q->getValue()-.525f)<.00001f:q->getValue()==base,"10 px drag is precise without skipping switch positions");
        check(s->adjustmentTip && s->adjustmentTip->text==q->getString() && !s->tip,"drag shows a single current parameter readout");
        s->onLeave(leave);
        check(s->adjustmentTip && APP->history->actionIndex==history,"leaving while held preserves readout, snap remainder and one gesture");
        move.mouseDelta=Vec(0,-14);s->onDragMove(move);
        check(kind==0?std::fabs(q->getValue()-.56f)<.00001f:q->getValue()==base+1,"24 px drag advances exactly one pulse/gate step");
        float raised=q->getValue();s->onDragEnd(end);
        check(!s->adjustmentTip && !APP->window->isCursorLocked() && APP->history->actionIndex==history+1,"release clears popup/capture and records one undo");
        APP->history->undo();check(q->getValue()==base,"slider gesture undo restores original value");
        APP->history->redo();check(q->getValue()==raised,"slider gesture redo restores new value");
        drag(s,24);check(std::fabs(q->getValue()-base)<.00001f,"same downward drag reverses the change");
    }
    // The shared behavior must also cover buttons, encoder, screen and knobs.
    click(find("BPM"));
    for(const char* name:{"BPM","Encoder","Metropolix display","CTRL 1","AUX X attenuverter"}) {
        auto* c=find(name);c->onDragStart(start);move.mouseDelta=Vec(0,-30);c->onDragMove(move);
        check(c->adjustmentTip && c->adjustmentTip->text==c->getParamQuantity()->getString(),std::string(name)+" has live drag readout");
        c->onDragEnd(end);APP->history->undo();
    }
    settings::tooltips=false;slider(0)->onDragStart(start);
    check(!slider(0)->adjustmentTip,"Rack's tooltips-off preference suppresses drag readout");
    slider(0)->onDragEnd(end);settings::tooltips=true;
    m->expanded.store(true);
    for(char ch:std::string("gjpqy")) {
        OledCanvas c;c.text(0,0,std::string(1,ch),false);
        int tail=0;for(int y=7;y<9;++y)for(int x=0;x<5;++x)tail+=c.pixels[y][x]>0;
        check(tail>=2,std::string("lowercase ")+ch+" has a complete below-baseline descender");
    }
    OledCanvas g,q;g.text(0,0,"g",false);q.text(0,0,"q",false);
    check(std::memcmp(g.pixels,q.pixels,sizeof(g.pixels))!=0,"g and q are distinct glyphs");
    OledCanvas proof;proof.text(0,1,"Stage / Step / Ping",false);proof.text(0,17,"gjpqy",false,2);
    proof.text(0,41,"Jump / Assigns",false);proof.text(0,57,"DESCENDER CHECK");
    std::ofstream pgm(dir+"/lettering.pgm",std::ios::binary);pgm<<"P5\n128 64\n255\n";pgm.write(reinterpret_cast<char*>(proof.pixels),128*64);
}
if(qaFrame==9) {
    ValueCell* cell=nullptr;
    for(auto* w:editor->children)if(auto* c=dynamic_cast<ValueCell*>(w))if(c->stageColumn==0 && c->stageRow==Metropolix::RATCHETS)cell=c;
    check(cell!=nullptr,"optional grid test field exists");
    if(cell) {
        cell->onDragStart(start);int id=cell->paramId;
        move.mouseDelta=Vec(0,-60);cell->onDragMove(move);cell->onLeave(leave);
        check(cell->adjustmentTip && cell->adjustmentTip->text==cell->getParamQuantity()->getString(),"optional grid has live readout after leaving cell");
        m->selectedTrack.store(1);cell->step();check(cell->paramId==id,"grid retains original parameter throughout held gesture");
        cell->onDragEnd(end);m->selectedTrack.store(0);cell->step();
        check(!cell->adjustmentTip && !APP->window->isCursorLocked(),"grid release clears popup and cursor capture");
        APP->history->undo();
    }
    m->expanded.store(false);click(find("ORDER"));
    m->params[Metropolix::trackParam(0,Metropolix::ORDER)].setValue(1);
    slider(0)->getParamQuantity()->setValue(.5f);
    slider(0)->onDragStart(start);move.mouseDelta=Vec(0,-40);slider(0)->onDragMove(move);
}
if(qaFrame==13)APP->window->screenshot(dir+"/live-adjustment.png");
if(qaFrame==14) { slider(0)->onDragEnd(end);settings::haloBrightness=0; }
if(qaFrame==18)APP->window->screenshot(dir+"/bloom-off.png");
if(qaFrame==19)settings::haloBrightness=1;
if(qaFrame==23)APP->window->screenshot(dir+"/bloom-on.png");
if(qaFrame==24)settings::rackBrightness=.2f;
if(qaFrame==28)APP->window->screenshot(dir+"/dark-room.png");
if(qaFrame==29) { settings::rackBrightness=1;settings::haloBrightness=.25f; }
const float zooms[]={.75f,1.f,1.25f,1.5f,2.f};
int i=(qaFrame-30)/6;
if(qaFrame>=30 && i<5) {
    if((qaFrame-30)%6==0)APP->scene->rackScroll->setZoom(zooms[i]);
    // RackScroll finishes layout after its children. Restore the framing on
    // subsequent frames too, once the new zoom's scroll bounds have settled.
    APP->scene->rackScroll->setGridOffset(Vec(-1.f,-.05f));
    if((qaFrame-30)%6==3) {
        APP->window->screenshot(dir+string::f("/zoom-%03d.png",int(100*zooms[i])));
        auto* display=find("Metropolix display");
        Vec pos=display->getAbsoluteOffset(Vec(0,0)),size=display->box.size.mult(zooms[i]);
        check(pos.x>=0 && pos.y>=30 && pos.x+size.x<1500 && pos.y+size.y<900,"OLED remains visible in zoom proof");
        std::ofstream bounds(dir+string::f("/zoom-%03d.txt",int(100*zooms[i])));
        bounds<<pos.x<<' '<<pos.y<<' '<<size.x<<' '<<size.y;
    }
}
static app::ModuleWidget* preview=nullptr;
if(qaFrame==60) {
    preview=modelMetropolix->createModuleWidget(nullptr);
    preview->box.pos=Vec(1040,100);APP->scene->addChild(preview);
}
if(qaFrame==64) {
    auto* display=dynamic_cast<HardwareDisplay*>(seek(preview,[](widget::Widget* w){return dynamic_cast<HardwareDisplay*>(w)!=nullptr;}));
    check(display && !display->displayImages.empty(),"module-browser preview creates OLED texture without an engine module");
    widget::Widget::ContextDestroyEvent event;event.vg=APP->window->vg;
    preview->onContextDestroy(event);
    check(display && display->displayImages.empty(),"graphics-context cleanup releases OLED images");
}
if(qaFrame==68) {
    auto* display=dynamic_cast<HardwareDisplay*>(seek(preview,[](widget::Widget* w){return dynamic_cast<HardwareDisplay*>(w)!=nullptr;}));
    check(display && !display->displayImages.empty(),"OLED texture is recreated on next render");
    APP->scene->removeChild(preview);delete preview;preview=nullptr;
    check(true,"rendered preview can be removed without a crash");
    auto* control=new HardwareControl;
    control->metroModule=m;control->binding=[]{return Metropolix::PITCH;};control->bind();
    APP->scene->addChild(control);
    APP->event->setHoveredWidget(control);
    APP->event->setDraggedWidget(control,GLFW_MOUSE_BUTTON_LEFT);
    check(control->adjustmentTip!=nullptr,"temporary control starts a real Rack event-state gesture");
    // Rack removal sends the event cleanup while the derived widget is alive.
    APP->scene->removeChild(control);delete control;
    check(!APP->event->getDraggedWidget() && !APP->window->isCursorLocked(),"removing a held control releases event state, readout and cursor");
}
if(qaFrame==72) { check(!qaFailed,"UI POLISH QA COMPLETE");APP->window->close(); }
