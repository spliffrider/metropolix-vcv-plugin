#pragma once
#include "BackupDocument.hpp"
namespace metrobackup {
inline void preserveTransport(json_t* module){json_object_set_new(json_object_get(module,"data"),"preserveTransportState",json_true());}
inline bool restore(Metropolix* m,json_t* document,Json& before,Json& after,std::string& error){
    Scope scope=All;if(!validate(m,document,scope,error))return false;
    before.reset(APP->engine->moduleToJson(m));preserveTransport(before.get());
    Json merged(merge(before.get(),document,scope));
    APP->engine->moduleFromJson(m,merged.get());
    after.reset(APP->engine->moduleToJson(m));preserveTransport(after.get());
    return true;
}
}
