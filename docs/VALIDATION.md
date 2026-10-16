# Validation and fidelity status — 2.70.0

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

## 2.63 native-reference scale screen and confirmed bank order

The supplied firmware1.6 images on pp62/66/67/68 provide eleven complete 128x64
screen masks. Every foreground pixel matches in actual Rack, including normal,
locked, searched, user, note-edit and Root-edit views. Saved captures were
independently compared again. The native/reference, all 52 current states and
both actual-source charsets were inspected. Eighty-one observed glyph masks
match their source bytes: 55 bold and 26 caption glyphs. Unobserved glyphs remain
companions; the existing thirteen observed large glyphs are unchanged.

The layout uses measured keyboard sizes/key profiles/checkers, root marks,
selection corners, bank/value headings, lock/search/MOD-lock, press/long-press
cues, ALT/EXIT badges, note arrow and footer. Compact bank headings beyond the
sampled Major/User views, a black-key root mark and two Dim aliases remain
interpretations. Foreground masks do not establish physical brightness, color
or animation equivalence.

The first actual Rack run passed all eleven native masks, root marks and Root
previews. It found that two long diminished-scale names exceeded their area;
compact OLED aliases correct this while preserving full menu/quantity names.
It also exposed an existing confirmed-browsing inconsistency: the confirmation
path advanced numeric scale IDs, whereas immediate navigation used hardware
bank positions. Confirmed previews now use bank positions too. Major therefore
advances to Dorian in both modes, while retaining all 149 legacy IDs. The
confirmation requirement itself is unchanged. The test's immediate note-cursor
drag was also lengthened to cross Rack's normal snap threshold; sensitivity
and note bindings are unchanged. The repeat run passes all cases.

New actual tests cover eleven complete native masks; 447 name/header states
across ordinary, queued and pending layouts; twelve transposed root markers;
four immediate/confirmed encoder/OLED browse cases; 596 confirmed transitions
across all 149 scales, both directions and clipped endpoints; twelve factory/
user/editor Root previews; and four note-cursor/toggle workflows. Full 787-value
Undo/Redo, preview isolation and no-op history are checked. Existing all-user/
factory editor guards, final-note protection, shortcuts, lock, desktop menus,
copy/paste and native patch reopening still pass.

All 27 affected Windows groups pass: Scales, Pending, Display, CtrlEditor, Backup,
Config, Storage, Presets, Polish, Fidelity, StagePair, Overrides, Slides, StageCv,
Clock, Outputs, Mx, Controls, Tracks, SetupOptions, Timing, Order, StagePattern,
Loopy, Chains, ScaleCopy and ScaleAudio. ScaleAudio observes C/Am/F/G definitions
and roots on both tracks during actual stereo/CV playback. This is automated
signal-path verification, not a human listen. Production-host and exact-source
release CI are recorded separately: 25 portable suites, three native platform
hosts and eighteen Linux Rack groups including these scale checks.

The confirmed SCALE preview traversal is the sole input-behavior correction.
Other confirmation, cursor/root sensitivity, sequencing code, 787 parameter IDs,
schema34, format11/522 snapshots and backup formats remain unchanged. Remaining
ownership markers, other diagrams/captions/materials and physical comparisons
are explicit work; no full physical-module equivalence is claimed.

## 2.62 native-reference chain display

Three native 128x64 OLED screens were extracted from firmware1.6 manual
pp171/174 without resampling. Twenty regular seven-capital/nine-descender
glyphs and twenty five-row caption glyphs reproduce their observed shapes and
spacing. Regular companions reduce the menu face; caption companions retain
the earlier tiny shapes. Unobserved glyphs remain interpretations.

Actual Rack tests compare all foreground pixels in each native screen except
one footer dot at (38,60). Its source luminance is 219 in two images and 29 in
the other, so it is excluded from the binary-mask claim. This does not prove
animation or brightness equivalence. Saved captures were independently compared
again; before/after/native layouts, twelve representative states and both actual
source charsets were visually inspected. Forty observed source glyph masks
also match the extracted reference bytes.

The new cases pass all three screen masks, 194 glyph metrics, 63 selected-link
and field states through 32-link scrolling, all eight mute combinations, actual
Quick Chain repeats with exact Undo/Redo, and actual Chain Follow toggles.
Rendering does not write raw parameters, chain state or history. Existing
exhaustive insertion/deletion/clear and bank/repeat history checks still pass,
along with actual saved-chain patch reopening.

All nine Windows groups pass: Chains, ChainAudio, Display, Pending, Polish,
Fidelity, Storage, Presets and Backup. ChainAudio verifies the stereo/CV signal
path and arrangement transitions; it is not a human listening test. Eighteen
new direct chain captures are retained alongside existing display, LIM, menu
and backup coverage. Production-host and exact-source release CI evidence is
recorded separately: 25 portable suites, three native platform hosts and
eighteen Linux Rack groups, including the same chain screen checks.

Only chain rendering changes: inverted row/field selection, measured row/column
geometry, captions/icons/mutes, hatched empty actions and the Quick Chain plus
cue. Its placement and compact Magenta alias are interpretations. Input policy,
playback, eight saved chains, all 787 IDs, schema34, format11/522 and backup
formats are unchanged. Scale-editor geometry, other diagrams, remaining tiny
lettering, ownership markers, materials and physical comparisons remain open.

## 2.61 performance lettering and encoder cues

The native OLED images were extracted without resampling from the supplied
firmware1.6 manual. Fifty-two observed bold glyphs and thirteen large glyphs
provide measured shapes, bearings, advances and right edges; companion/fallback
glyphs remain interpretations. Degree and plus/minus normalize before fitting.
Fields choose the large, bold, regular or tiny face according to available room.

Forty-three golden text regions come directly from the original image pixels,
independently of the font lookup. Actual Rack tests pass all 43 captions,
194 glyph metrics, seven independent native helper regions and 43 configured
module text regions without parameter/history changes. Four real encoder/OLED
tempo drags cover ordinary and modulated previews, one pending question mark,
confirmation and exact full-parameter Undo/Redo. Fourteen real helper states
cover clock, TYPE, AUX, CTRL, MOD, MX and A/B confirmation policies. Saved captures
were separately compared against all 43 foreground masks and the modulated base
tempo; both charsets and sixteen manual/module comparisons were inspected.
Foreground shape matching does not establish brightness or color matching.

All 24 affected Windows groups pass: Pending, Display, CtrlEditor, Backup,
Config, Storage, Presets, Polish, Fidelity, StagePair, Overrides, Scales, Slides,
StageCv, Clock, Outputs, Mx, Controls, Tracks, SetupOptions, Timing, Order,
StagePattern and Loopy. Existing menu/pending/ownership/conflict/Undo coverage,
181 Display captures, 73 LIM captures and 56 Backup list captures are retained.
The production package host and exact-source release CI are recorded separately;
release CI covers 25 portable suites, three native platform hosts and eighteen
Linux Rack groups including the new pixel and pending-tempo checks.

This changes rendering and the modulation-overlay predicate only. Engine,
control bindings, confirmation policy, 787 parameter IDs, schema34, format11/522
snapshots and backup formats are unchanged. Chain/scale-editor geometry, other
diagrams, animation, materials and physical-module comparisons remain open.
No physical timing, OS-input automation or human listening is inferred.

## 2.60 proportional menu lettering

The small OLED images were extracted without resampling from the supplied
firmware1.6 manual. Four source pages (33/48/126/135) provide 38 observed glyphs
for a separate regular menu font: nine-pixel capitals, eleven-pixel descenders,
variable widths, measured tracking and a one-pixel left bearing. Fifty-eight
companion glyphs follow these metrics; they are not hardware-verified. Tiny
headings and the existing performance/numeric faces are unchanged.

Fourteen golden text rows come directly from image pixels, independently of
the font lookup. Actual Rack callbacks pass every row comparison, all 96
printable glyph bounds/right alignment and plus/minus normalization. Source
bytes were also checked against the inspected full charset. Native-reference,
before/after and pending/owned captures were inspected at enlarged resolution.

The font is used by primary/secondary SETUP, Presets Setup, Backup/maintenance
and LIM. Four-row lists retain their compact spacing/status line with a
twelve-pixel selection bar, preserving all eleven descender rows. LIM retains
its existing three-row geometry. Conditional aliases keep long values and
pending markers apart; full hover/menu names remain. A first actual Rack run
caught a one-pixel Gate Type / Multi+Rst ? overlap; the conditional Gate label
corrects it. The repeat run passes all label/value fit checks.

All nine Windows groups pass: Pending, Display, CtrlEditor, Backup, Config,
Storage, Presets, Polish and Fidelity. Pending includes 2,528 applied and
4,896 actual pending secondary readouts; 99/178 preset-setting states;
132/240/48 applied/pending/owned LIM states; and their existing full-parameter
Undo/Redo, navigation, ownership and external-edit conflict checks. The 181
Display captures, 73 LIM captures and 56 Backup list captures are retained.
The production package host and exact-source release CI are recorded
separately; release CI covers 25 portable suites, three native platform hosts
and 18 Linux Rack groups including these golden image rows.

This is rendering only: the engine, input behavior, all 787 parameter IDs,
schema34, format11/522 snapshots and backup formats are unchanged. Remaining
typography/page geometry, animations, materials, brightness and hardware
comparisons remain explicit work. No OS input or human listening is inferred.

## 2.59 accumulator/LIM display

The firmware1.6 printed p126 OLED was extracted at its original 136×72 pixels,
including the four-pixel border, and inspected alongside before/after renders.
Its active area uses three rows at y10/25/40, a LIMITS + SETUP title, a 15-pixel
selection bar and track/ACCUMULATOR footer. LIM now follows that geometry with
mixed-case text, signed limits, dimmed CTRL-owned labels/values and knob markers.
Pending values add an inline `?` beside the existing confirmation footer.
The reference +7/-7 state was captured explicitly; plugin defaults remain +9/-9.
Typeface/glyph details, brightness and physical equivalence remain unverified.

A pre-change actual-callback probe passed all 32 CTRL-owned click-navigation
cases, 32 blocked drags and 24 pending value/footer cases. These behaviors
already worked; no driver/binding or accumulator fix was needed. This is a
display refinement. Six-field click cycling, direct vertical editing, grouping,
native menus, CTRL ownership and the existing long-press policy remain intact.

Windows Pending now checks 132 applied value renders, 240 actual pending
renders, 48 owned display states with both knobs/track scopes, nonoverlapping
labels, 24 field-cycle cases, 48 immediate/confirmed edits with full-parameter
exact Undo/Redo, 32 read-only drag/reset/navigation cases and 24 external-edit
conflicts. There are 73 direct LIM captures including the reference state,
extremes, pending values and selected/unselected ownership. ALT entry and EXIT
retain their existing targets. Actual native controller menus remain available.
All nine relevant Windows groups pass: Pending, Display, CtrlEditor, Backup,
Config, Storage, Presets, Polish and Fidelity. Display retains 181 states.

Only the OLED renderer changes in production. DSP, all 787 parameter IDs,
schema34, format11/522 musical snapshots and backup formats are unchanged.
No new LIM reset gesture, preference or serialized edit state is introduced.
The stripped Windows production host passes 22,624,609 frames, 339,369,135
output checks, 56,664 edits, 145 restores, 144 fresh instances, 12 initializes
and 145 reciprocal links at 44.1/48/96kHz across both models: 361.083 virtual
seconds, single-thread wall time 12.7699 seconds. The final package is tested
separately. Release CI runs 25 portable suites, native/production hosts on
Linux and both Mac architectures, plus 18 real Linux Rack groups including
the same Pending matrix. Exact-head receipts and package audits are verified
separately before publication. OS input and human listening are not inferred.

## 2.58 Backup and maintenance lists

Backup Files, its preset-selection submenu and maintenance now use the compact
mixed-case list style shared by the other setup screens. The Backup main page
shows all four actions. Preset Export scrolls its ten selections with the first
four together. Whole-row inversion marks browsing; titles remain visible at
the top and status messages occupy a separate bottom line. Maintenance keeps
the Lab software version, explicit action/scope text and Cancel-first Apply
confirmation. This is refinement of the plugin's desktop workflow, using the
manual pp33/48 list style and pp53/59/189 scope references. It does not add
hardware SysEx, calibration or EEPROM actions, or claim a firmware pixel match.

Forty actual Rack encoder/OLED navigation cases reach every action and both
boundaries without parameter or history changes. Fifty-six complete 128×64
pixel states cover every selection, notice text, confirmation action/choice,
mixed-case descenders and separate footer. Labels fit without abbreviation;
expired notices restore the page hint. The 56 direct OLED captures were saved,
and representative before/after captures were inspected against the 14 saved
2.57 baseline screens. The old geometry was a known UI gap; this pass does not
claim an additional functional defect in import, export or maintenance.

Both controls preserve native context menus and Cancel-first/EXIT cancellation.
Existing actual Rack tests also pass Unicode export, full/selective imports,
atomic invalid-file rejection, exact native Undo/Redo, settings/bank preservation,
correct EXIT parents, selective-selection reopen and disarmed maintenance
reopen. Eight relevant Windows application groups pass: Backup, Pending,
Display, Config, Storage, Presets, Polish and Fidelity. Display retains 181
separate states. File/maintenance actions, DSP, 787 parameter IDs, schema34,
format11/522 snapshots and backup formats are unchanged. Settings/All still
require 2.55; current musical banks still require 2.34.

The current-source stripped Windows production host passes 22,624,609 frames,
339,369,135 output checks, 56,664 parameter edits, 145 restores, 144 fresh
instances, 12 initializes and 145 reciprocal links at 44.1/48/96kHz across both
models: 361.083 simulated seconds, single-thread wall time 13.9991 seconds.
The final packaged binary is tested separately. Release CI runs all 25 portable
suites, native integration and stripped production hosts on Linux/Intel Mac/
Apple Silicon, plus 18 real Linux Rack groups including this Backup matrix.
Those receipts and package audits are verified separately before publication.
OS mouse/focus, human listening and hardware equivalence are not inferred.

## 2.57 Presets Setup list and pending readouts

Presets Setup now uses the shared four-row mixed-case list style. The manual's
pp53–56 define Queue Pulses, Pitch Load, Switch Load, Ctrl Load, Aux Load and
Set Init Preset in that order; the two Rack extensions follow a RACK heading.
Browsing highlights the whole row; editing highlights its value. Both encoder
and display support browse/edit, immediate or confirmed values, direct binary
toggles, native menus and selected-setting long resets. Fifteen direct 128×64
captures were inspected for first/last views, long labels, 4096-pulse values
and pending Init bank/slot. Exact glyph/firmware matching and physical-module
comparison remain open; Backup/maintenance layouts are the next display gap.

The actual-callback baseline reproduced missing question marks in all 16
pending field/control cases and incorrect Init values in two of them. The
other 14 value previews already used their pending value. Windows callbacks
now pass 30 navigation cases, 99 applied and 178 pending pixel states, 32
immediate/confirmed/binary edits with exact full-parameter Undo/Redo, 12
independent-edit conflicts and 16 selected-setting resets. All 65 Init choices
and representative 0–4096 pulse boundaries are covered. An obsolete held
confirmation preserves the independent edit and consumes the click without
a secondary action; a fresh click can leave editing. EXIT cancels the draft.
Linux Pending runs this matrix in its package job and is verified separately.

Native Rack integration passes 80 restores across schemas29–33 and 16 current
schema34 restores: every legacy logical field ID keeps the same parameter,
reopens browsing and preserves unrelated preset banks. Transient edit selection
is excluded from JSON and is cleared by native patch reopen and initialization.
Invalid settings reject without changing serialized or transient state. No
DSP behavior, 787 parameter IDs, schema34 or format11/522 musical values change.
Settings/All backups retain their 2.55 minimum; musical compatibility is unchanged.

All 32 neighboring Windows UI/audio groups, the 25 portable suites and native
integration pass. Display retains 181 separate states. The stripped production
host passes 22,624,609 frames, 339,369,135 output checks, 56,664 parameter edits,
145 restores, 144 fresh instances, 12 initializes and 145 reciprocal links at
44.1/48/96kHz across both models: 361.083 simulated seconds, single-thread
Windows wall time 13.1642 seconds. The final packaged DLL is checked separately.
This is signal-path testing, without an OS input or human listening claim.

## 2.56 track and secondary setup lists

The firmware1.6 manual pp33–37 and p48 show four compact rows with mixed-case
labels and inverted browsing/value selection. Track/MOD/Global/UI/MIDI lists
now share that geometry with primary SETUP. Actual 128×64 captures of the p33
first-four state, pending choices, extreme values and copy feedback were
inspected against the supplied reference. Presets Setup and Backup/maintenance
layouts, remaining glyph details and material matching remain open.

A baseline actual-callback/pixel test found 16 incorrect Shared→1–4 octave
previews across both tracks and encoder/OLED input. Its other 26 clock/scope/
reset/boot preview cases passed. Slider Octaves now reads the pending value;
numeric octave, signed transpose and compact gate-source names fit the list.
Software Auto/Legacy/Patch/Shared extensions retain their meaning. No DSP,
parameter IDs, settings schema34 or format11/522 musical positions change.

Windows Rack callbacks pass 168 navigation cases, 2,528 applied value renders,
4,896 actual pending renders, 256 immediate/confirmed/binary edits with exact
full-parameter Undo/Redo, and 128 selected-setting long resets. Checks cover
both tracks and all eight MOD lanes, every choice, selection/descender pixels,
nonoverlapping labels and the separate copy-feedback line. There are 151
additional direct OLED captures. Pending/reopen and the existing primary
SETUP migration/confirmation matrix also pass. Linux Pending runs this same
matrix in its package job; platform receipts are checked separately.

All 31 neighboring Windows UI/audio groups, the 25 portable engine suites and
native Rack integration pass. Display retains 181 dedicated states. The
current-source stripped production host passes 22,624,609 frames, 339,369,135
output checks, 56,664 parameter edits, 145 state restores, 144 fresh instances,
12 initializes and 145 reciprocal links at 44.1/48/96kHz across both models:
361.083 simulated seconds, single-thread Windows wall time 15.125 seconds.
The final packaged DLL is tested separately. No physical-module comparison,
OS input simulation, human listening assessment or Mac GUI playtest is claimed.

## 2.55 primary SETUP list and saved navigation

The first four entries follow the firmware1.6 manual p48 screenshot; p49–58
define the remaining settings and behavior. Actual 128×64 renders were
inspected against that reference: four compact mixed-case rows, right-aligned
values, inverted whole-row browsing and inverted value-only editing. This is
primary SETUP fidelity; advanced/track/MOD subpages still have named layout gaps.

Actual Windows Rack callbacks pass 34 encoder/OLED navigation cases, 28
multi-choice confirmations with Enc. Confirm Off/On and exact full-parameter
Undo/Redo, 28 independent-edit conflicts, 6 binary setting cases, and 20
selected long resets before/during editing. Tuner/chains/backup/preset/UI/factory
entry and correct EXIT parents pass, including nested Backup selection and the
desktop Slider Octaves/Global/MIDI shortcuts. Rendering checks verify the
selection bar and value-only highlight; all four 1–4 octave menu choices pass
with exact raw Undo/Redo and no-change history. Linux Pending runs the same callback
matrix as part of its package checks; platform receipts must pass separately.

Native engine integration passes 76 restores of released schemas29–33 and 34
current entry/edit states, plus nested-parent/tuner persistence and atomic
invalid-file rejection. Schema34 adds one boolean navigation field and expands
the saved cursor to 0–16; old main edits reopen browsing with explicit mapping.
All 787 parameter IDs and format11/522 musical positions are unchanged.
Settings/All backups now require 2.55; musical-only compatibility is unchanged.

All 30 neighboring Windows UI/audio groups and 25 local engine suites pass,
including Pending/reopen, Config, Backup/maintenance and Storage. Display
retains 181 states. The current-source stripped production host passes
22,624,609 frames, 339,369,135 output checks, 56,664 parameter edits, 145 state
restores, 144 fresh instances, 12 initializes and 145 reciprocal links at
44.1/48/96kHz across both models: 361.083 simulated seconds, single-thread
Windows wall time 12.9617 seconds. The final packaged DLL is tested separately.
No physical module comparison, OS input simulation or Mac GUI playtest is claimed.

## 2.54 MOD timing lane navigation

The firmware 1.6 manual p145 allows plain stage buttons to select the MOD lane
on ORDER/LEN/DIV. Actual Rack callbacks on the previous implementation instead
opened the stage editor in all 256 clicks and 256 held-movement cases. Half the
clicks also changed a gate value, and every movement changed stage values/history.
The buttons now act as unbound lane selectors in this context, preserving the
page/field and canceling an old preview only when selecting a different lane.
Held shortcut, EDIT/copy, ALT and Loopy priorities retain their existing paths.

Final Pending/reopen passes all 512 navigation cases, 1,024 encoder/OLED edits
after selection with confirmation Off/On and exact full-parameter Undo/Redo,
224 old-lane preview cancellations, and 256 right-click lane settings actions
after selection changes with destination Undo/Redo. Another 32 cases retain a
same-lane preview and 32 preserve independent recalled-pitch edits. Actual
preset-pickup GUI load/cross/Undo/takeover/save-reopen checks also pass. Lane lights and
mouseover text are checked. Prior native value, type, drag, reset, shortcut,
preview and persistence matrices remain passing. These are actual injected
Rack callbacks, not physical OS input or hardware measurements.

All 25 neighboring Windows UI/audio groups, 25 local engine suites and native
Rack integration pass. Display checks retain 181 implemented states. The
current-source stripped production host passes 22,624,609 frames, 339,369,135
output checks, 56,664 edits, 145 state restores, 144 fresh instances, 12 initializes
and 145 reciprocal links across both public models at 44.1/48/96 kHz: 361.083
simulated seconds in one thread, Windows wall time 13.2244 seconds. The final
packaged binary must pass separately and its assets must match current source.
DSP algorithms, parameter IDs and saved formats are unchanged: 787 parameters,
schema33, musical format11 and 522 fields.

## 2.53 native-style parameter actions

Actual Rack context-menu, numeric-field Enter and double-click callbacks on the
previous implementation reproduced fourteen failures: lost dormant raw values
in Undo, missing history when the displayed value was unchanged, reset of locked
MOD type/assignment preferences, numeric entry redirected to a newly selected
stage/track, and an older menu overwriting a newly CTRL-owned field. The shared
value control now resolves the parameter captured when its menu opened, checks
current editability at action time, uses the existing quantity parser/setter or
reset, and records the applied raw before/after values in one ParamChange.

The final Pending suite passes all focused reproductions plus 5,040 numeric,
Initialize and double-click cases across both tracks, all eight MOD lanes, eight
stages, three saved types and dormant/effective ranges. Another 528 cases retain
assigned type and global assignment preferences without false Undo. Exact
787-parameter arrays, Undo and Redo are checked. Existing Unmap and no-change
entry checks pass, as do older drag, preview, menu, shortcut and save/reopen
matrices. These are injected real Rack widget callbacks, not an OS mouse/focus
or physical-module comparison.

All 25 local engine suites, native Rack integration and 25 neighboring Windows
UI/audio groups pass. Display coverage remains 181 implemented states; drawing
is unchanged. The current-source stripped production host passes 22,624,609
frames, 339,369,135 output checks, 56,664 edits, 145 state restores, 144 fresh
instances, 12 initializes and 145 reciprocal links across both public models at
44.1/48/96 kHz: 361.083 simulated seconds in one thread, with Windows wall time
13.0342 seconds. The final packaged binary must pass separately; package audits
must match current source. No sequencer behavior or public formats changed:
787 parameters / schema33 / musical format11 / 522 fields remain intact.

## 2.52 locked MOD type grid context menu

Actual right-click callbacks reproduced a forced MOD type cell showing
**CTRL 0**. Its Clear assignment action reset Track 1 accumulation mode,
because controller index -1 addressed the preceding parameter. The grid
now distinguishes destination-controlled type from actual CTRL ownership;
the controller menu helper also rejects invalid indices.

The final Pending suite checks 912 assigned type menus across every lane,
saved type and internal destination. Menus show the effective type and its
destination while leaving all parameters/history intact. Another 72 cases
open the clicked lane's settings after selection changes, clear its internal
destination, restore the saved type and verify exact full-parameter Undo/Redo.
Invalid controller menu indices leave no options or state/history changes.
Earlier MOD type, 712 paired-page Undo, reset, shortcut, preview and save/reopen
matrices remain passing. These are actual injected Rack callbacks, not OS
mouse/focus or physical hardware comparison. No sequencer behavior, public
parameter IDs or formats changed: 787/33/11/522 remain intact.

All 25 local engine suites and native Rack integration pass. Twenty-five
neighboring Windows UI/audio groups pass after the menu correction. Final
Pending was rerun after the bounds refinement; other workflow paths are
unchanged. Display checks retain 181 implemented states; no panel/OLED drawing
changed. Finite signal recordings do not establish long human playtests.

The current-source stripped production host passes 22,624,609 frames,
339,369,135 output checks, 56,664 parameter edits, 145 state restores,
144 fresh instances, 12 initializes and 145 reciprocal links across both
public models at 44.1/48/96 kHz. This is 361.083 simulated seconds in one
thread, with Windows wall time 12.3784 seconds. The final packaged binary
must pass this host separately and its assets must match current source.

## 2.51 paired-page drag ownership

Actual Rack callbacks reproduced a drag claiming an independent write to
its paired reset partner. The control now has a separate drag capture scope;
whole-page long-reset scopes remain unchanged. CLK/GATE/SLIDE/LEN/DIV/ORDER
encoder and OLED turns capture their selected field. PROB/ACCUM turns capture
selected or ALT amounts, excluding secondary targets/triggers.

The final 712-case callback matrix covers both note tracks, all eight MOD
lanes where applicable, every PROB/ACCUM stage, both encoder/OLED surfaces,
selected stage buttons and ALT external target/trigger changes at every
stage. Half the partner writes have an independent parameter Undo action;
the rest simulate raw external writes. Exact full-parameter Undo/Redo and
cursor/tip release pass. The fixture uses percent units for GATE/SLIDE.

The entire Pending suite and save/reopen pass, retaining the 699 long-reset
cases, 48 held shortcuts, earlier raw-value history and confirmation conflict
matrices, and the MOD destination-controlled type checks. All 25 local engine
suites and native Rack integration pass. These are injected Rack callbacks,
not OS mouse/focus tests or a physical hardware comparison. Parameter IDs,
sequencing, routing and formats remain 787/33/11/522.

Windows Pending/save-reopen and 25 neighboring application groups pass:
CtrlEditor, Chains, ScaleCopy, Timing, StagePair, StagePattern, StageCv, Scales,
Swing, Overrides, Workflows, SetupOptions, Outputs, Fidelity, Polish, Display
(181 states), ChainAudio, Order, Modulation, ExtendedRouting, Midi, Mx, Group,
RoutingAudio and MidiAudio. Finite audio checks do not establish long human
sessions or a hardware comparison. No display drawing changed in this fix.

The current-source stripped production host passes 22,624,609 frames,
339,369,135 output checks, 56,664 parameter edits, 145 state restores,
144 fresh instances, 12 initializes and 145 reciprocal links across both
public models at 44.1/48/96 kHz. This is 361.083 simulated seconds in one
thread, with Windows wall time 12.6437 seconds. The final packaged binary
must pass this host separately and its assets must match current source.

## 2.50 MOD effective type and OLED TYPE workflow

The complete manual sections on MOD stage editing and the 1.6 type update
(printed pp.137–142/192) specify Bipolar or Gate from an internal destination.
A standalone 819-case probe reproduced 168 continuous-value failures with
saved Unipolar/Gate choices. Actual Rack callbacks reproduced continuous
range loss, accidental Gate toggling, MOD ALT+TYPE opening a desktop menu,
obsolete type confirmation and forced-type editing. A shared effective-type
rule now serves sequencing, ranges, ramps, graph/gate editing, A/B and MIDI.
The native type quantity preserves its saved preference under assignment.

All 25 local engine suites pass, including 819 explicit value cases, 117
ramp cases and raw-data preservation. Native integration passes 52,416
range/value cases across every lane, destination, saved type and stage;
type locking, preference recovery and exact preset/Rack roundtrip also pass.
Another 144 native cases drive both real A/B ports and capture running MIDI
and stopped CC learning across all lanes, three assignment modes and saved
types. Group mute covers all 256 eight-bit masks in all three saved modes with
unassigned, continuous and gate destinations, retaining phase/MIDI/MX tests.

Actual Rack Pending passes 936 OLED TYPE entries, 3,648 forced-type encoder/
OLED/grid drags and holds, and 936 stage selections/toggles with exact raw
Undo/Redo. Four original focused callbacks and obsolete type confirmation
pass; previous raw/history/conflict matrices and save/reopen still pass.
Fixtures explicitly initialize each selected target's destination, avoiding
demo settings leaking into an unassigned-type test. The forced Bipolar/Gate,
restored Unipolar and negative continuous stage OLED captures were inspected.
These are callback/pixel/native signal checks, not OS mouse/focus, human
listening or a physical hardware comparison. Stored values and formats remain
787 parameters, settings schema33, musical format11 and 522 musical values.

The stripped current-source production host passes 22,624,609 frames,
339,369,135 output checks, 56,664 parameter edits, 145 state restores,
144 fresh instances, 12 initializes and 145 reciprocal links across both
public models at 44.1/48/96 kHz. This simulates 361.083 seconds in a single
thread; Windows wall time was 12.2241 seconds. Packaging must retain the exact
tested binary and separately audit its assets against current source.

Windows Pending/save-reopen and 25 neighboring application groups pass:
CtrlEditor, Chains, ScaleCopy, Timing, StagePair, StagePattern, StageCv, Scales,
Swing, Overrides, Workflows, SetupOptions, Outputs, Fidelity, Polish, Display
(181 states), ChainAudio, Order, Modulation, ExtendedRouting, Midi, Mx, Group,
RoutingAudio and MidiAudio. Audio checks use finite captured signals; human
listening and long cross-plugin sessions remain unverified.

## 2.49 Dependent value history and confirmation

Actual Rack callbacks reproduced nine failures: four Track CV/MOD grid and
encoder edits gained phantom native history when their type changed during
the drag; four existing voltage/clock previews survived a change to their
type, Velocity source, destination or divider; and a held tiny preview could
fall through to a Gate click after UI invalidation. All nine original probes
pass after the fixes. Raw batches now finish native drag capture without a
bound quantity, then record their own raw receipt; they no longer briefly
rewind live parameters for native cleanup. Preview context captures its
type/range dependencies, and held controls remember the preview generation.

Expanded Pending verifies 5,760 type-changing Track CV/MOD edits across every
track/lane and stage, all six type transitions, encoder/OLED/grid, ALT and
two/22-pixel movement. Full raw-state assertions check each genuine edit and
click's Undo/Redo in order, leaving the external type unchanged. Another
2,816 previews cover type, Velocity source, MOD destination and CLK divider
changes across stages/targets, both controllers, ALT, immediate obsolete
confirmation and small/large held releases, including prior UI invalidation.
Fresh intentional motion recaptures a valid preview; a bound no-op still
performs its normal click. Fixtures explicitly initialize their selected
target's type/destination and check that the starting preview exists.

These are actual Rack callbacks with isolated history and paused processing,
not OS mouse/focus, human listening or physical hardware comparison. Existing
Pending, save/reopen, raw-value, reset and shortcut checks remain in the same
suite. There are no DSP, parameter-ID or saved-format changes: 787 parameters,
settings schema33, musical format11 and 522 stored musical values.

Windows expanded Pending and original save/reopen pass, alongside previous
raw-value edit/click matrices, 699 long-reset receipts, stationary clicks and
48 held GATE/SLIDE shortcuts. CtrlEditor, Chains, ScaleCopy, Timing, StagePair,
StagePattern, StageCv, Scales, Swing, Overrides, Workflows, SetupOptions,
Outputs, Fidelity, Polish, all 181 Display states, ChainAudio and Order pass.
ChainAudio records twelve real seconds at 48 kHz. These are finite checks.

Native integration and the stripped production host pass 22,624,609 frames,
339,369,135 output checks, 56,664 parameter edits, 145 state restores,
144 fresh instances, 12 initializes and 145 reciprocal links across both
public models at 44.1/48/96 kHz. This simulates 361.083 seconds in a single
thread, with Windows wall time 13.1095 seconds. Released binaries exclude QA
code; public platform packages are separately audited against current assets.

## 2.48 Raw MOD voltage and masked clock-offset history

Actual Rack callbacks reproduced four failures: Unipolar MOD grid and encoder
drags lost a dormant negative raw voltage on Undo; ALT MOD shifted other
stages from hidden raw negatives; and a two-pixel CLK offset edit lost a
stored offset hidden by a smaller divider. All four original probes pass
after their fixes. Track CV, MOD voltage and CLK offset share raw drag-history
selection. Every nonzero edit of those quantities is recorded from its stored
origin; tiny drags finish native capture, record the edit and then perform the
genuine click. MOD group offsets use each stage's effective origin and limits.
The previous Track CV range/click fixes remain covered in the same suite.

Expanded Pending performs 5,712 actual MOD drag/click cases. Core combinations
cover all eight lanes/stages/types, encoder/OLED/grid, ALT, both directions,
0.2/2/22-pixel movement and zero-delta events. All 38 assigned destinations are
also checked across types, both controllers, ALT and small/large movement.
Full raw state checks exclude unrelated writes and verify each edit/click
receipt in order. Additional MOD confirmation tests cover every lane, both
controllers and both scopes without an extra transition action. Another 1,536
actual CLK offset drags cover every division 1–256, both controllers, both
directions and small/large movement, checking raw Undo/Redo, field navigation
and disabled /1. Confirming offsets cover /1, /2, /4, /16 and /256.

The fixture pauses processing and isolates the bounded QA history, restoring
both before original Pending save/reopen. This is real application callback
coverage, not OS mouse/focus or physical hardware comparison. Sequencing,
output timing, parameter IDs/settings/musical formats remain unchanged at
787/33/11/522. Linux runs these checks in its existing Pending application group.

Windows expanded Pending and original save/reopen pass, alongside the existing
699 long-reset receipts, stationary-click probes and 48 held GATE/SLIDE
shortcuts. CtrlEditor, Chains, ScaleCopy, Timing, StagePair, StagePattern,
StageCv, Scales, Swing, Overrides, Workflows, SetupOptions, Outputs, Fidelity,
Polish, all 181 Display states, ChainAudio and Order pass. ChainAudio records
twelve real seconds at 48 kHz; these finite checks do not establish continuous
human sessions or physical sound equivalence.

Native integration and the stripped production host pass 22,624,609 frames,
339,369,135 output checks, 56,664 parameter edits, 145 state restores,
144 fresh instances, 12 initializes and 145 reciprocal links across both
public models at 44.1/48/96 kHz. This simulates 361.083 seconds in a single
thread, with Windows wall time 13.5792 seconds. Released binaries exclude QA
code; public platform packages are separately audited against current assets.

## 2.47 Small CV edits and effective ALT range

Actual Rack callbacks reproduced three failures: a two-pixel Unipolar grid
edit undid a dormant negative voltage to zero; a zero-delta movement silently
clamped that raw voltage; and a two-pixel encoder edit used the wrong raw Undo
origin before its genuine transition click. Existing Gate clicks already had
correct raw history and remain covered as passing controls.

Every nonzero CV edit now receives its raw-value receipt, including movement
below the existing three-pixel click threshold. Native cleanup suppresses the
intermediate Action for those tiny edits; after restoring and recording their
new values, the genuine click runs against the latest value. This preserves
edit-before-click ordering, exact raw Undo/Redo, menus, cursor release and
adjustment-tooltip cleanup. Zero-delta movement skips the native setter and
group postprocessing after the ownership check, preventing a displayed/clamped
value from overwriting dormant raw state.

Expanded Pending checks cover 256 actual grid drags and 1,024 actual encoder/
OLED drags: both tracks, all eight stages, both directions, 0.2/2/6-pixel
movement, all CV types and saved Bipolar forced to Unipolar by Velocity CV.
Additional zero-delta, ALT scope, preview/confirmation and upper-bound checks
verify exact full raw state and each individual history action. The independent
ALT expectation reproduced a further failure: group editing used the saved
Bipolar type instead of its effective Unipolar range. The group delta and each
stage now use the effective type; no sequencing or output-timing code changes.

The fixture pauses processing for deterministic raw receipts and isolates
Rack's bounded history before restoring the prior QA history. It runs through
the existing Pending group on Windows and Linux. These are real application
callbacks, not OS mouse/focus tests or comparison with physical hardware.
Parameter IDs/settings/musical formats remain 787/33/11/522.

Windows expanded Pending and original save/reopen pass, alongside the existing
699 long-reset receipts, stationary-click probes and 48 held GATE/SLIDE
shortcuts. CtrlEditor, Chains, ScaleCopy, Timing, StagePair, StagePattern,
StageCv, Scales, Swing, Overrides, Workflows, SetupOptions, Outputs, Fidelity,
Polish, all 181 Display states, ChainAudio and Order pass. ChainAudio records
twelve real seconds at 48 kHz; these finite checks do not establish continuous
human sessions or physical sound equivalence.

Native integration and the stripped production host pass 22,624,609 frames,
339,369,135 output checks, 56,664 parameter edits, 145 state restores,
144 fresh instances, 12 initializes and 145 reciprocal links across both
public models at 44.1/48/96 kHz. This simulates 361.083 seconds in a single
thread, with Windows wall time 13.6984 seconds. Released binaries exclude QA
code; public platform packages are separately audited against current assets.

## 2.46 Stationary-click history

Actual Rack callbacks reproduced false native drag history after an external
write during a stationary encoder field-navigation click and an optional gate
grid click. Rack's native Knob compares its mouse-down and release values
before dispatching its Action, even when the pointer never moved. Stationary
HardwareControl/ValueCell releases now finish capture with no bound quantity
and suppress that intermediate Action; after restoring the quantity, they
dispatch the genuine click exactly once. Field navigation creates no history;
toggles and chosen menu values record their own action from the latest value.

Expanded Pending callbacks check the actual encoder/OLED, gate and Slide grid;
temporary real controls/cells bind all fourteen direct destination quantities
on both tracks. Checks preserve full raw state/history, cursor/adjustment-tip
cleanup and quantity availability during the genuine Action. Slide/Skip and
Gate CV on both tracks plus every MOD gate retain exact one-action toggle
Undo/Redo after an external write. A choice cell opens its menu without history
and records the chosen value exactly. Two-pixel continuous control/grid drags
retain their native edit and Undo/Redo. The probe pauses processing for raw
receipts and removes its temporary widgets before original confirmation/save/
reopen. Linux runs the same probes in its existing Pending group (eighteen
application groups total). IDs/schema/musical formats stay 787/33/11/522.

This is actual callback coverage, not OS mouse/focus or hardware comparison.
The [official Rack 2.6.6 Knob source](https://raw.githubusercontent.com/VCVRack/Rack/v2.6.6/src/app/Knob.cpp)
defines the native history/Action order used here. No-movement is exact zero;
small nonzero movements retain the existing drag and click thresholds.

The broader Overrides suite exposed a legitimate stationary held-button edit:
GATE plus a stage shortcut. SLIDE has the same workflow. These parents now
explicitly mark intentional shortcut writes so release retains their normal
receipt. Expanded Pending checks all eight Gate Length, Gate Scale and Slide
shortcuts on both tracks (48 held cases) independently of the no-edit clicks.

Windows expanded Pending and original save/reopen pass. Its existing 699
long-reset receipts also pass alongside the stationary-click probes and
48 held GATE/SLIDE shortcuts. CtrlEditor, Chains, ScaleCopy, Timing, StagePair,
StagePattern, StageCv, Scales, Swing, Overrides, Workflows, SetupOptions,
Outputs, Fidelity, Polish, all 181 Display states, ChainAudio and Order pass.
These retain owned-value protection, original shortcut/clipboard history,
all chain edit positions and long EXIT Sync. ChainAudio records twelve real
seconds at 48 kHz; finite checks do not establish continuous human sessions.

Native integration and the stripped production host pass 22,624,609 frames,
339,369,135 output checks, 56,664 edits, 145 restores, 144 fresh instances,
12 initializes and 145 reciprocal links over 361.083 virtual seconds. Local
production wall time was 13.7158 seconds. Linux requires the expanded Pending
and its seventeen other application groups; all four packages require native
integration, stripped production runtime and byte/architecture/model audits.

## 2.45 Long-press reset history

Actual Rack callbacks reproduced two failures. A GATE Scale write during the
encoder hold was undone to the earlier mouse-down value. An A source changed
from its initial/default zero to another source during the hold, then reset
to zero, created no reset history because the old drag snapshot equalled the
reset result. The long action now captures its current scope and raw values
immediately before firing and records its grouped action then. Its release
finishes the native knob with no bound quantity, preventing a second action
based on Rack's earlier drag origin. Remaining movement is consumed because
the action may have opened a different page.

The existing Pending application suite includes reset-history callbacks for
track and MOD timing/order, paired PROB/ACCUM, Ratchets, ALT Slide/Skip, CV and
overrides, Gate/Slide/Swing/TYPE, AUX/CTRL/MOD routing, A/B, CLK/MX, UI/global/
MIDI setup and track menus. It checks current raw origins, immediate one-action
Undo/Redo, unrelated parameters, no-ops, 0.9/1.1-second boundaries, repeated
refresh, subsequent external writes, late movement/release and cursor cleanup.
Separate probes cover dormant negative Unipolar CV, changed track/stage/ALT
scope, an initially CTRL-owned gate with an unowned scale reset, and SETUP hold
opening the chain page. Linux runs these in its existing Pending group; the
number of Linux application groups remains eighteen.

The probe frame pauses processing for exact raw/history receipts and restores
the isolated QA history before the original confirmation/save/reopen fixture.
This tests application callbacks rather than OS mouse/focus events. Saved
IDs/schema/musical formats remain 787/33/11/522. Physical comparisons remain
unverified; this Undo behavior is a Rack desktop addition to the manual's
page-specific holds, not a claim about hardware Undo or external automation.

Windows Pending passes all 699 reset-family holds plus the focused conflict,
scope, dormant-CV, partial CTRL and page-opening probes; original confirmation
editing and save/reopen also pass. CtrlEditor, Chains, ScaleCopy, Timing,
StagePair, StagePattern, StageCv, Scales, Swing, Overrides, Workflows,
SetupOptions, Outputs, Fidelity (including one-action long EXIT Sync), Polish,
all 181 Display states and Order shortcuts pass. ChainAudio passes its real
twelve-second 48 kHz stereo/CV recording. ScaleCopy retains all 14,900 factory/
user clipboard combinations; Chains retains all insertion/delete positions
through 32 links and saved-chain preservation. These are callback checks and
finite recordings, not a physical comparison or continuous human playtest.

Native integration and the stripped production host pass 22,624,609 frames,
339,369,135 output checks, 56,664 edits, 145 restores, 144 fresh instances,
12 initializes and 145 reciprocal links over 361.083 virtual seconds. Local
production wall time was 13.4058 seconds. Linux requires the expanded Pending
and its seventeen other application groups; every platform requires native
integration and the production host. The expanded fixture initially expected
B to reset to Track 2 CV; that assertion was corrected to the manual's First
Pulse / Track 1 (packed source 14). Production B routing was already correct.

## 2.44 Live CTRL ownership

Actual Rack callbacks reproduced five failures: encoder and OLED movement
continued after direct CTRL takeover, their releases recorded controller writes
as drag history, and an optional gate grid likewise attributed the controller
write to native history. HardwareControl now observes ownership during movement,
refresh and release and latches cancellation until release. Its matching pending
confirmation is discarded. ValueCell also latches cancellation and finishes
Rack's cursor/knob state without native history. Applied controller values are
preserved; fresh gestures after unassignment restore ordinary editing/undo.
Starting already owned still permits short navigation and whole-page resets of
other unowned fields. Earlier movement in a cancelled gesture is not separately
committed; controller assignment actions retain their own history.

The extended CtrlEditor suite uses the visible encoder, OLED and actual gate
grid, then real HardwareControl/ValueCell callbacks bound to every one of the
fourteen direct destinations, both knobs and all track scopes. It covers takeover
before movement, before release, during refresh and after earlier live movement;
clearing while held, initially owned gestures, cursor/adjustment-tip cleanup,
fresh exact raw-state single-action undo/redo, short navigation and partial
long-reset preservation. Matching confirmation cancellation is checked even
when its base value did not change before the controller was cleared.
Linux runs the same extended CtrlEditor suite in its existing eighteen groups.
Manual printed146–153 specifies controller ownership; Clock Div In on p.150 is
the track's DIV field, not the global external-input divider. Saved IDs/schema/
musical formats stay 787/33/11/522.

Cursor cleanup uses the [official Rack 2.6.6 Knob implementation](https://raw.githubusercontent.com/VCVRack/Rack/v2.6.6/src/app/Knob.cpp):
ending the native gesture with no bound quantity releases capture and skips
its parameter history. This is actual callback coverage, not an OS mouse/focus
or physical-module equivalence check.

Windows extended CtrlEditor and its original routing/save/reopen pass. Raw
ownership/history receipts pause module processing for the probe frame and
emulate the controller's base write using its native direct-value mapping;
processing resumes before the original UI and rendering/storage checks. Offset
routes and direct assignments to the other track retain live editing/undo;
Tempo's ownership remains global. The probes are removed before saving.

Overrides exposed an asynchronous fixture mismatch: it expected a 75% gate
while assigning a noon CTRL that can legitimately write 50% on the processing
thread. Its knob position now maps to the expected 75%, preserving the strong
owned-value assertion rather than changing production behavior. Overrides,
Timing, Swing, Pending, Outputs, Pickup, Fidelity, Polish and all 181 Display
states pass. Controls also passes real twelve-second stereo/CV performance.
Native integration and the stripped production host pass 22,624,609 frames,
339,369,135 output checks, 56,664 edits, 145 restores, 144 fresh instances,
12 initializes and 145 reciprocal links over 361.083 virtual seconds; the local
host took 12.5019 seconds. Linux requires the extended CtrlEditor plus its
seventeen other application groups; all four packages require native and
production checks. Finite recordings and virtual runtime do not establish
long realtime sessions, human listening quality or physical hardware parity.

## 2.43 A/B output preview conflicts

Actual Rack callbacks reproduced three stale-output-preview failures: encoder
confirmation overwrote a newer applied source, standalone control refresh kept
the obsolete choice, and another drag movement continued from that choice.
Named output menu actions already cleared pending choices and required no fix.
The separate A/B preview now captures its applied raw source and checks it
before turning, confirming and refreshing controls/the module widget. An
obsolete confirmation is consumed; a later turn begins from the newest source.
Editing the other output retains a valid pending choice. IDs/schema/formats
remain 787/33/11/522; no output routing or signal algorithm changes are made.

The Output suite exercises all sixteen functions and all 38 source/target
choices on both A/B with Encoder Confirm off/on; mandatory confirmation,
encoder/display interaction, exact raw-state single-action undo/redo, external
edits during a drag and before confirmation/refresh, continued turns, unrelated
output edits, all named menu choices, EXIT, long/menu resets, hold thresholds,
clipped no-ops, sub-integer raw conflicts and original native save/reopen.
Linux adds mode Output to its seventeen existing application groups. The
firmware 1.6 routing workflow is referenced from printed pages115–119. These
callback checks do not establish hardware or OS mouse/focus equivalence.

The expanded Windows suite also exposed an external edit being attributed to
the preview's grouped drag-end history. A/B preview drags now finish Rack's
cursor/knob state without recording parameter changes, just like confirming
parameter previews; actual confirmation and long reset retain their own undo.
The original small unrelated-output probe was adapted to Rack's bounded
history deque after exhaustive edits filled it. Production behavior was not
changed for that fixture correction. Receipt checks in the expanded suite
require the previous action immediately before the new one, then verify the
entire raw parameter state through Undo/Redo.

Windows Output and its original save/reopen pass. OutputAudio and
OutputArrangement each pass real twelve-second 48 kHz signal-path tests:
Velocity spans 0–5 V and MOD8 clock pulses reach their cables. Pending,
CtrlEditor, SetupOptions, Workflows, Chains, Fidelity, Polish and all 181
Display states pass. Native integration and the stripped production host pass
22,624,609 frames, 339,369,135 output checks, 56,664 edits, 145 restores, 144 fresh
instances, 12 initializes and 145 reciprocal links over 361.083 virtual seconds;
the local host took 13.1857 seconds. Linux CI requires Output plus the seventeen
existing application groups; all four packages require native and production
checks. Finite recordings and virtual runtime do not establish long realtime
sessions, human listening quality or physical hardware parity.

## 2.42 Chain editing history

Actual Rack callbacks reproduced two history entries for new-link insertion,
Clear Chain and long-encoder deletion. Their existing direct ChainChange is
now retained alone; the shared navigation-end snapshot records only a real
drag, preserving grouped bank/repeat history. The manual's disabled-playback
and >1-second deletion workflow is unchanged (printed pages174/177).

The existing Chains suite is extended with all insertion/deletion positions
through 32 links, all clear sizes, all eight saved-chain preservation, loaded
indicator restoration, hold threshold/guards, both encoder/display bank/repeat
drags, all banks and 64 repeat values, clipped no-ops, reversal, browsing and
exact full-state undo/redo. History receipt checks handle Rack's bounded deque
without relying on a forever-increasing index. Original Quick Chain, storage,
playback, following and save/reopen checks remain. Linux now runs mode Chain
alongside its sixteen existing application suites. IDs/schema/formats remain
787/33/11/522; these tests do not establish hardware or OS mouse/focus parity.

Windows passes the expanded Chains suite and its original save/reopen workflow.
An initial exhaustive fixture assumed inactive insertion-line holds had no
short action and failed to restore its cursor/bank/queued runtime before the
old musical fixture. Those test assumptions were corrected: inactive long
holds retain the existing short click; queued fixture commands are discarded
under bypass and original state restored before normal processing resumes.
No production behavior was changed for those fixture corrections.

ChainAudio and PresetAudio both pass real twelve-second signal-path checks.
Pending, SetupOptions, Workflows, Fidelity, Polish and all 181 Display states
pass on Windows. Native integration and the stripped production host pass
22,624,609 frames, 339,369,135 output checks, 56,664 edits, 145 restores, 144 fresh
instances, 12 initializes and 145 reciprocal links over 361.083 virtual seconds.
The local host took 14.6259 seconds. Linux CI requires the extended Chain suite
and the sixteen existing application suites; all packages require native and
production checks. Virtual runtime and finite recordings do not establish
long realtime sessions, human listening quality or hardware parity.

## 2.41 Pending-edit conflicts

The dedicated actual-Rack callback suite first reproduced obsolete single and
bulk confirmation, false undo at confirming-drag release, and stale control
readouts. Drafts now capture raw values separately from effective CV values.
Validation checks every captured parameter, CTRL ownership, screen binding
and ALT scope before confirmation, continued drag and UI refresh. Confirming
drag release finishes native cursor/knob state without attributing external
writes to that preview. Matching native context menus cancel previews before
Initialize, choices or numeric entry, including no-op Initialize.

Parameter IDs and saved formats remain 787/schema33/format11/522. A/B source
browsing uses its existing separate navigation preview and is outside this
new parameter-draft test. These are callback tests, not OS mouse/focus tests
or a physical hardware comparison.

Windows passes all original reproductions plus external changes to every stage
of CV/Ratchet/PROB/ACCUM on both tracks, preview release without history/cursor
capture, continued drag from the latest value, changed ALT scope and new CTRL
ownership. Fresh bulk confirmation retains one exact undo/redo, including
dormant negative CV values. Rack's native no-op Initialize, Root choices and
numeric tempo field callbacks pass. Save/reopen keeps applied values and drops
the draft. Raw comparisons bypass audio processing only for their test frame;
normal processing is restored before rendering and saving.

Thirteen neighboring Windows suites pass: SetupOptions, Workflows, Fidelity,
CtrlEditor, StagePair, StagePattern, StageCv, Overrides, Modulation, Scales,
Timing, Polish and all 181 Display states. The Windows harness also corrects
three missing environment-provider prefixes that retained flags between
successive suites. Native integration and the stripped production host pass:
22,624,609 frames, 339,369,135 output checks, 56,664 parameter edits, 145 restores,
144 fresh instances, 12 initializes and 145 reciprocal links over 361.083
virtual seconds. The local host took 12.7637 seconds; virtual duration is not
continuous realtime audio or a human listening test. Linux CI now requires
the same Pending callbacks alongside the fifteen existing application suites.

## 2.40 User-scale editor guard

Printed page 67 requires SCALE to be selected before entering the user note
editor with a long encoder hold. ROOT-selected holds no longer enter it.
Already-open user editors can exit from notes or Root. A factory scale selected
by external automation automatically closes editing. The explicit desktop note-editor
choice still opens from Root and discards a pending Root preview. Inactive
holds retain the shared control's short-click action on release.

The Linux application harness now also runs the scale editor/lock/shortcut/
note-undo/save/reopen suite (mode Scale), alongside the existing Windows Scales
suite. Parameter IDs and formats stay 787/schema33/format11/522.

Windows Rack callbacks pass ROOT-selected entry guards for all 149 choices,
all 100 user slots' selected-SCALE entry, hold threshold, note/Root exit with
unchanged parameters/history, and all 49 factory choices' unavailable editor.
The existing automatic factory-selection exit passes without value/history
changes. Explicit desktop entry from Root cancels its pending preview. The
original scale browsing/locking, note undo, final-note guard, root transpose,
shortcuts, custom-scale pitch gestures and save/reopen checks still pass.
ScaleCopy, SetupOptions, Workflows and all 181 display states also pass on
Windows. Native library integration passes; platform CI requires the full
scale suite on Linux and the native/production checks on all builds.

## 2.39 CTRL assignment workflow

Printed page 148 requires encoder confirmation for destination and target,
and long-encoder clear to No assignment. The CTRL editor now follows those
steps even when optional Enc Confirm is Off. The black assignment buttons
also preview; the physical white performance knobs remain live. Explicit
desktop choices apply directly and discard an obsolete pending route draft.

The long clear preserves target selection and physical knob position, matching
the existing desktop Clear assignment scope. Only the destination becomes
None; the manual does not specify the target/position values after clear.
Native parameter IDs, settings schema and musical format stay 787/33/11/522.
Physical hardware comparison remains open. See [CTRL controls](CTRL-KNOBS.md).

Dedicated Windows Rack callbacks pass both knobs, every destination and target,
both Enc Confirm states, encoder/display previews, assignment-button drag,
confirmation versus field switching, EXIT cancellation, threshold/no-op long
clear from either field, stale draft cancellation from named desktop choices,
ownership transfer/release, live knob dragging and exact undo/redo. Confirmed
routes and musical preset storage survive save/reopen; drafts remain transient.
The suite bypasses module processing during raw undo comparisons to isolate
them from asynchronous direct CTRL writes, then restores processing for the
rendered scenes and patch save. Native integration separately passes routing
signals and persistence. The native console host also builds on Windows through
the shared Makefile, with console entry-point and runtime flags corrected.
Neighboring Controls audio (12 seconds at 48 kHz), SetupOptions, Pickup,
Workflows, Fidelity, Polish and all 181 display states pass on Windows.
The older display assertion now requires preview followed by confirmation.

## 2.38 Slide/Skip patterns and Ratchet gestures

Printed pages 87/88/94 specify ALT pattern rotation, inversion and reset, and
selected/all Ratchet reset with bulk count editing. Dedicated actual Windows
Rack callbacks pass every eight-bit Slide/Skip pattern on both tracks, forward/
backward/multi-wrap/full-wrap rotations, inversion twice, resets, threshold,
drag reversal, no-op and exact undo/redo. Every Ratchet stage on both tracks
passes selected/all default reset, relative count clipping/reversal and
eight-count pending confirmation. Tests also pass actual EDIT/ALT navigation,
display rotation, native menus, preset storage and save/reopen. Native engine/
library integration passes. Linux package CI runs the same gesture/render/
reopen suite; platform native and stripped production checks remain required.
ScaleCopy, Scales, Workflows, StagePair, Timing, Order, Storage, Group,
Fidelity, Polish and all 181 display states pass on Windows.

The display marks bulk editing and fills every edited Ratchet/Probability/
Accumulation bar. IDs and formats stay 787 parameters, schema33 and
format11/522. Pattern operations apply directly with Enc Confirm enabled,
matching the gate-pattern editor; Ratchet bulk amounts retain confirmation.
The rotation direction convention and Ratchet endpoint reversal remain
documented hardware-comparison details. See [Stage gestures](STAGE-PATTERNS.md).

## 2.37 SCALE copy/paste

Printed page 165 defines short ALT + SCALE copy and long ALT + SCALE paste to
a user slot. Dedicated actual Windows Rack callbacks pass all 149 displayed
source definitions against all 100 user destinations: 14,900 paired pastes,
with an immutable mask/root snapshot, exact one-action undo/redo and unrelated
parameters intact. Tests also cover threshold/no-op, empty/factory rejection,
latched/held panel ALT, keyboard-modified Button events, direct menus and
disabled states, pending-preview cancellation, Initialize, and save/reopen.
The scale clipboard clears on Initialize/reopen and remains separate from
stage/lane copy. Native engine/library integration passes. Linux package CI
runs the same copy/render/reopen suite alongside the existing platform checks.
Scales, Workflows, Timing, StagePair, Order, Storage, Group, Fidelity, Polish
and all 181 display states pass on Windows.

Saved IDs and formats stay 787 parameters, schema33 and format11/522. The
clipboard copies notes and root; page 67 stores both in user scales, while
page 165 does not explicitly enumerate the fields copied. Root inclusion is
documented for hardware comparison. Keyboard checks inject Rack's real Button
modifier events; OS keyboard/focus/menu behavior remains a human playtest.
See [Scale copying](SCALES.md#copy-and-paste-a-scale).

## 2.36 paired PROB/ACCUM gestures

Printed pages 99/101 specify paired long-encoder resets, ALT editing of all
eight amounts, and uniform ALT targets/triggers. Actual Windows Rack callback
checks pass on every stage of both tracks: selected/all reset thresholds,
every secondary choice, relative amount editing with saturation and reversal,
exact one-action undo/redo, context menus, eight-amount pending confirmation,
and patch/preset save/reopen. Reopening retains the selected screen and values
and clears transient ALT. Timing, Group, Order, Workflows, Scales, Slides,
Stage CV, Fidelity, Polish and all 181 display states pass. Native engine/library
integration passes. Linux package CI runs the same paired gesture/render/reopen
suite; all platform native and stripped production checks remain required.

No saved fields changed: 787 parameters, schema33 and format11/522 stay fixed.
ALT short press advances the selected target/trigger before applying it to all
stages, matching the existing CV gesture. The manual does not settle that order
or endpoint reversal details; both remain hardware-comparison items. These
checks use real Rack callbacks and rendering, rather than OS mouse/focus events.
See [PROB/ACCUM gestures](STAGE-PAIRS.md).

## 2.35 timing panel workflows

The requested firmware 1.6 manual, printed pages 76/78, specifies LEN stage/pulse
shortcuts, DIV clock/pulse-count shortcuts and whole-screen long encoder reset.
The new callbacks preserve the selected field. Dedicated actual Windows Rack
checks cover every stage/field on both note tracks and eight MOD lanes, threshold
and all-field resets, native numeric entry, direct menus, pending cancellation,
multi-target one-action undo/redo, CTRL exclusion, unrelated group mute and
save/reopen. Neighboring Group, Order, Workflows, Scales, Slides, Stage CV,
Fidelity, Polish and 181-state Display checks pass. Native engine/library
integration also passes. Platform CI retains
its own evidence; these are callback checks rather than OS mouse/focus playtests.

No saved fields changed: 787 parameters, schema33 and format11/522 remain fixed.
The selected shortcut indicator is a desktop interpretation; the manual does
not specify its combined LED priority. Physical timing/GUI comparisons remain
outstanding. See [timing controls](TIMING-SHORTCUTS.md).


## Independent MOD group mute — 2.34

Group mute now preserves all eight individual lane choices. ID786 appends after
the frozen 786-parameter prefix; settings33 and musical format11/522 include it.
Legacy settings29–32 and banks1–10 default the missing group Off and preserve
their stored individual masks; selective9–10 upgrade only listed slots.
Native Rack checks cover all256 masks in three CV types, actual A/B voltages,
internal routes, continuing phase, synchronized commands, independent edits,
preset history/Init and strict legacy migration. Actual Rack callbacks cover
held gestures, desktop menus/history, preset Save/Load, old selective import
and native reopen. See [MOD group mute](MOD-GROUP-MUTE.md) for the documented
separate-state/indicator interpretation and remaining hardware comparison.


## Musical preset storage — 2.33.0

Scale Lock and Loopy pulse/target settings now follow the manual's p.169 preset
column. Format10 appends three values after all 518 frozen format9 positions;
NUM786 and schema32 stay fixed. Native checks cover all locks/pulse counts,
load policies, all old full formats1–9, old/new selective imports with unrelated
slot preservation, queued boundaries, exact receipts, Init, chain and MIDI PC,
and native pitch output. Actual Rack tests cover save/load, undo/redo, custom
Init, legacy import and safe reopen. Old snapshots gain Unlocked/Auto/Current
Track; new banks require 2.33. See [Preset storage](PRESET-STORAGE.md).

## ORDER shortcuts — 2.32.0

The manual's p.74 held-ORDER shortcuts now save/recall order and direction as a
pair across all ten targets. Native tests cover 3,360 combinations, global
preset exclusion, CTRL ownership and schema29–31/770 versus schema32/786 backup
validation. Actual Rack tests cover held and desktop gestures, paired long
reset, target changes within a gesture, exact undo/redo and Rack save/reopen.
Controller-owned fields are excluded from the gesture receipt. Four new ORDER
screens extend display coverage to 181. The [ORDER guide](ORDER-SHORTCUTS.md)
records software default pairs and storage boundaries. Scale Lock preset recall
was corrected in 2.33 (manual p.169).

## Production runtime — 2.31.0

The actual stripped download now runs the same six-virtual-minute stress host
on Windows, Linux x64, Intel Mac and Apple Silicon. It checks all fifteen
outputs after every frame, repeated bounds/interior parameter edits, state
restores and fresh instances, Initialize, three sample rates and reciprocal/
stale/detached MX links. This complements actual Rack UI/audio-path tests; it
does not establish hardware behavior or multithreaded safety. The [runtime
guide](RUNTIME-VALIDATION.md) records counts, assumptions and reproduction.

## Maintenance — 2.30.0

Native host tests check every setting against its plugin default on Factory
Reset, all 64 empty slots on Clear, preservation of presets/working and saved
chain links/local devices/geometry, cancellation of pending loads, stopped
transport, held Run inhibition and exact before/after undo receipts. Invalid
actions leave the entire module unchanged. Old schema29 and30 full/settings
backups validate and default newly added cursors. Confirmation choices never
serialize; even an imported confirmation page reopens at the menu.

Actual Rack callbacks exercise Version / Factory navigation, hover, drag/clamp,
Cancel and EXIT with no history change, confirmed reset/clear, one-action undo/
redo and cable/identity preservation. A third actual Rack launch opens a patch
saved with Apply selected and verifies that it is disarmed, presets remain and
a default click cancels. Five new OLED cases extend coverage to 177 states.
The existing Backup/Fidelity/Polish/preset/Setup regressions remain applicable;
Linux runs the same maintenance and safe-reopen cases within the Backup group.

No parameter IDs or musical snapshot format changes. Schema31 adds a menu
cursor/page and retains schema29/30 backup compatibility. These are plugin
defaults; no claim is made about hardware output calibration, physical slider
positions or undocumented chain maintenance behavior. See [Maintenance](MAINTENANCE.md).

## Selective backups — 2.29.0

Native host tests cover each of the eight banks, sparse Filled and empty Filled,
explicit bank empty-slot clearing, absolute slot addressing, preservation of
every unrelated slot, chains, live settings, local devices, pickup and editor
state. Actual module receipts restore the exact before/after JSON on undo/redo.
Imports remain stopped under an already high external Run gate. Invalid slot
IDs (including oversized integers), duplicate IDs, foreign bank slots, wrong
versions/scopes/sizes and every out-of-range snapshot field reject before any
module mutation. UTF-8 filenames and signed/dormant float32 values roundtrip.
Old schema29 full/settings backups remain accepted and default the new cursor.

Actual Rack widget callbacks browse/clamp All/Filled/Bank1–8, expose context menus
and hover help, return to the parent with EXIT, export/import selective files,
create one history action and undo/redo exact state. A saved patch reopens with
the selected submenu and restored/untouched presets. Five additional OLED cases
bring coverage to 172 states. Existing Fidelity, Polish/Bloom and preset tests
remain regression checks. This coverage does not claim native OS file-dialog
automation or physical hardware SysEx compatibility.

The only new saved state is a submenu page/cursor in schema30. All 770 parameter
IDs and the format9/518-value musical snapshots stay fixed. Format1 full backups
retain replace-all behavior; format2 merges Filled or replaces one complete bank.
The manual specifies export selection but not SysEx receive merge behavior;
these explicit selective import semantics belong to the desktop plugin.

## Native platform packages — 2.28.0

The 25 portable suites pass. Ubuntu 22.04 x64, macOS 15 Intel and macOS 15
Apple Silicon run the complete native engine, persistence, file, MIDI and MX
host suite. A separate host registers and loads the stripped production binary,
constructs both public models, verifies 770 main parameters/seven outputs and
eight MX outputs, generates finite gates over 48,000 samples and restores JSON
state. Both exit status and an explicit completion marker are required; logging
pipelines cannot hide a host error. Each package audit checks native architecture,
the exact included source/document bytes, two public model slugs and absent QA
environment/recorder strings. SDK downloads use fixed versions and hashes.

Linux also runs actual Rack Free 2.6.6 under Xvfb/Mesa llvmpipe. Fidelity and
Backup callbacks include save/reopen; Polish checks gestures, readouts, removal,
graphics context recreation, OLED zoom and light Bloom. Display checks 167 OLED
cases and captures the real live scene. The ordinary GL front-buffer screenshots
were empty under Xvfb; QA now renders the live Rack scene to its own framebuffer,
drawing cache children directly and retaining normal halo behavior. Image checks
reject blank scenes, require blue OLED pixels and compare Bloom in the pitch area.
This is renderer/callback coverage; it does not claim native OS mouse/focus testing.

MX percussion and MIDI-CV voices each record 12 seconds at 48 kHz through actual
Rack cables. MX checks all eight 0/5V stage outputs and mutual exclusion. The
Linux probe recorded RMS .201064/.202382 V with peaks 1.32221/1.13008 V for MX,
and RMS .430394/.469804 V with peaks 1.59957/1.29377 V for MIDI. Small start/phase
differences between host runs are expected; finite samples, useful level and low
DC are checked independently. These are audio-path measurements, not a hardware
device-driver or subjective listening comparison.

Windows native integration, Fidelity/save-reopen, Polish/zoom/Bloom and both
12-second MX/MIDI audio regressions pass after the platform work. The prior
2.27 full panel/audio coverage remains
applicable: production behavior changes only by renaming the C++ MOD stage-offset
identifier to avoid a Linux system macro; its numeric ID stays fixed. Windows
uses the established SDK 2.3.0; the three new builds use SDK 2.6.6. All run the
same Rack 2 ABI, 770 parameter IDs, schema29 and format9/518-value musical banks.

Mac GUI/audio playtests, older OS versions, Rack Pro, physical hardware timing
and exact material matching remain unverified. See [Platform packages](PLATFORMS.md)
for installation, architecture selection and precise environments. Native logs,
audits, scene images, wave recordings and save fixtures are retained as workflow
artifacts; release checksums cover the publicly downloadable packages.

## Panel and display refinement — 2.27.0

Twenty-five portable suites and native Rack integration pass. Native tests check
MOD units/numeric conversion, octave/scale truncation, root limits, dormant
negative CV, effective velocity type and actual running/stopped gate MIDI
CC0/127 bytes. A ten-control PanelEdit preserves the running clock tick, reset
serial, stage and pulse position.

Real Rack widget callbacks pass held pending popups, leaving/releasing a drag,
confirmation/cancel and one undo; submenu back and tuner shutdown; all ten
Sync targets with one undo/redo and an unchanged-action no-op; MOD editor units;
expanded/compact MX adjacency, no overlap and reciprocal engine links; save/reopen.
All 22 prior panel modes pass. The 167 OLED captures include ten new unit,
preview, ALT, gate CC and Sync states. Enlarged bitmap and optional-editor
screenshots were visually inspected, including a maximum +400.0 BPM field.
Old routing tests now enter two destination units; tuner EXIT expects its parent.

Both real 12-second 48 kHz audio paths pass. MX percussion has RMS
.201048/.202356 V, peaks 1.32221/1.13008 V and DC .000502745/.000501498 V;
all eight 0/5V stage cables have six edges and are mutually exclusive. MIDI-CV
voices have RMS .426562/.469804 V, peaks 1.59957/1.29377 V and negligible DC.
These validate the Rack paths, not subjective or physical hardware equivalence.

No parameter IDs, schema29 or format9/518-value musical banks change. MOD
formatting reflects this engine's gains where the manual omits numeric gains;
numeric entry uses the displayed units. Virtual Jump controls move directly;
Sync releases recalled Thru pitch/CTRL values. Native OS mouse/focus, exact
material matching and physical comparisons remain tester checks. See
[Panel refinement](UI-FIDELITY.md).

## Portable backups — 2.26.0

All 25 portable suites pass. Native Rack engine/file tests verify three scopes,
all 64 preset slots, saved chains, physical/sounding pickup, custom scales,
MIDI/MX assignments, identity/geometry/device preservation and stopped restore
with Run on Boot On. A held external Run gate cannot restart until a new gate
cycle or explicit RUN. Old/new module receipts restore the correct RUN state.
Signed CV and dormant clock offsets survive serialization regardless of the
previous editing mode; malformed 64-bit parameter IDs cannot wrap to a control.

Future backup/settings/preset/chain versions, missing/foreign fields, invalid
pickup/chain slots, duplicate IDs, nonnumeric values and truncated banks reject
before mutation. Each of the 770 parameter bounds is checked. File tests cover
Unicode paths, float32 roundtrip, atomic overwrite, a locked destination
preserving its original file, duplicate keys, malformed/NaN JSON and the 4 MiB
limit. File work runs on the UI thread through Rack's locking serialization API.

Actual Rack widget tests pass MIDI/backup navigation, vertical dragging, one
import undo action, undo/redo of running state and bank, cable/device preservation,
invalid imports without state/history changes and patch save/reopen. All 21
prior panel modes pass. The 157 OLED cases include six new menu/status states;
their enlarged bitmap captures were visually inspected. Native OS file chooser
mouse/focus interaction remains a manual tester check.

Actual MX bass/lead/percussion playback still observes all eight 0/5V stage
cables, six edges each and at most one stage high. The 12-second 48 kHz recording
has RMS .201068/.202382 V, peaks 1.32221/1.13008 V and DC .000512853/.000511606 V.
This validates the Rack path; hardware/subjective equivalence is unverified.
[Backups](BACKUPS.md) documents the software JSON format and scope differences.


## MX expander — 2.25.0

Twenty-five portable suites pass. MxTests adds 86,705 assertions for all seven
modes, divider/offset phase, multiplied-clock rates at 44.1/48/96 kHz, missing
clock behavior, probability statistics, mute/stage selection and gates that
hold through Rest/zero note probability. Native Rack tests exercise actual
double-buffer/output signals, track swap, stopped Loopy Stage Player, MOD mute,
targeted reset phase preservation, exact 2 ms output delay plus one-sample bus
latency, malformed assignment bounds, detach and schema28 persistence.

The MX widget test passes mandatory mode/assignment confirmation with Enc Confirm
Off, display/encoder dragging, bounds, undo/redo, grouped long defaults, EXIT
cancellation and actual save/reopen. The 20 prior UI regression modes pass;
151 OLED cases include eleven MX states/previews. Nine enlarged MX bitmap
captures and the actual Rack percussion patch screenshot were visually inspected.

A 12-second actual Rack bass/lead/percussion recording at 48 kHz observes each
of all eight MX stage outputs through a cable (six rising edges per output),
with 0/5V values and at most one stage high. RMS is .201091/.202401 V; peaks are
1.32221/1.13008 V; DC is .000503372/.000502125 V. Kick, noise snare and hats use
the stage gates. Hardware timing, external device interaction and subjective
listening remain unverified. [MX](MX.md) documents assumptions where the manual
does not specify trigger widths, correlation or external-clock prediction.


## MIDI — 2.24.0

Twenty-four portable suites pass. MidiTests adds 1,652,451 assertions for
all channels/notes/velocities, note-off/legato order, CC ranges and clock rates.
Native Rack tests record actual MIDI bytes and timestamps for input/output
clock/transport, program-load delay, root, velocity/CV type, eight ratchets,
Hold, all slide types, Always amount, live CC/channel reassignment, track swap,
disconnect/removal, reset clock phase and schema/format migration. Fixed
audio-owned preset copies avoid taking the preset mutex on the audio thread.

Widget callbacks test MIDI menu navigation, vertical value dragging, native
undo and actual save/reopen; 140 OLED cases include the MIDI screens and CC
readout. An actual Rack Loopback16 → two Core MIDI-CV → two synth voice patch
records 12 seconds at 48 kHz: RMS .426539/.469804 V, peak 1.59845/1.29377 V.
Both voices receive pitch/gate exclusively through MIDI. This verifies the
Rack path; it does not establish hardware/DAW equivalence. See [MIDI](MIDI.md)
for explicit mappings, the clock contradiction and known interpretations.

## Preset pickup — 2.23.0

Twenty-three portable suites pass. PickupTests adds 599,116 assertions across
target/physical positions, both directions, exact/overshoot takeover, subsequent
recall and Off/Jump transitions. Native Rack tests exercise physical/effective
pitch and CTRL values, effective bank saving, exact recall undo state, chains,
schema migration and patch persistence. Actual widget callbacks cover partial
drag, pickup, undo/redo, explicit takeover, effective save and native reopen.
The display suite captures 126 states, including pickup dots and expanded
recall lists. A real two-voice patch also exercises persisted pickup targets.
See [Preset pickup](PRESET-PICKUP.md) for desktop behavior and interpretation.

## UI setup and confirmation — 2.22.0

Twenty-two portable suites pass. SetupTimingTests adds 1,030,604 assertions for
all 256 input divisions, 0–21 ms delay at four sample rates, and selected resets
across every play order, both accumulator modes and every skip position.
Native Rack integration checks the seven physical output ports at three rates,
24 ppq conversion, Run on Boot and schema migration. Widget tests cover preview,
confirmation, cancellation, grouped MOD edits, undo/redo, BPM shortcuts and actual
save/reopen. The display suite captures 122 OLED states. Prior workflow/clock/
track/stage callbacks and real two-voice playback are regression checks.
See [UI setup](UI-SETUP.md) for defaults and software-specific interpretations.

## Editing workflows — 2.21.0

Twenty-one portable suites pass. WorkflowTests adds 3,080,107 checks for
discrete/continuous bounds, unbiased discrete endpoints, the small-option
20% morph rate, large-range step limits and logical gate CV. Rack integration
checks initialized/custom/empty Init presets, recall policies, undo receipts,
format 8 persistence and schema migration. Native callbacks check multiple
stage pastes, toggle suppression, track screen/menu and whole MOD copies,
ALT baseline restore, paired AUX long clear, Init undo/redo, save/reopen and 113 OLED states.
See [Editing workflows](EDITING-WORKFLOWS.md) for boundary interpretations.

## Clock/reset configuration — 2.20.0

Twenty portable suites pass. ClockConfigTests adds 1,371,032 assertions for
clock-only operation, three sample rates, every output division/offset endpoint,
protected traversal across all 21 orders, forced reset, and fixed Bottom Pitch
for all 49 factory scales, 12 roots and four octave ranges. Native Rack checks
exercise Reset/Run/Run-C, explicit clock sources, actual tuner jacks, protected
tracks and format 7→8 migration. Native callbacks, undo, held TRK, save/reopen
and 108 OLED states are checked alongside the existing regression suites.
See [Clock/reset](CLOCK-RESET.md) for defaults and hardware interpretation.

## Complete AUX/MOD routing — 2.19.0

The nineteen portable suites pass. ExtendedRoutingTests adds 3,287,240 checks
against independently programmed endpoints, all 21 orders, three slide types,
three-rate tempo timing, targeted reset, hands-free Loopy and Gate Length Hold.
Native Rack tests check actual CLK and pitch ports, live BPM, all 20 new adapter
IDs, Global fields, On/Off entry and unchanged 719 parameters/format 7 persistence.
Widget/save-reopen, 99 OLED states and Bloom/zoom checks pass. The real four-scene
seven-link recording exercises all 20 added routes and preserves bass/percussion
connections; exact waveform measurements are saved with the release checkpoint.
See [Complete routing](COMPLETE-ROUTING.md) for gain and collision interpretations.

## Accumulator routing — 2.18.0

The new portable suite passes 2,230,784 checks: exhaustive Wrap reversal, literal
manual live Invert, Pendulum boundary reversal, gate thresholds/inversion,
Stage/Pulse/Ratchet pauses across all orders with skips, and brief targeted
resets through divided clocks at 44.1/48/96 kHz. Native Rack tests check actual
V/oct and gate ports, independent CTRL freezing, held-trigger suppression,
parameter metadata, patch persistence and internal preset banks.
Native widget/save-reopen checks, 93 OLED states and Bloom/zoom inspection pass.
The four-scene/seven-link demo exercises all six gates, both CTRL assignments,
velocity bass and independent percussion through real cables. Its 12-second
48 kHz stereo recording has RMS .223333/.262357 V, peaks 1.04887/1.14602 V and
DC .000288/.000291 V. This verifies signals and transitions, not a listening
judgment or physical-module fidelity.
The comparison interpretations are in [Accumulator routing](ACCUMULATOR-ROUTING.md).

## Track options — 2.17.0

Seventeen portable suites pass. TrackOptionsTests contributes 3,761,450 checks:
uniform pulse/gate sources match equivalently programmed switches across all
21 orders, skipped stages, accumulation, fixed lengths, stretching and ratchets;
shared switches remain unchanged. Rest Pitch Hold/Update, silent rests, all slide
types, CTRL pulse offsets and every transpose value −60…+60 are covered.
Rack host checks independent actual gates/traversal, native values, format 7
persistence and format6 migration. Callback tests cover ALT → TRK, browsing,
upward dragging, binary toggles, numeric endpoints, one-action long reset and
whole-track init/undo, then save/reopen with cursor and bank preserved.
The display suite adds ten menu states. Real stereo playback checks all four
source/Rest Pitch scenes across seven chain links, including velocity and MOD
percussion cables. Twelve-second stereo RMS is .227002/.267341 V, peaks
1.11651/1.2889 V and DC .000239/.000215 V. Hardware and native OS input comparison remain separate.
See [Track options](TRACK-OPTIONS.md) for precedence and remaining configuration.

## A/B output routing — 2.16.0

All sixteen portable suites pass. OutputRoutingTests has 178,586 checks for all
38 stable source/target IDs, First/Last across 21 orders and fixed lengths 1–64,
skips/all-skipped tracks, probability, manual reset, Loopy, raw/ranged sliders,
velocity offsets, output swapping and all eight MOD clocks. Trigger timing is
checked at 44.1/48/96 kHz. Rack-library checks verify every actual A/B voltage,
factory defaults, CTRL ownership, format6 banks, format5 migration and persistence.

Actual widget callbacks verify pending versus applied choices, confirmation,
track assignment, EXIT cancellation, menus, one-action long reset and undo,
native Rack save/reopen and discarded pending choices. The display suite adds
all sixteen function types. The public demonstration records actual stereo
audio plus velocity/MOD-clock cables. In a fixed 120 BPM first-scene timing
check, A spans 0–5 V and B produces 32 /3 triggers of 1 ms over twelve seconds.
Stereo RMS is .212932/.218993 V, peaks 1.04887/1.106 V, DC approximately
−.000037 V. The distributed patch retains its four-scene/seven-link arrangement.

These establish documented implementation behavior, not physical firmware
equivalence or native OS mouse/focus behavior. See [Output routing](OUTPUT-ROUTING.md)
for timing, reset, Loopy and assignment interpretations.

## Clock output — 2.15.0

The fifteen portable suites pass. ClockOutputTests adds 33,784,625 checks:
all 256 divisors and every legal offset, trigger duration at 44.1/48/96 kHz,
internal/external master clocks, track timing independence, reset, pause and
live division changes. The Rack host verifies the actual CLK output, numeric
entry, global settings excluded from presets, format5 unchanged at 478 entries,
schema18 migration and patch persistence.

Rack widget tests cover ALT → CLK, encoder/display upward drags, offset limits,
paired long reset with exactly one history action, /1's zero range, menus and
native save/reopen. The twelve-second real patch recording observes 24 `/4`
offset2 triggers at 0.5-second intervals through a cable, with 1 ms widths.
Stereo RMS is .430438/.469797 V; peaks 1.59849/1.29476 V; DC approximately
−.000045/−.000106 V. Trigger phase during live edits and transport, dormant
offset restoration, and pulse width are documented implementation choices.
Physical hardware has not been used for comparison.

## Stage overrides — 2.10.0

Pitch and gate overrides follow manual pp.89–92; track Gate Scale and Hold
follow pp.82–84. Ten portable suites include the four p83 waveform rows,
independent track notes, scale changes, root/transposition, pre/post modulation,
slider restoration, stage Hold, scaled Hold, mute, skip and reset. Rest/Hold
overrides are compared with an identical underlying pattern for all three
accumulator triggers; override articulation cannot change accumulator events.

Rack integration covers all 32 override parameters and both Gate Scale values,
format 1/2 bank migration, schema 12 patch migration, selective recall, undo
receipts, native note/special-state entry, and same-edge chain recall outputs.
Existing parameter IDs, port IDs and snapshot preference exclusions remain
unchanged. The appended parameters are 431–464; schema 13 / preset format 3.

`scripts/test-ui.ps1 -Overrides` runs production widget callbacks in an isolated
Rack profile: single/all-stage Sync, held encoder/OLED edits, all-stage undo
and redo, long clear/reset, gate field switching, held gate shortcuts,
CTRL ownership, stage colors, numeric readouts and context removal. It saves
a native Rack file and launches Rack again to verify patch/preset persistence.
`-Display` renders 41 OLED cases, including both override pages and Gate Scale.
`-Polish` retains drag popup, Bloom, glyph descender and texture lifecycle checks.
`-OverrideAudio` loads the public demonstration, compresses its chain to eight
base pulses per link, checks all seven links/loop and records the stereo output.

These checks establish our implementation's documented behavior and regressions.
They do not establish exact firmware matching or native OS mouse/focus behavior.
Chromatic-anchor storage across scale changes, Hold combined with Gate Scale,
physical Hold/Rest switch precedence, and silent Rest ratchet accumulation
especially benefit from a hardware owner's comparison. See the
[operation and demo guide](STAGE-OVERRIDES.md).

## Swing — 2.9.0

The ninth portable suite has 49,506 assertions. At 44.1/48/96 kHz it checks
every integer amount from 50–78%, internal/external clocking, and /1, /3, /7
track divisions. Actual gate onsets and track transitions follow the expected
long/short pair, with no cumulative drift; global CLK and MOD traversal stay
straight. Additional cases cover skipped stages, odd fixed pulse lengths,
ratchet modes and accumulation, Tempo slide, pending-pulse reset/stop/pause,
stalled external input, sudden acceleration, live CV changes and opposing
AUX/MOD offsets. Timing tolerance is two samples; each pair retains its length.

Rack-library tests cover both Swing parameters, CTRL range/assignment/ownership,
new bank persistence, format-1 migration to straight Swing, recall/undo, and
same-edge chain loading. `scripts/test-ui.ps1 -Swing` verifies the real button,
encoder, display and shortcut callbacks, context choices, one-gesture undo,
long reset, ownership locking and optional grid binding. A second Rack process
opens its saved file and confirms the page, values and bank entries.

Preset/chain UI and native-file reopen checks, the default widget suite,
`-Polish`, and all 38 `-Display` states remain regression checks. Actual OLED
captures are inspected against the manual's Swing page, including the right
aligned large percentage. `-SwingAudio` compresses the demo to eight global
pulses per link and confirms all seven links and straight/swung values during
12 seconds of real stereo playback. RMS is 0.363/0.373 V, peak 1.607/1.734 V,
and DC about -0.000209 V. No subjective listening assessment is claimed.

No physical Metropolix comparison has been made. Pair latching, automatic
track-wrap parity, gate/slide treatment, and extreme external-tempo handling
are documented interpretations; see [Swing operation](SWING.md). Windows OS
mouse/focus behavior remains a separate user playtest.

## Preset chains — 2.8.0

The eighth portable suite covers 32-link storage limits, insertion/deletion,
1–64 repeats, Total/Queue/Off timing, looping, reset, mute isolation and
phase preservation at 44.1/48/96 kHz with divided/skipped tracks. Rack-library
tests cover actual note/gate/MOD output on transition edges, accumulator resets,
empty preset retention, transport/pause, selective recall, schema migration,
malformed 64-bit JSON fields and patch-local chain/preset storage.

`scripts/test-ui.ps1 -Chains` exercises the actual Setup/encoder/stage callbacks,
held repeat drag with one undo, link mutes, long-save, clearing/recalling,
Quick Chain (including Red 1 and repeated presses), read-only playback and
Chain Follow. It saves a `.vcv` file and launches Rack again to verify the
working chain, saved chains, repeats, mutes and enabled state. OLED captures
cover the empty, editing and following views. `-ChainAudio` compresses the demo
to four pulses per link, observes all seven links and the loop, and records
12 seconds of the real stereo mix. This checks finite signal and healthy level,
not subjective musical quality. Existing UI/preset/polish/display regressions
remain part of the Windows validation.

Chains follow manual pp.171–178 and the later changelog fixes. Storage is local
to the Rack patch, and reopening starts at Link 1. Additional link mute flags
combine with stored mute settings; Queue fallback does not reset natural track
cycles between repeats; clock-source changes restart at Link 1. These combined
behaviors are documented interpretations requiring hardware-owner comparison.

## UI polish — 2.7.1

Tester feedback identified excessive slider sensitivity, missing adjustment
readouts, pitch LEDs that ignored Bloom, and malformed OLED descenders.
The slider multipliers now give a 400-pixel pitch range and approximately
24 pixels per pulse/gate step at Rack's default sensitivity. Live readouts use
the actual ParamQuantity, including its units and named choices. They honor
Tooltips off, persist outside the control while held, and clear on release.
Optional grid bindings stay fixed during a drag, preserving the correct undo.

Pitch lenses use Rack's LightWidget emissive layer and native halo. Their
brightness combines both track-position lights, with a dim idle level. The
hardware-style casing remains in the ordinary panel layer. Lowercase g/j/p/q/y
now have two complete descender rows; g and q no longer share a bitmap. Bitmap
pixels render as one filtered texture, preventing internal seams while keeping
small strokes visible at fractional zooms. CTRL-owned accumulator labels
also use a supported dim shade instead of disappearing from the renderer.

`scripts/test-ui.ps1 -Polish` checks actual Rack widget callbacks: 10-pixel
adjustments, single-step 24-pixel switch drags, upward/downward changes, live
readouts, pointer leave, one undo/redo, disabled tooltips, optional grid binding,
and cursor release. Front-buffer captures cover Bloom off/on, dim room lighting,
and 75/100/125/150/200% zoom. The existing widget suite, all 32 display states,
Rack host integration and seven portable engine suites also pass. These are
usability/rendering checks, not evidence of physical hardware equivalence.

Additional lifecycle checks remove a held control through Rack's event system,
render and remove a module-browser preview, release/recreate OLED textures,
and exit Rack normally. Windows Computer Use capture timed out twice, so native
OS mouse injection was not verified. The isolated test-launch script now
archives its preceding log before launch: abruptly stopped test sessions had
triggered Rack's recovery dialog and prevented the requested QA patch loading.
User profiles are not involved in that reset.

Implementation references: Rack's [Knob](https://github.com/VCVRack/Rack/blob/v2/src/app/Knob.cpp)
and [LightWidget](https://github.com/VCVRack/Rack/blob/v2/src/app/LightWidget.cpp).

## Preset banks and queued loading — 2.7.0

Portable tests cover queue multiples, reset-only waiting, Total fallback,
4096-pulse intervals, clock-phase preservation across 16 transitions at
44.1/48/96 kHz, same-edge note/MOD restart, and concurrent snapshot transfer.
Rack host tests cover all 64 slots, malformed bank data, schema migration,
selective loading, global preservation, exact output at an external boundary,
track-LEN/skip independence, cancellation/replacement, stop/button/jack reset,
Total resets, and before/after undo receipts. See [preset interpretations](PRESETS.md).

The preset suite passes 5,601,646 assertions, mostly repeated sample/transfer
checks. Actual widget callbacks cover save/overwrite/clear, confirmations,
bank navigation, non-editing slot drags, settings, recall undo/redo and queued
save blocking. A second Rack process reopens the saved native .vcv file and
verifies bank contents, empty slots, load preferences and a cleared queue.
The short Rack recording completes four transitions, with
stereo RMS 0.339/0.345 V, peaks 1.607/1.624 V and DC below 0.00021 V. This is
signal validation, not a claim of listening or physical hardware equivalence.

Checked on Windows x64 with Rack Free 2.6.6 and VCV Free 2.6.4. Reference: [Metropolix manual 1.6 (2025-09-24)](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf). Page numbers below are the printed page numbers; the PDF cover adds one to the file page index.

## Evidence

| Area | Check performed |
| --- | --- |
| Play orders, pp.71–74 | Literal forward sequences for all 17 deterministic orders; reversed sequences; three complete cycles; all 21 orders across lengths 1–8, offset wrapping, and all-skipped tracks |
| Random orders, p.73 | Shuffle visits every stage and repeats its permutation until reset; Quasi Random avoids consecutive repeats where required; fixed-seed Brownian distribution checks |
| Sequence length, pp.75–76 | Literal fixed-pulse examples, including restart partway through a stage; Auto does not clear accumulated transpositions |
| Gates, pp.94–97, 166–167 | Thirteen literal timing diagrams sampled across pulse-count division, stretching, and all three ratchet modes; partial groups truncate before the next stage |
| Accumulator modes, pp.126–127 | Literal Stage and Track four-note phrases at 44.1/48/96 kHz and actual Rack pitch/gate outputs; zero-amount stages inherit track transposition; modes and tracks stay independent |
| Skip + accumulation, pp.88, 102 | 672 seeded scenarios (2 modes × 21 orders × 16 seeds), 64 clock ticks each, randomized lengths/offsets/pulse counts/signed increments, live skip masks, all-skip silence/recovery, no skipped-stage increments; technical invariants, not musical-quality or hardware-equivalence judgments |
| Accumulator, pp.128–133 | Literal unipolar/bipolar wrap and pendulum sequences; positive/negative Hold limits; Random direction proportions and step bounds; large increments inside narrow limits |
| Accumulator triggers, p.102 | Both accumulator modes across all twelve combinations of four gate types and Stage/Pulse/Ratchet trigger, through the complete sequencer at 48 kHz |
| Probability, pp.98, 102 | Stage decisions remain consistent across a stage; Pulse decisions produce partially played stages; rejected pulses do not accumulate |
| Reset, p.134 | Manual retains accumulation across a fixed-length automatic restart; Auto clears it |
| MOD lanes, pp.137–145 | Literal stepped voltages, divided gates, full-stage ramps, unipolar clipping, 1.5 V gate threshold, independent timing, mute/stop/reset, offset wrapping, fixed pulse length, reverse, and all eight A/B sources at 44.1/48/96 kHz |
| MOD routing, pp.147, 157–162 | Base settings preserved; pre/post/continuous pitch paths, octave thresholds, track assignment, mute toggling, probability suppression, gate-length rest, emitted ratchet edges, multiple-lane summing, and route removal |
| Rack host | All 685 parameters, 64 preset slots and chain storage round-trip through Rack serialization; note/MOD/AUX UI state restores; legacy order/limit/AUX/mode/Swing migration; internal/external transport, Schmitt edges, reset priority, unipolar numeric entry, actual assignable A/B voltages, and AUX voltage/disconnection |
| Rack UI | Shared parameter bindings, independent tracks and MOD lanes, undo/redo, toggle handling, rectangular fader hit area, both widths, collision avoidance, stage/MOD editor switching, and patch restoration |
| Real signal path | 12-second, 48 kHz stereo recording inside Rack: sequencer → VCV oscillators and envelopes → VCAs; non-silent finite output, bounded peaks, and low DC on both channels |
| MOD signals in Rack | The modulation demo's A/B cables recorded by the test module: A reaches -2 and +2 V; B reaches 0 and +5 V with continuous ramps; both voices remain audible in the captured signal |

The automated suites contain 40,091 base-engine assertions, 64,738 manual/order/gate assertions, 395,400 accumulator/probability assertions, 119 MOD assertions, 60,635 AUX assertions, 129,097 CTRL assertions, 5,601,646 preset assertions, 9,713,498 chain assertions, 49,506 Swing assertions, 629,735 override assertions, and 5,040,487 Loopy/Stage Player assertions. Many are repeated sample or boundary checks; these numbers do not represent that many separate test scenarios. Host, editor, and audio checks run separately.

Actual Rack renderer images of both views were inspected. Windows Computer Use capture repeatedly failed with `window capture timed out: timed out waiting on channel`; native capture attempts were stopped. Editor tests dispatch Rack widget events from a dedicated test build, and image capture uses Rack's own renderer. Physical mouse/drag behavior and operating-system focus handling remain unverified in that session.

On 2026-10-12, the user reported completing their testing and finding the current build functional and satisfactory for now, with no issues reported. This establishes 2.1.0 as the accepted working baseline. Individual manual test cases were not enumerated; this feedback does not establish equivalence with physical Metropolix hardware.

## CTRL performance knobs — 2.6.0

Manual pp.146–153 informed direct versus offset behavior, defaults, track
assignment, ownership indications, and the CTRL display. The new engine suite
checks pitch quantization stages, per-track pulse offsets, probability and
ratchet gates, polarity, skip inversion/restart, output-CV swapping, route
removal and collisions. It includes 336 seeded scenarios (21 orders × 2 modes
× 8 seeds), 128 ticks each, with changing skip inversion and pulse counts.

Rack host tests cover new-module defaults, direct parameter ownership, route
conflicts, all 421 saved values, CTRL UI state, old-patch migration, and actual
swapped pitch/gate voltages. Display callback tests cover both knobs' assignment
pages, upward/downward drag, undo, field selection, locked destinations and
clear-assignment menus. The display test renders 28 states for inspection.

The 12-second CTRL demo recording at 48 kHz changes pulse-count and ratchet
knobs during playback, then returns them to noon. Both stereo channels pass
finite-sample, signal-level and DC checks: RMS 0.355/0.357 V, peaks 1.663/1.624 V,
absolute DC below 0.00018 V. This is technical signal verification, not a
listening judgment.

See [CTRL ranges and interpretations](CTRL-KNOBS.md). Knob gains not specified
by the manual, collision precedence, and Skip Invert reset scope remain
documented implementation choices. Version 2.18.0 implements Accum Dir separately
from signed increment inversion and completes the hardware's CTRL destination list.
No hardware comparison has been performed.

## Hardware UI pass — 2.2.0

The musical engine headers are unchanged from the accepted 2.1.0 baseline. The UI now follows the supplied 34 HP panel reference. All 404 parameter IDs and nine working port IDs remain unchanged; discrete switch quantities now snap when dragged. Schema 5 also saves screen/field/edit selection. Temporary ALT state clears on load.

The updated Rack widget suite passes: 24 pitch/pulse/gate controls, compact 34 HP and expanded 66 HP widths, collision avoidance, up/down drag direction, numeric button and encoder editing, toggle click versus held drag, current-value tooltips, direct numeric entry, named ORDER choices, undo/redo, independent track/lane bindings, editor switching, and save/load. EDIT → function → stage selection and Slide toggling follow manual pp.85–87. Leaving the function chooser through BPM or track selection restores normal editing. Slide/Skip indicators use the manual's red/orange colors.

`scripts/test-ui.ps1 -Hardware` passed using the new hardware demo through actual Rack cables at 48 kHz for 12 seconds. Channel 1 measured RMS 0.419756 V, peak 1.59967 V, DC 0.000037 V; channel 2 RMS 0.499084 V, peak 1.59992 V, DC -0.000244 V. A measured -2 to +2 V and B 0 to +5 V. The Rack integration suite passed again with all parameters and the new display state round-tripped.

Rendered compact, expanded MOD, numeric-menu, and audio-demo screenshots were inspected. Automated UI checks invoke the actual Rack callbacks; physical mouse/focus testing of this new interface remains for user playtesting. The first panel pass matches the arrangement, not every photographic detail. Hardware home views, long-press gestures, secondary functions, and component typography/artwork need more work. `HARDWARE-UI.md` records control limitations; visually represented unfinished features are not counted as implemented engine functions.

## Display refinement — 2.3.0

The five musical engine headers remain byte-identical to the accepted 2.1.0 baseline. New host-side observers supply the display with stage CV, pitch pre/post routes, and measured external BPM without changing sequencer processing. Host tests verify those signals, a 4800-sample external interval at 48 kHz (150 BPM), clock unplugging, all 404 saved parameters, schema 6 HOME/detail persistence, and schema 5 LEN field migration.

The original Rack widget suite passes on the revised controls. `test-ui.ps1 -Display` also passes HOME browsing in both drag directions, ORDER direction with undo, LEN pulse/offset selection, SCALE ALT selection, MOD lane selection and Step/Ramp, probability/accumulator trigger selection, and screen persistence. Twenty populated screen states and full-panel captures were inspected. These are captures of the actual production rendering component in an instrumented Rack build, not static design mockups.

The 2.3.0 hardware demo passed the 12-second, 48 kHz Rack audio/CV check: channel 1 RMS 0.423473 V, peak 1.59967 V, DC -0.000545 V; channel 2 RMS 0.499084 V, peak 1.59992 V, DC -0.000244 V. A reached -2 to +2 V and B reached 0 to +5 V. Both voices remain active through actual Rack cables.

The visual reference is the supplied photograph and native screen images from manual pp.28–31,60–62,70,75,77,80,82,85–104,115–117,137–141. Hardware artwork, lettering, switch grips, button colors, legends, logo, and socket detail were refined. Native-pixel OLED text/diagrams replace the earlier generic display. HOME now includes positions, notes, panel values, and CV/MOD activity.

The display does not imply additional engine features: Tempo slide, 100% gate scale, and /1 external clock division are fixed readouts; CTRL remains unimplemented; MOD values still use volts rather than destination-specific units. A/B routing directly selects the available track-CV/MOD sources. Exact photographic materials, every glyph/animation, long-press gestures, and complete firmware navigation remain unverified or unfinished. Windows capture failed with `FrameArrived timed out: timed out waiting on channel`; native physical mouse testing remains separate from the successful callback checks.

## AUX inputs — 2.4.0

Three inputs, attenuverters, destination/track controls, and a live AUX display
follow manual pp.154–156. All eight supported destinations share the existing
MOD destination mappings. The engine reads external CV every sample; AUX and
MOD contributions combine before destination saturation. Mute uses thresholded
gates and logical inversion. See [AUX controls and assumptions](AUX-INPUTS.md).

The five portable engine suites pass, including 53,903 new AUX assertions:
voltage polarity/limits, the 1.5 V gate threshold, manual unpatched gates,
track isolation, pitch/gate destinations, real ratchet edges, and AUX/MOD
interaction at 44.1/48/96 kHz. Existing sequencing suites still pass unchanged.
The Rack host verifies all 413 parameters, every-sample CV input, mono channel
selection, unplugging, live stopped meters, schema 7 screen persistence, and
old-preset loading over previously assigned AUX settings.

The widget suite checks all three real jacks, knob/button/encoder drags, named
destination menus, track isolation, clear assignment with undo, and patch
reload. AUX adds continuous-CV and gate settings screens to the display suite.
All 22 display states pass and their native-pixel renders were inspected. In
the 12-second Rack AUX demo, observed effective X/Y/Z ranges were -0.5..+0.5 V,
-0.497108..0 V, and 0..5 V. Both voices remained finite and active: channel 1
RMS 0.418846 V, peak 1.59958 V, DC 0.00185187 V; channel 2 RMS 0.482742 V,
peak 1.59994 V, DC -0.000233547 V. A/B retained their -2..+2 V and 0..+5 V ranges.
Run `scripts/test-ui.ps1 -Aux` to reproduce; evidence is in `tmp/aux-qa`.
The older UI-only revisions below retain their historical validation results;
2.4.0 intentionally extends the engine and is not byte-identical to 2.1.0.

## Independent creator wordmark — 2.4.1

The bottom panel credit is now `sensiburner <3 intellijel`, with matching bold
lowercase letters and red accents. It is a vector asset centered at the same
height as the old logo. Rack's brand/author fields now identify sensiburner.
The production build and Rack display suite pass; the actual rendered panel was
inspected for fit, readability, clipping, and overlap. The engine, parameter
IDs, ports, settings schema, and example patches are unchanged from 2.4.0.

## Interpretations and limits

- **No hardware comparison.** Passing a manual example establishes agreement with that example, not firmware equivalence.
- **Quantization and slide.** Pitch sliders choose the nearest enabled scale degree. Ties choose the lower degree. Tempo slide is linear over a fraction of a track pulse. Version 2.13.0 adds Analog/Acid constant-rate linear slew; their rate calibration is chosen locally, not measured from hardware.
- **Order names.** The Arp variants follow the printed sequences on p.72. Those sequences take precedence over a mathematical interpretation of their Shift labels. Odd-length variants generalize the patterns; not every length has a printed oracle.
- **Generative orders.** Random source and seeds are local implementation choices. Brownian has bounded retries when stages are skipped so it cannot stall the audio thread; behavior in heavily skipped patterns is not specified precisely by the manual.
- **Bipolar wrap.** On p.129, the body text says Unipolar below the BIPOLAR WRAP heading. This implementation follows the heading and plotted bipolar sequence.
- **Pulse probability with long gates.** Each physical track pulse makes a new decision in Pulse mode. That decision gates the current pulse, including a stretched gate. The manual explains probability targets but does not fully specify every long-gate interaction.
- **Accumulator combinations.** The p.102 trigger matrix is matched with ordinary gates and ratchets. Pulse accumulation uses each eligible track-clock pulse for MULTI; Ratchet accumulation follows emitted ratchet events. Interactions with pulse-count division, probability, stretch, or live setting changes need more reference evidence before claiming exact firmware behavior.
- **Gate duration.** Gate tests reproduce the printed 50% ratchet examples and 75% stretch examples. Gated-ratchet behavior at other gate lengths is inferred. The nominal minimum high duration is 333 microseconds, capped below the ratchet interval to preserve edges at unusually fast external clocks.
- **Clock input.** Explicit clock source and input division are implemented; one source edge is a sixteenth-note pulse before input division. Per-track Swing is available. The first external period is estimated from internal tempo until two edges arrive. See [Clock/reset](CLOCK-RESET.md) and [input division](UI-SETUP.md).
- **Pitch on rests.** Per-track Rest Pitch offers Hold and Update. Update changes pitch on a silent Rest stage; probability-rejected stages still hold it. CV lanes follow their selected stage unless muted/stopped. See [Track options](TRACK-OPTIONS.md).
- **MOD timing.** Each lane advances one stage per divided base-clock pulse, independently of note-stage pulse counts. Ramps interpolate from the previous voltage to the new stage's target over that whole stage; the initial voltage after reset is zero. A ramp uses the most recently estimated clock period and holds its target if the clock stalls. Exact first-ramp and abrupt external-tempo-change behavior has not been compared with hardware.
- **MOD pitch scaling.** Octave uses whole-volt thresholds at 1 V/oct, truncating toward zero. Pitch Pre adds 1 V/oct before scale quantization; Pitch Post rounds to chromatic semitones at 1 V/oct; Pitch Offset adds continuous voltage after quantization. Mute toggles the programmed mute while its route is at least 1.5 V. Gate-type lanes use 0/5 V and ignore ramps.
- **Inferred MOD gains.** Gate Length and Probability add 20 percentage points per volt; Ratchets adds a rounded 1.4 counts per volt. Thus ±5 V spans ±100 percentage points or ±7 ratchets before the destination's final range limit. The manual describes the direction and parameter bounds but does not provide these exact numeric gains. These mappings are implementation choices pending better reference evidence.
- **Combining MOD lanes.** Voltages from lanes targeting the same track and destination are summed, then clamped to ±5 V before mapping to the parameter. Gate/mute routes use the resulting threshold. This combination rule is inferred; the manual does not specify it in sufficient detail. Muting a lane removes its contribution without changing saved base settings or stopping that lane's position.
- **Persistence.** Rack saves configuration, not a running transport snapshot or the random generator state. Loading starts a fresh traversal and accumulator state.

## Track accumulation release (2.5.0)

The Stage/Track examples from pp.126–127 pass through the complete engine and
Rack's actual pitch and gate ports. Tests cover positive/negative contributors,
all four orders, zero-amount stages, Stage/Pulse/Ratchet triggers, skipped
sources, live skips, rest/probability/mute suppression, manual reset and fixed
pulse-length Auto/Manual behavior. All previous engine suites pass unchanged.
Host tests verify mode persistence, old-preset migration over a configured
module, unchanged IDs, and all six LIM fields. Widget/display tests cover
ALT → LIM, named mode choices, both drag directions, undo, independent tracks,
and 25 populated native-pixel screen states.

**Implementation interpretations:** Track mode uses one shared bank per track.
At each eligible nonzero trigger it plays the bank's pending value, then
computes the next value using that stage's increment. Zero-amount stages keep
the sounding offset without consuming the pending value; skipped/rest/rejected
events do not advance it. This reproduces the manual's neutral first pass.
The same rule for multiple contributors, asymmetric limits with mixed signs,
and generative orders needs physical reference evidence. Stage and Track
banks are retained separately on mode changes, applied at the next track-clock
boundary. Global reset clears both; fixed-length Auto resets both, Manual
retains both. These live-switch rules are explicit implementation choices.
Skip does not retroactively remove an already accumulated track offset.

The comparison patch is a functional demonstration, not a new composed song.
That accumulator release did not add preset banks/chaining or accumulator MOD
destinations. Version 2.7.0 adds banks and queued loading; 2.8.0 adds chains.
Its 12-second, 48 kHz Rack recording passes both stereo signal checks: RMS
0.261/0.435 V, peaks 1.579/1.600 V, DC below 0.0005 V. These measurements
confirm the patched signal path; they do not rate the musical result.
Reproduce with `scripts/test-ui.ps1 -Accumulator` after closing Rack.

## Next feature work

Routing, ALT workflows, Loopy, MIDI, MX and portable JSON backups now have
implementation and host tests. Remaining work includes auditing secondary
menu/display gestures against the manual and physical hardware/DAW comparisons.
Hardware SysEx compatibility requires a verified payload specification.

## Reproduce

Run the commands in README. Test reports and renderer images are written to `tmp/qa`; the earlier audio report and recording go to `tmp/audio-qa`. MOD evidence goes to `tmp/mod-qa`; the new hardware demo's audio report, recording, 100 Hz CV trace (`mod-cv.csv`), and rendered patch image go to `tmp/hardware-qa`. Rack logs are in `tmp/rack-qa/log.txt`. The release package contains no QA module, environment-variable test entry point, or test recorder.

## Loopy — 2.11.0

Manual pp.39–46/169 drive the implementation. The dedicated engine suite matches
the one/two-boundary examples and verifies unchanged background traversal, RNG,
probability and accumulation across all 21 orders with straight/swung/divided
clocks. Stopped audition freezes pitch/CV, ignores Gate Type and reaches A/B
track-CV output. Foreground accumulation runs independently and is discarded on
release; multi-stage stopped audition chooses the lowest held stage. These are
explicit interpretations pending hardware feedback. Performance mute gestures
apply on base-clock boundaries and do not create undo entries.

Rack host tests cover actual cables, global settings outside format-3 snapshots,
legacy patches, mute timing and transient gesture cleanup. Widget tests exercise
momentary press/release, range menus, navigation latching, encoder unlatching,
parameter undo, mutes and a native Rack file save/reopen. The display suite renders
44 states, including Auto, both tracks and latch. Real stereo playback tests observe
momentary/range/release transitions with a 12-second output recording. Native OS
input, hardware accumulator behavior and Loopy AUX/MOD routing remain unverified
or unavailable; see [the operation guide](LOOPY.md).

## Scales — 2.12.0

All 49 factory scales/chords and 100 custom definitions are available from SCALE.
Encoder browsing uses the hardware bank order while legacy numerical IDs stay fixed.
Long encoder presses enter/exit note editing, ALT selects Root, clicks lock/apply
or toggle notes. Hold SCALE + stage to recall a shortcut; long stage press saves.
Right-click menus provide the same operations. See [Scales](SCALES.md).

Portable validation adds 1,609,670 checks. Rack host, custom editing, lock/search,
held shortcuts, grouped undo, native saving/reopening, 49 display states and
actual stereo chord playback pass. Audio measured RMS .360/.363 V, peaks
1.730/1.733 V, DC approximately .00084 V at 48 kHz over twelve seconds.
Fractional-voltage truncation, default C-major custom slots and root-only empty
mask fallback are implementation choices; hardware comparison remains necessary.
Internal presets recall Scale/Root settings, as the manual describes on p.112;
the earlier claim that their recall differs from hardware was incorrect. Custom
definitions remain global. As of 2.33, Scale Lock follows the preset storage
column on p.169; see [Preset storage](PRESET-STORAGE.md).

## Slide models — 2.13.0

The manual pp.80–81/86 and Stage Player p.44 define the distinctions.
Tempo interpolates into the marked stage over a fraction of a track pulse.
Analog has a fixed voltage slew rate; Acid uses that rate into the next actually
played stage and replaces the marked stage's physical switch with Hold.
Explicit Rest overrides, zero Gate Scale, probability and mute still suppress
gates. Analog is the only model applied during stopped Stage Player.

The dedicated suite passes 2,160,397 checks, including interval and tempo
comparisons at 44.1/48/96 kHz, ascending/descending bounded slew, skipped
Acid sources, every physical gate type, multi-pulse Hold and frozen audition.
Rack tests cover actual pitch/gate jacks, model descriptions, parameter IDs,
format-4 snapshots and neutral Tempo migration from formats 1–3. Widget tests
cover cycling, upward dragging, grouped reset/undo, CTRL ownership, shortcuts,
independent tracks, MOD guards and native save/reopen. All 52 OLED states pass.

Real twelve-second stereo playback observes all three models on both tracks
and the full seven-link chain loop. RMS .382/.373 V, peaks 1.663/1.642 V,
DC approximately -.00081 V. Existing preset/chain/swing/override/Loopy/scale
checks and the slider/readout/Bloom zoom checks pass. These recordings establish
a finite signal path, not a hardware or musical quality comparison.

The manual gives no numeric rate calibration and contains conflicting Analog
time wording. We follow its close-versus-distant-note example using constant
voltage slew, with an exponential amount mapping from 1 ms to 1 second per
octave and instantaneous zero. Defaults and shortcuts remain explicit choices.
Owner feedback is needed for the curves and Acid's Hold/accumulator interactions;
see [Slide models](SLIDES.md).

## Stage CV — 2.14.0

Manual pp.103–106/136 define three types and three incoming transitions.
The dedicated engine suite passes 2,292,494 checks: printed four-pulse timing,
all pulse counts/types/transitions, voltage bounds, rests/probability/skips,
all 21 orders, 44.1/48/96 kHz, Swing/division, mutes and frozen audition.
Rack tests cover real A/B jacks, typed quantities, 478-field format-5 snapshots
and neutral Bipolar/Step migration from earlier formats.

Widget tests pass TYPE, transition cycling, ALT voltage offsets, bulk ramp
selection/reset, gate inversion/rotation, stored negative-value undo, optional
grid dragging and native save/reopen. All 60 OLED states pass, including each
type and ramp plus ALT vertex markers. Existing sequencing/UI checks pass.
The stereo demo records twelve seconds at 48 kHz: RMS .351/.344 V, peaks
1.503/1.552 V, DC approximately .00028 V. Actual A/B cable voltages both span
-2..+5 V, with 146,451 / 179,638 changing samples. All types and transitions
and the seven-link loop are observed during playback.

The chosen gate conversion threshold is 1.5 V; changing type preserves the
stored numerical program. First ramps start from zero, and a stalled clock
finishes the current pulse's progress without inventing later sRamp pulses.
These conversion/stall/live-edit rules require hardware feedback. The manual
conflicts on EDIT versus ALT for bulk ramp selection; this version uses ALT
consistently with its complete CV shortcut instructions. See [Stage CV](STAGE-CV.md).
