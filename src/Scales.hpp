#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace metro {
enum { FactoryScaleCount=49, UserScaleCount=100, ScaleCount=149 };
struct Scale { int count; int notes[12]; };
struct UserScale { unsigned mask=0xAB5; int root=0; }; // C major, relative to root.
struct FactoryScale { const char* name; const char* bank; Scale scale; };
inline const FactoryScale& factoryScale(int id) {
    // The original seven IDs remain fixed so old patches and internal presets
    // retain their notes. Menus below present the hardware's bank order.
    static const FactoryScale all[]={
        {"Chromatic","Miscellaneous",{12,{0,1,2,3,4,5,6,7,8,9,10,11}}},
        {"Major (Ionian)","Major scales",{7,{0,2,4,5,7,9,11}}},
        {"Minor (Aeolian)","Major scales",{7,{0,2,3,5,7,8,10}}},
        {"Dorian","Major scales",{7,{0,2,3,5,7,9,10}}},
        {"Mixolydian","Major scales",{7,{0,2,4,5,7,9,10}}},
        {"Major pentatonic","Miscellaneous",{5,{0,2,4,7,9}}},
        {"Minor pentatonic","Miscellaneous",{5,{0,3,5,7,10}}},
        {"Phrygian","Major scales",{7,{0,1,3,5,7,8,10}}},
        {"Lydian","Major scales",{7,{0,2,4,6,7,9,11}}},
        {"Locrian","Major scales",{7,{0,1,3,5,6,8,10}}},
        {"Melodic minor","Melodic minor",{7,{0,2,3,5,7,9,11}}},
        {"Dorian b2","Melodic minor",{7,{0,1,3,5,7,9,10}}},
        {"Lydian augmented","Melodic minor",{7,{0,2,4,6,8,9,11}}},
        {"Lydian dominant","Melodic minor",{7,{0,2,4,6,7,9,10}}},
        {"Mixolydian b6","Melodic minor",{7,{0,2,4,5,7,8,10}}},
        {"Aeolian b5","Melodic minor",{7,{0,2,3,5,6,8,10}}},
        {"Super Locrian","Melodic minor",{7,{0,1,3,4,6,8,10}}},
        {"Harmonic minor","Harmonic minor",{7,{0,2,3,5,7,8,11}}},
        {"Locrian natural 6","Harmonic minor",{7,{0,1,3,5,6,9,10}}},
        {"Ionian #5","Harmonic minor",{7,{0,2,4,5,8,9,11}}},
        {"Dorian #4","Harmonic minor",{7,{0,2,3,6,7,9,10}}},
        {"Phrygian dominant","Harmonic minor",{7,{0,1,4,5,7,8,10}}},
        {"Lydian #2","Harmonic minor",{7,{0,3,4,6,7,9,11}}},
        {"Altered","Harmonic minor",{7,{0,1,3,4,6,8,9}}},
        {"Whole tone","Miscellaneous",{6,{0,2,4,6,8,10}}},
        {"Whole half diminished","Miscellaneous",{8,{0,2,3,5,6,8,9,11}}},
        {"Half whole diminished","Miscellaneous",{8,{0,1,3,4,6,7,9,10}}},
        {"Minor blues","Miscellaneous",{6,{0,3,5,6,7,10}}},
        {"Bhairav / Arabic","World scales",{7,{0,1,4,5,7,8,11}}},
        {"Hungarian minor","World scales",{7,{0,2,3,6,7,8,11}}},
        {"Chinese","World scales",{5,{0,4,6,7,11}}},
        {"Hirajoshi","World scales",{5,{0,2,3,7,8}}},
        {"In-Sen","World scales",{5,{0,1,5,7,10}}},
        {"Kumoi","World scales",{5,{0,2,3,7,9}}},
        {"Pelog","World scales",{5,{0,1,3,7,8}}},
        {"Major triad","Major chords",{3,{0,4,7}}},
        {"Major 6th","Major chords",{4,{0,4,7,9}}},
        {"Major 7th","Major chords",{4,{0,4,7,11}}},
        {"Major 7th b5","Major chords",{4,{0,4,6,11}}},
        {"Major 7th #5","Major chords",{4,{0,4,8,11}}},
        {"Dominant 7th","Major chords",{4,{0,4,7,10}}},
        {"Major 9th","Major chords",{5,{0,2,4,7,11}}},
        {"Minor triad","Minor chords",{3,{0,3,7}}},
        {"Minor 6th","Minor chords",{4,{0,3,7,9}}},
        {"Minor 7th","Minor chords",{4,{0,3,7,10}}},
        {"Minor 7th b5","Minor chords",{4,{0,3,6,10}}},
        {"Diminished 7th","Minor chords",{4,{0,3,6,9}}},
        {"Minor 9th","Minor chords",{5,{0,2,3,7,10}}},
        {"Minor 11th","Minor chords",{6,{0,2,3,5,7,10}}}
    };
    return all[std::max(0,std::min(FactoryScaleCount-1,id))];
}
inline const Scale& scaleAt(int id) { return factoryScale(id).scale; }
inline unsigned scaleMask(const Scale& scale) {
    unsigned mask=0;for(int i=0;i<scale.count;++i)mask|=1u<<scale.notes[i];return mask;
}
inline Scale scaleFromMask(unsigned mask) {
    mask&=4095u;if(!mask)mask=1; // A malformed/empty scale safely becomes root only.
    Scale scale{};for(int n=0;n<12;++n)if(mask&(1u<<n))scale.notes[scale.count++]=n;return scale;
}
inline bool userScale(int id) { return id>=FactoryScaleCount && id<ScaleCount; }
inline std::string scaleName(int id) {
    id=std::max(0,std::min(ScaleCount-1,id));
    if(!userScale(id))return factoryScale(id).name;
    int slot=id-FactoryScaleCount;return "Scale "+std::to_string(slot/10+1)+"-"+std::to_string(slot%10+1);
}
inline std::vector<std::string> scaleNames() {
    std::vector<std::string> names;for(int i=0;i<ScaleCount;++i)names.push_back(scaleName(i));return names;
}
inline int scaleBrowseId(int position) {
    static const int ids[]={1,3,7,8,4,2,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,5,6,24,25,26,27,0,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48};
    position=std::max(0,std::min(ScaleCount-1,position));return position<FactoryScaleCount?ids[position]:position;
}
inline int scaleBrowsePosition(int id) {for(int i=0;i<ScaleCount;++i)if(scaleBrowseId(i)==id)return i;return 0;}
inline int degreeToSemitone(int degree, const Scale& scale) {
    int note=((degree%scale.count)+scale.count)%scale.count;
    return 12*((degree-note)/scale.count)+scale.notes[note];
}
inline int degreeToSemitone(int degree,int scale) {return degreeToSemitone(degree,scaleAt(scale));}
inline int nearestDegree(float semitone,const Scale& scale) {
    if(!std::isfinite(semitone))semitone=0;
    int octave=int(std::floor(semitone/12.f)),best=octave*scale.count;float distance=100.f;
    for(int d=(octave-1)*scale.count;d<=(octave+2)*scale.count;++d) {
        float candidate=std::abs(semitone-degreeToSemitone(d,scale));
        if(candidate<distance){distance=candidate;best=d;}
    }
    return best;
}
inline int nearestDegree(float semitone,int scale) {return nearestDegree(semitone,scaleAt(scale));}
inline int modulatedUserScale(int locked,float voltage) {
    if(!userScale(locked))return locked;
    if(!std::isfinite(voltage))voltage=0;
    // p68 specifies whole volt changes. Fractions truncate toward zero;
    // this boundary interpretation is documented for hardware comparison.
    int offset=int(std::trunc(std::max(-1000.f,std::min(1000.f,voltage))));
    int slot=locked-FactoryScaleCount,relative=((slot%10+offset)%10+10)%10;
    return FactoryScaleCount+slot/10*10+relative;
}
} // namespace metro
