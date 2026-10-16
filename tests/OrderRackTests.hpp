#include "../src/BackupRestore.hpp"
void orderRackTests() {
    using M=Metropolix;using namespace metrobackup;
    static_assert(M::FORMAT31_NUM_PARAMS==770 && M::ORDER_SHORTCUTS==770 && M::FORMAT32_NUM_PARAMS==786 && M::FORMAT34_NUM_PARAMS==787 && M::NUM_PARAMS==789 && M::FORMAT9_PARAMS==518 && M::FORMAT10_PARAMS==521 && M::SNAPSHOT_PARAMS==522,"order shortcuts append global IDs only");
    rack::plugin::Plugin plugin;plugin.slug="MetropolixLab";plugin.version="2.32.0";modelMetropolix->plugin=&plugin;modelMetropolix->slug="Metropolix";plugin.models.push_back(modelMetropolix);rack::plugin::plugins.push_back(&plugin);
    auto m=std::unique_ptr<M>(new M),other=std::unique_ptr<M>(new M);m->model=other->model=modelMetropolix;m->id=831;other->id=832;
    for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target)for(int slot=0;slot<8;++slot)for(int order=0;order<21;++order)for(int direction=0;direction<2;++direction) {
        auto ids=M::orderParams(mod,target);m->params[ids[0]].setValue(order);m->params[ids[1]].setValue(direction);
        require(m->orderShortcut(slot,true,mod,target),"save every order/direction from every note track and MOD lane");
        m->params[ids[0]].setValue((order+1)%21);m->params[ids[1]].setValue(1-direction);
        require(m->orderShortcut(slot,false,mod,target) && m->integer(ids[0])==order && m->integer(ids[1])==direction,"recall both shortcut fields to selected target");
    }
    Json before(m->paramsToJson());require(!m->orderShortcut(-1,true,false,0) && !m->orderShortcut(8,false,false,0) && !m->orderShortcut(0,true,false,2) && !m->orderShortcut(0,false,true,8),"invalid slot/target rejects");Json after(m->paramsToJson());require(json_equal(before.get(),after.get()),"invalid shortcuts do not change any setting");
    m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlOrder);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);m->params[M::trackParam(0,M::ORDER)].setValue(3);m->params[M::extraParam(0,M::REVERSED)].setValue(0);
    require(!m->orderShortcut(0,false,false,0) && m->integer(M::trackParam(0,M::ORDER))==3 && m->integer(M::extraParam(0,M::REVERSED))==0,"owned order prevents partial shortcut recall");require(m->orderShortcut(0,true,false,0),"controlled base setting can still be saved");m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    for(int slot=0;slot<8;++slot){m->params[M::orderShortcutParam(slot)].setValue(slot+10);m->params[M::orderShortcutParam(slot,true)].setValue(slot%2);}
    m->orderShortcutHeld.store(true);m->orderShortcutUsed.store(true);Json parameters(m->paramsToJson()),data(m->dataToJson());other->paramsFromJson(parameters.get());other->dataFromJson(data.get());
    for(int id=M::ORDER_SHORTCUTS;id<M::NUM_PARAMS;++id)require(other->params[id].getValue()==m->params[id].getValue(),"all global shortcuts survive patch restore");require(!other->orderShortcutHeld.load() && !other->orderShortcutUsed.load(),"held shortcut gestures never restore");
    for(int index=0;index<M::SNAPSHOT_PARAMS;++index)require(M::presetParam(index)<M::ORDER_SHORTCUTS || M::presetParam(index)==M::MOD_GROUP_MUTE,"musical snapshots exclude global shortcuts");
    auto preset=m->capturePreset();m->putPreset(0,preset);m->params[M::orderShortcutParam(0)].setValue(19);m->requestPreset(0);m->sendPresetCommand();Module::ProcessArgs args{};args.sampleRate=48000;args.sampleTime=1.f/48000;m->process(args);require(m->integer(M::orderShortcutParam(0))==19,"musical recall preserves custom order shortcuts");
    Json module(APP->engine->moduleToJson(m.get()));std::string error;Scope scope=All;
    for(int schema=29;schema<=31;++schema)for(int kind=0;kind<2;++kind) {
        Json legacy(capture(module.get(),static_cast<Scope>(kind)));json_object_set_new(legacy.get(),"params",json_deep_copy(json_object_get(legacy.get(),"params")));auto* state=json_object_get(legacy.get(),"state");json_object_set_new(state,"schemaVersion",json_integer(schema));json_object_del(state,"setupDirectChild");if(schema<31)json_object_del(state,"factoryCursor");if(schema<30)json_object_del(state,"backupPresetCursor");auto* values=json_object_get(legacy.get(),"params");while(json_array_size(values)>M::FORMAT31_NUM_PARAMS)json_array_remove(values,json_array_size(values)-1);
        require(validate(other.get(),legacy.get(),scope,error),"all old settings schemas retain strict 770-ID validation");for(int id=M::ORDER_SHORTCUTS;id<M::NUM_PARAMS;++id)other->params[id].setValue(other->paramQuantities[id]->maxValue);Json old,next;require(restore(other.get(),legacy.get(),old,next,error),"legacy settings import succeeds");for(int id=M::ORDER_SHORTCUTS;id<M::NUM_PARAMS;++id)require(other->params[id].getValue()==other->paramQuantities[id]->defaultValue,"legacy import initializes missing global shortcuts");
        Json bad(json_deep_copy(legacy.get()));json_object_set_new(json_array_get(json_object_get(bad.get(),"params"),0),"id",json_integer(M::ORDER_SHORTCUTS));require(!validate(other.get(),bad.get(),scope,error),"legacy count cannot hide a new ID and a missing old ID");
    }
    Json current(capture(module.get(),Settings));require(validate(other.get(),current.get(),scope,error),"789-ID current settings export validates");Json old,next;require(restore(other.get(),current.get(),old,next,error),"current settings restore includes global shortcuts");for(int id=M::ORDER_SHORTCUTS;id<M::NUM_PARAMS;++id)require(other->params[id].getValue()==m->params[id].getValue(),"current settings restores every custom pair");
    json_array_remove(json_object_get(current.get(),"params"),M::ORDER_SHORTCUTS);require(!validate(other.get(),current.get(),scope,error),"current settings cannot omit an appended shortcut");
    Module::ResetEvent reset{};other->orderShortcutHeld.store(true);other->onReset(reset);require(!other->orderShortcutHeld.load(),"Initialize clears held order gestures");for(int id=M::ORDER_SHORTCUTS;id<M::NUM_PARAMS;++id)require(other->params[id].getValue()==other->paramQuantities[id]->defaultValue,"Initialize restores all shortcut defaults");
    rack::plugin::plugins.pop_back();plugin.models.clear();modelMetropolix->plugin=nullptr;
    std::cout<<"PASS: global ORDER shortcuts, all targets/pairs, CTRL guard, preset exclusion and legacy/current settings\n";
}
