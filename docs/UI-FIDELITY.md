# Panel and display refinement — 2.70.0

## Firmware 1.6.5 additions — 2.70.0

Adds A/B Mod Trigs, MX Mod Gate/Mod Trig and MIDI MX channel/message selection.
The latter works without a physical expander. Two global parameters append
at 787/788; old parameter/cable identities and all 522 musical tuple positions
remain fixed. Schema 35/format 12 protect new globals and A/B route semantics.
Old settings and banks import with the new bus Off. See [file compatibility](BACKUPS.md).

New portable suites cover rising-edge 5 ms conversion, eight lane routing and
CC/note lifecycle at 44.1/48/96 kHz. Native Rack tests exercise actual A/B jacks,
expander independent MIDI with stage value levels, channel/type cleanup, panic and
legacy schema 34 navigation. The full MX default OLED comparison uses 8192
independent native cells; encoder/display tests cover all 9 modes and all 16
channels, mandatory confirmation, cancellation, direct CC/Note menus and exact
Undo/Redo. These tests do not verify an undocumented physical threshold or
MIDI conversion against hardware.

The existing views use the supplied firmware 1.6 manual, especially EXIT on p.27,
MOD editing on pp.138–141 and destination descriptions on pp.159–164.
The default panel remains the hardware arrangement with the independent
**sensiburner <3 intellijel** credit.

Primary SETUP and the Track/MOD/Global/UI/MIDI lists use native row spacing,
mixed-case lettering, right-aligned values and inverted selection, following
the manual's pp33/48 screenshots. Bottom-row descenders remain fully visible. Value editing highlights only the value. The
Shared octave extension correctly displays a pending 1–4 choice and `?`.
See [UI setup](UI-SETUP.md) for gestures, compact labels and compatibility.
Presets Setup now follows the manual's pp53–56 setting order, using the same
list style. Pending Init choices and question marks display correctly; see
[Preset settings](PRESETS.md) for gestures and saved navigation. Backup/maintenance
lists now share the compact rows and inverted selection, with separate status
lines and Cancel-first maintenance confirmation. Further edit-page, glyph,
animation and material matching still require comparison with the reference.

## Native stage diagrams and clear curve editing — 2.69.0

Slide, Skip, CV gates and gated MOD lanes use the manual's circle/X diagrams,
without extra stage numbers or edit arrows. Their action footer retains the
Track:Stage or MOD:Stage identifier. Pitch/gate overrides, ratchets, probability
and accumulation use the measured nine-pixel columns, per-stage inheritance,
trigger/target marks, selection corners and playing-position underline. A
maximum filled bar retains a contrasting edit arrow. Existing editing, ALT
gestures, Encoder Confirm and Undo stay in place.

Detailed CV/MOD curves now start beside a 37-pixel sidebar. Vertices follow the
previous stage, Step transitions and the incoming stage/pulse ramp. A pulse
ramp reaches its target at the first pulse boundary. The large edit arrow
remains as described by manual pp105/138. The playing underline stays beside
the sidebar. CV values use the observed bold face without a redundant V;
MOD precision uses the real bold-font width and preserves destination units.
The observed lowercase m completes Ramp/pRmp. Configured MIDI CC values appear
compactly beside the turn helper and always come from the actual MIDI mapping.
The CV Type footer follows its measured margin.

Eleven independent complete native foreground masks match all 90,112 cells in
actual Rack: five binary/output screens, five bar diagrams and the illuminated
CV Type screen. Two further filtered curve images match 4248 coherent
value, ramp-label, helper and footer cells; ambiguous filtered cells and their
illustrative graph/MIDI values are excluded from that claim. Actual callbacks
cover 112 binary button edits across both tracks/eight lanes with exact Undo,
graph limits, playing markers, pulse/stage ramp endpoints and read-only render.
The earlier full-screen, HOME, popup, font, ownership and pending checks remain
passing. See [manual coverage](MANUAL-COVERAGE.md) for the wider test map.

Column gains between unlabeled illustrated values are visual interpretations.
The curve ordinate uses the model's real ±5V range; the manual's MOD example
is not treated as a calibration oracle for its destination gain. MIDI digits
use the existing compact caption companions, not a claimed complete native
font. Unobserved glyphs, animation brightness and photographic materials still
need hardware comparison. Sequencing, 787 parameter IDs, settings schema34,
format11/522 musical snapshots and backup formats stay unchanged.

## Four HOME overviews and accurate base notes — 2.68.0

HOME follows the measured page-indicator margins, stage boxes, row spacing,
note/caption positions, panel controls and CV/MOD columns in manual pp28–31.
The note-track action footer retains its measured edge; existing MOD waveform
and routing layouts remain intact. Selected, first, skipped, unused and muted
stages remain distinct, including a skipped starting stage.

HOME2 now includes fixed Bottom Pitch in its base-note calculation and marks
per-stage pitch overrides above their notes. It follows the remembered Thru
pitch, selected scale/root, range, direction, transpose and stage overrides.
Temporary Pitch Pre/Post and accumulation stay out of note names, as required
by p29; Pre degrees and Post semitones appear separately in the header. The
physical-slider popup remains a separate view of the actual slider position.

HOME3 places remembered pitch/CTRL values and pickup dots above their controls,
with pulse counts and gate symbols below. Virtual switches recalled with Jump
actually move to their saved values; the desktop does not invent a separate
hidden physical switch or add hardware override dots for that moved switch.
HOME4 shows signed CV/MOD meters, destination-forced gate boxes and their
track/output assignments. The 1.5V gate threshold and underlying signals stay
unchanged. Meter gain and the knob diagram's 270-degree travel are visual
desktop interpretations of the available drawings.

HOME has its own observed five-row figures/degree mark and regular note face,
including F/G shapes and the measured left bearings of 7/8. Nine and unobserved
note glyphs remain companions. C harmonic minor displays the observed Eb;
seven-note factory scales use letter-degree spelling when a single accidental
suffices. Other notes retain the established sharp names. This is an explicit
desktop spelling policy, not a claim about every firmware enharmonic choice.
Compact aliases keep all 149 scale headers and roots clear of the modulation
columns; the full names remain in scale menus.

All 1,325 cells in six independent, unannotated heading/degree/lane-number
regions match the supplied images in actual Rack and a separate saved-image
comparison. The HOME source pictures are filtered and annotated: these region
checks do **not** establish full 8,192-cell screen equivalence. Twelve actual
captures were inspected alongside those reference pictures. A running reference
engine agrees with 194,400 base-note cases across both tracks, representative
factory/user scales, all roots, range/direction/transpose/Bottom Pitch choices
and five override states. Separate checks cover all note spellings/header
widths, sixteen override dots, remembered pickup, position symbols, gates,
module-browser rendering and read-only rendering with unchanged history.

The sequencing algorithm, 787 parameter IDs, schema34, format11/522 musical
snapshots and backup formats stay unchanged. Further stage/edit diagrams,
unobserved glyphs, animations and physical appearance remain under review.

## Physical pitch preview and recalled-value Undo — 2.67.0

Moving a shared pitch slider briefly overlays the manual p22 keyboard, stage
number and note/octave. Disabled scale notes are hatched; the selected note has
a marker. The selected note track supplies scale/root, Bottom Pitch, range,
direction and transpose. The preview shows the **physical slider selection**,
including during Thru pickup. It excludes the remembered pitch, stage override,
temporary Pitch Pre/Post and accumulator changes that may affect the output.
Labels use the hardware-relative C0 convention; output voltage is unchanged.

Actual slider drags, wheel changes, numeric entry and reset can open the popup.
Automation, preset recall and rendering do not. HOME2 already shows note names,
so the popup is suppressed there. Its 1.5-second timeout is a desktop
interpretation of the manual's brief display. Other controls dismiss it. The
first OLED click, drag, double-click, context click or wheel gesture dismisses
it without editing the hidden page; subsequent gestures resume normal editing.
Pending selections, page position and history survive. Reset/reopen clears it.

Recalled pitch and CTRL controls now retain both physical and remembered
pickup state through numeric entry, Initialize, double-click reset, CTRL's
Centre knob action and wheel Undo/Redo. A wheel gesture contributes one action;
crossing the target and returning to the starting raw value still records its
pickup change. Clamped/zero scrolling adds no action. Leaving a control
preserves intervening outside edits; reset/restore discards pending wheel state.
Delayed UI receipts preserve newer physical edits rather than rewinding them.

The complete native p22 example matches all 8,192 foreground cells in actual
Rack and an independent saved-image comparison. Its observed compact 3/D/0
glyphs match the reference; other compact glyphs are companion shapes derived
from the existing value face. Thirty-five captures were inspected. Checks cover
16 track/stage slider gestures, all twelve note markers, 64,800 base-note
comparisons against a running reference engine across representative factory/
user scales and all roots, first-gesture dismissal, pending-page restoration,
fourteen recalled value actions, wheel/drag transitions, queued physical edits,
no-op scrolling and reset/restore cleanup. This is not a hardware timing or
complete glyph-equivalence claim.

The sequencing algorithm, 787 parameter IDs, settings schema34, format11/522
musical snapshots and backup formats stay unchanged. The UI/audio receipt
adapter adds a transient guard for newer physical edits; no new saved fields
are introduced. Native host/package and production checks are reported
separately. HOME and remaining stage diagrams remain under review.

## Performance fields, live gauges and action footer — 2.66.0

Performance pages use the measured five-row captions, six-pixel selection
corners and encoder cue positions. BPM uses its shorter frame, including the
external-clock divider and the applied/base-tempo view. Preset Load Delay
retains its lower value baseline with the measured heading and press cue.
Five additional observed caption glyphs bring the native set to 33; the bold
and large value glyph tables are unchanged. Scale keeps its separately measured
frame and keyboard.

CTRL assignment shows a rectangular position indicator. AUX/MOD continuous
destinations show signed bars around the center; AUX reads the already
post-attenuverter value. Gate destinations retain the on/off indicator and
existing threshold. The CTRL travel interpolation and continuous meter gain
are desktop interpretations of the available samples, not hardware measurements.

The action footer uses proportional captions, four-pixel dots and a native
gate waveform for Slide/Skip or gate MOD lanes. It retains the current stage
function on CTRL/AUX pages, CV on note-track TYPE, and A/B routing suffixes.
Unassigned MOD lanes routed to A/B show OUTPUT. Loopy and preset pages show
their native action labels while keeping latch, mute, queued-load and error
messages. Pending confirmation, held-copy and workflow notices take priority.

Twelve complete 128×64 foreground masks match native manual images in Rack:
internal/external/modulated BPM, Order, Swing, Slide, Gate, Loopy, Output A/B,
AUX and CTRL. These compare all 98,304 cells with no excluded regions, using
source images independent of the renderer. Fourteen signed/bounded AUX/MOD
cases, gate-threshold checks and seven gate-MOD footer contexts preserve signal
and history state. All 118 current performance/ownership captures were inspected.
Existing
tempo preview, owned-control, menu, Scale, chain and full-parameter Undo tests
also pass. Saved captures are separately compared and inspected. These are
display-shape checks, not physical brightness or hardware timing measurements.

DSP, the 787 parameter IDs, settings schema34, format11/522 musical snapshots
and backup formats are unchanged. Native host/package and production runtime
checks are reported separately. The remaining HOME/stage diagrams are the next software display work.

## Native menu layout and complete descenders — 2.65.0

Track, primary SETUP, MOD, Global, UI, MIDI and Presets Setup menus use the
native fifteen-pixel row pitch, measured headings and margins, and inverted
row/value selections. Right alignment respects advance cells, including the
different trailing space of `t`. Zero transpose shows a single dash on the
OLED; its stored value remains zero. Long values retain compact label aliases
and full names in hover help and menus.

The fourth row moves upward when needed to keep lowercase tails visible.
The supplied SETUP image clips the `p` in Input; the desktop version retains
it. Backup/maintenance screens keep compact rows for their status lines, and
track copy feedback temporarily uses the compact layout. Browsing, binary
clicks, confirmation, navigation, native menus and exact Undo/Redo are unchanged.

The entire 8192-cell Track example matches the native p33 image. SETUP matches
6656 native cells across its heading and first three rows; its final 1536 cells
follow an independent bounded-row reference, using the native fourth row moved
up two pixels and restoring its clipped tail from observed glyphs. This is
explicitly a desktop adaptation, not a claim that every SETUP pixel matches
the cropped hardware image. No cells are excluded from either comparison.

Both complete reference arrays pass in actual Rack and a separate saved-image
comparison. All 194 current menu captures were inspected. Sixteen bottom-row
word/selection cases retain their tails, and the existing navigation, pending
value, stale-preview, reset and full-parameter Undo matrices pass. All 20
affected Windows groups pass; release CI separately checks native hosts and
eighteen Linux Rack groups. Font glyph tables, DSP, 787 parameter IDs, schema34, format11/522
snapshots and backup formats remain unchanged. Performance-field and gauge
refinements continue separately.

## CTRL ownership symbols and LIM captions — 2.64.0

Owned fields use a thirteen-pixel circled controller number with the native
outline and seven-row digits. The complete CTRL 2 symbol matches the inverted
encoder legend on manual p26. CTRL 1 uses the same outline and a digit observed
elsewhere in the manual; its composition and placement are interpretations.
Markers retain the existing gray and selected inversion on LIM. This change
does not alter which destinations own a field or the controller priority.

The LIM heading and footer use the proportional caption face, including two
more observed glyphs (plus and P), bringing its observed coverage to 28. The
footer, dots, existing menu rows and selected Negative Limit reproduce every
foreground cell of the firmware 1.6 p126 image in actual Rack. No pixels are
excluded. This is a shape comparison, not a hardware brightness or animation
measurement. Positive/Negative Limit follows the main p126 chapter; the p135
illustration uses the older Upper/Lower wording.

Tests retain all six LIM fields, 132 applied states, 240 actual pending states,
48 owned states, 24 field-navigation clicks, 48 immediate/confirmed edits,
32 owned read-only cases and 24 stale-confirmation conflicts. Another 80 actual
selected-field cases cover both controllers, both tracks and individual/both
assignment across ten fields. The independent symbol mask and contextual
pixels pass, including the lower selection corner where it crosses the marker
region. Encoder/OLED drag, reset, menus, preview isolation and exact Undo/Redo
retain their existing behavior. All 18 affected Windows groups pass; the
packaged production host and native/Linux checks run separately for release.

The first independent comparison caught a one-pixel LIM footer shift. A second
test assumption was corrected to include the existing selection-corner pixels;
the frame remains intact. Sequencing, 787 parameter IDs, settings schema34,
format11/522 snapshots and backup formats are unchanged. Other edit headings,
diagrams and panel materials remain separate visual work.

## Scale screen and editor — 2.63.0

Factory and user scale views follow the native headings, value baselines,
keyboards, root marks, lock/search/MOD-lock indicators, encoder hints, ALT badge
and footer. Note editing has its own larger keyboard, note arrow, EDITING/name
header and EXIT/long-press cue. The active frame moves between notes and Root.
Eleven complete native-screen foreground masks reproduce the supplied firmware
1.6 images on pp62/66/67/68 inside Rack, without excluding any pixels. This is
a foreground-shape comparison, not a physical brightness or animation claim.

Six additional caption glyphs and three bold glyphs come from these images.
There are now 26 observed five-row captions and 55 observed bold glyphs; the
thirteen observed large glyphs retain their existing rendering. Remaining glyphs
are companions. Compact bank headings outside the sampled Major/User views,
the black-key root mark and the two Dim aliases remain interpretations. Whole
Half Dim and Half Whole Dim keep their full names in hover help and menus.

With Encoder Confirm enabled, previews follow the same hardware bank order as
immediate browsing. A forward turn from Major now previews Dorian; it previously
previewed Minor by advancing a legacy stored ID. The change remains pending
until confirmed. Existing IDs and formats are preserved. Queued/pending hints
leave room for lock/long-press symbols and value question marks. Root and note
editing retain their confirmation preferences and exact Undo/Redo.

All 52 captured states and both actual-source charsets were inspected. Tests
cover all names in ordinary/queued/pending layouts, every confirmed bank
transition in both directions, roots, note editing and existing copy/shortcut/
lock/storage workflows. Sequencing code, all 787 parameter IDs, schema34,
format11/522 snapshots and backup formats are unchanged. Further ownership,
other page diagrams/captions and material matching remain separate work.

## Preset-chain display — 2.62.0

The chain table uses seven-pixel capitals with two rows for descenders, plus a
separate five-row caption face. Twenty observed glyphs in each face reproduce
the native firmware 1.6 images on pp171/174. Other glyphs remain companions;
they are not a claim of complete native-font equivalence.

Selected rows fill and invert; Preset and Repeats invert separately when being
edited. Eleven-pixel rows, the chain icon, table columns, mute symbols and track
footer follow the sampled layout. Unavailable empty-chain actions are hatched.
Magenta shortens to Mag when its full name would enter the repeat column; full
bank names remain in desktop menus. Quick Chain shows a plus beside the last
link's repeats, indicating that another matching stage press can increase them.
This cue follows the workflow on p175; its exact placement is an interpretation.
Playback retains the desktop repeat count and follow/static status.

Three independent native-screen foreground masks pass in actual Rack, with
one footer dot excluded because the source images give it different luminance.
Both font charsets and eighteen captured states were inspected. The 32-link
scrolling, field selection, mutes, Quick Chain Undo/Redo and Chain Follow cases
pass without render-time parameter, chain or history changes. Existing input,
playback, storage, 787 parameter IDs, schema34 and format11/522 remain unchanged.
Brightness, animation and physical-module equivalence remain unverified.

## Performance readouts — 2.61.0

Performance fields use separate bold and large lettering based on native OLED
images in the supplied firmware 1.6 manual. Fifty-two bold glyphs and thirteen
large glyphs reproduce the sampled shapes, bearings and spacing. Companion
glyphs remain interpretations; this does not establish a complete native font.
Tiny headings and the proportional menu font retain their existing roles.

Shared fields, stage values, tempo, tuner and assignments use the measured
baselines. Immediate, queued, pending and one/two/three-choice encoder cues
follow the existing confirmation policy. Stage accumulator values include the
degree unit; the SLIDE amount readout is unitless while its desktop quantity
continues to show percent. The main scale readout uses Ionian/Aeolian aliases
without changing scale IDs or full menu names. A/B and MOD assignment context
is clearer. Desktop status/help lines remain available.

Pending BPM edits preserve their question mark during modulation. The smaller
actual-tempo overlay compares against the applied base tempo, so previewing a
new base value does not itself appear as modulation. Confirmation and exact
Undo/Redo retain the existing behavior.

Forty-three independent native text masks, 194 glyph metrics, seven helper
masks, 43 actual module text regions, four pending-tempo drags and fourteen
queued/pending helper states pass inside Rack. These compare foreground pixels,
not physical brightness or color. Whole-screen captures and both charsets were
inspected. Chain and scale-editor geometry, other diagrams, animation and panel
materials remain separate refinement work. Sequencing, all 787 parameters,
schema34, format11/522 snapshots and backup formats are unchanged.

## Menu lettering — 2.60.0

SETUP, Track/MOD/Global/UI/MIDI, Presets Setup, Backup/maintenance and LIM
use a separate proportional font. The regular lettering has nine-pixel capitals
and eleven-pixel descenders. Thirty-eight observed glyphs reproduce native
OLED samples from the firmware 1.6 manual pp33/48/126/135; the remaining printable
glyphs follow those metrics and remain interpretations. Tiny headings/footer
lettering and the performance/numeric fonts keep their existing rendering.

Four-row lists retain their compact desktop spacing and separate status line.
Their selection bar now includes the full descenders, including the bottom row.
LIM retains its three fifteen-pixel rows. Existing pending markers, inverted
values, CTRL dimming/knob numbers and brightness behavior remain available.

Labels shorten only when their current value needs room: Slider Octaves becomes
Slider Oct, Slide Toggle/Amount CC becomes Slide Tog/Amt CC, and Gate Type
becomes Gate for its longest pending choice. Full names remain in hover help
and native menus. The input gestures, Undo/Redo, sequencing, all 787 parameters,
schema34 and format11/522 musical snapshots are unchanged.

Fourteen independent golden text rows check shapes and proportional spacing
against the embedded images; every printable glyph is checked for bounded
metrics and right alignment. Whole-screen captures include applied/pending
values, both CTRL owners and bottom-row descenders. This does not establish
complete typeface, animation, brightness or physical-module equivalence.

## Accumulator/LIM display — 2.59.0

The embedded OLED on manual p126 uses three rows. LIM now follows its title,
15-pixel row spacing, selection bar and track/ACCUMULATOR footer. The existing
six-field cycle and first/last groups of three remain. Labels use mixed case;
CTRL-owned Mode/Polarity labels and values are dimmed and identify CTRL 1/2.
Pending values show an inline `?` alongside the existing confirmation footer.
All six fields remain readable at their limits without abbreviated labels.
See [LIM controls](HARDWARE-UI.md) for the unchanged gestures and ownership rules.
The manual's +7/-7 example is used for comparison; the plugin's +9/-9 defaults
remain. Fonts, animation/material details and physical comparison remain open.

## Values and confirmation

During a held drag, the adjustment popup shows the value you are previewing.
A pending value has **?** and **Press encoder to apply**. Releasing the mouse
keeps the preview pending; pressing the encoder applies it with one undo.
EXIT cancels it. Rack's Tooltips preference also controls this popup.

MOD stages now show destination offsets in both the OLED detail view and the
optional editor. Right-click numeric entry uses the same units. None and A/B
voltage output remain in volts; gate destinations show On/Off.

| Destination | Display |
| --- | --- |
| Pitch Pre / Post | Semitones, rounded |
| Pitch Offset | Semitones, continuous |
| Octave | Whole octaves, truncated toward zero |
| Gate Length / Scale, Probability, Slide Amount, Swing | Percentage offset |
| Internal BPM | BPM offset |
| Root | Semitone offset, limited to ±12 |
| Scale (User) | Whole-scale offset, truncated toward zero |
| Clock / pulse division, ratchet, play order | Offset in the corresponding steps |
| Stage / pulse length and Loopy | Stage or pulse offset |
| Slider Range | Octave-range offset |

These are modulation **offsets**, not the resulting base-plus-modulation
setting. The graph retains its voltage axis. The gains match this engine's
existing routing; numeric gains that the manual does not specify remain
implementation choices pending hardware comparison. Formatting does not
change stored voltages or the sound of existing patches.

The small OLED field abbreviates long units (pl/stg/scl/ord/rt) and reduces
decimal places when necessary so the unit stays visible. Hover help and numeric
entry retain the full units and precision.

ALT shows all MOD vertices while editing the entire lane. MOD gate destinations
send MIDI CC **0/127** even with a saved Bipolar type, including stopped CC
learning. Ordinary bipolar voltage CCs keep their bipolar mapping.

When Velocity Source is CV, a saved Bipolar track CV type operates as
Unipolar. Its tooltip explains the 0–5 V effective range and retains the saved
choice. Changing Velocity Source restores that choice. A pending TYPE change
is shown on the OLED before confirmation.

## EXIT and panel sync

A short EXIT goes back through Setup submenus. Backup files and tuner opened
from primary SETUP return there; desktop shortcuts retain MIDI/Global parents.
Leaving the tuner stops its audition. Other editing screens
return Home.

Hold EXIT for more than one second to **Sync to Panel**, or use its right-click
menu. This releases all recalled pitch/CTRL pickup targets together so the
visible controls determine the values. One undo restores the remembered
targets and one redo releases them again. Sync preserves sequence position;
it does not initiate a preset recall or reset. An already synced panel adds
no history action.

In Rack, Jump recall moves the visible virtual pitch, pulse-count, gate-type
and CTRL controls directly. They therefore already match their settings.
Thru recall keeps visible pitch/CTRL positions and remembers sounding targets;
those are the values released by Sync. This is a desktop adaptation of the
hardware's physically immovable controls. See [Preset pickup](PRESET-PICKUP.md).

## MX and optional editors

Opening the 66 HP editor beside an attached MX keeps the main panel anchored
and moves MX to its new right edge. Closing the editor brings MX back to the
34 HP edge. Rack moves conflicting neighboring modules as needed. The MX link
and its existing cables survive both changes.

## Verification and limits

Native Rack tests check units/numeric conversion, saved/effective CV type and
actual gate-destination MIDI bytes. Real Rack widget callbacks check held
preview popups, confirmation/cancel, submenu back, all ten Sync targets with
undo/redo, optional MOD fields, MX resizing and save/reopen. Bitmap display
captures cover units, ALT vertices, CV overrides/previews and Sync notices.

Visual material details, native OS mouse/focus behavior and physical Metropolix
comparisons still need human testing. The photo and manual guide the design;
they do not prove pixel-perfect appearance or hardware timing equivalence.
