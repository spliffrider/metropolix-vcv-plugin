// Native image rows give an independent oracle for shapes and character spacing.
#include "OledMenuReference.hpp"
bool nativeRows=true;int nativeCount=0;
for(const auto& reference:menuReferences) {
    OledCanvas actual;actual.menuText(2,0,reference.left);actual.menuText(reference.rightX,0,reference.right);
    bool same=true;for(int y=0;y<reference.height;++y)for(int x=0;x<128;++x)same &= bool(actual.pixels[y][x])==bool(reference.packed[y*16+x/8]&(1<<(7-x%8)));
    if(!same)check(false,string::f("native menu font rows p%d: %s / %s",reference.page,reference.left,reference.right));
    nativeRows &= same;++nativeCount;
}
check(nativeRows,std::to_string(nativeCount)+" independent native OLED text rows reproduce observed menu glyphs and proportional spacing");
bool metrics=true;
for(int c=32;c<=127;++c) {
    auto glyph=metrodisplay::menuGlyph(static_cast<unsigned char>(c));std::string text(1,char(c));OledCanvas actual;actual.menuRight(127,0,text);
    int right=-1,bottom=-1;for(int y=0;y<64;++y)for(int x=0;x<128;++x)if(actual.pixels[y][x]){right=std::max(right,x);bottom=std::max(bottom,y);}
    metrics &= (c==32?actual.menuWidth(text)==0 && right==-1:right==126 && bottom<11) && glyph.advance>=glyph.width && glyph.width<=7;
}
OledCanvas actual,expected;actual.menuText(0,0,"±");expected.menuText(0,0,std::string(1,char(127)));metrics &= std::memcmp(actual.pixels,expected.pixels,sizeof(actual.pixels))==0;
check(metrics,"96 menu glyphs have bounded metrics, unclipped right alignment and normalized plus/minus");
