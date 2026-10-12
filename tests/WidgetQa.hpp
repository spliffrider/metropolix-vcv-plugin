// Included only by the dedicated Rack UI test build. Run scripts/test-ui.ps1.
// Events are sent to our widgets, not to the operating system. These tests do
// not certify native mouse capture or operating-system focus behavior.
void MetropolixWidget::qaStep() {
    const char* directory = std::getenv("METROPOLIX_QA_DIR");
    auto* m = getModule<Metropolix>();
    if (!directory || !m) return;
    ++qaFrame;
    const std::string dir(directory);
    if (std::getenv("METROPOLIX_AUDIO_QA")) {
        if (qaFrame == 180) APP->window->screenshot(dir + "/two-voice.png");
        return;
    }
    auto check = [&](bool condition, const std::string& description) {
        std::ofstream log(dir + "/report.txt", std::ios::app);
        log << (condition ? "PASS " : "FAIL ") << description << '\n';
        qaFailed |= !condition;
    };
    auto click = [&](widget::Widget* w) {
        if (!w) { check(false, "requested widget exists"); return; }
        widget::EventContext context;
        event::Button event;
        event.context = &context;
        event.pos = w->box.size.div(2);
        event.button = GLFW_MOUSE_BUTTON_LEFT;
        event.action = GLFW_PRESS;
        event.mods = 0;
        w->onButton(event);
        check(context.consumed && context.target == w, "widget consumes its click");
    };
    auto findButton = [&](const std::string& label) -> TextButton* {
        for (auto* child : children) {
            auto* button = dynamic_cast<TextButton*>(child);
            if (button && button->label && button->label() == label) return button;
        }
        return nullptr;
    };
    if (qaFrame == 30) {
        m->selectedTrack.store(0);
        m->selectedStage.store(4);
        m->selectedRow.store(Metropolix::RATCHETS);
        m->expanded.store(true);
    }
    if (qaFrame == 35) {
        ValueCell* compact = nullptr;
        ValueCell* grid = nullptr;
        ValueCell* slide = nullptr;
        for (auto* child : children) {
            auto* cell = dynamic_cast<ValueCell*>(child);
            if (cell && cell->compact) compact = cell;
        }
        for (auto* child : editor->children) {
            auto* cell = dynamic_cast<ValueCell*>(child);
            if (!cell || cell->stageColumn != 4) continue;
            if (cell->stageRow == Metropolix::RATCHETS) grid = cell;
            if (cell->stageRow == Metropolix::SLIDE) slide = cell;
        }
        check(compact && grid && slide, "compact and grid controls exist");
        if (compact && grid && slide) {
            auto* q = grid->getParamQuantity();
            check(q && compact->getParamQuantity() == q, "both editors bind to identical parameter");
            if (q) {
                float before = q->getValue();
                setWithHistory(q, 4.f);
                check(compact->getParamQuantity()->getValue() == 4.f, "grid edit appears in compact editor");
                APP->history->undo();
                check(q->getValue() == before, "Rack undo restores stage edit");
                APP->history->redo();
                check(q->getValue() == 4.f, "Rack redo restores stage edit");
            }
            click(slide);
            check(slide->getParamQuantity()->getValue() == 1.f, "slide toggles once on click");
            APP->history->undo();
            check(slide->getParamQuantity()->getValue() == 0.f, "slide toggle supports undo");
        }
        check(box.size.x == 1080 && editor->visible, "expanded width and visibility");
        // A tall fader must accept clicks outside a knob's circular hit region.
        for (auto* child : children) {
            auto* fader = dynamic_cast<PitchFader*>(child);
            if (!fader) continue;
            widget::EventContext context;
            event::Button event;
            event.context = &context;
            event.pos = Vec(4, 100);
            event.button = GLFW_MOUSE_BUTTON_LEFT;
            event.action = GLFW_PRESS;
            event.mods = 0;
            fader->onButton(event);
            check(context.target == fader, "full rectangular fader hit area");
            break;
        }
        click(findButton("TRACK 2"));
    }
    if (qaFrame == 40) {
        check(m->selectedTrack.load() == 1, "track selector changes selected track");
        for (auto* child : children) {
            auto* c = dynamic_cast<ValueCell*>(child);
            if (c && c->compact) check(c->paramId == Metropolix::stageParam(1, 4, Metropolix::RATCHETS), "compact editor rebinds to track 2");
        }
        check(m->params[Metropolix::stageParam(1, 4, Metropolix::RATCHETS)].getValue() == 1.f, "track 2 stays independent");
        click(findButton("TRACK 1"));
    }
    if (qaFrame == 45) APP->window->screenshot(dir + "/expanded.png");
    if (qaFrame == 50) click(findButton("CLOSE STAGE EDITOR"));
    if (qaFrame == 55) {
        check(box.size.x == 600 && !editor->visible, "compact width and hidden editor");
        APP->window->screenshot(dir + "/compact.png");
    }
    if (qaFrame == 60) click(findButton("OPEN STAGE EDITOR"));
    if (qaFrame == 65) {
        check(box.size.x == 1080 && editor->visible, "editor reopens at full width");
        bool overlaps = false;
        for (auto* other : parent->children) {
            if (other != this && dynamic_cast<ModuleWidget*>(other) && box.intersects(other->box)) overlaps = true;
        }
        check(!overlaps, "expansion does not overlap another module");
        APP->patch->save(dir + "/roundtrip.vcv");
        json_t* data = APP->engine->moduleToJson(m);
        m->selectedTrack.store(1);
        m->expanded.store(false);
        APP->engine->moduleFromJson(m, data);
        json_decref(data);
        check(m->expanded.load() && m->selectedTrack.load() == 0, "host roundtrip restores interface state");
        check(m->params[Metropolix::stageParam(0, 4, Metropolix::RATCHETS)].getValue() == 4.f, "host roundtrip retains stage values");
    }
    if (qaFrame == 70) click(findButton("MOD LANES"));
    if (qaFrame == 75) {
        check(modPanel->visible && !editor->visible && box.size.x == 1080, "MOD editor replaces stage editor at the same width");
        m->selectedLane.store(3);
    }
    if (qaFrame == 80) {
        for (auto* child : modPanel->children) {
            auto* c = dynamic_cast<ValueCell*>(child);
            if (!c) continue;
            if (c->modProperty == Metropolix::MOD_VALUE + 2) {
                check(c->paramId == Metropolix::modParam(3, Metropolix::MOD_VALUE + 2), "MOD value rebinds to selected lane");
                setWithHistory(c->getParamQuantity(), -2.5f);
                check(c->getParamQuantity()->getValue() == -2.5f, "MOD value edit applies");
                APP->history->undo();
                check(c->getParamQuantity()->getValue() == 0.f, "MOD value supports undo");
                APP->history->redo();
            }
            if (c->modProperty == Metropolix::MOD_RAMP + 2) {
                click(c);
                check(c->getParamQuantity()->getValue() == 1.f, "MOD transition toggles to ramp");
            }
        }
        check(m->params[Metropolix::modParam(0, Metropolix::MOD_VALUE + 2)].getValue() == 0.f, "editing MOD 4 leaves MOD 1 unchanged");
        setWithHistory(m->getParamQuantity(Metropolix::OUTPUT_SOURCE_A), 5.f);
        json_t* data = APP->engine->moduleToJson(m);
        m->modEditor.store(false); m->selectedLane.store(0);
        APP->engine->moduleFromJson(m, data);
        json_decref(data);
        check(m->modEditor.load() && m->selectedLane.load() == 3, "MOD selection and view survive host save/load");
        check(m->params[Metropolix::OUTPUT_SOURCE_A].getValue() == 5.f, "A output assignment survives host save/load");
    }
    if (qaFrame == 85) {
        APP->window->screenshot(dir + "/mod-editor.png");
        setWithHistory(m->getParamQuantity(Metropolix::modParam(3, Metropolix::MOD_TYPE)), metro::ModGate);
    }
    if (qaFrame == 90) {
        for (auto* child : modPanel->children) {
            auto* c = dynamic_cast<ValueCell*>(child);
            if (c && c->modProperty == Metropolix::MOD_VALUE + 2) {
                check(c->modGateCell(), "gate-type MOD values become toggles");
                click(c);
                check(c->getParamQuantity()->getValue() == 5.f, "gate toggle emits a 5 V value");
            }
        }
        click(findButton("MOD LANES"));
    }
    if (qaFrame == 95) {
        check(box.size.x == 600 && !modPanel->visible && !editor->visible, "MOD editor can close to compact width");
        click(findButton("OPEN STAGE EDITOR"));
    }
    if (qaFrame == 100) {
        check(editor->visible && !modPanel->visible, "stage editor can reopen after MOD editor");
        check(!qaFailed, "UI QA COMPLETE");
        APP->window->close();
    }
}
