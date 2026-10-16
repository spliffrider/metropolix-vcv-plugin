// Included inside HardwareDisplay. Action identifiers have their own compact
// spacing; menu/field captions retain their independently measured metrics.
metrodisplay::MenuGlyph actionGlyph(unsigned char ch,bool compactMod=false) const {
    auto glyph=OledCanvas::chainCaption(ch);
    if(ch=='1')glyph.advance=compactMod?5:6;
    if(ch==':')glyph.advance=2;
    if(ch=='3')glyph={4,6,{14,1,6,1,14}};
    if(ch=='M' && compactMod)glyph.advance=6;
    return glyph;
}
int actionBearing(unsigned char ch,bool compactMod=false) const {return ch==':' || (ch=='M' && compactMod)?0:1;}
int actionWidth(const std::string& text,bool compactMod=false) const {
    int cursor=0,edge=0;
    for(unsigned char ch:OledCanvas::glyphText(text)){ch=static_cast<unsigned char>(std::toupper(ch));auto glyph=actionGlyph(ch,compactMod);if(glyph.width)edge=cursor+actionBearing(ch,compactMod)+glyph.width;cursor+=glyph.advance;}
    return edge;
}
void actionText(OledCanvas& c,int x,int y,const std::string& text,bool compactMod=false) const {
    for(unsigned char ch:OledCanvas::glyphText(text)) {
        ch=static_cast<unsigned char>(std::toupper(ch));auto glyph=actionGlyph(ch,compactMod);
        for(int row=0;row<5;++row)for(int col=0;col<glyph.width;++col)if(glyph.rows[row]&(1<<(glyph.width-1-col)))c.dot(x+actionBearing(ch,compactMod)+col,y+row);
        x+=glyph.advance;
    }
}
void actionFooter(OledCanvas& c,const std::string& left,std::string right,bool wave,int edge,const std::string& suffix={},bool compactMod=false,bool accumulation=false,bool curve=false) const {
    // Keep the established right alignment boxes: a compact colon occupies
    // its reserved alignment cell while its printed pixels sit one cell left.
    int waveX=edge-8-(suffix.empty()?0:c.captionWidth(suffix)+1),textEdge=wave?waveX-6:edge;
    if(!wave)right+=suffix;
    right=OledCanvas::glyphText(right);
    while(actionWidth(left,compactMod)+c.captionWidth(right)+10>textEdge && !right.empty())right.pop_back();
    int rightX=textEdge-c.captionWidth(right),minimum=2+actionWidth(left,compactMod)+3;
    actionText(c,2,56,left,compactMod);actionText(c,rightX,56,right);
    // Detailed and MOD identifiers follow their measured references. Keep the
    // established ordinary track templates where no new reference replaces it.
    bool measured=!accumulation && (compactMod || left.find(':')!=std::string::npos || left.compare(0,4,"MOD ")==0);
    int first=accumulation?2+actionWidth(left)+7:measured?(compactMod?minimum+(curve?2:0):34):2+c.captionWidth(left)+7;
    for(int x=first;x<=rightX-(accumulation?5:compactMod?3:4);x+=4)if(!measured || x>=minimum)c.dot(x,58);
    if(wave){const uint16_t rows[]={120,72,72,72,79};c.bitmap(waveX,56,7,5,rows);if(!suffix.empty())actionText(c,edge-c.captionWidth(suffix)-1,56,suffix);}
}
