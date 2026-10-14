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
        {"schemaVersion",29,34,false},{"midiSetupCursor",0,5,false},{"backupCursor",0,3,false},
        {"backupPresetCursor",0,9,false},
        {"factoryCursor",0,1,false},
        {"uiSetupCursor",0,7,false},{"modMenuCursor",0,3,false},{"presetBank",0,7,false},
        {"selectedCtrl",0,1,false},{"trackMenuCursor",0,15,false},{"setupCursor",0,16,false},{"setupDirectChild",0,1,true},
        {"globalCursor",0,5,false},{"selectedAux",0,2,false},{"uiHome",0,3,false},
        {"uiStageDetail",0,1,true},{"uiPage",0,35,false},{"uiField",0,7,false},
        {"uiMod",0,1,true},{"uiEdit",0,1,true},{"uiChooseFunction",0,1,true},
        {"selectedLane",0,7,false},{"selectedTrack",0,1,false},{"selectedStage",0,7,false},{"selectedRow",0,7,false}
    };return fields;
}
inline bool value(Metropolix* m,int id,json_t* j){if(!json_is_number(j))return false;double v=json_number_value(j);auto* q=m->paramQuantities[id];return std::isfinite(v) && v>=q->minValue && v<=q->maxValue;}
inline int snapshotSize(int format){return Metropolix::presetSize(format);}
inline bool chain(json_t* links){if(!json_is_array(links) || json_array_size(links)>32)return false;for(size_t i=0;i<json_array_size(links);++i){auto* link=json_array_get(links,i);if(!json_is_object(link) || !integer(json_object_get(link,"slot"),0,63) || !integer(json_object_get(link,"repeats"),1,64) || !integer(json_object_get(link,"mutes"),0,7))return false;}return true;}
inline bool validate(Metropolix* m,json_t* document,Scope& scope,std::string& error){
    auto fail=[&](const char* message){error=message;return false;};
    if(!json_is_object(document))return fail("The file is not a backup object.");
    auto* magic=json_object_get(document,"type");if(!json_is_string(magic) || std::string(json_string_value(magic))!="MetropolixLabBackup")return fail("This is not a Metropolix Lab backup.");
    if(!integer(json_object_get(document,"format"),1,2))return fail("Unsupported backup version.");
    auto* kind=json_object_get(document,"scope");if(!json_is_string(kind))return fail("Missing backup scope.");
    std::string name=json_string_value(kind);if(name=="all")scope=All;else if(name=="settings")scope=Settings;else if(name=="presets")scope=Presets;else return fail("Unsupported backup scope.");
    if(json_integer_value(json_object_get(document,"format"))==2){
        if(scope!=Presets || json_object_size(document)!=5)return fail("Selective backups contain only preset slots.");
        auto* selection=json_object_get(document,"selection");auto* selectionKind=json_object_get(selection,"kind");
        if(!json_is_object(selection) || !json_is_string(selectionKind))return fail("Invalid preset selection.");
        std::string type=json_string_value(selectionKind);bool filled=type=="filled";int selectedBank=-1;
        if(filled){if(json_object_size(selection)!=1)return fail("Unexpected filled selection fields.");}
        else if(type=="bank" && json_object_size(selection)==2 && integer(json_object_get(selection,"bank"),1,8))selectedBank=int(json_integer_value(json_object_get(selection,"bank")))-1;
        else return fail("Invalid preset bank selection.");
        auto* bank=json_object_get(document,"bank");auto* slots=json_object_get(bank,"slots");
        if(!json_is_object(bank) || json_object_size(bank)!=2 || !integer(json_object_get(bank,"presetFormat"),9,Metropolix::PRESET_FORMAT))return fail("Unsupported selective preset format.");
        if(!json_is_array(slots) || json_array_size(slots)>64 || (!filled && json_array_size(slots)!=8))return fail("Invalid selective slot count.");
        int count=snapshotSize(int(json_integer_value(json_object_get(bank,"presetFormat"))));
        std::array<bool,64> seen{};
        for(size_t i=0;i<json_array_size(slots);++i){
            auto* entry=json_array_get(slots,i);auto* id=json_object_get(entry,"slot");auto* values=json_object_get(entry,"values");
            if(!json_is_object(entry) || json_object_size(entry)!=2 || !integer(id,0,63))return fail("Invalid selective slot entry.");
            int slot=int(json_integer_value(id));if(seen[slot] || (!filled && slot/8!=selectedBank))return fail("Duplicate or foreign preset slot.");seen[slot]=true;
            if(json_is_null(values)){if(filled)return fail("Filled selections cannot clear a slot.");continue;}
            if(!json_is_array(values) || json_array_size(values)!=size_t(count))return fail("Invalid selected preset size.");
            for(int n=0;n<count;++n)if(!value(m,Metropolix::presetParam(n),json_array_get(values,n)))return fail("Invalid selected preset value.");
        }
        error.clear();return true;
    }
    if(scope!=Presets){
        auto* state=json_object_get(document,"state");if(!json_is_object(state) || !integer(json_object_get(state,"schemaVersion"),29,34))return fail("Unsupported settings version.");
        int schema=int(json_integer_value(json_object_get(state,"schemaVersion")));
        int expected=schema<32?Metropolix::FORMAT31_NUM_PARAMS:schema==32?Metropolix::FORMAT32_NUM_PARAMS:Metropolix::NUM_PARAMS;
        auto* parameters=json_object_get(document,"params");if(!json_is_array(parameters) || json_array_size(parameters)!=size_t(expected))return fail("Incomplete parameter settings.");
        std::array<bool,Metropolix::NUM_PARAMS> seen{};
        for(size_t i=0;i<json_array_size(parameters);++i){auto* p=json_array_get(parameters,i);auto* id=json_object_get(p,"id");if(!integer(id,0,expected-1))return fail("Invalid parameter ID.");int n=int(json_integer_value(id));if(seen[n] || !value(m,n,json_object_get(p,"value")))return fail("Duplicate or invalid parameter value.");seen[n]=true;}
        if(json_object_size(state)!=stateFields().size()+1-(schema<30?1:0)-(schema<31?1:0)-(schema<34?1:0))return fail("Unexpected or missing settings fields.");
        for(const auto& field:stateFields()){
            if((schema<30 && std::strcmp(field.name,"backupPresetCursor")==0) || (schema<31 && std::strcmp(field.name,"factoryCursor")==0) || (schema<34 && std::strcmp(field.name,"setupDirectChild")==0))continue;
            auto* v=json_object_get(state,field.name);if(field.boolean?!json_is_boolean(v):!integer(v,field.low,field.high))return fail("Invalid settings state.");
        }
        if((schema<34 && json_integer_value(json_object_get(state,"setupCursor"))>(schema<31?6:7)) || json_integer_value(json_object_get(state,"uiPage"))>(schema<30?32:schema<31?33:35))return fail("Invalid legacy settings page.");
        {
            auto* v=json_object_get(state,"pickup");
                if(!json_is_array(v) || json_array_size(v)!=10)return fail("Invalid pickup state.");
                for(int n=0;n<10;++n){auto* p=json_array_get(v,n);if(!json_is_boolean(json_object_get(p,"active")) || !value(m,Metropolix::panelParam(n),json_object_get(p,"target")))return fail("Invalid pickup target.");}
        }
    }
    if(scope!=Settings){
        auto* bank=json_object_get(document,"bank");if(!json_is_object(bank) || !integer(json_object_get(bank,"presetFormat"),1,Metropolix::PRESET_FORMAT))return fail("Unsupported preset format.");
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
// bank=-1 exports occupied slots; 0..7 exports exactly one complete bank.
inline json_t* captureSelected(json_t* module,int bank){
    auto* root=json_pack("{s:s,s:i,s:s}","type","MetropolixLabBackup","format",2,"scope","presets");
    auto* selection=json_pack("{s:s}","kind",bank<0?"filled":"bank");
    if(bank>=0)json_object_set_new(selection,"bank",json_integer(bank+1));
    json_object_set_new(root,"selection",selection);
    auto* source=json_object_get(json_object_get(module,"data"),"presets");auto* slots=json_array();
    for(int slot=0;slot<64;++slot){
        auto* values=json_array_get(source,slot);
        if(bank<0?json_is_null(values):slot/8!=bank)continue;
        auto* entry=json_pack("{s:i}","slot",slot);json_object_set(entry,"values",values);json_array_append_new(slots,entry);
    }
    auto* exported=json_pack("{s:i}","presetFormat",Metropolix::PRESET_FORMAT);json_object_set_new(exported,"slots",slots);json_object_set_new(root,"bank",exported);return root;
}
inline json_t* merge(json_t* current,json_t* document,Scope scope){
    auto* merged=json_deep_copy(current);auto* data=json_object_get(merged,"data");
    if(scope!=Presets){json_object_set_new(merged,"params",json_deep_copy(json_object_get(document,"params")));json_object_update(data,json_object_get(document,"state"));}
    if(json_integer_value(json_object_get(document,"format"))==2){
        auto* slots=json_object_get(json_object_get(document,"bank"),"slots");auto* presets=json_object_get(data,"presets");
        for(size_t i=0;i<json_array_size(slots);++i){
            auto* entry=json_array_get(slots,i);auto* values=json_object_get(entry,"values");
            if(json_is_null(values)){json_array_set(presets,size_t(json_integer_value(json_object_get(entry,"slot"))),values);continue;}
            // A selective old file must be upgraded slot by slot. The receiving
            // bank is already current; changing its format would lose untouched
            // slots or discard old selected slots with a shorter prefix.
            Json upgraded(json_deep_copy(values));
            for(int n=int(json_array_size(upgraded.get()));n<Metropolix::SNAPSHOT_PARAMS;++n)
                json_array_append_new(upgraded.get(),json_real(Metropolix::presetParam(n)==Metropolix::SCALE_LOCK?-1.f:0.f));
            json_array_set(presets,size_t(json_integer_value(json_object_get(entry,"slot"))),upgraded.get());
        }
    }else if(scope!=Settings)for(const char* key:{"presets","presetFormat","chains","chainFormat"})json_object_set(data,key,json_object_get(json_object_get(document,"bank"),key));
    // Transport resumes only after an explicit RUN/external transport command.
    auto* params=json_object_get(merged,"params");for(size_t i=0;i<json_array_size(params);++i){auto* p=json_array_get(params,i);if(json_integer_value(json_object_get(p,"id"))==Metropolix::RUN || json_integer_value(json_object_get(p,"id"))==Metropolix::RESET)json_object_set_new(p,"value",json_real(0));}
    json_object_set_new(data,"preserveTransportState",json_true());
    return merged;
}
}
