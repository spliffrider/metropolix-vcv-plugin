#pragma once
#include "OledChainFont.hpp"
#include "OledCaptionFont.hpp"

namespace metrodisplay {
// The observed 7/8 figures start at the advance origin; other HOME figures
// retain the regular one-cell bearing. Both p30 and p31 show the seven.
inline int homeCaptionBearing(unsigned char c) {return c=='7' || c=='8'?0:1;}
// HOME's five-row figures and compact degree mark are visible in pp29–31.
// Keep this face local: the other pages have separately measured metrics.
inline MenuGlyph homeCaptionGlyph(unsigned char c) {
    switch(c) {
        case 1:return {2,4,{3,3,0,0,0}};
        case '0':return {4,6,{6,9,9,9,6}};
        case '3':return {4,6,{14,1,6,1,14}};
        case '4':return {4,6,{10,10,10,15,2}};
        case '5':return {4,6,{15,8,14,1,14}};
        case '6':return {4,6,{6,8,14,9,6}};
        case '7':return {4,6,{15,1,2,4,4}};
        case '8':return {4,6,{6,9,6,9,6}};
        // Nine is a companion: pulse switches and lane numbers stop at eight.
        case '9':return {4,6,{6,9,7,1,6}};
        default:return captionGlyph(c);
    }
}
inline MenuGlyph homeNoteGlyph(unsigned char c) {
    // C/B/E/b agree with the observed regular face. F/G differ from its
    // unobserved companions; the remaining characters retain those companions.
    if(c=='F')return {5,7,{31,16,16,28,16,16,16,0,0}};
    if(c=='G')return {5,7,{14,17,16,19,17,17,15,0,0}};
    return chainGlyph(c);
}
}
