#include "OledMenuFont.hpp"
#include "OledValueFont.hpp"
#include "OledChainFont.hpp"
#include "OledCaptionFont.hpp"

// A 128 x 64 monochrome display, matching the native screenshots in the manual.
// Text and diagrams are pixels, so zooming never substitutes a desktop font.
struct OledCanvas {
    uint8_t pixels[64][128] = {};
    void dot(int x,int y,uint8_t v=255) {
        if(static_cast<unsigned>(x)>=128 || static_cast<unsigned>(y)>=64)return;
        pixels[y][x]=v;
    }
    void fill(int x,int y,int w,int h,uint8_t v=255) { for(int j=0;j<h;++j)for(int i=0;i<w;++i)dot(x+i,y+j,v); }
    void line(int x,int y,int x2,int y2,uint8_t v=255) {
        int dx=std::abs(x2-x),sx=x<x2?1:-1,dy=-std::abs(y2-y),sy=y<y2?1:-1,e=dx+dy;
        for(;;) { dot(x,y,v); if(x==x2 && y==y2)break; int e2=e*2; if(e2>=dy){e+=dy;x+=sx;}if(e2<=dx){e+=dx;y+=sy;} }
    }
    void box(int x,int y,int w,int h,uint8_t v=255) { line(x,y,x+w-1,y,v);line(x,y+h-1,x+w-1,y+h-1,v);line(x,y,x,y+h-1,v);line(x+w-1,y,x+w-1,y+h-1,v); }
    void hatch(int x,int y,int w,int h) { for(int j=0;j<h;++j)for(int i=0;i<w;++i)if((i+j)%2==0)dot(x+i,y+j,0); }
    void circle(int x,int y,int r,bool solid=false,uint8_t v=255) {
        for(int j=-r;j<=r;++j)for(int i=-r;i<=r;++i)if(i*i+j*j<=r*r && (solid || i*i+j*j>=(r-1)*(r-1)))dot(x+i,y+j,v);
    }
    void arrow(int x,int y,bool down=true) { for(int i=0;i<3;++i)fill(x-2+i,y+(down?i:2-i),5-2*i,1); }
    void turn(int x,int y) { for(int i=0;i<4;++i){line(x+i,y+3-i,x+i,y+3+i);line(x+6+i,y+i,x+6+i,y+6-i);} }
    void corners(int x,int y,int w,int h) {
        uint8_t v=std::sin(system::getTime()*5.f)>.1f?255:155;
        for(int a:{0,1})for(int b:{0,1}) { int px=x+a*(w-1),py=y+b*(h-1);line(px,py,px+(a?-4:4),py,v);line(px,py,px,py+(b?-4:4),v); }
    }
    struct Glyph { int w; const char* rows; };
    static Glyph small(char c) {
        switch(c) {
            case 1:return {3,"25200"}; // degree
            case 'A':return {4,"69F99"}; case 'B':return {3,"65656"}; case 'C':return {3,"34443"};
            case 'D':return {3,"65556"}; case 'E':return {3,"74747"}; case 'F':return {3,"74744"};
            case 'G':return {4,"788B7"}; case 'H':return {3,"55755"}; case 'I':return {1,"11111"};
            case 'J':return {3,"11152"}; case 'K':return {4,"9AC A9"}; case 'L':return {3,"44447"};
            case 'M':return {5,"HRLHH"}; case 'N':return {4,"9DB99"}; case 'O':return {4,"69996"};
            case 'P':return {3,"65644"}; case 'Q':return {4,"699B7"}; case 'R':return {4,"E9EA9"};
            case 'S':return {3,"34216"}; case 'T':return {3,"72222"}; case 'U':return {4,"99996"};
            case 'V':return {5,"HHHA4"}; case 'W':return {5,"HHLLA"}; case 'X':return {3,"55255"};
            case 'Y':return {3,"55222"}; case 'Z':return {3,"71247"};
            case '0':return {3,"75557"}; case '1':return {3,"26227"}; case '2':return {3,"61647"};
            case '3':return {3,"61216"}; case '4':return {3,"55711"}; case '5':return {3,"74616"};
            case '6':return {3,"34757"}; case '7':return {3,"71222"}; case '8':return {3,"75757"};
            case '9':return {3,"75716"}; case '+':return {3,"02720"}; case '-':return {3,"00700"};
            case '/':return {3,"11244"}; case ':':return {1,"01010"}; case '.':return {1,"00001"};
            case '%':return {3,"51245"}; case '#':return {5,"AVAVA"}; case '?':return {3,"61202"};
            case '(':return {2,"12221"};case ')':return {2,"21112"};case '=':return {3,"07070"};
            case '>':return {3,"42124"};
            case 127:return {3,"27207"}; // plus/minus
            default:return {2,"00000"};
        }
    }
    static const char* large(char c) {
        switch(c) {
            case 1:return "EHHE000"; // degree
            case 'A':return "EH H VHHH"; case 'B':return "UHHUHHU"; case 'C':return "FGGGGGF";
            case 'D':return "UHHHHHU"; case 'E':return "VGGUGGV"; case 'F':return "VGGUGGG";
            case 'G':return "FGGNHHF"; case 'H':return "HHHVHHH"; case 'I':return "E44444E";
            case 'J':return "72222IC"; case 'K':return "HIKOKIH"; case 'L':return "GGGGGGV";
            case 'M':return "HRLLHHH"; case 'N':return "HPNLJHH"; case 'O':return "EHHHHHE";
            case 'P':return "UHHUGGG"; case 'Q':return "EHHHLIF"; case 'R':return "UHHUKIH";
            case 'S':return "FGGE11U"; case 'T':return "V444444"; case 'U':return "HHHHHHE";
            case 'V':return "HHHHHA4"; case 'W':return "HHHLLLA"; case 'X':return "HHA4AHH";
            case 'Y':return "HHA4444"; case 'Z':return "V1248GV";
            case '0':return "EHJLNHE"; case '1':return "4C4444E"; case '2':return "EH1248V";
            case '3':return "U11611U"; case '4':return "26AIV22"; case '5':return "VGGU11U";
            case '6':return "68GUHHE"; case '7':return "V124888"; case '8':return "EHHEHHE";
            case '9':return "EHHF124"; case '.':return "0000066"; case '-':return "000V000";
            case '+':return "004E400"; case '/':return "112488G"; case '%':return "PI248J3";
            case '#':return "AAVA VAA"; case ':':return "0660660";
            case '>':return "0842480";
            case '?':return "EH12404";
            case 127:return "04E040E";
            case 'a':return "00E1FHF";case 'b':return "GGUHHHU";case 'c':return "00FGGGF";
            case 'd':return "11FHHHF";case 'e':return "00EHVGF";case 'f':return "648U888";
            case 'g':return "00FHHHF1E";case 'h':return "GGUHHHH";case 'i':return "404444E";
            case 'j':return "2022222IC";case 'k':return "GGIKO KI";case 'l':return "C44444E";
            case 'm':return "00ULLLL";case 'n':return "00UHHHH";case 'o':return "00EHHHE";
            case 'p':return "00UHHHUGG";case 'q':return "00FHHHF11";case 'r':return "00NOGGG";
            case 's':return "00FGE1U";case 't':return "88U8886";case 'u':return "00HHHJF";
            case 'v':return "00HHHA4";case 'w':return "00HHLLA";case 'x':return "00HA4AH";
            case 'y':return "00HHHHF1E";case 'z':return "00V248V";
            default:return "0000000";
        }
    }
    static int bits(char c) { return c<='9'?c-'0':c-'A'+10; }
    static std::string glyphText(const std::string& s) {
        std::string result;
        for(size_t i=0;i<s.size();++i) {
            if(static_cast<unsigned char>(s[i])==0xc2 && i+1<s.size() && static_cast<unsigned char>(s[i+1])==0xb1){result+=char(127);++i;}
            else if(static_cast<unsigned char>(s[i])==0xc2 && i+1<s.size() && static_cast<unsigned char>(s[i+1])==0xb0){result+=char(1);++i;}
            else result+=s[i];
        }
        return result;
    }
    int menuWidth(const std::string& s) const {
        int cursor=0,right=0;
        for(unsigned char c:glyphText(s)) {auto g=metrodisplay::menuGlyph(c);if(g.width)right=cursor+1+g.width;cursor+=g.advance;}
        return right;
    }
    int menuInkHeight(const std::string& s) const {
        int height=0;
        for(unsigned char c:glyphText(s)){auto g=metrodisplay::menuGlyph(c);for(int y=0;y<11;++y)if(g.rows[y])height=std::max(height,y+1);}
        return height;
    }
    int menuAdvanceWidth(const std::string& s) const {
        int width=0;for(unsigned char c:glyphText(s))width+=metrodisplay::menuGlyph(c).advance;return width;
    }
    void menuText(int x,int y,const std::string& s,uint8_t v=255) {
        for(unsigned char c:glyphText(s)) {
            auto g=metrodisplay::menuGlyph(c);
            for(int row=0;row<11;++row)for(int col=0;col<g.width;++col)if(g.rows[row]&(1<<(g.width-1-col)))dot(x+1+col,y+row,v);
            x+=g.advance;
        }
    }
    void menuRight(int x,int y,const std::string& s,uint8_t v=255) {menuText(x-menuWidth(s),y,s,v);}
    static metrodisplay::MenuGlyph chainCaption(unsigned char c) {
        c=static_cast<unsigned char>(std::toupper(c));
        auto glyph=metrodisplay::captionGlyph(c);
        if(glyph.width>=0)return glyph;
        auto source=small(static_cast<char>(std::toupper(c)));
        glyph.width=source.w;glyph.advance=source.w+2;
        int row=0;
        for(const char* p=source.rows;*p && row<5;++p)if(*p!=' ')glyph.rows[row++]=uint8_t(bits(*p));
        return glyph;
    }
    int chainWidth(const std::string& s,bool caption=false) const {
        int cursor=0,right=0;
        for(unsigned char c:glyphText(s)){auto g=caption?chainCaption(c):metrodisplay::chainGlyph(c);if(g.width)right=cursor+1+g.width;cursor+=g.advance;}
        return right;
    }
    void chainText(int x,int y,const std::string& s,uint8_t v=255,bool caption=false) {
        for(unsigned char c:glyphText(s)) {
            auto g=caption?chainCaption(c):metrodisplay::chainGlyph(c);
            for(int row=0;row<(caption?5:9);++row)for(int col=0;col<g.width;++col)if(g.rows[row]&(1<<(g.width-1-col)))dot(x+1+col,y+row,v);
            x+=g.advance;
        }
    }
    int captionWidth(const std::string& s) const {return chainWidth(s,true);}
    void captionText(int x,int y,const std::string& s,uint8_t v=255,int tracking=2,bool wideDash=false) {
        for(unsigned char c:glyphText(s)) {
            auto g=chainCaption(c);
            // The MX heading uses the wider dash; Scale retains its own face.
            if(wideDash && c=='-')g={3,5,{0,0,7,0,0}};
            for(int row=0;row<5;++row)for(int col=0;col<g.width;++col)if(g.rows[row]&(1<<(g.width-1-col)))dot(x+1+col,y+row,v);
            x+=g.width?g.advance+tracking-2:g.advance;
        }
    }
    void captionRight(int x,int y,const std::string& s,uint8_t v=255) {captionText(x-captionWidth(s),y,s,v);}
    void bitmap(int x,int y,int w,int h,const uint16_t* rows,uint8_t v=255) {
        for(int j=0;j<h;++j)for(int i=0;i<w;++i)if(rows[j]&(1<<(w-1-i)))dot(x+i,y+j,v);
    }
    void ownerHint(int x,int y,int owner,uint8_t v=255) {
        // The outline and CTRL 2 digit follow the native encoder legend.
        // CTRL 1 uses the same outline and the observed seven-row digit face.
        const uint16_t circle[]={496,520,1028,2050,4097,4097,4097,4097,4097,2050,1028,520,496};
        bitmap(x,y,13,13,circle,v);
        auto digit=metrodisplay::chainGlyph(owner?'2':'1');
        for(int row=0;row<7;++row)for(int col=0;col<digit.width;++col)
            if(digit.rows[row]&(1<<(digit.width-1-col)))dot(x+(13-digit.width)/2+col,y+3+row,v);
    }
    void scaleCorners(int x,int y,int w,int h) {
        uint8_t v=std::sin(system::getTime()*5.f)>.1f?255:155;
        for(int a:{0,1})for(int b:{0,1}) {
            int px=x+a*(w-1),py=y+b*(h-1),extent=b && !a?4:5;
            line(px,py,px+(a?-5:5),py,v);line(px,py,px,py+(b?-extent:extent),v);
        }
    }
    void fieldCorners(int x,int y,int w,int h) {
        uint8_t v=std::sin(system::getTime()*5.f)>.1f?255:155;
        for(int a:{0,1})for(int b:{0,1}) {
            int px=x+a*(w-1),py=y+b*(h-1);
            line(px,py,px+(a?-5:5),py,v);line(px,py,px,py+(b?-5:5),v);
        }
    }
    void bipolarGauge(float voltage) {
        box(82,2,43,7);
        int end=103+static_cast<int>(std::round(std::max(-5.f,std::min(voltage,5.f))*4.f));
        if(end==103)line(103,3,103,7);
        else fill(std::min(end,103),4,std::abs(end-103)+1,3);
    }
    void controlGauge(float value) {
        box(82,2,43,7);
        // One native sample establishes a moving thirteen-pixel indicator.
        // Interpolation across its available travel is a desktop interpretation.
        int x=83+static_cast<int>((std::max(-1.f,std::min(value,1.f))+1.f)*14.f);
        fill(x,4,13,3);
    }
    void scaleLongHint(int x,int y,uint8_t v=255) {const uint16_t rows[]={56,68,186,381,381,381,186,68,56};bitmap(x,y,9,9,rows,v);}
    void scaleLockHint(int x,int y,uint8_t v=255) {const uint16_t rows[]={28,450,994,1007,1007,479,63};bitmap(x,y,10,7,rows,v);}
    void scaleStatus(bool search) {
        if(search){const uint16_t rows[]={112,136,260,260,260,136,116,2,1};bitmap(116,2,9,9,rows);}
        else {const uint16_t rows[]={28,34,34,127,127,127,127};bitmap(117,3,7,7,rows);}
    }
    int valueWidth(const std::string& s,bool big=false) const {
        int cursor=0,right=0;
        for(unsigned char c:glyphText(s)){auto g=metrodisplay::valueGlyph(c,big);right=cursor+g.right;cursor+=g.advance;}
        return right;
    }
    void valueText(int x,int y,const std::string& s,bool big=false,uint8_t v=255) {
        for(unsigned char c:glyphText(s)) {
            auto g=metrodisplay::valueGlyph(c,big);
            for(int row=0;row<g.height;++row)for(int col=0;col<g.width;++col)if(g.rows[row]&(1<<(g.width-1-col)))dot(x+g.bearing+col,y+row,v);
            x+=g.advance;
        }
    }
    void valueRight(int x,int y,const std::string& s,bool big=false,uint8_t v=255) {valueText(x-valueWidth(s,big),y,s,big,v);}
    void valueText7(int x,int y,const std::string& s,uint8_t v=255) {
        // The popup's observed 3/D/0 face has these seven Bold9 rows. Other
        // characters retain the same metrics as companion glyphs.
        const int rows[]={0,1,3,4,5,7,8};
        for(unsigned char c:glyphText(s)) {
            auto g=metrodisplay::valueGlyph(c,false);
            for(int row=0;row<7;++row)for(int col=0;col<g.width;++col)if(g.rows[rows[row]]&(1<<(g.width-1-col)))dot(x+g.bearing+col,y+row,v);
            x+=g.advance;
        }
    }
    int valueFit(int x,int bottom,int w,const std::string& s,bool big=false,bool left=false,uint8_t v=255) {
        if(big && valueWidth(s,true)<=w){valueText(left?x:x+w-valueWidth(s,true),bottom-13,s,true,v);return 14;}
        if(valueWidth(s)<=w){valueText(left?x:x+w-valueWidth(s),bottom-8,s,false,v);return 9;}
        if(menuWidth(s)<=w){menuText(left?x:x+w-menuWidth(s),bottom-8,s,v);return 9;}
        std::string shown=glyphText(s);while(!shown.empty() && width(shown)>w)shown.pop_back();text(left?x:x+w-width(shown),bottom-4,shown,true,1,v);return 5;
    }
    void pressHint(int x,int y,int count=1,uint8_t v=255) {
        const uint8_t solid[]={14,31,31,31,14},outline[]={14,17,17,17,14};
        for(int n=0;n<count;++n)for(int row=0;row<5;++row)for(int col=0;col<5;++col)if((n?outline[row]:solid[row])&(1<<(4-col)))dot(x+n*8+col,y+row,v);
    }
    void turnHint(int x,int y,bool queued=false,bool pending=false,uint8_t v=255,bool typeStyle=false) {
        int right=queued?(typeStyle?12:13):6;
        for(int i=0;i<4;++i){line(x+i,y+3-i,x+i,y+3+i,v);line(x+right+i,y+i,x+right+i,y+6-i,v);}
        if(!queued)return;
        if(pending){pressHint(x+6,y+1,1,v);return;}
        // TYPE retains the full-height question mark shown in its reference.
        const uint8_t compact[]={14,1,6,0,4},tall[]={6,9,1,2,2,0,2};
        const uint8_t* mark=typeStyle?tall:compact;
        int height=typeStyle?7:5,dx=typeStyle?6:7,dy=typeStyle?0:1;
        for(int row=0;row<height;++row)for(int col=0;col<4;++col)if(mark[row]&(1<<(3-col)))dot(x+dx+col,y+dy+row,v);
    }
    int width(const std::string& s,bool tiny=true,int scale=1) const { int w=0;for(char c:glyphText(s))w+=(tiny?small(static_cast<char>(std::toupper(static_cast<unsigned char>(c)))).w+2:6)*scale;return std::max(0,w-(tiny?2:1)*scale); }
    void text(int x,int y,const std::string& s,bool tiny=true,int scale=1,uint8_t v=255) {
        for(char c:glyphText(s)) {
            Glyph g=tiny?small(static_cast<char>(std::toupper(static_cast<unsigned char>(c)))):Glyph{5,large(c)};
            int row=0;
            // Large letters share a seven-row baseline; g/j/p/q/y extend two
            // rows below it. All display fields reserve this nine-row height.
            for(const char* p=g.rows;*p && row<(tiny?5:9);++p) {
                if(*p==' ')continue;
                int mask=bits(*p);
                for(int i=0;i<g.w;++i)if(mask & (1<<(g.w-1-i)))fill(x+i*scale,y+row*scale,scale,scale,v);
                ++row;
            }
            x+=(g.w+(tiny?2:1))*scale;
        }
    }
    void right(int x,int y,const std::string& s,bool tiny=true,int scale=1) { text(x-width(s,tiny,scale),y,s,tiny,scale); }
    void fit(int x,int y,int w,const std::string& s,int scale=2) {
        if(width(s,false,scale)<=w)right(x+w,y,s,false,scale);
        else if(width(s,false,1)<=w)right(x+w,y+3,s,false,1);
        else { std::string shortText=s;while(!shortText.empty() && width(shortText)>w)shortText.pop_back();right(x+w,y+4,shortText); }
    }
    void paint(NVGcontext* vg,int& image,float brightness=1) const {
        // One filtered bitmap avoids seams between adjoining vector rectangles
        // and keeps subpixel strokes visible when Rack zooms the OLED down.
        uint8_t rgba[64][128][4];
        for(int y=0;y<64;++y)for(int x=0;x<128;++x) {
            rgba[y][x][0]=172;rgba[y][x][1]=222;rgba[y][x][2]=242;
            rgba[y][x][3]=uint8_t(pixels[y][x]*brightness);
        }
        const auto* data=reinterpret_cast<const unsigned char*>(rgba);
        if(image<=0)image=nvgCreateImageRGBA(vg,128,64,0,data);
        else nvgUpdateImage(vg,image,data);
        if(image<=0)return;
        nvgBeginPath(vg);nvgRect(vg,0,0,128,64);
        nvgFillPaint(vg,nvgImagePattern(vg,0,0,128,64,0,image,1));nvgFill(vg);
    }
};
