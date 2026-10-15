// Included inside HardwareDisplay. A physical-slider preview does not apply
// stage overrides, accumulation or temporary pitch modulation to its note.
int previewScale(int track) const {
    auto* m=metroModule;int locked=m->integer(Metropolix::SCALE_LOCK),shown=locked<0?m->integer(Metropolix::SCALE):locked;
    if(metro::userScale(locked)){int playing=m->soundingScale[track].load();if(metro::userScale(playing) && (playing-49)/10==(locked-49)/10)shown=playing;}
    return shown;
}
int baseSliderPitch(int track,int stage,int shown,bool physical,bool overrides) const {
    auto* m=metroModule;auto scale=m->uiScale(shown);int root=m->integer(m->scaleRootParam(shown));
    int anchor=overrides?integer(Metropolix::stageParam(track,stage,Metropolix::PITCH_OVERRIDE),-1):-1;
    float source=anchor<0?m->uiSliderPosition(track,stage,physical)*12.f*m->uiSliderOctaves(track):float(anchor);
    int bottom=m->integer(Metropolix::BOTTOM_PITCH);
    if(anchor<0 && bottom<12){int relative=metro::clamp(bottom,0,11)-root,first=metro::nearestDegree(float(relative),scale);if(metro::degreeToSemitone(first,scale)<relative)++first;source+=metro::degreeToSemitone(first,scale);}
    return metro::degreeToSemitone(metro::nearestDegree(source,scale),scale)+root+m->integer(Metropolix::trackParam(track,Metropolix::TRANSPOSE));
}
int previewPitch(int track,int stage,int shown) const {return baseSliderPitch(track,stage,shown,true,false);}
void pitchPreview(OledCanvas& c) const {
    auto* m=metroModule;int track=metro::clamp(m->selectedTrack.load(),0,1),stage=metro::clamp(m->pitchPreviewStage.load(),0,7),shown=previewScale(track);
    auto scale=m->uiScale(shown);int root=m->integer(m->scaleRootParam(shown)),pitch=previewPitch(track,stage,shown),note=metro::modulo(pitch,12);
    auto enabled=[&](int n){for(int i=0;i<scale.count;++i)if(scale.notes[i]==metro::modulo(n-root,12))return true;return false;};
    const int whites[]={0,2,4,5,7,9,11},blacks[]={1,3,6,8,10},lefts[]={30,44,65,79,93};
    for(int k=0;k<7;++k){int left=23+k*12;if(enabled(whites[k]))c.fill(left,6,11,28);else for(int y=6;y<34;++y)for(int x=left;x<left+11;++x)if((x+y)&1)c.dot(x,y);}
    for(int k=0;k<5;++k){int left=lefts[k];c.fill(left,6,7,18,0);if(!enabled(blacks[k]))for(int y=6;y<23;++y)for(int x=left+1;x<left+6;++x)if((x-left-1+y-6)%2==0)c.dot(x,y);}
    for(int k=0;k<7;++k)if(whites[k]==note)c.fill(26+k*12,26,5,5,0);
    for(int k=0;k<5;++k)if(blacks[k]==note)c.fill(lefts[k]+2,18,3,3);
    c.fill(23,35,83,17);c.valueText7(25,41,std::to_string(stage+1),0);
    std::string label=std::string(notes[note])+" "+std::to_string((pitch-note)/12);
    c.valueText7(104-c.valueWidth(label),41,label,0);
}
