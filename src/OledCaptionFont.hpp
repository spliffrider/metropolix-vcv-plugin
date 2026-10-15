#pragma once
#include "OledMenuFont.hpp"

namespace metrodisplay {
// Five-row proportional captions. Thirty-three observed glyphs reproduce native
// chain/scale/LIM and performance images. Negative widths request companions.
inline MenuGlyph captionGlyph(unsigned char c) {
    switch(c) {
        case ' ':return {0,4,{0,0,0,0,0}};
        case '#':return {5,7,{10,31,10,31,10}};
        case '(':return {2,4,{1,2,2,2,1}};
        case ')':return {2,4,{2,1,1,1,2}};
        case '+':return {5,7,{4,4,31,4,4}};
        case '-':return {2,4,{0,0,3,0,0}};
        case '/':return {3,5,{1,1,2,4,4}};
        case '1':return {3,5,{6,2,2,2,7}};
        case '2':return {4,6,{14,1,6,8,15}};
        case ':':return {1,3,{0,1,0,1,0}};
        case 'A':return {4,6,{6,9,15,9,9}};
        case 'B':return {4,6,{14,9,14,9,14}};
        case 'C':return {4,6,{6,9,8,9,6}};
        case 'D':return {4,6,{14,9,9,9,14}};
        case 'E':return {3,5,{7,4,7,4,7}};
        case 'F':return {3,5,{7,4,7,4,4}};
        case 'G':return {4,6,{7,8,11,9,6}};
        case 'H':return {4,6,{9,9,15,9,9}};
        case 'I':return {1,3,{1,1,1,1,1}};
        case 'J':return {4,6,{1,1,1,9,6}};
        case 'K':return {4,6,{9,10,12,10,9}};
        case 'L':return {3,5,{4,4,4,4,7}};
        case 'M':return {5,7,{17,27,21,17,17}};
        case 'N':return {5,7,{17,25,21,19,17}};
        case 'O':return {4,6,{6,9,9,9,6}};
        case 'P':return {4,6,{14,9,14,8,8}};
        case 'R':return {4,6,{14,9,14,10,9}};
        case 'S':return {4,6,{7,8,6,1,14}};
        case 'T':return {3,5,{7,2,2,2,2}};
        case 'U':return {4,6,{9,9,9,9,6}};
        case 'V':return {5,7,{17,17,10,10,4}};
        case 'W':return {5,7,{17,21,21,21,10}};
        case 'X':return {5,7,{17,10,4,10,17}};
        case 'Y':return {5,7,{17,10,4,4,4}};
        default:return {-1,0,{0,0,0,0,0}};
    }
}
}
