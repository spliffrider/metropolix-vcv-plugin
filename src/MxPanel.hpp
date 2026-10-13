// Included in the main widget translation unit to share its metal jack artwork.
struct MxPanel : widget::Widget {
    void draw(const DrawArgs& a) override {
        auto* vg=a.vg;nvgBeginPath(vg);nvgRect(vg,0,0,60,RACK_GRID_HEIGHT);
        nvgFillPaint(vg,nvgLinearGradient(vg,0,0,60,380,nvgRGB(241,242,239),nvgRGB(203,208,205)));nvgFill(vg);
        for(int y=0;y<380;y+=2)line(vg,0,y,60,y,nvgRGBA(255,255,255,y%6?12:25));
        font(vg);text(vg,42,20,"MX",15,ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
        for(int out=0;out<8;++out){float y=53+out*36+(out>=4?16:0);rect(vg,6,y-15,31,35,nvgRGB(36,39,38),0);text(vg,21,y+15,std::to_string(out+1),8,nvgRGB(245,245,239),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);line(vg,33,y,44,y,ink());}
        text(vg,26,352,"LINK",6,ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
        text(vg,30,365,"sensiburner",6,ink(),NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
    }
};
struct MxWidget : ModuleWidget {
    explicit MxWidget(Mx* m){setModule(m);box.size=Vec(60,RACK_GRID_HEIGHT);
        auto* cache=new widget::FramebufferWidget;cache->box.size=box.size;auto* art=new MxPanel;art->box.size=box.size;cache->addChild(art);addChild(cache);
        addChild(createWidgetCentered<ScrewSilver>(Vec(14,15)));
        for(int out=0;out<8;++out){float y=53+out*36+(out>=4?16:0);addOutput(createOutputCentered<MetalPort>(Vec(21,y),m,out));addChild(createLightCentered<SmallLight<YellowLight>>(Vec(47,y),m,out));}
        addChild(createLightCentered<SmallLight<GreenLight>>(Vec(47,352),m,Mx::CONNECTED_LIGHT));
    }
    void appendContextMenu(ui::Menu* menu) override {
        auto* m=getModule<Mx>();if(!m)return;menu->addChild(new ui::MenuSeparator);
        auto* source=m->leftExpander.module;auto* main=source && source->model==modelMetropolix?dynamic_cast<Metropolix*>(source):nullptr;
        if(main)appendMxMenu(menu,main);else menu->addChild(createMenuLabel("Place MX immediately to the right of Metropolix Lab"));
    }
};
