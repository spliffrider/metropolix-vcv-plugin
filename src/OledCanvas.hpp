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
            default:return {2,"00000"};
        }
    }
    static const char* large(char c) {
        switch(c) {
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
            case 'a':return "00E1FHF";case 'b':return "GGUHHHU";case 'c':return "00FGGGF";
            case 'd':return "11FHHHF";case 'e':return "00EHVGF";case 'f':return "648U888";
            case 'g':return "00FHHF1";case 'h':return "GGUHHHH";case 'i':return "404444E";
            case 'j':return "20222IC";case 'k':return "GGIKO KI";case 'l':return "C44444E";
            case 'm':return "00ULLLL";case 'n':return "00UHHHH";case 'o':return "00EHHHE";
            case 'p':return "00UHHUG";case 'q':return "00FHHF1";case 'r':return "00NOGGG";
            case 's':return "00FGE1U";case 't':return "88U8886";case 'u':return "00HHHJF";
            case 'v':return "00HHHA4";case 'w':return "00HHLLA";case 'x':return "00HA4AH";
            case 'y':return "00HHF1E";case 'z':return "00V248V";
            default:return "0000000";
        }
    }
    static int bits(char c) { return c<='9'?c-'0':c-'A'+10; }
    int width(const std::string& s,bool tiny=true,int scale=1) const { int w=0;for(char c:s)w+=(tiny?small(static_cast<char>(std::toupper(c))).w+2:6)*scale;return std::max(0,w-(tiny?2:1)*scale); }
    void text(int x,int y,const std::string& s,bool tiny=true,int scale=1,uint8_t v=255) {
        for(char c:s) {
            Glyph g=tiny?small(static_cast<char>(std::toupper(c))):Glyph{5,large(c)};
            int row=0;
            for(const char* p=g.rows;*p && row<(tiny?5:7);++p) {
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
    void paint(NVGcontext* vg) const {
        // Coalesce whole rectangles, avoiding antialias seams between rows in
        // solid stage blocks while retaining the native grid in small letters.
        bool painted[64][128]={};
        for(uint8_t shade:{uint8_t(90),uint8_t(155),uint8_t(255)}) {
            nvgBeginPath(vg);
            for(int y=0;y<64;++y)for(int x=0;x<128;) {
                if(pixels[y][x]!=shade || painted[y][x]){++x;continue;}
                int start=x;while(x<128 && pixels[y][x]==shade && !painted[y][x])++x;
                int h=1;
                for(int yy=y+1;yy<64;++yy){bool same=true;for(int xx=start;xx<x;++xx)if(pixels[yy][xx]!=shade || painted[yy][xx]){same=false;break;}if(!same)break;++h;}
                for(int yy=y;yy<y+h;++yy)for(int xx=start;xx<x;++xx)painted[yy][xx]=true;
                nvgRect(vg,start,y,x-start,h);
            }
            nvgFillColor(vg,nvgRGBA(172,222,242,shade));nvgFill(vg);
        }
    }
};
