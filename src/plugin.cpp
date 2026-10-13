#include "plugin.hpp"
#ifdef METROPOLIX_QA
#include "../tests/RackAudioQa.hpp"
namespace metrovideo {
std::atomic<int> samples{0};
std::atomic<bool> armed{false};
}
#endif
Plugin* pluginInstance;
void init(Plugin* p) {
    pluginInstance = p;
    p->addModel(modelMetropolix);
    p->addModel(modelMX);
#ifdef METROPOLIX_QA
    p->addModel(createModel<RackAudioQa, RackAudioQaWidget>("AudioQA"));
#endif
}
