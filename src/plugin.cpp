#include "plugin.hpp"
#ifdef METROPOLIX_QA
#include "../tests/RackAudioQa.hpp"
#endif
Plugin* pluginInstance;
void init(Plugin* p) {
    pluginInstance = p;
    p->addModel(modelMetropolix);
#ifdef METROPOLIX_QA
    p->addModel(createModel<RackAudioQa, RackAudioQaWidget>("AudioQA"));
#endif
}
