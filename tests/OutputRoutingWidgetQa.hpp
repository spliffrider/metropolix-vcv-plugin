if(std::getenv("METROPOLIX_OUTPUT_REOPEN_QA")){
    if(qaFrame==6){check(m->integer(Metropolix::OUTPUT_SOURCE_A)==37 && m->integer(Metropolix::OUTPUT_SOURCE_B)==25 && m->outputDraft[0].load()==-1 && m->outputDraft[1].load()==-1,"native patch stores applied sources and discards unconfirmed choices");check(m->getPreset(0).occupied && m->getPreset(0).values[478]==3,"native bank retains format6 track options");check(true,"OUTPUT REOPEN QA COMPLETE");APP->window->close();}return;
}
if(qaFrame==6){
    for(bool tiny:{false,true})for(const std::string symbol:{std::string("±"),std::string("?")}) {
        OledCanvas canvas;canvas.text(0,0,symbol,tiny);int lit=0;for(auto& row:canvas.pixels)for(uint8_t pixel:row)lit+=pixel>0;
        check(lit>3 && canvas.width(symbol,tiny)<=5,"pending and bipolar OLED symbols render with one glyph width");
    }
    using M=Metropolix;m->params[M::RUN].setValue(0);m->params[M::OUTPUT_SOURCE_A].setValue(0);m->params[M::OUTPUT_SOURCE_B].setValue(1);
    click(find("ALT"));click(find("Stage 3 / PITCH"));check(m->uiPage.load()==M::A_PAGE && m->uiField.load()==0,"ALT A opens output function field");
    drag(find("Encoder"),-22);check(m->integer(M::OUTPUT_SOURCE_A)==0 && shownOutputSource(m)==14,"drag proposes First Pulse without changing applied CV");
    click(find("Encoder"));check(m->integer(M::OUTPUT_SOURCE_A)==14 && m->uiField.load()==1 && m->outputDraft[0].load()<0,"encoder confirms function and selects assignment");APP->history->undo();check(m->integer(M::OUTPUT_SOURCE_A)==0,"confirmed function has one Rack undo");APP->history->redo();
    drag(find("Metropolix display"),-22);check(m->integer(M::OUTPUT_SOURCE_A)==14 && shownOutputSource(m)==15,"display proposes track2 independently");click(find("Encoder"));check(m->integer(M::OUTPUT_SOURCE_A)==15,"assignment confirms on encoder press");APP->history->undo();check(m->integer(M::OUTPUT_SOURCE_A)==14,"assignment confirmation undoes");
    m->uiField.store(0);drag(find("Encoder"),-220);check(m->outputDraft[0].load()>=0,"type proposal can span menu functions");click(find("EXIT"));check(m->integer(M::OUTPUT_SOURCE_A)==14 && m->outputDraft[0].load()<0,"EXIT discards pending output changes");
    click(find("ALT"));click(find("Stage 4 / GATE"));m->params[M::OUTPUT_SOURCE_B].setValue(25);m->uiField.store(1);
    press(find("Encoder"),GLFW_MOUSE_BUTTON_RIGHT);check(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Velocity";})!=nullptr,"output menus include all hardware functions");closeMenus();
    auto* encoder=find("Encoder");int before=APP->history->actionIndex;widget::EventContext context;event::DragStart begin;begin.context=&context;begin.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragStart(begin);encoder->heldSince=system::getTime()-1.1;encoder->step();event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragEnd(end);
    check(m->integer(M::OUTPUT_SOURCE_B)==14 && m->uiField.load()==0 && APP->history->actionIndex==before+1,"long encoder resets B to hardware First Pulse with one history action");APP->history->undo();check(m->integer(M::OUTPUT_SOURCE_B)==25,"output reset undoes");
    m->params[M::OUTPUT_SOURCE_A].setValue(37);m->params[M::OUTPUT_SOURCE_B].setValue(25);m->params[M::outputTrackParam(0,M::VELOCITY_SOURCE)].setValue(3);m->putPreset(0,m->capturePreset());
    screenPage(m,M::A_PAGE);drag(find("Encoder"),22);check(m->outputDraft[0].load()>=0,"pending selection exists during save");APP->patch->save(dir+"/output-roundtrip.vcv");
}
if(qaFrame==8)metroqa::screenshot(dir+"/output-confirmation-display.png");
if(qaFrame==12){check(!qaFailed,"OUTPUT UI QA COMPLETE");APP->window->close();}
