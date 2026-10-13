#pragma once
#include "Metropolix.hpp"
#include <memory>
#include <limits>
#include <cstring>
namespace metrobackup {
constexpr size_t MaxBytes=4*1024*1024;
enum Scope {All,Settings,Presets};
inline const char* scopeName(Scope scope){return scope==Settings?"settings":scope==Presets?"presets":"all";}
struct JsonDelete {void operator()(json_t* j) const {json_decref(j);}};
using Json=std::unique_ptr<json_t,JsonDelete>;
inline bool integer(json_t* j,json_int_t lo,json_int_t hi){return json_is_integer(j) && json_integer_value(j)>=lo && json_integer_value(j)<=hi;}
struct StateField {const char* name;int low,high;bool boolean;};
inline const std::vector<StateField>& stateFields(){
    static const std::vector<StateField> fields={
        {"schemaVersion",29,29,false},{"midiSetupCursor",0,5,false},{"backupCursor",0,3,false},
        {"uiSetupCursor",0,7,false},{"modMenuCursor",0,3,false},{"presetBank",0,7,false},
        {"selectedCtrl",0,1,false},{"trackMenuCursor",0,15,false},{"setupCursor",0,6,false},
        {"globalCursor",0,5,false},{"selectedAux",0,2,false},{"uiHome",0,3,false},
        {"uiStageDetail",0,1,true},{"uiPage",0,32,false},{"uiField",0,7,false},
        {"uiMod",0,1,true},{"uiEdit",0,1,true},{"uiChooseFunction",0,1,true},
        {"selectedLane",0,7,false},{"selectedTrack",0,1,false},{"selectedStage",0,7,false},{"selectedRow",0,7,false}
    };return fields;
}
inline bool value(Metropolix* m,int id,json_t* j){if(!json_is_number(j))return false;double v=json_number_value(j);auto* q=m->paramQuantities[id];return std::isfinite(v) && v>=q->minValue && v<=q->maxValue;}
inline int snapshotSize(int format){using M=Metropolix;const int counts[]={0,M::TOTAL_PULSES+1,M::TOTAL_PULSES+3,M::FORMAT3_PARAMS,M::FORMAT4_PARAMS,M::FORMAT5_PARAMS,M::FORMAT6_PARAMS,M::FORMAT7_PARAMS,M::FORMAT8_PARAMS,M::SNAPSHOT_PARAMS};return format>=1 && format<=9?counts[format]:0;}
inline bool chain(json_t* links){if(!json_is_array(links) || json_array_size(links)>32)return false;for(size_t i=0;i<json_array_size(links);++i){auto* link=json_array_get(links,i);if(!json_is_object(link) || !integer(json_object_get(link,"slot"),0,63) || !integer(json_object_get(link,"repeats"),1,64) || !integer(json_object_get(link,"mutes"),0,7))return false;}return true;}
inline bool validate(Metropolix* m,json_t* document,Scope& scope,std::string& error){
    auto fail=[&](const char* message){error=message;return false;};
    if(!json_is_object(document))return fail("The file is not a backup object.");
    auto* magic=json_object_get(document,"type");if(!json_is_string(magic) || std::string(json_string_value(magic))!="MetropolixLabBackup")return fail("This is not a Metropolix Lab backup.");
    if(!integer(json_object_get(document,"format"),1,1))return fail("Unsupported backup version.");
    auto* kind=json_object_get(document,"scope");if(!json_is_string(kind))return fail("Missing backup scope.");
    std::string name=json_string_value(kind);if(name=="all")scope=All;else if(name=="settings")scope=Settings;else if(name=="presets")scope=Presets;else return fail("Unsupported backup scope.");
    if(scope!=Presets){
        auto* parameters=json_object_get(document,"params");if(!json_is_array(parameters) || json_array_size(parameters)!=Metropolix::NUM_PARAMS)return fail("Incomplete parameter settings.");
        std::array<bool,Metropolix::NUM_PARAMS> seen{};
        for(size_t i=0;i<json_array_size(parameters);++i){auto* p=json_array_get(parameters,i);auto* id=json_object_get(p,"id");if(!integer(id,0,Metropolix::NUM_PARAMS-1))return fail("Invalid parameter ID.");int n=int(json_integer_value(id));if(seen[n] || !value(m,n,json_object_get(p,"value")))return fail("Duplicate or invalid parameter value.");seen[n]=true;}
        auto* state=json_object_get(document,"state");if(!json_is_object(state) || !integer(json_object_get(state,"schemaVersion"),29,29))return fail("Unsupported settings version.");
        if(json_object_size(state)!=stateFields().size()+1)return fail("Unexpected or missing settings fields.");
        for(const auto& field:stateFields()){auto* v=json_object_get(state,field.name);if(field.boolean?!json_is_boolean(v):!integer(v,field.low,field.high))return fail("Invalid settings state.");}
        {
            auto* v=json_object_get(state,"pickup");
                if(!json_is_array(v) || json_array_size(v)!=10)return fail("Invalid pickup state.");
                for(int n=0;n<10;++n){auto* p=json_array_get(v,n);if(!json_is_boolean(json_object_get(p,"active")) || !value(m,Metropolix::panelParam(n),json_object_get(p,"target")))return fail("Invalid pickup target.");}
        }
    }
    if(scope!=Settings){
        auto* bank=json_object_get(document,"bank");if(!json_is_object(bank) || !integer(json_object_get(bank,"presetFormat"),1,9))return fail("Unsupported preset format.");
        auto* slots=json_object_get(bank,"presets");int count=snapshotSize(int(json_integer_value(json_object_get(bank,"presetFormat"))));if(!json_is_array(slots) || json_array_size(slots)!=64)return fail("A bank must contain 64 slots.");
        for(int slot=0;slot<64;++slot){auto* values=json_array_get(slots,slot);if(json_is_null(values))continue;if(!json_is_array(values) || json_array_size(values)!=size_t(count))return fail("Invalid preset size.");for(int i=0;i<count;++i)if(!value(m,Metropolix::presetParam(i),json_array_get(values,i)))return fail("Invalid preset value.");}
        if(json_object_size(bank)!=4)return fail("Unexpected bank fields.");
        if(!integer(json_object_get(bank,"chainFormat"),1,1))return fail("Unsupported chain format.");
        auto* chains=json_object_get(bank,"chains");auto* saved=json_object_get(chains,"saved");
        if(!json_is_object(chains) || !chain(json_object_get(chains,"working")) || !json_is_array(saved) || json_array_size(saved)!=8)return fail("Invalid chain bank.");
        for(int i=0;i<8;++i)if(!chain(json_array_get(saved,i)))return fail("Invalid saved chain.");
        if(!json_is_boolean(json_object_get(chains,"enabled")) || !json_is_boolean(json_object_get(chains,"follow")) || !integer(json_object_get(chains,"loaded"),-1,7) || !integer(json_object_get(chains,"cursor"),0,66))return fail("Invalid chain settings.");
        if(json_is_true(json_object_get(chains,"enabled")) && json_array_size(json_object_get(chains,"working"))==0)return fail("An enabled chain must contain a link.");
    }
    error.clear();return true;
}
inline json_t* capture(json_t* module,Scope scope){
    auto* root=json_pack("{s:s,s:i,s:s}","type","MetropolixLabBackup","format",1,"scope",scopeName(scope));auto* data=json_object_get(module,"data");
    if(scope!=Presets){json_object_set(root,"params",json_object_get(module,"params"));Json state(json_deep_copy(data));for(const char* key:{"midiInput","midiOutput","presets","presetFormat","chains","chainFormat","expanded","modEditor"})json_object_del(state.get(),key);json_object_set_new(root,"state",state.release());}
    if(scope!=Settings){auto* bank=json_object();for(const char* key:{"presets","presetFormat","chains","chainFormat"})json_object_set(bank,key,json_object_get(data,key));json_object_set_new(root,"bank",bank);}
    return root;
}
inline json_t* merge(json_t* current,json_t* document,Scope scope){
    auto* merged=json_deep_copy(current);auto* data=json_object_get(merged,"data");
    if(scope!=Presets){json_object_set_new(merged,"params",json_deep_copy(json_object_get(document,"params")));json_object_update(data,json_object_get(document,"state"));}
    if(scope!=Settings)for(const char* key:{"presets","presetFormat","chains","chainFormat"})json_object_set(data,key,json_object_get(json_object_get(document,"bank"),key));
    // Transport resumes only after an explicit RUN/external transport command.
    auto* params=json_object_get(merged,"params");for(size_t i=0;i<json_array_size(params);++i){auto* p=json_array_get(params,i);if(json_integer_value(json_object_get(p,"id"))==Metropolix::RUN || json_integer_value(json_object_get(p,"id"))==Metropolix::RESET)json_object_set_new(p,"value",json_real(0));}
    json_object_set_new(data,"preserveTransportState",json_true());
    return merged;
}
}
