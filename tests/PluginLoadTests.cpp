// Load the stripped download through the public Rack API. No instrument
// implementation is compiled into this host.
#ifdef ARCH_WIN
#define NOMINMAX
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <engine/Engine.hpp>
#include <plugin.hpp>
#undef PRIVATE
#include <rack.hpp>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void require(bool ok, const std::string& message) {
    if (!ok) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
void require(bool ok, const char* message) {
    if (!ok) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
struct JsonDelete { void operator()(json_t* value) const { json_decref(value); } };
using Json = std::unique_ptr<json_t, JsonDelete>;
using Module = rack::engine::Module;
using Init = void (*)(rack::plugin::Plugin*);

Init load(const char* path) {
    // Keep the library loaded until process exit: model vtables belong to it.
#ifdef ARCH_WIN
    int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, nullptr, 0);
    require(size > 0, "UTF-8 production path");
    std::vector<wchar_t> wide(size);
    require(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide.data(), size) != 0, "wide production path");
    HMODULE binary = LoadLibraryW(wide.data());
    require(binary != nullptr, "LoadLibrary production binary (Windows error " + std::to_string(GetLastError()) + ")");
    auto symbol = GetProcAddress(binary, "init");
    Init init = nullptr;
    static_assert(sizeof(init) == sizeof(symbol), "Windows function pointer size");
    std::memcpy(&init, &symbol, sizeof(init));
#else
    void* binary = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!binary) { std::cerr << dlerror() << '\n'; std::exit(1); }
    Init init = reinterpret_cast<Init>(dlsym(binary, "init"));
#endif
    require(init != nullptr, "production init exported");
    return init;
}
int param(Module& module, const std::string& name) {
    for (size_t id = 0; id < module.params.size(); ++id)
        if (module.paramQuantities[id] && module.paramQuantities[id]->name == name) return int(id);
    require(false, "production parameter metadata: " + name); return -1;
}
void restore(Module& module, json_t* saved) {
    Json receipt(json_deep_copy(saved));
    require(receipt != nullptr, "copy complete production receipt");
    // A Rack history restore preserves RUN regardless of Run on boot.
    json_t* data = json_object_get(receipt.get(), "data");
    if (data) json_object_set_new(data, "preserveTransportState", json_true());
    module.fromJson(receipt.get());
    Json after(module.toJson());
    auto* beforeParams = json_object_get(saved, "params");
    auto* afterParams = json_object_get(after.get(), "params");
    if (!json_equal(beforeParams, afterParams)) {
        size_t index; json_t* value;
        json_array_foreach(beforeParams, index, value) {
            if (!json_equal(value, json_array_get(afterParams, index))) {
                std::cerr << "PARAMETER DIFFERENCE: " << index << " before=" << json_number_value(json_object_get(value, "value"))
                    << " after=" << json_number_value(json_object_get(json_array_get(afterParams, index), "value")) << '\n';
            }
        }
        json_dump_file(saved, "build/tests/production-restore-before.json", JSON_INDENT(2));
        json_dump_file(after.get(), "build/tests/production-restore-after.json", JSON_INDENT(2));
        require(false, "exact production parameter restore");
    }
    auto* beforeData = json_object_get(saved, "data");
    auto* afterData = json_object_get(after.get(), "data");
    if ((beforeData || afterData) && !json_equal(beforeData, afterData)) {
        const char* key; json_t* value;
        json_object_foreach(beforeData, key, value)
            if (!json_equal(value, json_object_get(afterData, key))) std::cerr << "RESTORE DIFFERENCE: " << key << '\n';
        json_dump_file(saved, "build/tests/production-restore-before.json", JSON_INDENT(2));
        json_dump_file(after.get(), "build/tests/production-restore-after.json", JSON_INDENT(2));
        require(false, "exact production bank/settings restore");
    }
}
struct Soak {
    Module& main; Module& mx;
    Module::ProcessArgs args{};
    uint32_t seed = 0x4d455452u;
    uint64_t frames = 0, checked = 0, mutations = 0, restores = 0, freshRestores = 0, resets = 0, reconnects = 0;
    double virtualSeconds = 0;
    bool linked = false;
    int run, reset, clockSource;
    std::vector<bool> touchedMin, touchedMax, touchedInterior;
    Soak(Module& main, Module& mx) : main(main), mx(mx),
        run(param(main, "Transport")), reset(param(main, "Reset position and accumulators")), clockSource(param(main, "Clock source")),
        touchedMin(main.params.size()), touchedMax(main.params.size()), touchedInterior(main.params.size()) {}
    uint32_t random() { seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5; return seed; }
    void rate(float value) {
        args.sampleRate = value; args.sampleTime = 1.f / value;
        Module::SampleRateChangeEvent event{}; event.sampleRate = value; event.sampleTime = args.sampleTime;
        main.onSampleRateChange(event); mx.onSampleRateChange(event);
    }
    void link(bool value, bool stale = false) {
        main.rightExpander.module = value && !stale ? &mx : nullptr;
        mx.leftExpander.module = value ? &main : nullptr;
        linked = value && !stale;
        Module::ExpanderChangeEvent event{}; event.side = true; main.onExpanderChange(event);
        event.side = false; mx.onExpanderChange(event);
        if (linked) ++reconnects;
    }
    void frame() {
        args.frame = int64_t(frames++);
        virtualSeconds += args.sampleTime;
        main.process(args); mx.process(args);
        for (size_t id = 0; id < main.outputs.size(); ++id) {
            float value = main.outputs[id].getVoltage();
            if (!std::isfinite(value)) require(false, "finite main output at frame " + std::to_string(args.frame));
            if (id == 1 || id == 3 || id == 4) require(value == 0.f || value == 5.f, "main digital output is 0/5 V");
            ++checked;
        }
        for (auto& output : mx.outputs) {
            float value = output.getVoltage();
            require(std::isfinite(value) && (value == 0.f || value == 5.f), "finite 0/5 V MX output");
            if (!linked) require(value == 0.f, "detached/stale production MX immediately low");
            ++checked;
        }
        if (mx.leftExpander.messageFlipRequested) {
            std::swap(mx.leftExpander.producerMessage, mx.leftExpander.consumerMessage);
            mx.leftExpander.messageFlipRequested = false;
        }
    }
    void mutate(int phase) {
        for (size_t id = 0; id < main.params.size(); ++id) {
            auto* q = main.paramQuantities[id];
            require(q && std::isfinite(q->minValue) && std::isfinite(q->maxValue), "bounded parameter metadata");
            float value = phase == 0 ? q->minValue : phase == 1 ? q->maxValue :
                q->minValue + (q->maxValue - q->minValue) * (float(random() & 0xffff) / 65535.f);
            if (q->snapEnabled) value = std::round(value);
            value = std::max(q->minValue, std::min(q->maxValue, value));
            main.params[id].setValue(value); ++mutations;
            (phase == 0 ? touchedMin : phase == 1 ? touchedMax : touchedInterior)[id] = true;
        }
        main.params[reset].setValue(0);
    }
    void roundtrip() {
        Json saved(main.toJson()); restore(main, saved.get()); ++restores;
        // Loading a patch constructs a fresh instance, unlike history restore.
        std::unique_ptr<Module> fresh(main.model->createModule());
        require(fresh != nullptr, "fresh production factory instance");
        fresh->model = main.model; fresh->id = main.id + 1000;
        Module::SampleRateChangeEvent event{}; event.sampleRate = args.sampleRate; event.sampleTime = args.sampleTime;
        fresh->onSampleRateChange(event); restore(*fresh, saved.get()); ++freshRestores;
    }
};
int main(int argc, char** argv) {
    require(argc == 3, "production binary and backup fixture paths supplied");
    const auto started = std::chrono::steady_clock::now();
    Init init = load(argv[1]);
    rack::random::init();
    rack::Context context; rack::contextSet(&context); context.engine = new rack::engine::Engine;
    rack::plugin::Plugin plugin; plugin.slug = "MetropolixLab";
    std::string binaryPath = argv[1]; size_t slash = binaryPath.find_last_of("/\\");
    std::string manifestPath = binaryPath.substr(0, slash + 1) + "plugin.json";
    json_error_t error{}; Json manifest(json_load_file(manifestPath.c_str(), 0, &error));
    require(manifest && json_is_string(json_object_get(manifest.get(), "version")), "adjacent production manifest");
    plugin.version = json_string_value(json_object_get(manifest.get(), "version")); init(&plugin);
    rack::plugin::plugins.push_back(&plugin);
    require(plugin.models.size() == 2, "production registers exactly two public models");
    std::unique_ptr<Module> main, mx;
    for (auto* model : plugin.models) {
        std::unique_ptr<Module> module(model->createModule());
        require(module != nullptr, "production factory constructs module"); module->model = model;
        if (model->slug == "Metropolix") main = std::move(module);
        else if (model->slug == "MX") mx = std::move(module);
        else require(false, "only public production model slugs");
    }
    require(main && mx && main->params.size() == 789 && main->outputs.size() == 7 && mx->outputs.size() == 8, "production parameter/port identities");
    main->id = 901; mx->id = 902;
    Soak soak(*main, *mx); soak.rate(48000); soak.link(true);
    main->params[soak.run].setValue(1);
    bool gate = false, mxSignal = false;
    for (int frame = 0; frame < 48000; ++frame) {
        soak.frame(); gate |= main->outputs[1].getVoltage() == 5.f;
        for (auto& output : mx->outputs) mxSignal |= output.getVoltage() == 5.f;
    }
    require(gate && mxSignal, "actual default production sequencer and linked MX generate signals");
    Json backup(json_load_file(argv[2], 0, &error));
    require(backup && json_is_object(json_object_get(backup.get(), "bank")), "public backup fixture loads");
    Json initial(main->toJson()); json_t* data = json_object_get(initial.get(), "data");
    json_t* bank = json_object_get(backup.get(), "bank");
    for (const char* field : {"presets", "presetFormat", "chains", "chainFormat"})
        require(json_object_set(data, field, json_object_get(bank, field)) == 0, "copy public bank fixture");
    // Published JSON has decimal floats; the first import canonicalizes them
    // to Rack's float precision. Exact receipts start from that loaded state.
    json_object_set_new(data, "preserveTransportState", json_true());
    main->fromJson(initial.get());
    Json fixture(main->toJson());
    auto* loadedPresets = json_object_get(json_object_get(fixture.get(), "data"), "presets");
    require(json_array_size(loadedPresets) == 64, "public fixture restores 64-slot bank");
    size_t occupied = 0, expectedOccupied = 0;
    for (size_t slot = 0; slot < 64; ++slot) {
        occupied += json_is_array(json_array_get(loadedPresets, slot));
        expectedOccupied += json_is_array(json_array_get(json_object_get(bank, "presets"), slot));
    }
    require(occupied > 0 && occupied == expectedOccupied, "occupied public preset fixture remains populated");
    restore(*main, fixture.get()); ++soak.restores;
    // 24 five-second blocks at each rate = 360 virtual seconds.
    for (float rate : {44100.f, 48000.f, 96000.f}) {
        soak.rate(rate);
        for (int block = 0; block < 24; ++block) {
            if (block % 6 == 0) {
                Module::ResetEvent event{}; main->onReset(event); mx->onReset(event); ++soak.resets;
                Json resetState(main->toJson());
                require(json_equal(json_object_get(json_object_get(resetState.get(), "data"), "presets"),
                    json_object_get(json_object_get(fixture.get(), "data"), "presets")), "Rack Initialize preserves saved production presets");
            }
            soak.mutate(block % 3);
            main->params[soak.clockSource].setValue(block % 2 ? 2.f : 1.f);
            main->params[soak.run].setValue(block % 4 == 3 ? 0.f : 1.f);
            main->inputs[0].channels = block % 2; main->inputs[1].channels = 1;
            for (int input = 2; input < 5; ++input) {
                main->inputs[input].channels = 1;
                main->inputs[input].setVoltage(float(int(soak.random() % 2001) - 1000) / 100.f);
            }
            soak.link(true);
            main->inputs[0].setVoltage(0); main->inputs[1].setVoltage(0);
            // CTRL direct assignments update their target parameters on the
            // control tick. Settle bulk edits before saving the musical state.
            for (int frame = 0; frame < 64; ++frame) soak.frame();
            soak.roundtrip();
            int length = int(rate * 5);
            for (int frame = 0; frame < length; ++frame) {
                // Detach/stale half-links with pending delayed output; reconnect.
                if (frame == length / 4) soak.link(false);
                if (frame == length / 2) soak.link(true, true);
                if (frame == 3 * length / 4) soak.link(true);
                if (frame == length / 2 + 1) soak.roundtrip();
                main->inputs[0].setVoltage(frame % int(rate / 8) < 32 ? 5.f : 0.f);
                main->inputs[1].setVoltage(frame == length / 3 ? 5.f : 0.f);
                soak.frame();
            }
        }
        std::cout << "PASS: production rate " << int(rate) << " Hz, 120 virtual seconds, cumulative frames " << soak.frames << std::endl;
    }
    for (size_t id = 0; id < main->params.size(); ++id)
        require(soak.touchedMin[id] && soak.touchedMax[id] && soak.touchedInterior[id], "every parameter visits all edit phases");
    soak.link(false); soak.frame();
    Json mxState(mx->toJson()); restore(*mx, mxState.get());
    double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
    std::cout << "METRICS frames=" << soak.frames << " outputChecks=" << soak.checked << " parameterEdits=" << soak.mutations
        << " stateRestores=" << soak.restores << " freshInstances=" << soak.freshRestores
        << " initializes=" << soak.resets << " reciprocalLinks=" << soak.reconnects
        << " virtualSeconds=" << soak.virtualSeconds << " seed=0x4d455452 wallSeconds=" << seconds << std::endl;
    main.reset(); mx.reset(); rack::plugin::plugins.clear(); plugin.models.clear();
    delete context.engine; context.engine = nullptr; rack::contextSet(nullptr);
    std::cout << "PASS: stripped production binary loads, generates signals and survives runtime soak with exact state restores\n";
}
