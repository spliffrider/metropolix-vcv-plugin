// Shared Rack drag behavior for panel controls and the optional value grids.
// Keep a compact readout alive even when the pointer leaves during a drag.
bool needsRawDragHistory(ParamQuantity* q) {
    return dynamic_cast<TrackCvQuantity*>(q) || dynamic_cast<ModVoltageQuantity*>(q) || dynamic_cast<ClockOffsetQuantity*>(q);
}
// Menus outlive a changing stage/page binding. Resolve the clicked parameter,
// then capture its applied stored value at action time rather than its display.
struct ParameterMenuTarget {
    int64_t moduleId=-1;int paramId=-1;
    ParameterMenuTarget()=default;
    ParameterMenuTarget(int64_t moduleId,int paramId):moduleId(moduleId),paramId(paramId){}
    ParamQuantity* quantity() const {
        auto* m=APP->engine->getModule(moduleId);
        return m && paramId>=0 && paramId<int(m->params.size()) && paramId<int(m->paramQuantities.size())?m->getParamQuantity(paramId):nullptr;
    }
    bool readOnly() const {
        auto* q=quantity();if(!q)return true;
        if(auto* m=dynamic_cast<Metropolix*>(q->module))if(m->ctrlOwner(paramId)>=0)return true;
        if(auto* type=dynamic_cast<ModTypeQuantity*>(q))if(type->forced())return true;
        if(auto* assignment=dynamic_cast<RoutingAssignmentQuantity*>(q))if(assignment->global())return true;
        return false;
    }
    void write(const std::function<void(ParamQuantity*)>& apply,const std::string& name) const {
        auto* q=quantity();if(!q || readOnly())return;
        auto* m=dynamic_cast<Metropolix*>(q->module);
        auto pickup=m && m->waitingPickup(paramId)?uiPickup(m,paramId):metro::Pickup{};
        float before=q->ParamQuantity::getValue();apply(q);float after=q->ParamQuantity::getValue();
        if(m){m->cancelDraft();m->previewPitchEdit(paramId,before,after);}
        if(before==after)return;
        if(pickup.active){panelControlHistory(m,paramId,before,after,pickup,pickupAfterValue(m,paramId,pickup,after),name);return;}
        auto* h=new history::ParamChange;h->name=name;h->moduleId=moduleId;h->paramId=paramId;h->oldValue=before;h->newValue=after;APP->history->push(h);
    }
};
struct ParameterMenuLabel : ui::MenuLabel {
    ParameterMenuTarget target;
    void step() override {auto* q=target.quantity();text=q?q->getString():"Parameter unavailable";ui::MenuLabel::step();}
};
struct ParameterMenuField : ui::TextField {
    ParameterMenuTarget target;
    void step() override {
        if(target.readOnly()){if(auto* overlay=getAncestorOfType<ui::MenuOverlay>())overlay->requestDelete();return;}
        APP->event->setSelectedWidget(this);ui::TextField::step();
    }
    void onSelectKey(const event::SelectKey& e) override {
        if(e.action==GLFW_PRESS && (e.mods & RACK_MOD_MASK)==0 && (e.key==GLFW_KEY_ENTER || e.key==GLFW_KEY_KP_ENTER)) {
            std::string entry=text;target.write([entry](ParamQuantity* q){q->setDisplayValueString(entry);},"enter parameter");
            if(auto* overlay=getAncestorOfType<ui::MenuOverlay>()){overlay->requestDelete();}
            e.consume(this);return;
        }
        ui::TextField::onSelectKey(e);
    }
};
struct ParameterMenuChoice : ui::MenuItem {
    ParameterMenuTarget target;float value=0;
    void step() override {auto* q=target.quantity();disabled=target.readOnly();rightText=q?CHECKMARK(std::floor(q->getValue()-q->getMinValue())==std::floor(value-q->getMinValue())):"";ui::MenuItem::step();}
    void onAction(const event::Action&) override {target.write([this](ParamQuantity* q){q->setValue(value);},"set parameter");}
};
struct AdjustmentTooltip : ui::Tooltip {
    app::ParamWidget* owner=nullptr;
    std::function<std::string()> readout;
    void step() override {
        if(readout)text=readout();else if(auto* q=owner->getParamQuantity())text=q->getString();
        ui::Tooltip::step();
        box.pos=owner->getAbsoluteOffset(owner->box.size).round();
        if(parent)box=box.nudge(parent->box.zeroPos());
    }
};

struct ValueKnob : app::Knob {
    AdjustmentTooltip* adjustmentTip=nullptr;
    bool adjusting=false;
    virtual std::string adjustmentText(){auto* q=getParamQuantity();return q?q->getString():std::string();}
    ~ValueKnob() { clearAdjustmentTip(); destroyTooltip(); }
    void createContextMenu() {
        auto* q=getParamQuantity();if(!q || !module)return;
        ParameterMenuTarget target(module->id,paramId);
        if(auto* m=dynamic_cast<Metropolix*>(module))if(std::find(m->uiDraftIds.begin(),m->uiDraftIds.end(),paramId)!=m->uiDraftIds.end())m->cancelDraft();
        auto* menu=createMenu();auto* label=new ParameterMenuLabel;label->target=target;label->text=q->getString();menu->addChild(label);
        auto* choices=dynamic_cast<SwitchQuantity*>(q);
        if(target.readOnly())menu->addChild(createMenuLabel(q->getDescription()));
        else if(choices){
            for(size_t i=0;i<choices->labels.size();++i){auto* item=new ParameterMenuChoice;item->target=target;item->text=choices->labels[i];item->value=q->getMinValue()+float(i);item->rightText=CHECKMARK(int(std::floor(q->getValue()-q->getMinValue()))==int(i));menu->addChild(item);}
            if(!choices->labels.empty())menu->addChild(new ui::MenuSeparator);
        }
        else {auto* field=new ParameterMenuField;field->target=target;field->box.size.x=100;field->text=q->getDisplayValueString();field->selectAll();menu->addChild(field);}
        if(!target.readOnly() && q->resetEnabled && q->isBounded())menu->addChild(createMenuItem("Initialize",choices?"":"Double-click",[target]{target.write([](ParamQuantity* q){if(q->resetEnabled && q->isBounded())q->reset();},"reset parameter");}));
        if(!choices && !target.readOnly())menu->addChild(createMenuItem("Fine",std::string(RACK_MOD_CTRL_NAME)+" + drag",nullptr,true));
        if(auto* handle=APP->engine->getParamHandle(target.moduleId,target.paramId))menu->addChild(createMenuItem("Unmap",handle->text,[target]{if(auto* current=APP->engine->getParamHandle(target.moduleId,target.paramId))APP->engine->updateParamHandle(current,-1,0);}));
        appendContextMenu(menu);
    }
    void onButton(const event::Button& e) override {
        if(e.button==GLFW_MOUSE_BUTTON_RIGHT && e.action==GLFW_PRESS && (e.mods & RACK_MOD_MASK)==0){destroyTooltip();createContextMenu();e.consume(this);return;}
        app::ParamWidget::onButton(e);
    }
    void onDoubleClick(const event::DoubleClick&) override {
        auto* q=getParamQuantity();if(module && q && q->resetEnabled && q->isBounded())ParameterMenuTarget(module->id,paramId).write([](ParamQuantity* q){if(q->resetEnabled && q->isBounded())q->reset();},"reset parameter");
    }
    void clearAdjustmentTip() {
        if(!adjustmentTip)return;
        if(adjustmentTip->parent)adjustmentTip->parent->removeChild(adjustmentTip);
        delete adjustmentTip;adjustmentTip=nullptr;
    }
    void updateAdjustmentTip() {
        if(!adjusting || !settings::tooltips || !getParamQuantity()) { clearAdjustmentTip();return; }
        if(!adjustmentTip) {
            adjustmentTip=new AdjustmentTooltip;adjustmentTip->owner=this;
            adjustmentTip->readout=[this]{return adjustmentText();};
            APP->scene->addChild(adjustmentTip);
        }
        adjustmentTip->text=adjustmentText();
    }
    void step() override { app::Knob::step();updateAdjustmentTip(); }
    void onEnter(const event::Enter& e) override { if(!adjusting)app::Knob::onEnter(e); }
    void onLeave(const event::Leave& e) override {
        // Knob::onLeave also finishes scroll history. During a held drag that
        // would split the gesture and discard its fractional snap movement.
        if(adjusting)destroyTooltip();else app::Knob::onLeave(e);
    }
    void onDragStart(const event::DragStart& e) override {
        if(e.button!=GLFW_MOUSE_BUTTON_LEFT)return;
        destroyTooltip();adjusting=true;
        app::Knob::onDragStart(e);updateAdjustmentTip();
    }
    void onDragMove(const event::DragMove& e) override {
        // A zero-distance event must not send the effective display value back
        // through a setter which can clamp a dormant raw parameter.
        if(e.mouseDelta.norm()>0)app::Knob::onDragMove(e);
        updateAdjustmentTip();
    }
    void onDragEnd(const event::DragEnd& e) override {
        if(e.button!=GLFW_MOUSE_BUTTON_LEFT || !adjusting)return;
        app::Knob::onDragEnd(e);
        adjusting=false;clearAdjustmentTip();
    }
};
