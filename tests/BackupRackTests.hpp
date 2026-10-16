#include <plugin.hpp>
#include "../src/BackupFile.hpp"
#include "../src/BackupRestore.hpp"
void backupRackTests(){
    using M=Metropolix;using namespace metrobackup;
    rack::plugin::Plugin plugin;plugin.slug="MetropolixLab";modelMetropolix->plugin=&plugin;modelMetropolix->slug="Metropolix";plugin.models.push_back(modelMetropolix);rack::plugin::plugins.push_back(&plugin);
    auto source=std::unique_ptr<M>(new M),destination=std::unique_ptr<M>(new M);source->model=destination->model=modelMetropolix;source->id=822;destination->id=823;
    source->params[M::TEMPO].setValue(141.125f);source->params[M::LOAD_PITCH].setValue(M::PitchThru);source->params[M::PITCH].setValue(.17f);
    source->panelPickup[0].recall(.17f,.7f);source->params[M::RUN_ON_BOOT].setValue(1);source->params[M::RUN].setValue(1);source->params[M::RESET].setValue(1);
    source->params[M::MX_MODE].setValue(6);source->params[M::MX_LANE].setValue(7);source->params[M::midiTrackParam(0,M::MIDI_CHANNEL)].setValue(16);source->params[M::userScaleParam(0)].setValue(137);
    source->params[M::stageParam(0,0,M::CV)].setValue(-2.75f);source->params[M::modParam(0,M::MOD_TYPE)].setValue(1);source->params[M::modParam(0,M::MOD_VALUE)].setValue(-3.25f);source->params[M::CLOCK_OUT_OFFSET].setValue(200);
    source->backupCursor.store(2);source->selectedTrack.store(1);source->uiPage.store(M::BACKUP_PAGE);source->expanded.store(true);source->modEditor.store(true);
    for(int n=0;n<64;++n){auto preset=source->capturePreset();preset.values[M::PITCH]=n/63.f;source->putPreset(n,preset);}
    M::ChainState chain;metro::ChainLink link;link.slot=63;link.repeats=64;link.mutes=7;chain.working.insert(0,link);chain.saved[7]=chain.working;chain.loaded=7;source->setChainState(chain);source->chainFollow.store(false);
    std::string error;Scope scope=All;Json module(APP->engine->moduleToJson(source.get()));
    for(int kind=0;kind<3;++kind){Json backup(capture(module.get(),static_cast<Scope>(kind)));require(validate(source.get(),backup.get(),scope,error) && scope==kind,"all three exported scopes validate");require(!json_object_get(json_object_get(backup.get(),"state"),"midiInput") && !json_object_get(backup.get(),"id"),"backup omits device and module identity");
        destination->params[M::TEMPO].setValue(110);destination->params[M::RUN_ON_BOOT].setValue(1);destination->params[M::RUN].setValue(1);destination->expanded.store(false);destination->modEditor.store(false);destination->putPreset(0,M::Snapshot{});
        destination->params[M::CV_TYPE_1].setValue(metro::UnipolarCv);
        Json devices(destination->dataToJson()),before,after;require(restore(destination.get(),backup.get(),before,after,error),"actual engine restore applies validated backup");
        require(destination->integer(M::RUN)==0 && destination->integer(M::RESET)==0,"restore stops transport even with Run on Boot On");
        require(!destination->expanded.load() && !destination->modEditor.load() && destination->id==823,"restore preserves module identity and panel geometry");
        Json restored(destination->dataToJson());require(json_equal(json_object_get(devices.get(),"midiInput"),json_object_get(restored.get(),"midiInput")) && json_equal(json_object_get(devices.get(),"midiOutput"),json_object_get(restored.get(),"midiOutput")),"MIDI device choices remain local");
        if(kind!=Presets)require(destination->params[M::TEMPO].getValue()==141.125f && destination->waitingPickup(M::PITCH) && destination->panelValue(M::PITCH)==.7f && destination->integer(M::MX_LANE)==7 && destination->integer(M::userScaleParam(0))==137,"settings retain parameters, pickup, MX and custom scales");
        else require(destination->params[M::TEMPO].getValue()==110,"bank-only import preserves current settings");
        if(kind!=Presets)require(destination->params[M::stageParam(0,0,M::CV)].getValue()==-2.75f && destination->params[M::modParam(0,M::MOD_VALUE)].getValue()==-3.25f && destination->params[M::CLOCK_OUT_OFFSET].getValue()==200,"restore preserves signed CV and dormant values across old dynamic bounds");
        if(kind!=Settings)require(destination->getPreset(0).occupied && destination->getPreset(63).values[M::PITCH]==1 && destination->getChainState().saved[7].links[0].slot==63,"presets and saved chains restore completely");
        else require(!destination->getPreset(0).occupied,"settings-only import preserves existing bank");
        APP->engine->moduleFromJson(destination.get(),before.get());require(destination->integer(M::RUN)==1 && destination->params[M::TEMPO].getValue()==110 && !destination->getPreset(0).occupied,"history receipt restores prior running settings and bank");
        APP->engine->moduleFromJson(destination.get(),after.get());require(destination->integer(M::RUN)==0,"history redo remains stopped with Run on Boot On");
    }
    Json good(capture(module.get(),All));
    {Json malformed(json_array());json_array_append_new(malformed.get(),json_pack("{s:I,s:f}","id",json_int_t(4294967296LL),"value",1.));Json before(destination->paramsToJson());destination->paramsFromJson(malformed.get());Json after(destination->paramsToJson());require(json_equal(before.get(),after.get()),"out-of-range 64-bit patch parameter ID cannot wrap to a valid control");}
    {Json gated(json_deep_copy(good.get()));json_object_set_new(json_array_get(json_object_get(gated.get(),"params"),M::RESET_INPUT_MODE),"value",json_real(1));destination->params[M::RESET_INPUT_MODE].setValue(1);destination->params[M::RUN].setValue(1);destination->inputs[M::RESET_IN].channels=1;destination->inputs[M::RESET_IN].setVoltage(5);
        Module::ProcessArgs args{};args.sampleRate=48000;args.sampleTime=1.f/48000;for(int n=0;n<64;++n)destination->process(args);Json before,after;require(restore(destination.get(),gated.get(),before,after,error),"gated transport backup imports");for(int n=0;n<64;++n)destination->process(args);require(!destination->sequencer.running && destination->outputs[M::GATE_1].getVoltage()==0,"held external Run gate cannot auto-resume after import");destination->inputs[M::RESET_IN].setVoltage(0);for(int n=0;n<64;++n)destination->process(args);destination->inputs[M::RESET_IN].setVoltage(5);for(int n=0;n<64;++n)destination->process(args);require(destination->sequencer.running,"new external Run gate resumes restored pattern");destination->inputs[M::RESET_IN].channels=0;destination->params[M::RUN].setValue(0);destination->params[M::RESET_INPUT_MODE].setValue(0);for(int n=0;n<64;++n)destination->process(args);
    }
    auto rejects=[&](json_t* bad,const char* message){Json before(APP->engine->moduleToJson(destination.get())),old,next;require(!restore(destination.get(),bad,old,next,error) && !error.empty(),message);Json after(APP->engine->moduleToJson(destination.get()));require(json_equal(before.get(),after.get()),"rejected backup leaves entire module unchanged");};
    {Json bad(json_deep_copy(good.get()));json_object_set_new(bad.get(),"format",json_integer(3));rejects(bad.get(),"future backup versions reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_object_get(bad.get(),"state"),"schemaVersion",json_integer(36));rejects(bad.get(),"future settings schemas reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_object_get(bad.get(),"bank"),"presetFormat",json_integer(13));rejects(bad.get(),"future preset formats reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_object_get(bad.get(),"bank"),"chainFormat",json_integer(2));rejects(bad.get(),"future chain formats reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_object_get(bad.get(),"state"),"midiOutput",json_integer(0));rejects(bad.get(),"foreign device fields reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_object_get(bad.get(),"bank"),"midiInput",json_object());rejects(bad.get(),"bank cannot change device settings");}
    {Json bad(json_deep_copy(good.get()));json_object_del(json_object_get(bad.get(),"state"),"pickup");rejects(bad.get(),"incomplete settings reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_array_get(json_object_get(bad.get(),"params"),1),"id",json_integer(0));rejects(bad.get(),"duplicate parameter IDs reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_array_get(json_object_get(bad.get(),"params"),0),"value",json_string("NaN"));rejects(bad.get(),"nonnumeric values reject");}
    {Json bad(json_deep_copy(good.get()));json_array_remove(json_object_get(json_object_get(bad.get(),"bank"),"presets"),63);rejects(bad.get(),"truncated banks reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_array_get(json_object_get(json_object_get(json_object_get(bad.get(),"bank"),"chains"),"working"),0),"slot",json_integer(64));rejects(bad.get(),"invalid chain slots reject");}
    {Json bad(json_deep_copy(good.get()));json_object_set_new(json_array_get(json_object_get(json_object_get(bad.get(),"state"),"pickup"),0),"target",json_real(2));rejects(bad.get(),"out of range pickup rejects");}
    for(int id=0;id<M::NUM_PARAMS;++id){Json bad(json_deep_copy(good.get()));json_object_set_new(json_array_get(json_object_get(bad.get(),"params"),id),"value",json_real(source->paramQuantities[id]->maxValue+100.));require(!validate(source.get(),bad.get(),scope,error),"every parameter rejects values beyond its metadata range");}
    const std::string path="build/tests/backup-é-音.json";require(write(path,good.get(),error),"Unicode filename saves atomically");Json loaded(read(path,error));require(loaded && validate(destination.get(),loaded.get(),scope,error),"Unicode backup file reloads");
    auto* pitch=json_object_get(json_array_get(json_object_get(loaded.get(),"params"),M::PITCH),"value");require(float(json_number_value(pitch))==source->params[M::PITCH].getValue(),"nine-digit JSON preserves exact float32 values");
    Json settings(capture(module.get(),Settings));require(write(path,settings.get(),error),"atomic save replaces existing file");loaded.reset(read(path,error));require(loaded && std::string(json_string_value(json_object_get(loaded.get(),"scope")))=="settings","replacement contains complete new document");
    require(!write("build/tests/no-such-backup-folder/file.json",good.get(),error),"missing directory reports save failure");
    auto bytes=std::unique_ptr<char,decltype(&std::free)>(json_dumps(loaded.get(),0),std::free);FILE* held=openRead(path);require(held,"existing backup available");
#ifdef ARCH_WIN
    HANDLE locked=CreateFileW(wide(path).c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);require(locked!=INVALID_HANDLE_VALUE,"backup locked for replacement failure test");require(!write(path,good.get(),error),"locked destination save fails without destroying original");CloseHandle(locked);
#endif
    std::fclose(held);Json preserved(read(path,error));require(json_equal(preserved.get(),loaded.get()),"failed save preserves prior file bytes and contents");
    auto raw=[&](const std::string& text){FILE* f=stream(createExclusive(path+".malformed"));require(f,"malformed fixture opened");std::fwrite(text.data(),1,text.size(),f);std::fclose(f);Json rejected(read(path+".malformed",error));require(!rejected && !error.empty(),"bounded parser rejects malformed or oversized file");discard(path+".malformed");};
    raw("{\"format\":1,\"format\":2}");raw("{\"type\":");raw(std::string(MaxBytes+1,' '));raw("{\"value\":NaN}");
    discard(path);rack::plugin::plugins.pop_back();plugin.models.clear();modelMetropolix->plugin=nullptr;
    std::cout<<"Backup file and native engine restore tests passed\n";
}
