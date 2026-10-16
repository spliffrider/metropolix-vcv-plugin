// Captures actual Rack front-buffer frames, not a recreation of the panel.
// Camera and page selection affect presentation only, never the musical patch.
void MetropolixWidget::videoStep() {
    auto* m = getModule<Metropolix>();
    const char* directory = std::getenv("METROPOLIX_QA_DIR");
    if (!m || !directory) return;
    static int shot = 0, previousSample = 0, lastCaptured = -48000;
    static bool previousWide = false;
    static std::ofstream timeline(std::string(directory) + "/frames.csv");
    if (qaFrame == 1) timeline << "file,seconds,wide\n";
    if (qaFrame < 30) return; // Let fonts, assets and layout settle first.
    int sample = metrovideo::samples.load(std::memory_order_acquire);
    double seconds = sample / 48000.0;
    // screenshot() reads the front buffer: it contains the previous UI step.
    bool club=std::getenv("METROPOLIX_CLUB_VIDEO")!=nullptr;
    if (previousSample - lastCaptured >= (club?1600:2400) || shot == 0) {
        std::string file = string::f("frame-%05d.png", shot++);
        metroqa::screenshot(std::string(directory) + "/" + file);
        timeline << file << ',' << previousSample / 48000.0 << ',' << previousWide << '\n';
        timeline.flush();
        lastCaptured = previousSample;
    }
    if (std::getenv("METROPOLIX_PERFORMANCE_VIDEO")) {
        if (qaFrame == 30) {
            for (auto* cw : APP->scene->rack->getCompleteCables())
                if (cw->getCable()->inputModule->id == 100) cw->hide();
        }
        m->uiPage.store(Metropolix::HOME);
        m->uiHome.store(1);
        m->uiMod.store(false);
        m->expanded.store(false);
        if(club){
            double bar=seconds*138./240.;int section=int(bar/8.);
            m->uiField.store(0);m->uiAlt.store(false);m->selectedTrack.store(section==2 || section==3?1:0);
            if(section==1)m->uiHome.store(0);
            if(section==2){m->uiPage.store(Metropolix::ACCUM_PAGE);m->selectedRow.store(Metropolix::ACCUMULATION);}
            if(section==3){m->uiPage.store(Metropolix::STAGE_PAGE);m->selectedRow.store(Metropolix::RATCHETS);}
            if(section==4){m->uiMod.store(true);m->selectedLane.store(5);m->uiPage.store(Metropolix::STAGE_PAGE);}
            if(section==5)m->uiPage.store(Metropolix::MX_SETUP_PAGE);
            if(section==6){m->uiPage.store(Metropolix::CHAIN_PAGE);m->chainCursor.store(1);}
            if(section>=7)m->uiHome.store(1);
        }
        if (qaFrame == 30) APP->scene->rackScroll->setZoom(1.f);
        APP->scene->rackScroll->setGridOffset(Vec(-1.f, -.05f));
        previousSample = sample;
        if (!metrovideo::armed.load(std::memory_order_relaxed)) {
            m->params[Metropolix::RUN].setValue(1.f);
            metrovideo::armed.store(true, std::memory_order_release);
        }
        return;
    }
    bool wide = seconds >= 17.777778 && seconds < 44.444444;
    m->expanded.store(wide);
    m->modEditor.store(wide);
    m->uiMod.store(wide);
    m->uiPage.store(Metropolix::HOME);
    m->uiHome.store(seconds < 8.888889 ? 0 : seconds < 17.777778 ? 1 : wide ? 3 : 0);
    m->selectedTrack.store(seconds < 8.888889 ? 0 : 1);
    m->selectedLane.store(seconds < 26.666667 ? 3 : seconds < 35.555556 ? 2 : 7);
    m->selectedRow.store(seconds >= 53.333333 && seconds < 62.222222 ? Metropolix::RATCHETS : Metropolix::SLIDE);
    if (qaFrame == 30 || isExpanded != previousWide) {
        APP->scene->rackScroll->setZoom(isExpanded ? 1.65f : 2.25f);
    }
    // Parent RackScrollWidget finishes its layout after this child step.
    // Reapply the offset on subsequent frames after a zoom change settles.
    APP->scene->rackScroll->setGridOffset(Vec(-1.f, -.05f));
    previousWide = isExpanded;
    previousSample = sample;
    if (!metrovideo::armed.load(std::memory_order_relaxed)) {
        m->params[Metropolix::RUN].setValue(1.f);
        metrovideo::armed.store(true, std::memory_order_release);
    }
}
