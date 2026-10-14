#pragma once
#include "BackupRestore.hpp"
namespace metrofactory {
enum Action {ResetSettings,ClearPresets};
inline bool restore(Metropolix* m,Action action,metrobackup::Json& before,metrobackup::Json& after,std::string& error){
    using namespace metrobackup;
    if(action!=ResetSettings && action!=ClearPresets){error="Unknown maintenance action.";return false;}
    before.reset(APP->engine->moduleToJson(m));preserveTransport(before.get());Json merged;
    if(action==ResetSettings){
        auto defaults=std::unique_ptr<Metropolix>(new Metropolix);defaults->model=m->model;
        Json state(defaults->toJson()),settings(capture(state.get(),Settings));Scope checked=All;
        if(!validate(m,settings.get(),checked,error))return false;
        merged.reset(merge(before.get(),settings.get(),Settings));
    }else{
        merged.reset(json_deep_copy(before.get()));auto* bank=json_array();
        for(int slot=0;slot<64;++slot)json_array_append_new(bank,json_null());
        json_object_set_new(json_object_get(merged.get(),"data"),"presets",bank);
        auto* params=json_object_get(merged.get(),"params");
        for(size_t i=0;i<json_array_size(params);++i){auto* p=json_array_get(params,i);int id=int(json_integer_value(json_object_get(p,"id")));if(id==Metropolix::RUN || id==Metropolix::RESET)json_object_set_new(p,"value",json_real(0));}
    }
    // Musical banks/chain links stay local; maintenance cannot resume a chain.
    auto* data=json_object_get(merged.get(),"data");json_object_set_new(json_object_get(data,"chains"),"enabled",json_false());
    json_object_set_new(data,"setupCursor",json_integer(Metropolix::SetupVersion));json_object_set_new(data,"uiPage",json_integer(Metropolix::FACTORY_PAGE));json_object_set_new(data,"uiField",json_integer(0));json_object_set_new(data,"factoryCursor",json_integer(action));
    preserveTransport(merged.get());APP->engine->moduleFromJson(m,merged.get());
    after.reset(APP->engine->moduleToJson(m));preserveTransport(after.get());error.clear();return true;
}
}
