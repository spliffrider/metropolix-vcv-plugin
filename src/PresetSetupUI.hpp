constexpr int PresetSetupCount=8;
void screenPage(Metropolix* m,int page);
bool presetSetupPage(Metropolix* m){return m && m->uiPage.load()==Metropolix::PRESET_SETUP_PAGE;}
// Keep legacy field IDs stable; only the presentation follows the manual order.
int presetSetupItem(int position){const int items[]={1,3,4,5,6,7,0,2};return items[metro::clamp(position,0,PresetSetupCount-1)];}
int presetSetupPosition(int item){for(int n=0;n<PresetSetupCount;++n)if(presetSetupItem(n)==item)return n;return 0;}
int presetSetupDisplayRow(int item){int position=presetSetupPosition(item);return position+(position>=6);}
int presetSetupDisplayItem(int row){return row==6?-1:presetSetupItem(row-(row>6));}
const char* presetSetupName(int item){const char* names[]={"Total Pulses","Queue Pulses","Load Delay","Pitch Load","Switch Load","Ctrl Load","Aux Load","Set Init Preset"};return names[metro::clamp(item,0,PresetSetupCount-1)];}
int presetSetupParam(Metropolix* m,int item=-1){if(!m)return -1;int field=metro::clamp(item<0?m->uiField.load():item,0,PresetSetupCount-1);return field==7?Metropolix::INIT_PRESET:Metropolix::TOTAL_PULSES+field;}
std::string presetSetupValue(Metropolix* m,int item){
    if(!m)return {};int id=presetSetupParam(m,item),v=int(std::round(m->uiValue(id)));std::string text;
    if(item<2)text=v?std::to_string(v):"--";
    else if(item==2){const char* names[]={"Instant","Queue","Total"};text=names[metro::clamp(v,0,2)];}
    else if(item==3){const char* names[]={"Off","Thru","Jump"};text=names[metro::clamp(v,0,2)];}
    else if(item==4)text=v?"Jump":"Off";
    else if(item==5){const char* names[]={"Off","Assigns","Thru","Jump"};text=names[metro::clamp(v,0,3)];}
    else if(item==6)text=v?"Assigns":"Off";
    else {v=metro::clamp(v,0,64);text=v?string::f("%d-%d",(v-1)/8+1,(v-1)%8+1):"Off";}
    if(m->uiDraftParam==id)text+=" ?";return text;
}
void openPresetSetup(Metropolix* m){if(m){screenPage(m,Metropolix::PRESET_SETUP_PAGE);m->uiAlt.store(false);}}
void presetSetupEncoderPress(Metropolix* m){
    int id=presetSetupParam(m);auto* q=m->getParamQuantity(id);if(m->ctrlOwner(id)>=0)return;
    if(q->getRange()==1){m->cancelDraft();setWithHistory(q,q->getValue()==q->getMinValue()?q->getMaxValue():q->getMinValue());m->presetSetupEditing.store(false);}
    else m->presetSetupEditing.store(!m->presetSetupEditing.load());
}
std::string presetSetupHelp(Metropolix* m){
    if(!presetSetupPage(m))return {};int item=m->uiField.load();auto* q=m->getParamQuantity(presetSetupParam(m));
    return std::string(presetSetupName(item))+" = "+presetSetupValue(m,item)+(q->getRange()==1?"\nClick to toggle. Hold encoder to restore default.":m->presetSetupEditing.load()?"\nDrag up/down to edit. Click to finish; ? needs confirmation.":"\nDrag to browse. Click to edit. Hold encoder to restore default.");
}
