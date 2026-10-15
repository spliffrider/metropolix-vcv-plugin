using M=Metropolix;
#include "MenuDisplayReference.hpp"
APP->engine->bypassModule(m,true);std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));bool native=true;int count=0;
for(const auto& ref:menuDisplayReferences){
    for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->clearWorkflows();m->cancelDraft();m->params[M::RUN].setValue(0);m->uiMod.store(false);m->uiAlt.store(false);m->selectedTrack.store(0);
    if(ref.page==33){screenPage(m,M::TRACK_MENU_PAGE);m->trackMenuCursor.store(1);m->params[M::trackParam(0,M::TRANSPOSE)].setValue(0);m->params[M::outputTrackParam(0,M::SLIDER_OCTAVES)].setValue(3);m->params[M::outputTrackParam(0,M::SLIDER_DIRECTION)].setValue(0);m->params[M::trackBehaviorParam(0,M::PULSE_SOURCE)].setValue(0);}
    else {screenPage(m,M::SETUP_PAGE);m->setupCursor.store(1);m->params[M::CLOCK_SOURCE].setValue(1);m->params[M::CLOCK_RUNNING_ONLY].setValue(1);m->params[M::RUN_MODE].setValue(1);m->params[M::RESET_INPUT_MODE].setValue(0);}
    m->uiField.store(0);auto before=raw();auto actual=display->render();bool same=raw()==before && !APP->history->actionIndex;
    for(int y=0;y<64;++y)for(int x=0;x<128;++x)same &= bool(actual.pixels[y][x])==bool(ref.packed[y*16+x/8]&(1<<(7-x%8)));
    if(!same)check(false,string::f("independent menu p%d foreground reference",ref.page));native &= same;++count;
    std::ofstream image(dir+"/native-menu-p"+std::to_string(ref.page)+".pgm",std::ios::binary);image<<"P5\n128 64\n255\n";image.write(reinterpret_cast<char*>(actual.pixels),8192);
}
check(native,std::to_string(count)+" independent menu screens match Track8192 native cells and SETUP6656 native plus1536 bounded-row cells");
bool descenders=true;int descenderCount=0;
for(const std::string& word:{"g","p","j","q","y","Single","Legacy","Probability"})for(bool editing:{false,true}){
    OledCanvas actual;HardwareDisplay::listRow(actual,3,"Setting:",word,true,editing);OledCanvas glyph;glyph.menuRight(124,53,word);bool found=false;
    for(int y=53;y<64;++y)for(int x=124-glyph.menuWidth(word);x<124;++x)if(glyph.pixels[y][x]){descenders &= actual.pixels[y][x]==0;found |= y>=62;}
    descenders &= found;++descenderCount;
}
check(descenders,std::to_string(descenderCount)+" bottom-row descending words retain every glyph pixel, including tails at y62/63, in browse/value selection");
for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
