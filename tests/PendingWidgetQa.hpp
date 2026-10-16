using M=Metropolix;
if(std::getenv("METROPOLIX_PENDING_REOPEN_QA")) {
    if(qaFrame==6) {
        check(m->integer(M::ROOT)==9 && m->integer(M::MOD_GROUP_MUTE)==1,"confirmed external root and unrelated mute survive reopen");
        check(m->uiDraftParam<0,"pending edit is transient across patch reopening");
        check(!qaFailed,"PENDING REOPEN QA COMPLETE");APP->window->close();
    }return;
}
if(qaFrame==3){
#include "ResetHistoryWidgetQa.hpp"
}
if(qaFrame==4){
#include "ShortPressHistoryWidgetQa.hpp"
}
if(qaFrame==5){
{
#include "Mx165WidgetQa.hpp"
}
#include "MicroCvHistoryWidgetQa.hpp"
{
#include "RawValueHistoryWidgetQa.hpp"
}
{
#include "DependentValueWidgetQa.hpp"
}
{
#include "ModTypeWidgetQa.hpp"
}
{
#include "PairedDragHistoryWidgetQa.hpp"
}
{
#include "LockedModMenuWidgetQa.hpp"
}
{
#include "NativeValueActionsWidgetQa.hpp"
}
{
#include "NativeValueMatrixWidgetQa.hpp"
}
{
#include "ModTimingNavigationWidgetQa.hpp"
}
{
#include "PrimarySetupWidgetQa.hpp"
}
{
#include "SecondaryMenuWidgetQa.hpp"
}
{
#include "PresetSetupWidgetQa.hpp"
}
{
#include "AccumDisplayWidgetQa.hpp"
}
{
#include "MenuFontWidgetQa.hpp"
}
{
#include "PerformanceDisplayWidgetQa.hpp"
}
{
#include "OwnershipDisplayWidgetQa.hpp"
}
{
#include "MenuDisplayWidgetQa.hpp"
}
{
#include "PerformanceFieldsWidgetQa.hpp"
}
{
#include "PitchPreviewWidgetQa.hpp"
}
{
#include "PickupValueActionsWidgetQa.hpp"
}
{
#include "HomeDisplayWidgetQa.hpp"
}
{
#include "BinaryStageWidgetQa.hpp"
}
{
#include "BarStageWidgetQa.hpp"
}
{
#include "CurveStageWidgetQa.hpp"
}
}
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
auto root=[&]{m->cancelDraft();m->params[M::SCALE].setValue(1);m->params[M::ROOT].setValue(0);screenPage(m,M::SCALE_PAGE);m->uiField.store(0);};
widget::EventContext pendingContext;event::DragStart pendingStart;pendingStart.context=&pendingContext;pendingStart.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragMove pendingMove;pendingMove.context=&pendingContext;pendingMove.button=GLFW_MOUSE_BUTTON_LEFT;pendingMove.mouseDelta=Vec(0,-22);
event::DragEnd pendingEnd;pendingEnd.context=&pendingContext;pendingEnd.button=GLFW_MOUSE_BUTTON_LEFT;
if(qaFrame==6) {
    APP->engine->bypassModule(m,true);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->params[M::RUN].setValue(0);m->params[M::RUN_ON_BOOT].setValue(2);m->params[M::ENC_CONFIRM].setValue(1);
    auto* encoder=find("Encoder");root();drag(encoder,-22);check(pendingConfirmation(m) && m->uiDraftValue==1,"prepare pending root through actual encoder drag");
    m->getParamQuantity(M::ROOT)->setValue(9);auto before=raw();int history=APP->history->actionIndex;click(encoder);
    check(raw()==before && m->uiDraftParam<0 && APP->history->actionIndex==history,"obsolete confirmation preserves an external root edit without new history");
    root();encoder->onDragStart(pendingStart);encoder->onDragMove(pendingMove);m->getParamQuantity(M::ROOT)->setValue(9);before=raw();history=APP->history->actionIndex;encoder->onDragEnd(pendingEnd);
    check(raw()==before && APP->history->actionIndex==history && !APP->window->isCursorLocked(),"external change during confirming drag survives release without fake native history or cursor capture");
    m->cancelDraft();m->uiMod.store(false);m->selectedTrack.store(0);m->selectedStage.store(0);m->selectedRow.store(M::CV);screenPage(m,M::STAGE_PAGE);m->uiAlt.store(true);m->params[M::cvTypeParam(0)].setValue(metro::UnipolarCv);
    for(int s=0;s<8;++s)m->params[M::stageParam(0,s,M::CV)].setValue(s?1.2f:-3.75f);
    drag(encoder,-100);check(m->uiDraftIds.size()==8,"prepare bulk CV draft with dormant negative raw value");m->getParamQuantity(M::stageParam(0,3,M::CV))->setValue(2.75f);before=raw();history=APP->history->actionIndex;click(encoder);
    check(raw()==before && m->uiDraftParam<0 && APP->history->actionIndex==history,"bulk confirmation preserves an external edit to another captured stage");
    root();drag(encoder,-22);m->getParamQuantity(M::ROOT)->setValue(9);before=raw();history=APP->history->actionIndex;encoder->step();
    check(m->uiDraftParam<0 && raw()==before && APP->history->actionIndex==history,"UI step clears stale preview without changing external value or history");
    root();drag(encoder,-22);m->params[M::MOD_GROUP_MUTE].setValue(1);history=APP->history->actionIndex;click(encoder);
    check(m->integer(M::ROOT)==1 && m->integer(M::MOD_GROUP_MUTE)==1 && APP->history->actionIndex==history+1,"unrelated external change retains valid confirmation and one genuine undo");
    auto* display=find("Metropolix display");bool bulkCancel=true,bulkUndo=true,release=true;
    for(int t=0;t<2;++t)for(int changed=0;changed<8;++changed)for(int row:{M::CV,M::RATCHETS,M::PROBABILITY,M::ACCUMULATION}) {
        m->cancelDraft();m->uiMod.store(false);m->selectedTrack.store(t);m->selectedStage.store((changed+1)%8);m->selectedRow.store(row);screenPage(m,M::STAGE_PAGE);m->uiAlt.store(true);
        m->params[M::cvTypeParam(t)].setValue(metro::UnipolarCv);
        for(int s=0;s<8;++s)m->params[M::stageParam(t,s,row)].setValue(row==M::CV?(s?-1.f:-3.75f):row==M::RATCHETS?2:row==M::PROBABILITY?50:0);
        auto* c=changed%2?display:encoder;c->onDragStart(pendingStart);pendingMove.mouseDelta=Vec(0,row==M::CV?-100:-22);c->onDragMove(pendingMove);
        int id=M::stageParam(t,changed,row);m->getParamQuantity(id)->setValue(row==M::CV?2.75f:row==M::RATCHETS?4:row==M::PROBABILITY?71:5);
        before=raw();history=APP->history->actionIndex;c->onDragEnd(pendingEnd);
        release &= raw()==before && APP->history->actionIndex==history && !APP->window->isCursorLocked() && !c->adjustmentTip;
        drag(c,row==M::CV?-100:-22);m->getParamQuantity(id)->setValue(row==M::CV?1.5f:row==M::RATCHETS?6:row==M::PROBABILITY?33:-5);before=raw();history=APP->history->actionIndex;click(encoder);bulkCancel &= raw()==before && APP->history->actionIndex==history && m->uiDraftParam<0;
        // A fresh, valid bulk preview still applies once, with exact raw undo.
        screenPage(m,M::STAGE_PAGE);m->uiAlt.store(true);drag(c,row==M::CV?-100:-22);before=raw();history=APP->history->actionIndex;click(encoder);auto after=raw();
        bulkUndo &= after!=before && APP->history->actionIndex==history+1;APP->history->undo();bulkUndo &= raw()==before;APP->history->redo();bulkUndo &= raw()==after;
    }
    check(bulkCancel && release,"external edits at all eight stages on both tracks survive CV/Ratchet/PROB/ACCUM preview release and confirmation without false history");
    check(bulkUndo,"fresh bulk previews after conflict retain exact single-action undo/redo including dormant negative CV values");
    root();encoder->onDragStart(pendingStart);pendingMove.mouseDelta=Vec(0,-22);encoder->onDragMove(pendingMove);m->getParamQuantity(M::ROOT)->setValue(9);encoder->onDragMove(pendingMove);encoder->onDragEnd(pendingEnd);before=raw();history=APP->history->actionIndex;click(encoder);
    check(before[M::ROOT]==9 && m->integer(M::ROOT)==10 && APP->history->actionIndex==history+1,"continued intentional movement restarts preview from latest external value");APP->history->undo();check(raw()==before,"restarted preview undoes to external value");
    root();drag(encoder,-22);m->uiAlt.store(!m->uiAlt.load());before=raw();history=APP->history->actionIndex;click(encoder);
    check(raw()==before && APP->history->actionIndex==history && m->uiDraftParam<0,"changed ALT scope discards obsolete draft without applying values");m->uiAlt.store(false);
    m->params[M::SCALE_LOCK].setValue(-1);screenPage(m,M::LENGTH_PAGE);m->selectedTrack.store(0);int length=M::trackParam(0,M::LENGTH);m->params[length].setValue(4);drag(encoder,-22);
    m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlStagesLength);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);before=raw();history=APP->history->actionIndex;click(encoder);
    check(raw()==before && APP->history->actionIndex==history && m->uiDraftParam<0,"new CTRL ownership prevents pending confirmation from overriding its target");m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    auto overlay=[&]()->widget::Widget*{for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if((*it)->visible && dynamic_cast<ui::MenuOverlay*>(*it))return *it;return nullptr;};
    root();drag(encoder,-22);history=APP->history->actionIndex;press(encoder,GLFW_MOUSE_BUTTON_RIGHT);
    auto* menu=overlay();auto* initialize=menu?dynamic_cast<ui::MenuItem*>(seek(menu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Initialize";})):nullptr;
    check(initialize && m->uiDraftParam<0,"actual native context menu opening cancels matching preview");if(initialize){event::Action e;initialize->onAction(e);}closeMenus();
    check(m->integer(M::ROOT)==0 && m->uiDraftParam<0 && APP->history->actionIndex==history,"native no-op initialize retains value and adds no undo");
    root();m->params[M::ROOT].setValue(5);drag(display,-22);before=raw();history=APP->history->actionIndex;press(display,GLFW_MOUSE_BUTTON_RIGHT);menu=overlay();
    auto* choice=menu?dynamic_cast<ui::MenuItem*>(seek(menu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="A";})):nullptr;if(choice){event::Action e;choice->onAction(e);}closeMenus();auto after=raw();
    bool nativeUndo=choice && m->integer(M::ROOT)==9 && m->uiDraftParam<0 && APP->history->actionIndex==history+1;APP->history->undo();nativeUndo &= raw()==before;APP->history->redo();nativeUndo &= raw()==after;
    check(nativeUndo,"native Root menu replaces preview with one exact undo/redo action");
    screenPage(m,M::BPM_PAGE);m->uiAlt.store(false);m->params[M::CLOCK_SOURCE].setValue(1);m->externalClock.store(false);m->params[M::TEMPO].setValue(120);drag(encoder,-100);before=raw();history=APP->history->actionIndex;press(encoder,GLFW_MOUSE_BUTTON_RIGHT);menu=overlay();
    auto* field=menu?dynamic_cast<ui::TextField*>(seek(menu,[](widget::Widget* w){return dynamic_cast<ui::TextField*>(w)!=nullptr;})):nullptr;
    if(field){field->text="137";event::SelectKey e;e.context=&pendingContext;e.key=GLFW_KEY_ENTER;e.action=GLFW_PRESS;e.mods=0;field->onSelectKey(e);}closeMenus();after=raw();
    nativeUndo=field && m->integer(M::TEMPO)==137 && m->uiDraftParam<0 && APP->history->actionIndex==history+1;APP->history->undo();nativeUndo &= raw()==before;APP->history->redo();nativeUndo &= raw()==after;
    check(nativeUndo,"actual native numeric entry replaces preview with one exact undo/redo action");
    APP->engine->bypassModule(m,false);
    m->cancelDraft();m->uiAlt.store(false);m->params[M::ROOT].setValue(9);m->params[M::MOD_GROUP_MUTE].setValue(1);screenPage(m,M::SCALE_PAGE);m->uiField.store(0);drag(encoder,22);
}
if(qaFrame==8)metroqa::screenshot(dir+"/pending-conflict.png");
if(qaFrame==10){APP->patch->save(dir+"/pending-roundtrip.vcv");check(!qaFailed,"PENDING UI QA COMPLETE");APP->window->close();}
