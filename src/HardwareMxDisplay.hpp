// Included inside HardwareDisplay. Native1.6.5 splits assignment/channel on the
// lower row; the separate MIDI bus does not require an attached gate expander.
void mxScreen(OledCanvas& c) const {
    auto* m=metroModule;if(!m)return;int f=mxField(m),assignment=mxAssignment(m);
    field(c,0,"MX - EXPANDER",previewDisplayString(m,Metropolix::MX_MODE),f==0,128,1);c.captionRight(122,3,"GATE");
    c.captionText(2,30,"MX ASSIGN");std::string target=assignment<0?"Global":previewDisplayString(m,assignment);
    c.valueFit(18,50,42,target,false);
    if(f==1 && assignment>=0){c.fieldCorners(0,28,62,27);c.turnHint(3,46,true,pendingConfirmation(m));}
    else if(assignment>=0)c.pressHint(3,46);
    // This heading has a two-pixel word space in the native image.
    c.captionText(68,30,"MIDI");c.captionText(89,30,"CH");int channel=integer(Metropolix::MIDI_MX_CHANNEL);std::string shown=channel?std::to_string(channel):"-";
    if(f==2 && pendingConfirmation(m))shown+=" ?";
    // The native off dash is five pixels; other small channel figures follow
    // the existing seven-row companion face instead of shrinking Bold9.
    c.text(125-c.width(shown,false),43,shown,false);
    if(f==2){c.fieldCorners(66,28,62,27);c.turnHint(69,43,true,pendingConfirmation(m));}
    else{c.box(69,42,18,9);c.captionText(70,44,"ALT");}
    footer(c);
}
