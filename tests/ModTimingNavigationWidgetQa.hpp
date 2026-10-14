// Firmware 1.6 p145: plain stage buttons select the MOD lane while its
// ORDER/LEN/DIV screen stays open. A selector must not edit stage voltage.
using M=Metropolix;
APP->engine->bypassModule(m,true);
auto savedHistory=std::move(APP->history->actions);
int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;
APP->history->actionIndex=APP->history->savedIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
auto original=raw();
auto clean=[&]{
    for(auto* action:APP->history->actions)delete action;
    APP->history->actions.clear();APP->history->actionIndex=APP->history->savedIndex=0;
    for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->clearWorkflows();m->cancelDraft();m->timingShortcutHeld.store(0);m->orderShortcutHeld.store(false);m->scaleShortcutHeld.store(false);m->loopyMuteHeld.store(false);
    m->uiMod.store(true);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->uiStageDetail.store(false);m->selectedStage.store(2);m->selectedRow.store(M::CV);
    m->params[M::ENC_CONFIRM].setValue(0);
    for(int lane=0;lane<8;++lane){
        m->params[M::modParam(lane,M::MOD_TYPE)].setValue(metro::ModBipolar);
        m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(lane%2?metro::ModDirection:metro::ModNone);
        for(int stage=0;stage<8;++stage)m->params[M::modParam(lane,M::MOD_VALUE+stage)].setValue(-1.25f+stage*.25f+lane*.02f);
    }
};
struct Context{int page,field;};
const Context contexts[]={{M::ORDER_PAGE,0},{M::LENGTH_PAGE,0},{M::LENGTH_PAGE,1},{M::DIV_PAGE,0}};
const char* names[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
for(float movement:{0.f,-22.f}){
    int failedNavigation=0,failedValues=0,total=0;
    for(auto context:contexts)for(int source=0;source<8;++source)for(int target=0;target<8;++target){
        clean();m->selectedLane.store(source);screenPage(m,context.page);m->uiField.store(context.field);
        auto before=raw();auto* control=find("Stage "+std::to_string(target+1)+" / "+names[target]);
        drag(control,movement);++total;
        if(m->uiPage.load()!=context.page || m->uiField.load()!=context.field || m->selectedLane.load()!=target || m->selectedStage.load()!=2 || !m->uiMod.load())++failedNavigation;
        if(raw()!=before || APP->history->actionIndex!=0 || m->uiDraftParam>=0 || APP->window->isCursorLocked())++failedValues;
    }
    check(failedNavigation==0,std::to_string(total)+" MOD timing lane "+(movement==0?"clicks":"held movements")+" keep ORDER/LEN/DIV and its active field; failures="+std::to_string(failedNavigation));
    check(failedValues==0,std::to_string(total)+" MOD timing lane "+(movement==0?"clicks":"held movements")+" preserve all stage values and history; failures="+std::to_string(failedValues));
}
auto parameter=[](Context context,int lane){return M::modParam(lane,context.page==M::ORDER_PAGE?M::MOD_ORDER:context.page==M::DIV_PAGE?M::MOD_DIVISION:context.field?M::MOD_PULSE_LIMIT:M::MOD_LENGTH);};
bool edits=true,previews=true;int editCases=0,previewCases=0;
for(auto context:contexts)for(int source=0;source<8;++source)for(int target=0;target<8;++target)for(const char* controlName:{"Encoder","Metropolix display"})for(int confirmation:{0,1}){
    clean();m->selectedLane.store(source);screenPage(m,context.page);m->uiField.store(context.field);m->params[M::ENC_CONFIRM].setValue(confirmation);
    int id=parameter(context,target);m->params[id].setValue(4);
    click(find("Stage "+std::to_string(target+1)+" / "+names[target]));auto before=raw();
    drag(find(controlName),-44);
    if(confirmation){edits &= raw()==before && m->uiDraftParam==id && pendingConfirmation(m);click(find("Encoder"));}
    auto after=raw(),expected=before;expected[id]=after[id];
    edits &= after==expected && after[id]>4 && APP->history->actionIndex==1 && m->uiPage.load()==context.page && m->uiField.load()==context.field;
    if(APP->history->actionIndex==1){APP->history->undo();edits &= raw()==before;APP->history->redo();edits &= raw()==after;}
    ++editCases;
}
check(edits,std::to_string(editCases)+" encoder/OLED edits after MOD lane selection target only the new lane, with confirmation and exact Undo/Redo");
for(auto context:contexts)for(int source=0;source<8;++source)for(int target=0;target<8;++target)if(source!=target){
    clean();m->selectedLane.store(source);screenPage(m,context.page);m->uiField.store(context.field);m->params[M::ENC_CONFIRM].setValue(1);
    m->params[parameter(context,source)].setValue(4);auto before=raw();drag(find("Encoder"),-44);
    previews &= m->uiDraftParam==parameter(context,source) && pendingConfirmation(m) && raw()==before;
    click(find("Stage "+std::to_string(target+1)+" / "+names[target]));
    previews &= m->uiDraftParam<0 && raw()==before && APP->history->actionIndex==0 && m->selectedLane.load()==target && m->uiPage.load()==context.page && m->uiField.load()==context.field;
    ++previewCases;
}
check(previews,std::to_string(previewCases)+" MOD lane changes cancel the previous lane's pending setting without applying it or adding Undo");
bool retained=true;int retainedCases=0;
for(auto context:contexts)for(int lane=0;lane<8;++lane){
    clean();m->selectedLane.store(lane);screenPage(m,context.page);m->uiField.store(context.field);m->params[M::ENC_CONFIRM].setValue(1);
    int id=parameter(context,lane);m->params[id].setValue(4);auto before=raw();drag(find("Encoder"),-44);float value=m->uiDraftValue;auto serial=m->uiDraftSerial.load();
    click(find("Stage "+std::to_string(lane+1)+" / "+names[lane]));
    retained &= pendingConfirmation(m) && m->uiDraftParam==id && m->uiDraftValue==value && m->uiDraftSerial.load()==serial && raw()==before && APP->history->actionIndex==0;
    click(find("Encoder"));auto after=before;after[id]=value;retained &= raw()==after && APP->history->actionIndex==1;
    if(APP->history->actionIndex==1){APP->history->undo();retained &= raw()==before;APP->history->redo();retained &= raw()==after;}++retainedCases;
}
check(retained,std::to_string(retainedCases)+" same-lane selections retain the valid pending setting and its exact confirmed Undo/Redo");
auto overlay=[&]()->widget::Widget*{for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if(dynamic_cast<ui::MenuOverlay*>(*it))return *it;return nullptr;};
auto item=[&](widget::Widget* menu,const std::string& text)->ui::MenuItem*{return menu?dynamic_cast<ui::MenuItem*>(seek(menu,[&](widget::Widget* w){auto* entry=dynamic_cast<ui::MenuItem*>(w);return entry && entry->text==text;})):nullptr;};
auto dismiss=[&]{std::vector<widget::Widget*> menus;for(auto* w:APP->scene->children)if(dynamic_cast<ui::MenuOverlay*>(w))menus.push_back(w);APP->event->setSelectedWidget(nullptr);for(auto* w:menus){APP->scene->removeChild(w);delete w;}};
bool menus=true,indicators=true;int menuCases=0;
for(auto context:contexts)for(int source=0;source<8;++source)for(int target=0;target<8;++target){
    dismiss();clean();m->selectedLane.store(source);screenPage(m,context.page);m->uiField.store(context.field);int destination=M::modParam(target,M::MOD_DESTINATION);m->params[destination].setValue(metro::ModPitchPost);auto before=raw();
    auto* control=find("Stage "+std::to_string(target+1)+" / "+names[target]);press(control,GLFW_MOUSE_BUTTON_RIGHT);auto* link=item(overlay(),"Lane settings");
    menus &= link && raw()==before && APP->history->actionIndex==0 && m->selectedLane.load()==target && m->uiPage.load()==context.page && m->uiField.load()==context.field;
    for(int lane=0;lane<8;++lane){auto* button=find("Stage "+std::to_string(lane+1)+" / "+names[lane]);indicators &= button && button->active()==(lane==target);}
    indicators &= control->tooltipText().find("MOD lane "+std::to_string(target+1))!=std::string::npos;
    m->selectedLane.store((target+3)%8);if(link){event::Action action;link->onAction(action);}auto* destinationItem=item(overlay(),"Internal destination");auto* child=destinationItem?destinationItem->createChildMenu():nullptr;auto* none=item(child,"None");
    if(none){event::Action action;none->onAction(action);}auto after=before;after[destination]=metro::ModNone;
    menus &= destinationItem && none && raw()==after && APP->history->actionIndex==1;
    if(APP->history->actionIndex==1){APP->history->undo();menus &= raw()==before;APP->history->redo();menus &= raw()==after;}
    delete child;++menuCases;
}
dismiss();
check(menus,std::to_string(menuCases)+" MOD timing right-click settings stay bound to the clicked lane after selection changes, with exact destination Undo/Redo");
check(indicators,"MOD timing lane selectors show the selected lane light and lane-specific mouseover text");
// Lane navigation must preserve independent pitch edits and recalled state.
unsigned savedPickupMask=m->pickupActiveMask.load();float savedPickupTarget=m->pickupTargets[0].load();
bool savedInFlight=m->presetCommandInFlight.load();auto savedCommands=std::move(m->presetUiCommands);
m->presetCommandInFlight.store(true);bool pickup=true;int pickupCases=0;
widget::EventContext pickupContext;event::DragStart pickupStart;pickupStart.context=&pickupContext;pickupStart.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragMove pickupMove;pickupMove.context=&pickupContext;pickupMove.button=GLFW_MOUSE_BUTTON_LEFT;pickupMove.mouseDelta=Vec(0,-22);
event::DragEnd pickupEnd;pickupEnd.context=&pickupContext;pickupEnd.button=GLFW_MOUSE_BUTTON_LEFT;
for(auto context:contexts)for(int source=0;source<8;++source){
    clean();m->presetUiCommands.clear();m->selectedLane.store(source);screenPage(m,context.page);m->uiField.store(context.field);
    m->params[M::LOAD_PITCH].setValue(M::PitchThru);m->params[M::PITCH].setValue(.25f);m->pickupTargets[0].store(.75f);m->pickupActiveMask.store(1);
    int target=(source+1)%8;auto* control=find("Stage "+std::to_string(target+1)+" / "+names[target]);control->onDragStart(pickupStart);
    m->getParamQuantity(M::PITCH)->setValue(.9f);auto before=raw();control->onDragMove(pickupMove);control->onDragEnd(pickupEnd);
    pickup &= raw()==before && APP->history->actionIndex==0 && m->presetUiCommands.empty() && m->pickupActiveMask.load()==1 && m->pickupTargets[0].load()==.75f;
    ++pickupCases;
}
check(pickup,std::to_string(pickupCases)+" unbound MOD lane selectors preserve an independent pitch edit and recalled pickup state without claiming its Undo or queuing panel edits");
m->presetUiCommands=std::move(savedCommands);m->presetCommandInFlight.store(savedInFlight);m->pickupActiveMask.store(savedPickupMask);m->pickupTargets[0].store(savedPickupTarget);
for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);
m->clearWorkflows();m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(false);m->selectedLane.store(0);m->selectedStage.store(0);screenPage(m,M::HOME);closeMenus();
for(auto* action:APP->history->actions)delete action;
APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;
APP->engine->bypassModule(m,false);
