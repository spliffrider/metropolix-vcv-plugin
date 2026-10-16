#include <plugin.hpp>
#include "../src/BackupRestore.hpp"
void presetSetupNavigationRackTests(){
    using M=Metropolix;using namespace metrobackup;
    rack::plugin::Plugin plugin;plugin.slug="MetropolixLab";modelMetropolix->plugin=&plugin;modelMetropolix->slug="Metropolix";plugin.models.push_back(modelMetropolix);rack::plugin::plugins.push_back(&plugin);
    auto source=std::unique_ptr<M>(new M),copy=std::unique_ptr<M>(new M);source->model=copy->model=modelMetropolix;source->id=852;copy->id=853;
    source->params[M::ROOT].setValue(7);source->params[M::RUN].setValue(0);source->params[M::INIT_PRESET].setValue(64);source->params[M::QUEUE_PULSES].setValue(4096);source->params[M::LOAD_PITCH].setValue(2);source->params[M::LOAD_CTRL].setValue(2);source->params[M::stageParam(1,4,M::CV)].setValue(-2.75f);
    copy->params[M::ROOT].setValue(3);copy->putPreset(0,copy->capturePreset());Json bankBefore(copy->presetsToJson());
    auto raw=[](M* m){std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};auto expected=raw(source.get());
    int legacyCases=0,currentCases=0;Scope scope=Settings;std::string error;
    for(int schema=29;schema<=34;++schema)for(int item=0;item<8;++item)for(int editing=0;editing<2;++editing){
        source->uiPage.store(M::PRESET_SETUP_PAGE);source->uiField.store(item);source->presetSetupEditing.store(editing!=0);Json module(APP->engine->moduleToJson(source.get()));auto* data=json_object_get(module.get(),"data");require(!json_object_get(data,"presetSetupEditing"),"preset value-selection flag stays transient and outside saved schema34");
        json_object_set_new(data,"schemaVersion",json_integer(schema));if(schema<34)json_object_del(data,"setupDirectChild");if(schema<30)json_object_del(data,"backupPresetCursor");if(schema<31)json_object_del(data,"factoryCursor");
        auto* parameters=json_object_get(module.get(),"params");int count=schema<32?M::FORMAT31_NUM_PARAMS:schema==32?M::FORMAT32_NUM_PARAMS:M::FORMAT34_NUM_PARAMS;while(json_array_size(parameters)>size_t(count))json_array_remove(parameters,json_array_size(parameters)-1);
        Json backup(capture(module.get(),Settings)),before,after;require(validate(copy.get(),backup.get(),scope,error),"released preset-setting logical IDs remain valid settings backups");copy->presetSetupEditing.store(true);require(restore(copy.get(),backup.get(),before,after,error),"preset settings restore through native Rack engine");require(copy->uiPage.load()==M::PRESET_SETUP_PAGE && copy->uiField.load()==item && !copy->presetSetupEditing.load(),"all saved preset-setting IDs retain selected parameter and reopen browsing");require(raw(copy.get())==expected,"preset list navigation restore preserves all raw parameter values and legacy defaults");Json bankAfter(copy->presetsToJson());require(json_equal(bankBefore.get(),bankAfter.get()),"settings-only restore preserves unrelated preset bank");if(schema<34)++legacyCases;else ++currentCases;
    }
    source->uiPage.store(M::PRESET_SETUP_PAGE);source->uiField.store(7);source->presetSetupEditing.store(true);Json patch(APP->engine->moduleToJson(source.get()));copy->presetSetupEditing.store(true);APP->engine->moduleFromJson(copy.get(),patch.get());require(copy->uiField.load()==7 && !copy->presetSetupEditing.load() && raw(copy.get())==expected,"native Rack patch reopening retains Init selection while disarming transient editing");
    Json valid(capture(patch.get(),Settings));json_object_set_new(json_object_get(valid.get(),"state"),"uiField",json_integer(8));copy->presetSetupEditing.store(true);Json before(APP->engine->moduleToJson(copy.get())),old,next;require(!restore(copy.get(),valid.get(),old,next,error),"invalid preset field rejects before mutation");Json after(APP->engine->moduleToJson(copy.get()));require(json_equal(before.get(),after.get()) && copy->presetSetupEditing.load(),"rejected preset-settings file preserves serialized and transient state");
    rack::engine::Module::ResetEvent reset;copy->onReset(reset);require(!copy->presetSetupEditing.load(),"module initialization disarms preset value editing");
    rack::plugin::plugins.pop_back();plugin.models.clear();modelMetropolix->plugin=nullptr;
    std::cout<<"PASS: Presets Setup navigation "<<legacyCases<<" legacy restores, "<<currentCases<<" current restores, transient selection, bank preservation and rejection\n";
}
