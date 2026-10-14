// Shared Rack drag behavior for panel controls and the optional value grids.
// Keep a compact readout alive even when the pointer leaves during a drag.
bool needsRawDragHistory(ParamQuantity* q) {
    return dynamic_cast<TrackCvQuantity*>(q) || dynamic_cast<ModVoltageQuantity*>(q) || dynamic_cast<ClockOffsetQuantity*>(q);
}
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
