// UI-only actions. Rack history retains IDs and values, never module pointers.
struct PresetBankChange : history::ModuleAction {
    int slot = 0;
    Metropolix::Snapshot before, after;
    void set(const Metropolix::Snapshot& s) {
        if (auto* m = dynamic_cast<Metropolix*>(APP->engine->getModule(moduleId))) m->putPreset(slot, s);
    }
    void undo() override { set(before); }
    void redo() override { set(after); }
};
struct PresetLoadChange : history::ModuleAction {
    Metropolix::PresetRecall recall;
    void restore(bool undoing) {
        if (auto* m = dynamic_cast<Metropolix*>(APP->engine->getModule(moduleId))) {
            if(m->chainEnabled.load()){auto state=m->getChainState();state.enabled=false;m->setChainState(state);}
            Metropolix::PresetCommand c;
            c.kind = Metropolix::PresetCommand::Restore;
            c.snapshot = undoing ? recall.before : recall.after;
            c.other = undoing ? recall.after : recall.before;
            c.pickup=undoing?recall.pickupBefore:recall.pickupAfter;c.physical=undoing?recall.physicalBefore:recall.physicalAfter;c.pickupMask=recall.pickupMask;
            m->presetUiCommands.push_back(c);
        }
    }
    void undo() override { restore(true); }
    void redo() override { restore(false); }
};
void pumpPresetUi(Metropolix* m) {
    Metropolix::PresetRecall event;
    while (m->presetRecalls.pop(event)) {
        m->cancelDraft();
        auto* h = new PresetLoadChange;
        h->moduleId = m->id; h->recall = event;
        h->name = event.slot<0?"initialize Metropolix preset":string::f("load Metropolix preset %d-%d", event.slot / 8 + 1, event.slot % 8 + 1);
        APP->history->push(h);
    }
    m->sendPresetCommand();
    m->sendChainCommand();
    m->sendMidiBank();
}
bool presetPage(Metropolix* m) {
    return m && (m->uiPage.load() == Metropolix::LOAD_PAGE || m->uiPage.load() == Metropolix::SAVE_PAGE);
}
void openPresetPage(Metropolix* m, bool save) {
    if (!m) return;
    screenPage(m, save ? Metropolix::SAVE_PAGE : Metropolix::LOAD_PAGE);
    m->uiAlt.store(false); m->presetNotice.store(0);
    if (save) m->presetSaveAction.store(0);
}
void savePresetSlot(Metropolix* m, int slot, bool clear, bool exitAfter) {
    if (m->presetSaveBlocked()) { m->presetNotice.store(m->chainEnabled.load()?6:5); return; }
    pumpPresetUi(m);
    auto* h = new PresetBankChange;
    h->moduleId = m->id; h->slot = slot; h->before = m->getPreset(slot);
    h->name = string::f("%s Metropolix preset %d-%d", clear ? "clear" : "save", slot / 8 + 1, slot % 8 + 1);
    if (!clear) {
        json_t* json = APP->engine->moduleToJson(m);
        h->after = m->presetFromParamsJson(json_object_get(json, "params"),json_object_get(json,"data"));
        json_decref(json);
    }
    m->putPreset(slot, h->after);
    APP->history->push(h);
    m->presetNotice.store(clear ? 2 : 1);
    if (exitAfter && !clear) screenPage(m, Metropolix::HOME);
}
void confirmPresetSave(Metropolix* m, int slot, bool clear, bool exitAfter) {
    if (m->presetSaveBlocked()) { m->presetNotice.store(m->chainEnabled.load()?6:5); return; }
    bool occupied = m->getPreset(slot).occupied;
    if (clear && !occupied) { m->presetNotice.store(3); return; }
    if (!occupied) { savePresetSlot(m, slot, clear, exitAfter); return; }
    auto* menu = createMenu();
    menu->addChild(createMenuLabel(string::f("Preset %d-%d", slot / 8 + 1, slot % 8 + 1)));
    menu->addChild(createMenuItem(clear ? "Confirm clear" : "Confirm overwrite", "", [m, slot, clear, exitAfter] {
        savePresetSlot(m, slot, clear, exitAfter);
    }));
    menu->addChild(createMenuItem("Cancel", "", [] {}));
}
void activatePresetSlot(Metropolix* m, int stage) {
    int slot = m->presetBankIndex.load() * 8 + stage;
    if (m->uiPage.load() == Metropolix::LOAD_PAGE) m->requestPreset(slot);
    else {
        int action = m->presetSaveAction.load();
        confirmPresetSave(m, slot, action == 2, action == 0);
    }
}
void appendPresetMenu(ui::Menu* menu, Metropolix* m) {
    if (!m) return;
    appendChainMenu(menu,m);
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuLabel("Internal presets: 8 banks x 8 slots"));
    menu->addChild(createMenuItem("Load preset bank", "ALT + LOAD", [m] { openPresetPage(m, false); }));
    menu->addChild(createMenuItem("Save preset bank", "ALT + SAVE", [m] { openPresetPage(m, true); }));
    menu->addChild(createMenuItem("Preset settings", "", [m] { openPresetSetup(m); }));
    menu->addChild(createMenuItem("Cancel queued load", "", [m] { m->cancelPreset(); }, m->pendingSlot.load() < 0 && m->presetUiCommands.empty()));
    auto choice = [menu, m](const char* label, int id, std::vector<std::string> values) {
        auto* q = m->getParamQuantity(id);
        menu->addChild(createIndexSubmenuItem(label, values, [q] { return size_t(q->getValue()); },
            [q](size_t i) { setWithHistory(q, float(i)); }));
    };
    menu->addChild(createMenuItem("Initialize preset","",[m]{m->requestInitialize();}));
    menu->addChild(createIndexSubmenuItem("Set Init preset",dynamic_cast<SwitchQuantity*>(m->getParamQuantity(Metropolix::INIT_PRESET))->labels,[m]{return size_t(m->integer(Metropolix::INIT_PRESET));},[m](size_t slot){setWithHistory(m->getParamQuantity(Metropolix::INIT_PRESET),float(slot));}));
    choice("Load delay", Metropolix::LOAD_DELAY, {"Instant", "Queue Pulses", "Total Pulses"});
    choice("Pitch load", Metropolix::LOAD_PITCH, {"Off: keep sliders", "Thru: pickup saved pitches", "Jump: recall sliders"});
    choice("Switch load", Metropolix::LOAD_SWITCHES, {"Off: keep switches", "Jump: recall switches"});
    choice("CTRL load", Metropolix::LOAD_CTRL, {"Off", "Assigns", "Thru: pickup saved positions", "Jump: assignments and positions"});
    choice("AUX load", Metropolix::LOAD_AUX, {"Off", "Assigns"});
    menu->addChild(createMenuItem("Recall complete pattern", "", [m] {
        auto* h = new history::ComplexAction; h->name = "set complete preset recall";
        const int ids[] = {Metropolix::LOAD_PITCH, Metropolix::LOAD_SWITCHES, Metropolix::LOAD_CTRL, Metropolix::LOAD_AUX};
        const int values[] = {Metropolix::PitchJump, 1, Metropolix::CtrlJump, 1};
        for (int i = 0; i < 4; ++i) {
            auto* change = new history::ParamChange;
            change->moduleId = m->id; change->paramId = ids[i];
            change->oldValue = m->params[ids[i]].getValue(); change->newValue = float(values[i]);
            m->params[ids[i]].setValue(change->newValue); h->push(change);
        }
        APP->history->push(h);
    }));
    menu->addChild(createMenuLabel("Queue / Total counts: edit on the Preset settings screen."));
    menu->addChild(createMenuLabel("Banks are saved inside this Rack patch. Save the patch to keep them."));
}
void presetSlotMenu(Metropolix* m, int stage) {
    int slot = m->presetBankIndex.load() * 8 + stage;
    auto* menu = createMenu();
    menu->addChild(createMenuLabel(string::f("Preset %d-%d%s", slot / 8 + 1, slot % 8 + 1, m->getPreset(slot).occupied ? "" : " (empty)")));
    menu->addChild(createMenuItem("Load / queue", "", [m, slot] { m->requestPreset(slot); }));
    menu->addChild(createMenuItem("Save current pattern here", "", [m, slot] { confirmPresetSave(m, slot, false, false); }, m->presetSaveBlocked()));
    menu->addChild(createMenuItem("Clear slot", "", [m, slot] { confirmPresetSave(m, slot, true, false); }, m->presetSaveBlocked() || !m->getPreset(slot).occupied));
    appendPresetMenu(menu, m);
}
