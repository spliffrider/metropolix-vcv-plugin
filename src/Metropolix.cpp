#ifdef METROPOLIX_QA
// Test builds use Rack's renderer capture; no test hooks enter the release DLL.
#include <window/Window.hpp>
#undef PRIVATE
#include <fstream>
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

const char* rowNames[] = {"RATCHETS", "PROBABILITY", "ACCUM", "SLIDE", "SKIP", "CV"};
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
            [q, offset](size_t index) { setWithHistory(q, static_cast<float>(index + offset)); }));
    };
    choice("Direction", Metropolix::extraParam(t, Metropolix::REVERSED), {"Forward", "Reverse"});
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
    choice("Accumulator order", Metropolix::accumParam(t, Metropolix::ACCUM_ORDER), {"Wrap", "Pendulum", "Random", "Hold"});
    choice("Accumulator polarity", Metropolix::accumParam(t, Metropolix::BIPOLAR), {"Unipolar", "Bipolar"});
    std::vector<std::string> limits;
    for (int i = 1; i <= 28; ++i) limits.push_back(std::to_string(i) + " degrees");
    choice("Positive accumulation limit", Metropolix::trackParam(t, Metropolix::ACCUM_LIMIT), limits, 1);
    choice("Negative accumulation limit", Metropolix::accumParam(t, Metropolix::NEGATIVE_LIMIT), limits, 1);
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

// All views bind to the same ParamQuantity, preserving Rack automation, reset,
// numeric entry, patch serialization, and undo in both compact and open views.
struct ValueCell : app::Knob {
    Metropolix* metroModule = nullptr;
    int trackProperty = -1;
    int stageColumn = -1;
    int stageRow = -1;
    int modProperty = -1;
    bool compact = false;
    bool dark = false;
    int decimals = 0;
    std::vector<std::string> choices;
    std::vector<std::string> shortChoices;

    ValueCell() {
        box.size = Vec(40, 24);
        smooth = false;
        forceLinear = true;
        speed = 0.55f;
    }
    void step() override {
        if (metroModule) {
            int t = metroModule->selectedTrack.load();
            if (trackProperty >= 0) paramId = Metropolix::trackParam(t, trackProperty);
            if (stageRow >= 0) paramId = Metropolix::stageParam(t, stageColumn, stageRow);
            if (compact) paramId = Metropolix::stageParam(t,
                metroModule->selectedStage.load(), metroModule->selectedRow.load());
            if (modProperty >= 0) paramId = Metropolix::modParam(metroModule->selectedLane.load(), modProperty);
        }
        app::Knob::step();
    }
    std::string valueText() {
        auto* q = getParamQuantity();
        if (!q) return "--";
        if (modProperty >= Metropolix::MOD_RAMP) return q->getValue() > .5f ? "RAMP" : "STEP";
        if (modGateCell()) return q->getValue() >= 1.5f ? "ON" : "OFF";
        int row = compact && metroModule ? metroModule->selectedRow.load() : stageRow;
        if (row == Metropolix::SLIDE || row == Metropolix::SKIP) return q->getValue() > 0.5f ? "ON" : "OFF";
        const auto& labels = shortChoices.empty() ? choices : shortChoices;
        if (!labels.empty()) return labels[metro::clamp(static_cast<int>(std::round(q->getValue())), 0, static_cast<int>(labels.size()) - 1)];
        int precision = row == Metropolix::CV ? 2 : decimals;
        return string::f(precision == 2 ? "%.2f" : precision == 1 ? "%.1f" : "%.0f", q->getValue());
    }
    void draw(const DrawArgs& args) override {
        font(args.vg);
        rect(args.vg, 0, 0, box.size.x, box.size.y, dark ? ink() : nvgRGB(244, 243, 235));
        line(args.vg, 2, box.size.y - 1, box.size.x - 2, box.size.y - 1, nvgRGB(190, 191, 184));
        text(args.vg, box.size.x / 2, box.size.y / 2, valueText(), 10.5f,
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
    void onDragStart(const event::DragStart& e) override { if (!toggleCell() && choices.empty()) app::Knob::onDragStart(e); }
    void onDragMove(const event::DragMove& e) override { if (!toggleCell() && choices.empty()) app::Knob::onDragMove(e); }
    void onDragEnd(const event::DragEnd& e) override { if (!toggleCell() && choices.empty()) app::Knob::onDragEnd(e); }
    void onHover(const event::Hover& e) override { app::ParamWidget::onHover(e); }
    void onButton(const event::Button& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && toggleCell()) {
            if (auto* q = getParamQuantity()) {
                bool gate = modGateCell();
                setWithHistory(q, q->getValue() >= (gate ? 1.5f : .5f) ? 0.f : gate ? 5.f : 1.f);
            }
            e.consume(this);
            return;
        }
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && !choices.empty() && getParamQuantity()) {
            auto* menu = createMenu();
            auto* q = getParamQuantity();
            for (size_t i = 0; i < choices.size(); ++i) {
                menu->addChild(createCheckMenuItem(choices[i], "",
                    [q, i] { return static_cast<int>(std::round(q->getValue())) == static_cast<int>(i); },
                    [q, i] { setWithHistory(q, static_cast<float>(i)); }));
            }
            e.consume(this);
            return;
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
    TransportButton() { box.size = Vec(86, 25); }
    void draw(const DrawArgs& args) override {
        font(args.vg);
        bool on = getParamQuantity() && getParamQuantity()->getValue() > 0.5f;
        rect(args.vg, 0, 0, box.size.x, box.size.y, on ? orange() : ink());
        text(args.vg, box.size.x / 2, box.size.y / 2, reset ? "RESET" : on ? "STOP" : "RUN", 11,
             paper(), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
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

struct Panel : widget::Widget {
    Metropolix* module = nullptr;
    void draw(const DrawArgs& args) override {
        font(args.vg);
        rect(args.vg, 0, 0, box.size.x, box.size.y, paper(), 0);
        rect(args.vg, 0, 0, 600, 34, ink(), 0);
        text(args.vg, 18, 18, "METROPOLIX", 18, paper());
        text(args.vg, 584, 18, "LAB  /  03", 10, nvgRGB(220, 167, 100), NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        line(args.vg, 385, 44, 385, 306, nvgRGB(176, 180, 173));
        text(args.vg, 18, 44, "SHARED STAGES", 8, muted());
        text(args.vg, 18, 202, "PULSE COUNT", 8, muted());
        text(args.vg, 18, 246, "GATE TYPE", 8, muted());
        text(args.vg, 18, 287, "ROOT", 8, muted());
        text(args.vg, 82, 287, "SCALE", 8, muted());
        text(args.vg, 399, 207, "TEMPO", 8, muted());
        text(args.vg, 504, 207, "SLIDER RANGE", 8, muted());
        text(args.vg, 399, 251, "LENGTH", 8, muted());
        text(args.vg, 459, 251, "DIVIDE", 8, muted());
        text(args.vg, 519, 251, "ORDER", 8, muted());
        line(args.vg, 18, 317, 582, 317, nvgRGB(176, 180, 173));
        const float xs[] = {31, 84, 182, 229, 291, 338, 413, 488, 550};
        const char* labels[] = {"CLK IN", "RESET", "PITCH 1", "GATE 1", "PITCH 2", "GATE 2", "CLK OUT", "OUT A", "OUT B"};
        for (int i = 0; i < 9; ++i)
            text(args.vg, xs[i], 357, labels[i], 8, i >= 2 && i < 4 ? orange() : i >= 4 && i < 6 ? teal() : ink(), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        text(args.vg, 300, 373, "TWO TRACKS  /  ONE SET OF EIGHT STAGES", 7.5f, muted(), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        if (box.size.x > 600) {
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
            const char* labels3[] = {"TRANSPOSE", "GATE %", "SLIDE %", "ACC LIMIT +"};
            for (int i = 0; i < 4; ++i) text(args.vg, 620 + i * 85, 95, labels3[i], 8, muted());
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

    explicit MetropolixWidget(Metropolix* module) {
        setModule(module);
        panel = new Panel;
        panel->module = module;
        panel->box.size = Vec(600, RACK_GRID_HEIGHT);
        setPanel(panel);
        for (int s = 0; s < 8; ++s) {
            float x = 36.f + s * 46.f;
            auto* b = button(this, x - 17, 54, 34, [s] { return std::to_string(s + 1); },
                [module, s] { if (module) module->selectedStage.store(s); });
            b->active = [module, s] { return module && module->selectedStage.load() == s; };
            b->rightAction = [module, s] { if (module) { module->selectedStage.store(s); stageSettingsMenu(module, s); } };
            addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x - 7, 81), module, Metropolix::POSITION_1 + s));
            addChild(createLightCentered<SmallLight<GreenLight>>(Vec(x + 7, 81), module, Metropolix::POSITION_2 + s));
            addParam(createParam<PitchFader>(Vec(x - 14, 87), module, Metropolix::PITCH + s));
            cell(this, module, Metropolix::PULSES + s, x - 18, 211, 36);
            cell(this, module, Metropolix::GATES + s, x - 21, 255, 42)->choices = {"REST", "SNGL", "MULT", "HOLD"};
        }
        auto* display = new MainDisplay;
        display->module = module;
        display->box = Rect(Vec(399, 44), Vec(184, 68));
        addChild(display);
        for (int t = 0; t < 2; ++t) {
            auto* b = button(this, 399 + t * 96, 120, 88, [t] { return t ? "TRACK 2" : "TRACK 1"; },
                [module, t] { if (module) module->selectedTrack.store(t); });
            b->active = [module, t] { return module && module->selectedTrack.load() == t; };
            b->color = trackColor(t);
        }
        addParam(createParam<TransportButton>(Vec(399, 150), module, Metropolix::RUN));
        auto* reset = createParam<TransportButton>(Vec(497, 150), module, Metropolix::RESET);
        reset->reset = true;
        reset->momentary = true;
        addParam(reset);
        button(this, 399, 181, 118, [module] {
            int row = module ? module->selectedRow.load() : 0;
            int s = module ? module->selectedStage.load() + 1 : 1;
            return string::f("%d: %s", s, rowNames[row]);
        }, [module] {
            if (!module) return;
            auto* menu = createMenu();
            for (int row = 0; row < 6; ++row)
                menu->addChild(createCheckMenuItem(rowNames[row], "",
                    [module, row] { return module->selectedRow.load() == row; },
                    [module, row] { module->selectedRow.store(row); }));
        });
        auto* compact = cell(this, module, Metropolix::stageParam(0, 0, 0), 525, 181, 58);
        compact->compact = true;
        compact->dark = true;
        cell(this, module, Metropolix::TEMPO, 399, 216, 93)->decimals = 1;
        cell(this, module, Metropolix::RANGE, 505, 216, 78);
        const int properties[] = {Metropolix::LENGTH, Metropolix::DIVISION, Metropolix::ORDER};
        for (int i = 0; i < 3; ++i) {
            auto* c = cell(this, module, Metropolix::trackParam(0, properties[i]), 399 + i * 60, 259, i == 2 ? 64 : 51);
            c->trackProperty = properties[i];
            if (i == 2) {
                c->choices = Metropolix::orderLabels();
                c->shortChoices = {"Linear", "PingPong", "Pendulum", "Odd/Even", "Jump 2", "Jump 3", "Jump 4", "Jump 5",
                    "Hop", "Arp3/-1", "Arp4/-2", "Arp4/-1", "Converge", "Diverge", "Growing", "Shrink", "Pedal", "Brownian", "Shuffle", "Quasi", "Random"};
            }
        }
        cell(this, module, Metropolix::ROOT, 18, 294, 55)->choices = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        cell(this, module, Metropolix::SCALE, 82, 294, 113)->choices = {"Chromatic", "Major", "Minor", "Dorian", "Mixolydian", "Major pent.", "Minor pent."};
        button(this, 215, 294, 158, [module] { return module && module->expanded.load() && !module->modEditor.load() ? "CLOSE STAGE EDITOR" : "OPEN STAGE EDITOR"; },
            [module] { if (module) {
                bool open = !module->expanded.load() || module->modEditor.load();
                module->modEditor.store(false);
                module->expanded.store(open);
            } });
        auto* modButton = button(this, 399, 294, 76, [] { return "MOD LANES"; }, [module] {
            if (!module) return;
            bool open = !module->expanded.load() || !module->modEditor.load();
            module->modEditor.store(true);
            module->expanded.store(open);
        });
        modButton->active = [module] { return module && module->expanded.load() && module->modEditor.load(); };
        button(this, 483, 294, 100, [] { return "TRACK SETUP"; }, [module] { trackSettingsMenu(module); });
        addInput(createInputCentered<PJ301MPort>(Vec(31, 336), module, Metropolix::CLOCK_IN));
        addInput(createInputCentered<PJ301MPort>(Vec(84, 336), module, Metropolix::RESET_IN));
        const float positions[] = {182, 229, 291, 338, 413, 488, 550};
        for (int i = 0; i < 7; ++i)
            addOutput(createOutputCentered<PJ301MPort>(Vec(positions[i], 336), module, i));

        editor = new widget::Widget;
        editor->box = Rect(Vec(600, 0), Vec(480, 380));
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
        for (int row = 0; row < 6; ++row) {
            for (int s = 0; s < 8; ++s) {
                auto* c = cell(editor, module, Metropolix::stageParam(0, s, row), 94 + s * 44, 157 + row * 30, 38);
                c->stageColumn = s;
                c->stageRow = row;
            }
        }
        editor->visible = false;
        modPanel = new widget::Widget;
        modPanel->box = Rect(Vec(600, 0), Vec(480, 380));
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
        bool expanded = m && m->expanded.load();
        if (expanded != isExpanded) {
            isExpanded = expanded;
            box.size.x = expanded ? 1080 : 600;
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
            [m] { return m->expanded.load(); }, [m] { m->expanded.store(!m->expanded.load()); }));
        menu->addChild(createMenuItem("Selected track settings", "", [m] { trackSettingsMenu(m); }));
        menu->addChild(createMenuItem("Selected stage settings", "", [m] { stageSettingsMenu(m, m->selectedStage.load()); }));
        menu->addChild(createMenuItem("Open MOD lanes", "", [m] { m->expanded.store(true); m->modEditor.store(true); }));
        menu->addChild(createMenuItem("Selected MOD lane settings", "", [m] { modSettingsMenu(m, m->selectedLane.load()); }));
        menu->addChild(createMenuLabel("Assign A/B sources in MOD lanes. External clock = 1 pulse / 16th."));
    }
};

#ifdef METROPOLIX_QA
#include "../tests/WidgetQa.hpp"
#endif

Model* modelMetropolix = createModel<Metropolix, MetropolixWidget>("Metropolix");
