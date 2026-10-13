// Included as Metropolix members. UI/serialization owns storage; the engine
// receives an immutable fixed-size program. No locks or allocations on clocks.
struct ChainState {
    metro::PresetChain working;
    std::array<metro::PresetChain,8> saved{};
    bool enabled = false;
    int loaded = -1;
};
struct ChainCommand {
    bool enabled = false;
    metro::PresetChain chain;
    std::array<Snapshot,32> presets{};
};
ChainState chainState;
metro::PresetMailbox<ChainCommand,2> chainCommands;
std::deque<ChainCommand> chainUiCommands;
ChainCommand audioChain, chainScratch;
metro::ChainPosition chainPosition;
std::atomic<bool> chainEnabled{false}, chainFollow{true};
std::atomic<int> chainPlayingLink{-1}, chainPlayingRepeat{1}, chainCursor{0}, chainEdit{0};
std::atomic<unsigned> chainPulse{0};
bool chainWasRunning=false, chainWasExternal=false;
bool chainPreviewed=false;
// UI only, never stored as part of a patch or saved chain.
bool chainCreating=false, chainQuick=false, chainQuickCancel=false;
unsigned chainQuickMutes=0;

ChainState getChainState() {
    std::lock_guard<std::mutex> lock(presetMutex);return chainState;
}
void setChainState(const ChainState& state) {
    ChainCommand command;
    bool wasEnabled;
    {
        std::lock_guard<std::mutex> lock(presetMutex);
        wasEnabled=chainState.enabled;
        chainState=state;
        chainState.enabled=state.enabled && state.working.size>0;
        command.enabled=chainState.enabled;command.chain=state.working;
        if(command.enabled)for(int i=0;i<command.chain.size;++i)command.presets[i]=presetBank[command.chain.links[i].slot];
        chainEnabled.store(command.enabled);
    }
    // Every history operation is ordered, including enable/disable/enable in
    // the same frame. Only the UI allocates these temporary copies.
    if(command.enabled || wasEnabled)chainUiCommands.push_back(command);
    if(command.enabled) { presetUiCommands.clear();cancelPreset(); }
}
void sendChainCommand() {
    if(!chainUiCommands.empty() && chainCommands.push(chainUiCommands.front()))chainUiCommands.pop_front();
}
void applyChainLink(bool atClock) {
    int link=chainPosition.link;
    const auto& s=audioChain.presets[link];
    if(s.occupied)recallMusical(s);
    // An empty referenced preset retains the currently playing settings,
    // while this link's repeats and mute flags still apply (manual p.178).
    sequencer.chainMutes=audioChain.chain.links[link].mutes;
    readSettings();
    if(s.occupied)lastLoadedSlot.store(audioChain.chain.links[link].slot);
    presetTiming.reset();
    if(atClock)sequencer.restartAtClock();else sequencer.reset();
    chainPlayingLink.store(link);chainPlayingRepeat.store(chainPosition.repeat);chainPulse.store(0);
}
void processChainCommands(bool run, bool external, bool reset) {
    bool consumed=chainCommands.pop(chainScratch);
    if(consumed) {
        audioChain=chainScratch;chainPosition.reset();chainPlayingLink.store(-1);chainPreviewed=false;
        audioPendingSlot=-1;pendingSlot.store(-1);
        sequencer.chainMutes=0;readSettings();
    }
    if(audioChain.enabled) {
        if(reset || external!=chainWasExternal || (run!=chainWasRunning && integer(RUN_MODE)==0)) { chainPosition.reset();chainPreviewed=false; }
        if(chainPosition.fresh && (!run || reset) && !chainPreviewed) {
            applyChainLink(false);
            chainPreviewed=true;
            // Keep fresh: first run edge must load and start the first link.
        }
        sequencer.resumeOnRun=integer(RUN_MODE)!=0;
    } else sequencer.resumeOnRun=false;
    chainWasRunning=run;chainWasExternal=external;
}
bool chainClock() {
    if(!audioChain.enabled || !audioChain.chain.size)return false;
    if(chainPosition.fresh || chainPosition.advance(audioChain.chain,integer(TOTAL_PULSES),integer(QUEUE_PULSES)))applyChainLink(true);
    else if(presetTiming.totalDue(integer(TOTAL_PULSES))) {
        sequencer.restartAtClock();presetTiming.phrasePulse=0;
    }
    chainPosition.tick();presetTiming.tick();
    chainPlayingRepeat.store(chainPosition.repeat);chainPulse.store(chainPosition.pulse);
    presetPulse.store(presetTiming.phrasePulse,std::memory_order_relaxed);
    return true;
}
void resetChainRuntime() {
    chainCommands.clear();chainUiCommands.clear();audioChain=ChainCommand();
    chainPosition.reset();sequencer.chainMutes=0;sequencer.resumeOnRun=false;
    chainPlayingLink.store(-1);chainPlayingRepeat.store(1);chainPulse.store(0);
    chainWasRunning=false;chainWasExternal=false;chainPreviewed=false;
    chainQuick=false;chainQuickCancel=false;chainCreating=false;chainEdit.store(0);
}
static json_t* chainToJson(const metro::PresetChain& chain) {
    json_t* links=json_array();
    for(int i=0;i<chain.size;++i) {
        const auto& l=chain.links[i];
        json_array_append_new(links,json_pack("{s:i,s:i,s:i}","slot",l.slot,"repeats",l.repeats,"mutes",int(l.mutes)));
    }
    return links;
}
static metro::PresetChain chainFromJson(json_t* links) {
    metro::PresetChain c;
    if(!json_is_array(links))return c;
    for(size_t i=0;i<json_array_size(links) && c.size<32;++i) {
        json_t* link=json_array_get(links,i);
        json_t* slot=json_object_get(link,"slot"),*repeats=json_object_get(link,"repeats"),*mutes=json_object_get(link,"mutes");
        if(!json_is_integer(slot) || !json_is_integer(repeats) || !json_is_integer(mutes))continue;
        auto clamp=[](json_t* v,int lo,int hi) { return int(std::max(json_int_t(lo),std::min(json_integer_value(v),json_int_t(hi)))); };
        metro::ChainLink l;l.slot=clamp(slot,0,63);l.repeats=clamp(repeats,1,64);l.mutes=clamp(mutes,0,7);c.insert(c.size,l);
    }
    return c;
}
json_t* chainsToJson() {
    auto state=getChainState();
    json_t* root=json_object(),*saved=json_array();
    json_object_set_new(root,"working",chainToJson(state.working));
    for(const auto& c:state.saved)json_array_append_new(saved,chainToJson(c));
    json_object_set_new(root,"saved",saved);
    json_object_set_new(root,"enabled",json_boolean(state.enabled));
    json_object_set_new(root,"loaded",json_integer(state.loaded));
    json_object_set_new(root,"follow",json_boolean(chainFollow.load()));
    json_object_set_new(root,"cursor",json_integer(chainCursor.load()));
    return root;
}
void chainsFromJson(json_t* root) {
    resetChainRuntime();
    ChainState state;
    if(json_integer_value(json_object_get(root,"chainFormat"))==1) {
        json_t* object=json_object_get(root,"chains");
        state.working=chainFromJson(json_object_get(object,"working"));
        for(int i=0;i<8;++i)state.saved[i]=chainFromJson(json_array_get(json_object_get(object,"saved"),i));
        state.enabled=json_is_true(json_object_get(object,"enabled"));
        json_t* loaded=json_object_get(object,"loaded");
        state.loaded=json_is_integer(loaded) && json_integer_value(loaded)>=0 && json_integer_value(loaded)<8?int(json_integer_value(loaded)):-1;
        chainFollow.store(!json_is_false(json_object_get(object,"follow")));
        chainCursor.store(int(std::max(json_int_t(0),std::min(json_integer_value(json_object_get(object,"cursor")),json_int_t(2*state.working.size+2)))));
    } else { chainCursor.store(0);chainFollow.store(true); }
    // Engine exclusion is held during Rack loading: upload directly, without
    // waiting for a UI widget to exist. An enabled patch restarts at Link 1.
    setChainState(state);sendChainCommand();
}
