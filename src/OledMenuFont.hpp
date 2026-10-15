#pragma once
#include <cstdint>

namespace metrodisplay {
// Regular list lettering. The 38 observed glyphs reproduce the embedded OLED
// samples on firmware 1.6 manual pp33,48,126,135; companion glyphs follow their
// stroke/baseline metrics. This is separate from the performance-value font.
struct MenuGlyph { int width,advance; uint8_t rows[11]; };
inline MenuGlyph menuGlyph(unsigned char c) {
    switch(c) {
        case 1:return {3,5,{2,5,2,0,0,0,0,0,0,0,0}}; // degree companion
        case ' ':return {0,4,{0,0,0,0,0,0,0,0,0,0,0}};
        case '!':return {1,3,{1,1,1,1,1,1,1,0,1,0,0}};
        case '"':return {3,5,{5,5,5,0,0,0,0,0,0,0,0}};
        case '#':return {5,7,{0,10,10,31,10,10,31,10,10,0,0}};
        case '$':return {5,7,{4,15,20,20,14,5,5,30,4,0,0}};
        case '%':return {5,7,{25,25,18,2,4,8,19,3,3,0,0}};
        case '&':return {5,7,{12,17,17,12,10,19,21,18,13,0,0}};
        case '\'':return {1,3,{1,1,0,0,0,0,0,0,0,0,0}};
        case '(':return {3,5,{1,2,4,4,4,4,4,2,1,0,0}};
        case ')':return {3,5,{4,2,1,1,1,1,1,2,4,0,0}};
        case '*':return {5,7,{0,0,4,21,14,21,4,0,0,0,0}};
        case '+':return {5,7,{0,0,4,4,31,4,4,0,0,0,0}}; // reference
        case ',':return {2,4,{0,0,0,0,0,0,0,3,3,2,0}};
        case '-':return {4,6,{0,0,0,0,15,0,0,0,0,0,0}}; // reference
        case '.':return {2,4,{0,0,0,0,0,0,0,3,3,0,0}};
        case '/':return {3,5,{1,1,1,2,2,2,4,4,4,0,0}}; // reference
        case '0':return {5,7,{14,17,17,19,21,23,17,17,14,0,0}};
        case '1':return {3,5,{2,6,2,2,2,2,2,2,7,0,0}};
        case '2':return {5,7,{14,17,1,1,2,4,8,16,31,0,0}};
        case '3':return {5,7,{14,17,1,1,6,1,1,17,14,0,0}}; // reference
        case '4':return {5,7,{2,2,6,10,10,18,31,2,2,0,0}};
        case '5':return {5,7,{31,16,16,30,1,1,1,17,14,0,0}};
        case '6':return {5,7,{14,17,16,16,30,17,17,17,14,0,0}};
        case '7':return {5,7,{31,1,1,2,4,8,16,16,16,0,0}}; // reference
        case '8':return {5,7,{14,17,17,17,14,17,17,17,14,0,0}};
        case '9':return {5,7,{14,17,17,17,15,1,1,17,14,0,0}};
        case ':':return {1,3,{0,0,1,0,0,0,0,0,1,0,0}}; // reference
        case ';':return {1,3,{0,0,1,0,0,0,1,1,0,0,0}};
        case '<':return {4,6,{0,0,1,2,4,8,4,2,1,0,0}};
        case '=':return {5,7,{0,0,0,31,0,31,0,0,0,0,0}};
        case '>':return {4,6,{0,0,8,4,2,1,2,4,8,0,0}};
        case '?':return {5,7,{14,17,1,1,2,4,4,0,4,0,0}};
        case '@':return {5,7,{14,17,31,23,21,21,16,16,14,0,0}};
        case 'A':return {5,7,{14,17,17,17,31,17,17,17,17,0,0}};
        case 'B':return {5,7,{30,17,17,17,30,17,17,17,30,0,0}}; // reference
        case 'C':return {5,7,{14,17,16,16,16,16,16,17,14,0,0}}; // reference
        case 'D':return {5,7,{30,17,17,17,17,17,17,17,30,0,0}}; // reference
        case 'E':return {5,7,{31,16,16,16,30,16,16,16,31,0,0}};
        case 'F':return {5,7,{31,16,16,16,30,16,16,16,16,0,0}};
        case 'G':return {5,7,{14,17,16,16,23,17,17,17,14,0,0}};
        case 'H':return {5,7,{17,17,17,17,31,17,17,17,17,0,0}};
        case 'I':return {1,3,{1,1,1,1,1,1,1,1,1,0,0}}; // reference
        case 'J':return {5,7,{1,1,1,1,1,1,17,17,14,0,0}};
        case 'K':return {5,7,{17,17,18,20,24,20,18,17,17,0,0}};
        case 'L':return {5,7,{16,16,16,16,16,16,16,16,31,0,0}}; // reference
        case 'M':return {7,9,{65,65,99,85,73,65,65,65,65,0,0}}; // reference
        case 'N':return {5,7,{17,17,17,25,21,19,17,17,17,0,0}}; // reference
        case 'O':return {5,7,{14,17,17,17,17,17,17,17,14,0,0}}; // reference
        case 'P':return {5,7,{30,17,17,17,30,16,16,16,16,0,0}}; // reference
        case 'Q':return {5,7,{14,17,17,17,17,17,21,18,15,0,0}};
        case 'R':return {5,7,{30,17,17,17,30,20,18,17,17,0,0}}; // reference
        case 'S':return {5,7,{14,17,16,16,14,1,1,17,14,0,0}}; // reference
        case 'T':return {5,7,{31,4,4,4,4,4,4,4,4,0,0}}; // reference
        case 'U':return {5,7,{17,17,17,17,17,17,17,17,14,0,0}}; // reference
        case 'V':return {5,7,{17,17,17,17,17,17,10,10,4,0,0}};
        case 'W':return {7,9,{65,65,65,65,73,73,73,85,34,0,0}};
        case 'X':return {5,7,{17,17,10,10,4,10,10,17,17,0,0}};
        case 'Y':return {5,7,{17,17,10,10,4,4,4,4,4,0,0}};
        case 'Z':return {5,7,{31,1,1,2,4,8,16,16,31,0,0}};
        case '[':return {3,5,{7,4,4,4,4,4,4,4,7,0,0}};
        case '\\':return {5,7,{16,16,8,4,2,2,1,1,1,0,0}};
        case ']':return {3,5,{7,1,1,1,1,1,1,1,7,0,0}};
        case '^':return {5,7,{4,10,17,0,0,0,0,0,0,0,0}};
        case '_':return {5,7,{0,0,0,0,0,0,0,0,31,0,0}};
        case '`':return {2,4,{2,1,0,0,0,0,0,0,0,0,0}};
        case 'a':return {5,7,{0,0,14,17,1,15,17,17,15,0,0}}; // reference
        case 'b':return {5,7,{16,16,30,17,17,17,17,17,30,0,0}};
        case 'c':return {5,7,{0,0,14,17,16,16,16,17,14,0,0}}; // reference
        case 'd':return {5,7,{1,1,15,17,17,17,17,17,15,0,0}}; // reference
        case 'e':return {5,7,{0,0,14,17,17,31,16,17,14,0,0}}; // reference
        case 'f':return {4,6,{3,4,15,4,4,4,4,4,4,0,0}};
        case 'g':return {5,7,{0,0,15,17,17,17,17,15,1,17,14}}; // reference
        case 'h':return {5,7,{16,16,30,17,17,17,17,17,17,0,0}}; // reference
        case 'i':return {1,3,{1,0,1,1,1,1,1,1,1,0,0}}; // reference
        case 'j':return {4,6,{0,1,0,1,1,1,1,1,9,6,0}};
        case 'k':return {5,7,{16,16,17,18,20,24,20,18,17,0,0}}; // reference
        case 'l':return {1,3,{1,1,1,1,1,1,1,1,1,0,0}}; // reference
        case 'm':return {7,9,{0,0,118,73,73,73,73,65,65,0,0}}; // reference
        case 'n':return {5,7,{0,0,30,17,17,17,17,17,17,0,0}}; // reference
        case 'o':return {5,7,{0,0,14,17,17,17,17,17,14,0,0}}; // reference
        case 'p':return {5,7,{0,0,30,17,17,17,17,17,30,16,16}}; // reference
        case 'q':return {5,7,{0,0,15,17,17,17,17,17,15,1,1}};
        case 'r':return {5,7,{0,0,19,20,24,16,16,16,16,0,0}}; // reference
        case 's':return {5,7,{0,0,14,17,16,14,1,17,14,0,0}}; // reference
        case 't':return {5,6,{0,8,30,8,8,8,8,8,7,0,0}}; // reference
        case 'u':return {5,7,{0,0,17,17,17,17,17,17,14,0,0}}; // reference
        case 'v':return {5,7,{0,0,17,17,17,17,17,10,4,0,0}}; // reference
        case 'w':return {7,9,{0,0,65,65,73,73,73,73,54,0,0}}; // reference
        case 'x':return {5,7,{0,0,17,17,10,4,10,17,17,0,0}};
        case 'y':return {5,7,{0,0,17,17,17,17,17,17,15,1,14}};
        case 'z':return {5,7,{0,0,31,1,2,4,8,16,31,0,0}};
        case '{':return {4,6,{1,2,4,4,8,4,4,2,1,0,0}};
        case '|':return {1,3,{1,1,1,1,1,1,1,1,1,0,0}};
        case '}':return {3,5,{4,2,1,1,2,1,1,2,4,0,0}};
        case '~':return {5,7,{0,0,0,0,10,21,0,0,0,0,0}};
        case 127:return {5,7,{0,4,4,31,4,4,0,0,31,0,0}};
        default:return menuGlyph('?');
    }
}
}
