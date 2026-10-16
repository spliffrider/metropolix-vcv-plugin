// Members of Metropolix. Banks are accessed by UI/serialization only. The audio
// thread receives a fixed-size copy and owns the pending load and pulse clocks.
// Format11 appends independent All MOD mute after the frozen format10 prefix.
// Format12 retains all522 fields and admits the new Mod Trigs output sources.
// Preset preferences, RUN_MODE and CLK settings remain global.
static constexpr int FORMAT3_PARAMS = TOTAL_PULSES + 1 + LOOPY_PULSES - SWING_1;
static constexpr int FORMAT4_PARAMS = FORMAT3_PARAMS + 2;
static constexpr int FORMAT5_PARAMS = FORMAT4_PARAMS + CV_RAMPS + 16 - CV_TYPE_1;
static constexpr int FORMAT6_PARAMS = FORMAT5_PARAMS + 6;
static constexpr int FORMAT7_PARAMS = FORMAT6_PARAMS + 6;
static constexpr int FORMAT8_PARAMS = FORMAT7_PARAMS + 2;
static constexpr int FORMAT9_PARAMS = FORMAT8_PARAMS + MIDI_CLOCK_OUT-MIDI_TRACK;
// The manual's p.169 preset column includes Scale Lock and both Loopy settings.
// Append them after every frozen format9 index; custom definitions stay global.
static constexpr int FORMAT10_PARAMS = FORMAT9_PARAMS + 3;
static constexpr int PRESET_FORMAT = 12;
static constexpr int SNAPSHOT_PARAMS = FORMAT10_PARAMS + 1;
static int presetSize(int format) {
    const int counts[]={0,TOTAL_PULSES+1,TOTAL_PULSES+3,FORMAT3_PARAMS,FORMAT4_PARAMS,FORMAT5_PARAMS,FORMAT6_PARAMS,FORMAT7_PARAMS,FORMAT8_PARAMS,FORMAT9_PARAMS,FORMAT10_PARAMS,SNAPSHOT_PARAMS,SNAPSHOT_PARAMS};
    return format>=1 && format<=PRESET_FORMAT?counts[format]:0;
}
static int presetParam(int index) { return index <= TOTAL_PULSES ? index : index<FORMAT3_PARAMS?SWING_1 + index - TOTAL_PULSES - 1:index<FORMAT5_PARAMS?SLIDE_TYPE_1+index-FORMAT3_PARAMS:index<FORMAT6_PARAMS?OUTPUT_TRACK_OPTIONS+index-FORMAT5_PARAMS:index<FORMAT7_PARAMS?TRACK_BEHAVIOR+index-FORMAT6_PARAMS:index<FORMAT8_PARAMS?TRACK_GLOBAL_RESET_1+index-FORMAT7_PARAMS:index<FORMAT9_PARAMS?MIDI_TRACK+index-FORMAT8_PARAMS:index==FORMAT9_PARAMS?SCALE_LOCK:index==FORMAT9_PARAMS+1?LOOPY_PULSES:index==FORMAT9_PARAMS+2?LOOPY_TARGET:MOD_GROUP_MUTE; }
static int presetIndex(int id) { if(id==MOD_GROUP_MUTE)return FORMAT10_PARAMS;if(id==SCALE_LOCK)return FORMAT9_PARAMS;if(id==LOOPY_PULSES)return FORMAT9_PARAMS+1;if(id==LOOPY_TARGET)return FORMAT9_PARAMS+2;return id <= TOTAL_PULSES ? id : id >= SWING_1 && id < LOOPY_PULSES ? TOTAL_PULSES + 1 + id - SWING_1 : id>=SLIDE_TYPE_1 && id<CV_RAMPS+16?FORMAT3_PARAMS+id-SLIDE_TYPE_1:id>=OUTPUT_TRACK_OPTIONS && id<OUTPUT_TRACK_OPTIONS+6?FORMAT5_PARAMS+id-OUTPUT_TRACK_OPTIONS:id>=TRACK_BEHAVIOR && id<TRACK_BEHAVIOR+6?FORMAT6_PARAMS+id-TRACK_BEHAVIOR:id>=TRACK_GLOBAL_RESET_1 && id<=TRACK_GLOBAL_RESET_2?FORMAT7_PARAMS+id-TRACK_GLOBAL_RESET_1:id>=MIDI_TRACK && id<MIDI_CLOCK_OUT?FORMAT8_PARAMS+id-MIDI_TRACK:-1; }
using Snapshot = metro::PresetSnapshot<SNAPSHOT_PARAMS>;
#include "PickupAdapter.hpp"
struct PresetCommand {
    enum Kind { Load, Cancel, Restore, Initialize, PanelEdit } kind = Cancel;
    Snapshot snapshot, other;
    metro::PanelPickup pickup;std::array<float,10> physical{};unsigned pickupMask=0;
    bool preservePhysicalEdits=false; // UI receipts must not rewind a newer direct edit.
    int slot = -1;
};
struct PresetRecall {
    Snapshot before, after;
    metro::PanelPickup pickupBefore,pickupAfter;std::array<float,10> physicalBefore{},physicalAfter{};unsigned pickupMask=0;
    int slot = -1;
};
std::array<Snapshot, 64> presetBank{};
struct MidiBankUpdate {int slot=0;Snapshot snapshot;};
std::array<Snapshot,64> midiPresetBank{};
metro::PresetMailbox<MidiBankUpdate,65> midiBankUpdates;
MidiBankUpdate midiAudioBankUpdate;
std::array<bool,64> midiBankDirty{}; // UI thread, under presetMutex
bool audioPendingMidi=false;
std::mutex presetMutex;
metro::PresetMailbox<PresetCommand, 2> presetCommands;
metro::PresetMailbox<PresetRecall, 8> presetRecalls;
std::deque<PresetCommand> presetUiCommands; // UI thread only
metro::PresetTiming presetTiming;
Snapshot pendingPreset;
PresetCommand audioPresetCommand; // reusable audio-thread scratch, never allocated per sample
int audioPendingSlot = -1;
std::atomic<int> pendingSlot{-1}, lastLoadedSlot{-1}, presetBankIndex{0}, presetSaveAction{0};
std::atomic<int> presetNotice{0}; // 0 normal, 1 saved, 2 cleared, 3 empty, 4 cancel, 5 queued-save blocked
std::atomic<unsigned> presetPulse{0};
std::atomic<bool> presetCommandInFlight{false};
bool presetWasRunning = false, presetWasExternal = false;

void configurePresets() {
    configParam(TOTAL_PULSES, 0, 4096, 0, "Preset total pulses (0 = Off)")->snapEnabled = true;
    configParam(QUEUE_PULSES, 0, 4096, 32, "Preset queue pulses (0 = manual reset)")->snapEnabled = true;
    configSwitch(LOAD_DELAY, 0, 2, 0, "Preset load delay", {"Instant", "Queue Pulses", "Total Pulses"});
    configSwitch(LOAD_PITCH, 0, 2, 0, "Preset pitch load", {"Off (keep sliders)", "Thru (pickup saved pitches)", "Jump (recall sliders)"});
    configSwitch(LOAD_SWITCHES, 0, 1, 0, "Preset switch load", {"Off (keep switches)", "Jump (recall switches)"});
    configSwitch(LOAD_CTRL, 0, 3, 1, "Preset CTRL load", {"Off", "Assigns", "Thru (pickup saved positions)", "Jump (assignments and positions)"});
    configSwitch(LOAD_AUX, 0, 1, 1, "Preset AUX load", {"Off", "Assigns"});
    configSwitch(RUN_MODE, 0, 1, 0, "Chain Run button", {"Run/Stop (restart)", "Run/Pause (resume)"});
    for (int id = TOTAL_PULSES; id <= RUN_MODE; ++id) paramQuantities[id]->randomizeEnabled = false;
}

Snapshot capturePreset() {
    Snapshot s;
    s.occupied = true;
    for (int i = 0; i < SNAPSHOT_PARAMS; ++i){int id=presetParam(i),panel=panelIndex(id);s.values[i]=panel>=0 && panelPickup[panel].active?panelPickup[panel].target:params[id].getValue();}
    return s;
}
// Used on the UI thread with Engine::moduleToJson(), which excludes process().
Snapshot presetFromParamsJson(json_t* array,json_t* data=nullptr) {
    Snapshot s;
    s.occupied = true;
    for (int i = 0; i < SNAPSHOT_PARAMS; ++i) s.values[i] = paramQuantities[presetParam(i)]->defaultValue;
    size_t i; json_t* entry;
    json_array_foreach(array, i, entry) {
        int id = static_cast<int>(json_integer_value(json_object_get(entry, "id")));
        json_t* value = json_object_get(entry, "value");
        int index = presetIndex(id);
        if (id >= 0 && index >= 0 && json_is_number(value)) s.values[index] = sanitizePresetValue(id, json_number_value(value));
    }
    if(data)snapshotPickupJson(s,data);
    return s;
}
float sanitizePresetValue(int id, double v) const {
    auto* q = paramQuantities[id];
    if (!std::isfinite(v)) return q->defaultValue;
    float result = static_cast<float>(std::max(double(q->minValue), std::min(v, double(q->maxValue))));
    return q->snapEnabled ? std::round(result) : result;
}
Snapshot getPreset(int slot) {
    std::lock_guard<std::mutex> lock(presetMutex);
    return slot >= 0 && slot < 64 ? presetBank[slot] : Snapshot{};
}
void putPreset(int slot, const Snapshot& s) {
    if (slot < 0 || slot >= 64) return;
    if(chainEnabled.load()) {auto state=getChainState();state.enabled=false;setChainState(state);}
    std::lock_guard<std::mutex> lock(presetMutex);
    presetBank[slot] = s;
    midiBankDirty[slot]=true;sendMidiBankLocked();
}
void sendMidiBankLocked(){for(int slot=0;slot<64;++slot)if(midiBankDirty[slot]){MidiBankUpdate update;update.slot=slot;update.snapshot=presetBank[slot];if(!midiBankUpdates.push(update))return;midiBankDirty[slot]=false;}}
void sendMidiBank(){std::lock_guard<std::mutex> lock(presetMutex);sendMidiBankLocked();}
// UI submissions are transferred at most once per frame, after draining all
// recall receipts. Thus at most two new receipts can arrive before the next
// frame (one existing queued load and one new command); eight cells suffice.
void requestPreset(int slot) {
    if(chainEnabled.load()) { presetNotice.store(6);return; }
    PresetCommand c;
    c.slot = slot;
    c.snapshot = getPreset(slot);
    c.kind = c.snapshot.occupied ? PresetCommand::Load : PresetCommand::Cancel;
    if (!c.snapshot.occupied) presetNotice.store(pendingSlot.load() >= 0 ? 4 : 3);
    else presetNotice.store(0);
    presetUiCommands.push_back(c);
}
void requestInitialize() {
    if(chainEnabled.load()){auto state=getChainState();state.enabled=false;setChainState(state);}
    PresetCommand c;c.kind=PresetCommand::Initialize;c.slot=-2;int init=integer(INIT_PRESET);
    if(init)c.snapshot=getPreset(init-1);
    if(!c.snapshot.occupied){c.snapshot.occupied=true;for(int i=0;i<SNAPSHOT_PARAMS;++i)c.snapshot.values[i]=paramQuantities[presetParam(i)]->defaultValue;}
    presetUiCommands.push_back(c);
}
void cancelPreset() {
    presetUiCommands.push_back(PresetCommand{});
    presetNotice.store(4);
}
bool presetSaveBlocked() const { return chainEnabled.load() || pendingSlot.load() >= 0 || presetCommandInFlight.load() || !presetUiCommands.empty(); }
void sendPresetCommand() {
    if (!presetUiCommands.empty() && !presetCommandInFlight.load()) {
        presetCommandInFlight.store(true);
        if (presetCommands.push(presetUiCommands.front())) presetUiCommands.pop_front();
        else presetCommandInFlight.store(false);
    }
}
bool recallParam(int id) {
    if (id < ROOT || id == RANGE || id == OUTPUT_SOURCE_A || id == OUTPUT_SOURCE_B) return false;
    if (id >= PITCH && id < PULSES) return integer(LOAD_PITCH) != 0;
    if (id >= PULSES && id < TRACK) return integer(LOAD_SWITCHES) != 0;
    if (id >= AUX_PARAMS && id < ACCUM_MODE)
        return (id - AUX_PARAMS) % AUX_STRIDE != AUX_AMOUNT && integer(LOAD_AUX) != 0;
    if (id >= CTRL_PARAMS && id < TOTAL_PULSES)
        return integer(LOAD_CTRL) >= ((id - CTRL_PARAMS) % CTRL_STRIDE == CTRL_VALUE ? 2 : 1);
    return true;
}
void applyPendingPreset(bool atClock) {
    PresetRecall event;
    event.slot = audioPendingSlot;
    event.before = capturePreset();
    event.pickupBefore=panelPickup;event.physicalBefore=physicalControls();
    recallMusical(pendingPreset);
    readSettings();
    event.after = capturePreset();event.pickupAfter=panelPickup;event.physicalAfter=physicalControls();
    for(int i=0;i<10;++i)if(event.pickupBefore[i]!=event.pickupAfter[i] || event.physicalBefore[i]!=event.physicalAfter[i])event.pickupMask|=1u<<i;
    // This push cannot overflow: the UI drains receipts before sending a
    // command, and without another command only one pending load can finish.
    if(!audioPendingMidi)presetRecalls.push(event);
    audioPendingMidi=false;
    lastLoadedSlot.store(audioPendingSlot);
    audioPendingSlot = -1;
    pendingSlot.store(-1);
    presetTiming.reset();
    if (atClock) sequencer.restartAtClock(); else sequencer.reset();
}
void processPresetCommands(bool run, bool external, bool reset) {
    if (reset || (!audioChain.enabled && run != presetWasRunning) || external != presetWasExternal) presetTiming.reset();
    presetWasRunning = run;
    presetWasExternal = external;
    auto& c = audioPresetCommand;
    bool consumed = presetCommands.pop(c);
    if (consumed) {
        if(c.kind!=PresetCommand::PanelEdit)audioPendingMidi=false;
        if(c.kind==PresetCommand::PanelEdit){for(int i=0;i<10;++i)if(c.pickupMask & (1u<<i)){int id=panelParam(i);if(!c.preservePhysicalEdits || params[id].getValue()==c.physical[i])params[id].setValue(c.physical[i]);panelPickup[i]=c.pickup[i];}readSettings();}
        else if(c.kind==PresetCommand::Initialize){pendingPreset=c.snapshot;audioPendingSlot=-2;applyPendingPreset(false);}
        else if (c.kind == PresetCommand::Restore) {
            audioPendingSlot = -1;
            pendingSlot.store(-1);
            for (int i = 0; i < SNAPSHOT_PARAMS; ++i)
                if (c.snapshot.values[i] != c.other.values[i] && panelIndex(presetParam(i))<0) params[presetParam(i)].setValue(c.snapshot.values[i]);
            for(int i=0;i<10;++i)if(c.pickupMask & (1u<<i)){params[panelParam(i)].setValue(c.physical[i]);panelPickup[i]=c.pickup[i];}
            readSettings();
            sequencer.reset();
            presetTiming.reset();
            lastLoadedSlot.store(-1);
        } else if (c.kind == PresetCommand::Cancel || c.slot == audioPendingSlot) {
            audioPendingSlot = -1;
            pendingSlot.store(-1);
            presetNotice.store(c.kind == PresetCommand::Cancel ? presetNotice.load() : 4);
        } else if(!audioChain.enabled) {
            pendingPreset = c.snapshot;
            audioPendingSlot = c.slot;
            pendingSlot.store(c.slot);
        }
    }
    if (audioPendingSlot >= 0 && (!run || reset || integer(LOAD_DELAY) == metro::LoadInstant)) applyPendingPreset(false);
    if (consumed) presetCommandInFlight.store(false);
}
void presetClock() {
    if(chainClock())return;
    int total = integer(TOTAL_PULSES);
    if (audioPendingSlot >= 0 && presetTiming.loadDue(integer(LOAD_DELAY), integer(QUEUE_PULSES), total))
        applyPendingPreset(true);
    else if (presetTiming.totalDue(total)) {
        sequencer.restartAtClock();
        presetTiming.phrasePulse = 0;
    }
    presetTiming.tick();
    presetPulse.store(presetTiming.phrasePulse, std::memory_order_relaxed);
}
void resetPresetRuntime() {
    presetCommands.clear(); presetRecalls.clear(); presetUiCommands.clear();
    presetCommandInFlight.store(false);
    audioPendingSlot = -1; pendingSlot.store(-1); lastLoadedSlot.store(-1);
    audioPendingMidi=false;
    presetTiming.reset(); presetPulse.store(0); presetNotice.store(0);
    presetWasRunning = false; presetWasExternal = false;
}
json_t* presetsToJson() {
    std::lock_guard<std::mutex> lock(presetMutex);
    json_t* slots = json_array();
    for (const auto& s : presetBank) {
        if (!s.occupied) { json_array_append_new(slots, json_null()); continue; }
        json_t* values = json_array();
        for (float v : s.values) json_array_append_new(values, json_real(v));
        json_array_append_new(slots, values);
    }
    return slots;
}
void presetsFromJson(json_t* root) {
    resetPresetRuntime();
    std::lock_guard<std::mutex> lock(presetMutex);
    presetBank = {};
    int format = int(json_integer_value(json_object_get(root, "presetFormat")));
    if (format >= 1 && format <= PRESET_FORMAT) {
        json_t* slots = json_object_get(root, "presets");
        for (int slot = 0; slot < 64; ++slot) {
            json_t* values = json_array_get(slots, slot);
            int count = presetSize(format);
            if (!json_is_array(values) || json_array_size(values) != size_t(count)) continue;
            Snapshot s; s.occupied = true;
            for(int i=0;i<SNAPSHOT_PARAMS;++i)s.values[i]=paramQuantities[presetParam(i)]->defaultValue;
            if(format<9){for(int t=0;t<2;++t)s.values[presetIndex(midiTrackParam(t,MIDI_CHANNEL))]=0;for(int l=0;l<8;++l)s.values[presetIndex(midiModParam(l,MIDI_MOD_CHANNEL))]=0;}
            for (int i = 0; i < count; ++i) {
                json_t* value = json_array_get(values, i);
                if (!json_is_number(value)) { s.occupied = false; break; }
                int id=presetParam(i);double raw=json_number_value(value);
                if(format<12 && std::isfinite(raw) && (id==OUTPUT_SOURCE_A || id==OUTPUT_SOURCE_B))raw=std::min(raw,37.);
                s.values[i] = sanitizePresetValue(id,raw);
            }
            if (s.occupied) presetBank[slot] = s;
        }
    }
    presetBankIndex.store(metro::clamp(int(json_integer_value(json_object_get(root, "presetBank"))), 0, 7));
    midiBankUpdates.clear();midiPresetBank=presetBank;midiBankDirty.fill(false);
    presetSaveAction.store(0);
}
