// Native owner2 table mask and actual selected-field interaction regressions.
using M=Metropolix;
#include "OwnershipDisplayReference.hpp"
OledCanvas icon;icon.ownerHint(0,0,1);bool exact=true;
for(int y=0;y<64;++y)for(int x=0;x<128;++x)exact &= bool(icon.pixels[y][x])==(x<13 && y<13 && bool(nativeOwner2[y]&(1<<(12-x))));
check(exact,"native CTRL 2 ownership symbol matches the complete inverted encoder legend mask");
APP->engine->bypassModule(m,true);std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
auto clean=[&]{closeMenus();for(auto* a:APP->history->actions)delete a;APP->history->actions.clear();APP->history->savedIndex=APP->history->actionIndex=0;for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->clearWorkflows();m->cancelDraft();for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);m->params[M::RUN].setValue(0);m->params[M::ENC_CONFIRM].setValue(1);m->uiMod.store(false);m->uiAlt.store(false);m->externalClock.store(false);};
auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));auto* encoder=find("Encoder");
struct OwnedField {int page,field,destination;};
const OwnedField cases[]={{M::BPM_PAGE,0,metro::CtrlTempo},{M::ORDER_PAGE,0,metro::CtrlOrder},{M::LENGTH_PAGE,0,metro::CtrlStagesLength},{M::LENGTH_PAGE,1,metro::CtrlPulsesLength},{M::DIV_PAGE,0,metro::CtrlClockDiv},{M::DIV_PAGE,1,metro::CtrlPulseDiv},{M::SLIDE_PAGE,0,metro::CtrlSlideAmount},{M::GATE_PAGE,0,metro::CtrlGateLength},{M::GATE_PAGE,1,metro::CtrlGateScale},{M::SWING_PAGE,0,metro::CtrlSwing}};
bool pixels=true,locked=true,menus=true;int count=0;
for(int track=0;track<2;++track)for(int knob=0;knob<2;++knob)for(bool both:{false,true})for(const auto& item:cases){
    clean();m->selectedTrack.store(track);screenPage(m,item.page);m->uiField.store(item.field);m->params[M::ctrlParam(knob,M::CTRL_DESTINATION)].setValue(item.destination);m->params[M::ctrlParam(knob,M::CTRL_ASSIGNMENT)].setValue(both?2:track);
    int id=screenParam(m),top=item.field?42:14;auto before=raw();auto actual=display->render();pixels &= m->ctrlOwner(id)==knob && raw()==before && !APP->history->actionIndex;
    const uint16_t outline[]={496,520,1028,2050,4097,4097,4097,4097,4097,2050,1028,520,496};const uint8_t one[]={1,3,5,1,1,1,1};
    // Lower and BPM selections' bottom corners also occupy five cells in this
    // region. Compare their union explicitly, without excluding any cells.
    for(int y=0;y<13;++y){uint16_t row=knob?nativeOwner2[y]:outline[y];if(!knob && y>=3 && y<10)row|=uint16_t(one[y-3])<<5;for(int x=0;x<13;++x)pixels &= bool(actual.pixels[top+y][1+x])==(bool(row&(1<<(12-x))) || ((item.field || item.page==M::BPM_PAGE) && y==12 && x<5));}
    if(track==0 && !both){std::ofstream out(dir+"/owner-field-"+std::to_string(count)+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<char*>(actual.pixels),8192);}
    for(auto* control:{encoder,static_cast<HardwareControl*>(display)}){drag(control,-220);locked &= raw()==before && !pendingConfirmation(m) && !APP->history->actionIndex;widget::EventContext ctx;event::DoubleClick dbl;dbl.context=&ctx;control->onDoubleClick(dbl);locked &= raw()==before && !APP->history->actionIndex;press(control,GLFW_MOUSE_BUTTON_RIGHT);menus &= seek(APP->scene,[&](widget::Widget* w){auto* label=dynamic_cast<ui::MenuLabel*>(w);return label && label->text=="CTRL "+std::to_string(knob+1);})!=nullptr && raw()==before && !APP->history->actionIndex;closeMenus();}
    ++count;
}
check(pixels,std::to_string(count)+" actual selected CTRL ownership markers retain distinct centered digits on ten fields and both tracks");
check(locked,"owned encoder/OLED drag and reset remain read-only without previews or history on every marker case");
check(menus,"owned encoder/OLED menus retain the correct CTRL identity on every marker case");
clean();for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
