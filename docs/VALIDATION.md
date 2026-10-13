# Validation and fidelity status — 2.25.0

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
- **Clock input.** One edge is a sixteenth note. Hardware input-mode/PPQN options are absent; per-track Swing is available. The first external period is estimated from internal tempo until two edges arrive.
- **Pitch on rests.** Rest/rejected stages hold pitch. CV lanes still follow the selected stage unless muted/stopped. Hardware configuration alternatives are not implemented.
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

The routing, ALT workflows, Loopy and MIDI listed in the latest sections now have implementation and host tests. Remaining work includes MX, documented software backup/interchange, secondary UI details and physical hardware/DAW comparisons. Hardware SysEx compatibility requires a verified payload specification.

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
Internal presets retain the earlier plugin selector/root recall behavior, unlike
the hardware global classification; custom definitions and locks stay global.

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
