#ifdef METROPOLIX_QA
// Test builds use Rack's renderer capture; no test hooks enter the release DLL.
#include <window/Window.hpp>
#undef PRIVATE
#include <fstream>
#include "../tests/VideoCaptureState.hpp"
#endif
#include "Metropolix.hpp"
#ifdef METROPOLIX_QA
#include <patch.hpp>
#endif
#include <functional>

namespace {
NVGcolor ink() { return nvgRGB(38, 44, 47); }
NVGcolor muted() { return nvgRGB(116, 121, 119); }
NVGcolor paper() { return nvgRGB(229, 227, 217); }
NVGcolor orange() { return nvgRGB(215, 103, 55); }
NVGcolor teal() { return nvgRGB(45, 140, 137); }
NVGcolor trackColor(int t) { return t ? teal() : orange(); }

void font(NVGcontext* vg) {
    auto face = APP->window->loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
    if (face) nvgFontFaceId(vg, face->handle);
}
void text(NVGcontext* vg, float x, float y, const std::string& value, float size = 10.f,
          NVGcolor color = ink(), int align = NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE) {
    nvgFontSize(vg, size);
    nvgFillColor(vg, color);
    nvgTextAlign(vg, align);
    nvgText(vg, x, y, value.c_str(), nullptr);
}
void rect(NVGcontext* vg, float x, float y, float w, float h, NVGcolor color, float radius = 3.f) {
    nvgBeginPath(vg);
    nvgRoundedRect(vg, x, y, w, h, radius);
    nvgFillColor(vg, color);
    nvgFill(vg);
}
void line(NVGcontext* vg, float x1, float y1, float x2, float y2, NVGcolor color) {
    nvgBeginPath(vg);
    nvgMoveTo(vg, x1, y1);
    nvgLineTo(vg, x2, y2);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, 1.f);
    nvgStroke(vg);
}
void setWithHistory(ParamQuantity* q, float value) {
    if (!q || !q->module) return;
    if(auto* m=dynamic_cast<Metropolix*>(q->module))if(m->ctrlOwner(q->paramId)>=0)return;
    float before = q->getValue();
    q->setValue(value);
    if (q->getValue() == before) return;
    auto* change = new history::ParamChange;
    change->moduleId = q->module->id;
    change->paramId = q->paramId;
    change->oldValue = before;
    change->newValue = q->getValue();
    APP->history->push(change);
}

const char* rowNames[] = {"RATCHETS", "PROBABILITY", "ACCUM", "SLIDE", "SKIP", "CV", "PITCH", "GATE"};

void parameterHistory(Metropolix* m,const std::vector<int>& ids,const std::vector<float>& before,const std::string& name) {
    auto* batch=new history::ComplexAction;batch->name=name;
    for(size_t i=0;i<ids.size();++i) {
        float after=m->params[ids[i]].getValue();if(after==before[i])continue;
        auto* change=new history::ParamChange;change->moduleId=m->id;change->paramId=ids[i];change->oldValue=before[i];change->newValue=after;batch->push(change);
    }
    if(batch->isEmpty())delete batch;else APP->history->push(batch);
}
bool overridePage(Metropolix* m) { return m && !m->uiMod.load() && m->uiPage.load()==Metropolix::STAGE_PAGE && m->selectedRow.load()>=Metropolix::PITCH_OVERRIDE; }
std::vector<int> overrideParams(Metropolix* m) {
    std::vector<int> ids;if(!overridePage(m))return ids;
    for(int s=0;s<8;++s)if(m->uiAlt.load() || s==m->selectedStage.load())ids.push_back(Metropolix::stageParam(m->selectedTrack.load(),s,m->selectedRow.load()));
    return ids;
}
void syncPitchOverrides(Metropolix* m) {
    auto ids=overrideParams(m);std::vector<float> before;
    for(int id:ids) {
        before.push_back(m->params[id].getValue());int s=(id-Metropolix::STAGE_OVERRIDES)%16;
        float slider=m->params[Metropolix::PITCH+s].getValue();
        for(int k=0;k<2;++k)if(m->integer(Metropolix::ctrlParam(k,Metropolix::CTRL_DESTINATION))==metro::CtrlSliderInvert &&
            (m->integer(Metropolix::ctrlParam(k,Metropolix::CTRL_ASSIGNMENT))==2 || m->integer(Metropolix::ctrlParam(k,Metropolix::CTRL_ASSIGNMENT))==m->selectedTrack.load()))
            slider=m->params[Metropolix::ctrlParam(k,Metropolix::CTRL_VALUE)].getValue()>0?1.f-m->params[Metropolix::PITCH+s].getValue():m->params[Metropolix::PITCH+s].getValue();
        m->getParamQuantity(id)->setValue(slider*12*m->integer(Metropolix::RANGE));
    }
    parameterHistory(m,ids,before,"sync pitch overrides");
}
void overrideMenu(ui::Menu* menu,Metropolix* m) {
    if(!overridePage(m))return;
    menu->addChild(new ui::MenuSeparator);
    for(bool all:{false,true}) {
        if(m->selectedRow.load()==Metropolix::PITCH_OVERRIDE)
            menu->addChild(createMenuItem(all?"Sync all pitch overrides to sliders":"Sync pitch override to slider","",[m,all]{bool alt=m->uiAlt.load();m->uiAlt.store(all);syncPitchOverrides(m);m->uiAlt.store(alt);}));
        menu->addChild(createMenuItem(all?"Remove all overrides on this track":"Remove this stage override","",[m,all]{
            bool alt=m->uiAlt.load();m->uiAlt.store(all);auto ids=overrideParams(m);m->uiAlt.store(alt);std::vector<float> before;
            for(int id:ids){before.push_back(m->params[id].getValue());m->params[id].setValue(-1);}
            parameterHistory(m,ids,before,"remove stage overrides");
        }));
    }
}
const char* notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

void trackSettingsMenu(Metropolix* m) {
    if (!m) return;
    int t = m->selectedTrack.load();
    auto* menu = createMenu();
    menu->addChild(createMenuLabel(string::f("Track %d settings", t + 1)));
    auto choice = [menu, m](const std::string& label, int id, std::vector<std::string> values, int offset = 0) {
        auto* q = m->getParamQuantity(id);
        menu->addChild(createIndexSubmenuItem(label, values,
            [q, offset] { return static_cast<size_t>(std::round(q->getValue()) - offset); },
            [q, offset](size_t index) { setWithHistory(q, static_cast<float>(index + offset)); }, m->ctrlOwner(id)>=0));
    };
    choice("Direction", Metropolix::extraParam(t, Metropolix::REVERSED), {"Forward", "Reverse"});
    std::vector<std::string> swingValues;
    for(int n=50;n<=78;++n)swingValues.push_back(std::to_string(n)+"%");
    choice("Swing", Metropolix::swingParam(t), swingValues, 50);
    std::vector<std::string> divisions;
    for (int i = 1; i <= 8; ++i) divisions.push_back("/ " + std::to_string(i));
    choice("Pulse-count division", Metropolix::extraParam(t, Metropolix::PULSE_DIV), divisions, 1);
    std::vector<std::string> pulses = {"Auto"};
    for (int i = 1; i <= 64; ++i) pulses.push_back(std::to_string(i) + " pulses");
    choice("Sequence length in pulses", Metropolix::extraParam(t, Metropolix::PULSE_LIMIT), pulses);
    menu->addChild(new ui::MenuSeparator);
    choice("Gate stretching", Metropolix::extraParam(t, Metropolix::GATE_STRETCH), {"Off", "On"});
    choice("Ratchet mode (stretching on)", Metropolix::extraParam(t, Metropolix::RATCHET_MODE), {"Multiply", "Pulse", "Gated"});
    choice("Accumulator reset", Metropolix::extraParam(t, Metropolix::ACCUM_AUTO_RESET), {"Manual", "Auto (at fixed pulse length)"});
    choice("Accumulator mode", Metropolix::accumModeParam(t), {"Stage", "Track"});
    choice("Accumulator order", Metropolix::accumParam(t, Metropolix::ACCUM_ORDER), {"Wrap", "Pendulum", "Random", "Hold"});
    choice("Accumulator polarity", Metropolix::accumParam(t, Metropolix::BIPOLAR), {"Unipolar", "Bipolar"});
    std::vector<std::string> limits;
    for (int i = 1; i <= 28; ++i) limits.push_back(std::to_string(i) + " degrees");
    choice("Positive accumulation limit", Metropolix::trackParam(t, Metropolix::ACCUM_LIMIT), limits, 1);
    choice("Negative accumulation limit", Metropolix::accumParam(t, Metropolix::NEGATIVE_LIMIT), limits, 1);
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuItem("Open stage editor", "", [m] {
        m->uiMod.store(false); m->modEditor.store(false); m->expanded.store(true);
    }));
}

void stageSettingsMenu(Metropolix* m, int stage) {
    if (!m) return;
    int t = m->selectedTrack.load();
    auto* menu = createMenu();
    menu->addChild(createMenuLabel(string::f("Track %d, stage %d", t + 1, stage + 1)));
    auto* probability = m->getParamQuantity(Metropolix::stageOption(t, stage, Metropolix::PROB_TARGET));
    menu->addChild(createIndexSubmenuItem("Probability target", {"Stage", "Pulse"},
        [probability] { return static_cast<size_t>(probability->getValue()); },
        [probability](size_t value) { setWithHistory(probability, static_cast<float>(value)); }));
    auto* accum = m->getParamQuantity(Metropolix::stageOption(t, stage, Metropolix::ACCUM_TRIGGER));
    menu->addChild(createIndexSubmenuItem("Accumulator trigger", {"Stage", "Pulse", "Ratchet"},
        [accum] { return static_cast<size_t>(accum->getValue()); },
        [accum](size_t value) { setWithHistory(accum, static_cast<float>(value)); }));
}

void modSettingsMenu(Metropolix* m, int lane) {
    if (!m) return;
    auto* menu = createMenu();
    menu->addChild(createMenuLabel(string::f("MOD lane %d", lane + 1)));
    auto choice=[menu,m,lane](const std::string& label,int property,std::vector<std::string> values) {
        auto* q=m->getParamQuantity(Metropolix::modParam(lane,property));
        menu->addChild(createIndexSubmenuItem(label,values,
            [q] { return static_cast<size_t>(q->getValue()); },
            [q](size_t i) { setWithHistory(q,static_cast<float>(i)); }));
    };
    choice("CV type",Metropolix::MOD_TYPE,{"Bipolar","Unipolar","Gate"});
    choice("Internal destination",Metropolix::MOD_DESTINATION,Metropolix::modDestinations());
    choice("Destination tracks",Metropolix::MOD_ASSIGNMENT,{"Track 1","Track 2","Both tracks"});
    choice("Mute",Metropolix::MOD_MUTE,{"Off","On"});
    menu->addChild(new ui::MenuSeparator);
    auto* direction = m->getParamQuantity(Metropolix::modParam(lane, Metropolix::MOD_REVERSED));
    menu->addChild(createIndexSubmenuItem("Direction", {"Forward", "Reverse"},
        [direction] { return static_cast<size_t>(direction->getValue()); },
        [direction](size_t value) { setWithHistory(direction, static_cast<float>(value)); }));
    std::vector<std::string> values = {"Auto"};
    for (int i = 1; i <= 64; ++i) values.push_back(std::to_string(i) + " pulses");
    auto* pulses = m->getParamQuantity(Metropolix::modParam(lane, Metropolix::MOD_PULSE_LIMIT));
    menu->addChild(createIndexSubmenuItem("Sequence length in pulses", values,
        [pulses] { return static_cast<size_t>(pulses->getValue()); },
        [pulses](size_t value) { setWithHistory(pulses, static_cast<float>(value)); }));
    menu->addChild(createMenuLabel("Each MOD stage lasts one divided clock pulse."));
}

void appendAuxMenu(ui::Menu* menu, Metropolix* m, int input) {
    if(!m) return;
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuLabel(string::f("AUX %c",'X'+input)));
    auto choice=[menu,m,input](const std::string& label,int property,std::vector<std::string> values) {
        auto* q=m->getParamQuantity(Metropolix::auxParam(input,property));
        menu->addChild(createIndexSubmenuItem(label,values,
            [q] { return static_cast<size_t>(q->getValue()); },
            [q](size_t value) { setWithHistory(q,static_cast<float>(value)); }));
    };
    choice("Destination",Metropolix::AUX_DESTINATION,Metropolix::modDestinations());
    choice("Target tracks",Metropolix::AUX_ASSIGNMENT,{"Track 1","Track 2","Both tracks"});
    menu->addChild(createMenuItem("Clear assignment","",[m,input] {
        setWithHistory(m->getParamQuantity(Metropolix::auxParam(input,Metropolix::AUX_DESTINATION)),0.f);
    }));
}

void appendCtrlMenu(ui::Menu* menu, Metropolix* m, int knob) {
    if(!m)return;
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuLabel(string::f("CTRL %d",knob+1)));
    auto choice=[menu,m,knob](const std::string& label,int property,std::vector<std::string> values) {
        auto* q=m->getParamQuantity(Metropolix::ctrlParam(knob,property));
        menu->addChild(createIndexSubmenuItem(label,values,
            [q]{return static_cast<size_t>(q->getValue());},
            [q](size_t value){setWithHistory(q,static_cast<float>(value));}));
    };
    choice("Destination",Metropolix::CTRL_DESTINATION,Metropolix::ctrlDestinations());
    choice("Target tracks",Metropolix::CTRL_ASSIGNMENT,{"Track 1","Track 2","Both tracks"});
    menu->addChild(createMenuItem("Centre knob (noon)","",[m,knob] {
        setWithHistory(m->getParamQuantity(Metropolix::ctrlParam(knob,Metropolix::CTRL_VALUE)),0.f);
    }));
    menu->addChild(createMenuItem("Clear assignment","",[m,knob] {
        setWithHistory(m->getParamQuantity(Metropolix::ctrlParam(knob,Metropolix::CTRL_DESTINATION)),0.f);
    }));
    menu->addChild(createSubmenuItem("Not implemented yet","",[](ui::Menu* sub) {
        for(auto name:{"Accum Dir","Gate Scale","Scale (User)","Slider Range","Velocity"})
            sub->addChild(createMenuLabel(name));
    }));
}

// All views bind to the same ParamQuantity, preserving Rack automation, reset,
// numeric entry, patch serialization, and undo in both compact and open views.
#include "ValueKnob.hpp"
struct ValueCell : ValueKnob {
    Metropolix* metroModule = nullptr;
    int trackProperty = -1;
    bool swingProperty = false;
    int stageColumn = -1;
    int stageRow = -1;
    int modProperty = -1;
    bool compact = false;
    bool dark = false;
    int decimals = 0;
    std::vector<std::string> choices;
    std::vector<std::string> shortChoices;
    float dragDistance = 0.f;
    bool dragStarted = false;
    bool locked() const { return metroModule && metroModule->ctrlOwner(paramId)>=0; }

    ValueCell() {
        box.size = Vec(40, 24);
        smooth = false;
        forceLinear = true;
        speed = 0.55f;
    }
    void step() override {
        if (metroModule && !dragStarted) {
            int t = metroModule->selectedTrack.load();
            if (trackProperty >= 0) paramId = Metropolix::trackParam(t, trackProperty);
            if (swingProperty) paramId = Metropolix::swingParam(t);
            if (stageRow >= 0) paramId = Metropolix::stageParam(t, stageColumn, stageRow);
            if (compact) paramId = Metropolix::stageParam(t,
                metroModule->selectedStage.load(), metroModule->selectedRow.load());
            if (modProperty >= 0) paramId = Metropolix::modParam(metroModule->selectedLane.load(), modProperty);
        }
        ValueKnob::step();
        if (auto* q = getParamQuantity()) speed = q->snapEnabled ? std::max(1.f, 30.f / q->getRange()) : 1.f;
    }
    std::string valueText() {
        auto* q = getParamQuantity();
        if (!q) return "--";
        if (modProperty >= Metropolix::MOD_RAMP) return q->getValue() > .5f ? "RAMP" : "STEP";
        if (modGateCell()) return q->getValue() >= 1.5f ? "ON" : "OFF";
        int row = compact && metroModule ? metroModule->selectedRow.load() : stageRow;
        if (row == Metropolix::SLIDE || row == Metropolix::SKIP) return q->getValue() > 0.5f ? "ON" : "OFF";
        if(row>=Metropolix::PITCH_OVERRIDE || trackProperty==Metropolix::GATE_LENGTH)return q->getDisplayValueString();
        const auto& labels = shortChoices.empty() ? choices : shortChoices;
        if (!labels.empty()) return labels[metro::clamp(static_cast<int>(std::round(q->getValue())), 0, static_cast<int>(labels.size()) - 1)];
        int precision = row == Metropolix::CV ? 2 : decimals;
        return string::f(precision == 2 ? "%.2f" : precision == 1 ? "%.1f" : "%.0f", q->getValue());
    }
    void draw(const DrawArgs& args) override {
        font(args.vg);
        rect(args.vg, 0, 0, box.size.x, box.size.y, dark ? ink() : nvgRGB(244, 243, 235));
        line(args.vg, 2, box.size.y - 1, box.size.x - 2, box.size.y - 1, nvgRGB(190, 191, 184));
        text(args.vg, box.size.x / 2, box.size.y / 2, locked()?string::f("C%d ",metroModule->ctrlOwner(paramId)+1)+valueText():valueText(), 10.5f,
             dark ? paper() : ink(), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    }
    bool toggleCell() const {
        int row = compact && metroModule ? metroModule->selectedRow.load() : stageRow;
        return row == Metropolix::SLIDE || row == Metropolix::SKIP || modProperty >= Metropolix::MOD_RAMP || modGateCell();
    }
    bool modGateCell() const {
        if (!metroModule || modProperty < Metropolix::MOD_VALUE || modProperty >= Metropolix::MOD_RAMP) return false;
        int lane = metroModule->selectedLane.load();
        return metroModule->integer(Metropolix::modParam(lane, Metropolix::MOD_TYPE)) == metro::ModGate
            || metro::modGateDestination(static_cast<metro::ModDestination>(metroModule->integer(Metropolix::modParam(lane, Metropolix::MOD_DESTINATION))));
    }
    void onDragStart(const event::DragStart& e) override { if(e.button!=GLFW_MOUSE_BUTTON_LEFT)return; dragDistance = 0.f; dragStarted=!locked(); if(dragStarted)ValueKnob::onDragStart(e); }
    void onDragMove(const event::DragMove& e) override { dragDistance += e.mouseDelta.norm(); if(dragStarted && !locked())ValueKnob::onDragMove(e); }
    void onDragEnd(const event::DragEnd& e) override {
        // Automation may assign a CTRL while this drag is active. Always finish
        // a started Rack gesture so cursor capture and history are released.
        if(e.button!=GLFW_MOUSE_BUTTON_LEFT)return;
        if(dragStarted)ValueKnob::onDragEnd(e);
        dragStarted=false;
    }
    void onDoubleClick(const event::DoubleClick& e) override { if(!locked())app::Knob::onDoubleClick(e); }
    void onHover(const event::Hover& e) override { app::ParamWidget::onHover(e); }
    void onAction(const event::Action&) override {
        if(locked())return;
        if (dragDistance >= 3.f) return;
        if (toggleCell()) {
            if (auto* q = getParamQuantity()) {
                bool gate = modGateCell();
                setWithHistory(q, q->getValue() >= (gate ? 1.5f : .5f) ? 0.f : gate ? 5.f : 1.f);
            }
            return;
        }
        if (!choices.empty() && getParamQuantity()) {
            auto* menu = createMenu();
            auto* q = getParamQuantity();
            for (size_t i = 0; i < choices.size(); ++i) {
                menu->addChild(createCheckMenuItem(choices[i], "",
                    [q, i] { return static_cast<int>(std::round(q->getValue())) == static_cast<int>(i); },
                    [q, i] { setWithHistory(q, static_cast<float>(i)); }));
            }
            return;
        }
    }
    void onButton(const event::Button& e) override {
        if(locked() && e.button==GLFW_MOUSE_BUTTON_RIGHT && e.action==GLFW_PRESS) {
            appendCtrlMenu(createMenu(),metroModule,metroModule->ctrlOwner(paramId));e.consume(this);return;
        }
        app::ParamWidget::onButton(e);
    }
};

struct PitchFader : app::Knob {
    PitchFader() { box.size = Vec(28, 109); forceLinear = true; smooth = false; speed = 0.65f; }
    void onHover(const event::Hover& e) override { app::ParamWidget::onHover(e); }
    void onButton(const event::Button& e) override { app::ParamWidget::onButton(e); }
    void draw(const DrawArgs& args) override {
        float value = getParamQuantity() ? getParamQuantity()->getValue() : 0.35f;
        float y = 8 + (1.f - value) * 91.f;
        for (int i = 0; i <= 6; ++i)
            line(args.vg, 2, 8 + i * 91.f / 6.f, 26, 8 + i * 91.f / 6.f, nvgRGB(176, 179, 174));
        rect(args.vg, 11, 5, 6, 99, ink(), 2);
        rect(args.vg, 3, y - 7, 24, 16, nvgRGBA(0, 0, 0, 40), 2);
        rect(args.vg, 1, y - 9, 24, 16, ink(), 2);
        rect(args.vg, 4, y - 3, 18, 3, nvgRGB(228, 175, 89), 1);
    }
};

struct TextButton : widget::OpaqueWidget {
    std::function<void()> action;
    std::function<void()> rightAction;
    std::function<std::string()> label;
    std::function<bool()> active;
    NVGcolor color = orange();
    void draw(const DrawArgs& args) override {
        font(args.vg);
        bool on = active && active();
        rect(args.vg, 0, 0, box.size.x, box.size.y, on ? color : ink());
        text(args.vg, box.size.x / 2, box.size.y / 2, label ? label() : "", 10,
             nvgRGB(251, 248, 237), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    }
    void onButton(const event::Button& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS && rightAction) {
            rightAction();
            e.consume(this);
            return;
        }
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            if (action) action();
            e.consume(this);
        } else widget::OpaqueWidget::onButton(e);
    }
};

struct TransportButton : app::Switch {
    bool reset = false;
    TransportButton() { box.size = Vec(16, 16); }
    void draw(const DrawArgs& args) override {
        bool on = getParamQuantity() && getParamQuantity()->getValue() > 0.5f;
        nvgBeginPath(args.vg); nvgCircle(args.vg,8,8,7.7f);
        nvgFillColor(args.vg,nvgRGB(96,98,99)); nvgFill(args.vg);
        nvgBeginPath(args.vg); nvgCircle(args.vg,8,8,6.8f);
        nvgFillPaint(args.vg,nvgLinearGradient(args.vg,3,2,12,14,
            on ? (reset?nvgRGB(243,103,92):nvgRGB(131,210,151)) : nvgRGB(211,217,224),
            on ? (reset?nvgRGB(161,49,48):nvgRGB(57,139,82)) : nvgRGB(169,178,192)));
        nvgFill(args.vg);
    }
};

struct MainDisplay : widget::Widget {
    Metropolix* module = nullptr;
    void draw(const DrawArgs& args) override {
        font(args.vg);
        rect(args.vg, 0, 0, box.size.x, box.size.y, nvgRGB(26, 36, 38), 5);
        int t = module ? module->selectedTrack.load() : 0;
        int stage = module ? module->playStage[t].load() : -1;
        int pulse = module ? module->playPulse[t].load() : 0;
        bool external = module && module->externalClock.load();
        text(args.vg, 10, 13, t ? "TRACK 2" : "TRACK 1", 10, trackColor(t));
        bool reversed = module && module->params[Metropolix::extraParam(t, Metropolix::REVERSED)].getValue() > 0.5f;
        text(args.vg, box.size.x - 10, 13, std::string(reversed ? "REV / " : "") + (external ? "EXT" : "INT"), 9, paper(), NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        text(args.vg, 10, 34, stage < 0 ? "READY" : string::f("STAGE %d  /  PULSE %d", stage + 1, pulse + 1), 12, paper());
        float pitch = module ? module->playPitch[t].load() : 0.f;
        int semi = static_cast<int>(std::round(pitch * 12.f));
        int pitchClass = metro::modulo(semi, 12);
        text(args.vg, 10, 54, string::f("%s%d", notes[pitchClass], 4 + (semi - pitchClass) / 12), 12, paper());
        for (int s = 0; s < 8; ++s)
            rect(args.vg, 64 + s * 12, 49, 8, 9, stage == s ? trackColor(t) : nvgRGB(71, 80, 79), 1);
    }
};

#include "HardwareUI.hpp"
#include "PresetUI.hpp"
#include "ChainUI.hpp"

struct Panel : widget::Widget {
    Metropolix* module = nullptr;
    Panel() {
        auto* cache=new widget::FramebufferWidget;
        cache->box.size=Vec(HARDWARE_WIDTH,RACK_GRID_HEIGHT);
        auto* art=new HardwarePanel; art->box.size=cache->box.size;
        cache->addChild(art); addChild(cache);
    }
    void draw(const DrawArgs& args) override {
        widget::Widget::draw(args);
        if (box.size.x <= HARDWARE_WIDTH) return;
        nvgSave(args.vg);
        nvgTranslate(args.vg, HARDWARE_WIDTH - 600, 0);
        font(args.vg);
        drawExtra(args);
        nvgRestore(args.vg);
    }
    void drawExtra(const DrawArgs& args) {
        if (box.size.x > HARDWARE_WIDTH) {
            rect(args.vg, 600, 0, 480, 380, nvgRGB(215, 218, 208), 0);
            rect(args.vg, 600, 0, 480, 34, nvgRGB(50, 62, 61), 0);
            if (module && module->modEditor.load()) {
                int lane = module->selectedLane.load();
                text(args.vg, 620, 18, "MOD LANES", 14, paper());
                text(args.vg, 1060, 18, string::f("MOD %d  /  %+.2f V", lane + 1, module->modVoltage[lane].load()), 10, paper(), NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
                const float xs[] = {620,680,740,800,934,994};
                const char* labels[] = {"LENGTH","OFFSET","DIVIDE","PLAY ORDER","MUTE","CV TYPE"};
                for (int i = 0; i < 6; ++i) text(args.vg, xs[i], 81, labels[i], 8, muted());
                for (int s = 0; s < 8; ++s) {
                    bool playing = module->modStage[lane].load() == s;
                    text(args.vg, 644 + 55 * s, 137, std::to_string(s + 1), 10, playing ? orange() : muted(), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
                }
                text(args.vg, 620, 122, "STAGE VOLTAGES / GATES", 8, muted());
                text(args.vg, 620, 177, "TRANSITION INTO EACH STAGE", 8, muted());
                text(args.vg, 620, 279, "INTERNAL DESTINATION", 8, muted());
                text(args.vg, 854, 279, "APPLY TO", 8, muted());
                text(args.vg, 620, 326, "OUTPUT A SOURCE", 8, muted());
                text(args.vg, 848, 326, "OUTPUT B SOURCE", 8, muted());
                text(args.vg, 620, 370, "Right-click a lane number for direction and pulse length.", 8.5f, muted());
                return;
            }
            text(args.vg, 620, 18, "STAGE EDITOR", 14, paper());
            text(args.vg, 1060, 18, "SELECTED TRACK", 9, nvgRGB(177, 202, 190), NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
            const float positions[] = {620, 680, 740, 800, 934};
            const char* labels2[] = {"LENGTH", "OFFSET", "DIVIDE", "PLAY ORDER", "MUTE"};
            for (int i = 0; i < 5; ++i) text(args.vg, positions[i], 48, labels2[i], 8, muted());
            const char* labels3[] = {"TRANSPOSE", "GATE %", "SLIDE %", "ACC LIMIT +", "SWING %"};
            for (int i = 0; i < 5; ++i) text(args.vg, 620 + i * 85, 95, labels3[i], 8, muted());
            line(args.vg, 620, 132, 1060, 132, nvgRGB(170, 178, 169));
            for (int s = 0; s < 8; ++s) text(args.vg, 713 + s * 44, 146, std::to_string(s + 1), 9, muted(), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            for (int row = 0; row < 6; ++row)
                text(args.vg, 620, 169 + row * 30, rowNames[row], row == 1 ? 8 : 9, ink());
            text(args.vg, 620, 351, "Drag a value to edit. Click ON / OFF to toggle.", 9, muted());
            text(args.vg, 620, 367, "Right-click a stage number for its trigger settings.", 9, muted());
        }
    }
};

struct ModGraph : widget::Widget {
    Metropolix* module = nullptr;
    void draw(const DrawArgs& args) override {
        rect(args.vg, 0, 0, box.size.x, box.size.y, ink());
        line(args.vg, 0, 25, box.size.x, 25, nvgRGB(92, 102, 99));
        if (!module) return;
        int lane = module->selectedLane.load();
        int stage = module->modStage[lane].load();
        int type = module->integer(Metropolix::modParam(lane, Metropolix::MOD_TYPE));
        bool gated = type == metro::ModGate || metro::modGateDestination(static_cast<metro::ModDestination>(module->integer(Metropolix::modParam(lane, Metropolix::MOD_DESTINATION))));
        float lastY = 25;
        for (int s = 0; s < 8; ++s) {
            float value = module->params[Metropolix::modParam(lane, Metropolix::MOD_VALUE + s)].getValue();
            if (type == metro::ModUnipolar) value = std::max(0.f, value);
            if (gated) value = value >= 1.5f ? 5.f : 0.f;
            float y = 25 - value * 4;
            float x = s * 55;
            bool ramp = !gated && module->integer(Metropolix::modParam(lane, Metropolix::MOD_RAMP + s));
            if (stage == s) rect(args.vg, x, 1, 54, 48, nvgRGBA(215, 103, 55, 35), 0);
            if (ramp) line(args.vg, x, lastY, x + 54, y, paper());
            else {
                line(args.vg, x, lastY, x, y, teal());
                line(args.vg, x, y, x + 54, y, teal());
            }
            lastY = y;
        }
    }
};
} // namespace

struct MetropolixWidget : ModuleWidget {
    Panel* panel;
    widget::Widget* editor;
    widget::Widget* modPanel;
    bool isExpanded = false;
#ifdef METROPOLIX_QA
    int qaFrame = 0;
    bool qaFailed = false;
    void qaStep();
    void videoStep();
#endif

    ValueCell* cell(widget::Widget* parent, Metropolix* module, int id, float x, float y, float width) {
        auto* c = createParam<ValueCell>(Vec(x, y), module, id);
        c->metroModule = module;
        c->box.size.x = width;
        parent->addChild(c);
        return c;
    }
    TextButton* button(widget::Widget* parent, float x, float y, float width,
                       std::function<std::string()> label, std::function<void()> action) {
        auto* b = new TextButton;
        b->box = Rect(Vec(x, y), Vec(width, 23));
        b->label = label;
        b->action = action;
        parent->addChild(b);
        return b;
    }

    void buildHardware(Metropolix* module);
    explicit MetropolixWidget(Metropolix* module) {
        setModule(module);
        panel = new Panel;
        panel->module = module;
        panel->box.size = Vec(HARDWARE_WIDTH, RACK_GRID_HEIGHT);
        setPanel(panel);
        buildHardware(module);

        editor = new widget::Widget;
        editor->box = Rect(Vec(HARDWARE_WIDTH, 0), Vec(EDITOR_WIDTH, 380));
        addChild(editor);
        const int props[] = {Metropolix::LENGTH, Metropolix::OFFSET, Metropolix::DIVISION, Metropolix::ORDER, Metropolix::MUTE};
        const float xs[] = {20, 80, 140, 200, 334};
        for (int i = 0; i < 5; ++i) {
            auto* c = cell(editor, module, Metropolix::trackParam(0, props[i]), xs[i], 57, i == 3 ? 120 : 49);
            c->trackProperty = props[i];
            if (i == 3) c->choices = Metropolix::orderLabels();
            if (i == 4) c->choices = {"OFF", "ON"};
        }
        const int props2[] = {Metropolix::TRANSPOSE, Metropolix::GATE_LENGTH, Metropolix::SLIDE_TIME, Metropolix::ACCUM_LIMIT};
        for (int i = 0; i < 4; ++i) {
            auto* c = cell(editor, module, Metropolix::trackParam(0, props2[i]), 20 + i * 85, 103, 72);
            c->trackProperty = props2[i];
        }
        cell(editor,module,Metropolix::swingParam(0),360,103,72)->swingProperty=true;
        for (int row = 0; row < 6; ++row) {
            for (int s = 0; s < 8; ++s) {
                auto* c = cell(editor, module, Metropolix::stageParam(0, s, row), 94 + s * 44, 157 + row * 30, 38);
                c->stageColumn = s;
                c->stageRow = row;
            }
        }
        editor->visible = false;
        modPanel = new widget::Widget;
        modPanel->box = Rect(Vec(HARDWARE_WIDTH, 0), Vec(EDITOR_WIDTH, 380));
        addChild(modPanel);
        for (int lane = 0; lane < 8; ++lane) {
            auto* b = button(modPanel, 20 + lane * 55, 46, 48, [lane] { return "MOD " + std::to_string(lane + 1); },
                [module, lane] { if (module) module->selectedLane.store(lane); });
            b->active = [module, lane] { return module && module->selectedLane.load() == lane; };
            b->rightAction = [module, lane] { if (module) { module->selectedLane.store(lane); modSettingsMenu(module, lane); } };
        }
        auto modCell = [&](int property, float x, float y, float width) {
            auto* c = cell(modPanel, module, Metropolix::modParam(0, property), x, y, width);
            c->modProperty = property;
            return c;
        };
        modCell(Metropolix::MOD_LENGTH, 20, 90, 49);
        modCell(Metropolix::MOD_OFFSET, 80, 90, 49);
        modCell(Metropolix::MOD_DIVISION, 140, 90, 49);
        modCell(Metropolix::MOD_ORDER, 200, 90, 120)->choices = Metropolix::orderLabels();
        modCell(Metropolix::MOD_MUTE, 334, 90, 49)->choices = {"OFF", "ON"};
        modCell(Metropolix::MOD_TYPE, 394, 90, 66)->choices = {"Bipolar", "Unipolar", "Gate"};
        for (int stage = 0; stage < 8; ++stage) {
            modCell(Metropolix::MOD_VALUE + stage, 20 + stage * 55, 147, 48)->decimals = 2;
            modCell(Metropolix::MOD_RAMP + stage, 20 + stage * 55, 185, 48);
        }
        auto* graph = new ModGraph;
        graph->module = module;
        graph->box = Rect(Vec(20,217), Vec(440,50));
        modPanel->addChild(graph);
        modCell(Metropolix::MOD_DESTINATION, 20, 289, 220)->choices = Metropolix::modDestinations();
        modCell(Metropolix::MOD_ASSIGNMENT, 254, 289, 206)->choices = {"Track 1", "Track 2", "Both tracks"};
        cell(modPanel, module, Metropolix::OUTPUT_SOURCE_A, 20, 336, 212)->choices = Metropolix::outputSources();
        cell(modPanel, module, Metropolix::OUTPUT_SOURCE_B, 248, 336, 212)->choices = Metropolix::outputSources();
        modPanel->visible = false;
    }

    void step() override {
        auto* m = getModule<Metropolix>();
        if (m) pumpPresetUi(m);
        bool expanded = m && m->expanded.load();
        if (expanded != isExpanded) {
            isExpanded = expanded;
            box.size.x = expanded ? HARDWARE_WIDTH + EDITOR_WIDTH : HARDWARE_WIDTH;
            panel->box.size.x = box.size.x;
            if (parent && APP->scene && APP->scene->rack)
                APP->scene->rack->setModulePosNearest(this, box.pos);
        }
        editor->visible = expanded && m && !m->modEditor.load();
        modPanel->visible = expanded && m && m->modEditor.load();
        ModuleWidget::step();
#ifdef METROPOLIX_QA
        qaStep();
#endif
    }
    void appendContextMenu(ui::Menu* menu) override {
        auto* m = getModule<Metropolix>();
        if (!m) return;
        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createCheckMenuItem("Expanded stage editor", "",
            [m] { return m->expanded.load() && !m->modEditor.load(); }, [m] { bool open=!m->expanded.load() || m->modEditor.load(); m->modEditor.store(false); m->uiMod.store(false); m->expanded.store(open); }));
        menu->addChild(createMenuItem("Selected track settings", "", [m] { trackSettingsMenu(m); }));
        menu->addChild(createMenuItem("Selected stage settings", "", [m] { stageSettingsMenu(m, m->selectedStage.load()); }));
        menu->addChild(createMenuItem("Open MOD lanes", "", [m] { m->expanded.store(true); m->modEditor.store(true); m->uiMod.store(true); }));
        menu->addChild(createMenuItem("Selected MOD lane settings", "", [m] { modSettingsMenu(m, m->selectedLane.load()); }));
        menu->addChild(createMenuLabel("Assign A/B sources in MOD lanes. External clock = 1 pulse / 16th."));
        appendPresetMenu(menu, m);
    }
};

#include "HardwareLayout.hpp"

#ifdef METROPOLIX_QA
#include "../tests/RackVideoQa.hpp"
#include "../tests/WidgetQa.hpp"
#endif

Model* modelMetropolix = createModel<Metropolix, MetropolixWidget>("Metropolix");
