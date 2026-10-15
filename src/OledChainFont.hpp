#pragma once
#include "OledMenuFont.hpp"

namespace metrodisplay {
// Chain lists use seven-pixel capitals with room for two descender rows.
// Twenty observed glyphs match native manual pp171/174. Other glyphs follow
// the regular menu face with two redundant rows removed; they are companions.
inline MenuGlyph chainGlyph(unsigned char c) {
    switch(c) {
        case '1':return {3,5,{1,3,5,1,1,1,1,0,0}};
        case '2':return {5,7,{14,17,1,2,4,8,31,0,0}};
        case '5':return {5,7,{31,16,30,1,1,17,14,0,0}};
        case '6':return {5,7,{14,17,16,30,17,17,14,0,0}};
        case 'B':return {5,7,{30,17,17,30,17,17,30,0,0}};
        case 'C':return {5,7,{14,17,16,16,16,17,14,0,0}};
        case 'E':return {5,7,{31,16,16,28,16,16,31,0,0}};
        case 'N':return {5,7,{17,17,25,21,19,17,17,0,0}};
        case 'a':return {5,7,{0,0,14,1,15,17,15,0,0}};
        case 'b':return {5,7,{16,16,30,17,17,17,30,0,0}};
        case 'e':return {5,7,{0,0,14,17,31,16,14,0,0}};
        case 'h':return {5,7,{16,16,30,17,17,17,17,0,0}};
        case 'i':return {1,3,{1,0,1,1,1,1,1,0,0}};
        case 'l':return {1,3,{1,1,1,1,1,1,1,0,0}};
        case 'n':return {5,7,{0,0,30,17,17,17,17,0,0}};
        case 'r':return {5,7,{0,0,19,20,24,16,16,0,0}};
        case 'u':return {5,7,{0,0,17,17,17,17,14,0,0}};
        case 'w':return {7,9,{0,0,65,65,73,73,54,0,0}};
        case 'x':return {5,7,{0,0,17,10,4,10,17,0,0}};
        case 'y':return {5,7,{0,0,17,17,17,15,1,14,0}};
        default:break;
    }
    auto source=menuGlyph(c);MenuGlyph result{};
    result.width=source.width;result.advance=source.advance;
    for(int y=0,n=0;y<11;++y)if(y!=3 && y!=7)result.rows[n++]=source.rows[y];
    return result;
}
}
