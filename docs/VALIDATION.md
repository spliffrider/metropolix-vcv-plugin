# Validation and fidelity status — 2.7.1

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
| Rack host | All 428 parameters and 64 preset slots round-trip through Rack serialization; note/MOD/AUX UI state restores; legacy order/limit/AUX/mode migration; internal/external transport, Schmitt edges, reset priority, unipolar numeric entry, actual assignable A/B voltages, and AUX voltage/disconnection |
| Rack UI | Shared parameter bindings, independent tracks and MOD lanes, undo/redo, toggle handling, rectangular fader hit area, both widths, collision avoidance, stage/MOD editor switching, and patch restoration |
| Real signal path | 12-second, 48 kHz stereo recording inside Rack: sequencer → VCV oscillators and envelopes → VCAs; non-silent finite output, bounded peaks, and low DC on both channels |
| MOD signals in Rack | The modulation demo's A/B cables recorded by the test module: A reaches -2 and +2 V; B reaches 0 and +5 V with continuous ramps; both voices remain audible in the captured signal |

The automated suites contain 40,091 base-engine assertions, 64,738 manual/order/gate assertions, 395,400 accumulator/probability assertions, 119 MOD assertions, 53,903 AUX assertions, 129,095 CTRL assertions, and 5,601,646 preset assertions. Many are repeated sample or boundary checks; these numbers do not represent that many separate test scenarios. Host, editor, and audio checks run separately.

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
documented implementation choices. Accum Dir is deferred rather than equated
with signed increment inversion. Six hardware destinations remain unavailable.
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
- **Quantization and slide.** Pitch sliders choose the nearest enabled scale degree. Ties choose the lower degree. Slide is linear over a fraction of a track pulse. Hardware calibration and Acid/Analog curves have not been reproduced.
- **Order names.** The Arp variants follow the printed sequences on p.72. Those sequences take precedence over a mathematical interpretation of their Shift labels. Odd-length variants generalize the patterns; not every length has a printed oracle.
- **Generative orders.** Random source and seeds are local implementation choices. Brownian has bounded retries when stages are skipped so it cannot stall the audio thread; behavior in heavily skipped patterns is not specified precisely by the manual.
- **Bipolar wrap.** On p.129, the body text says Unipolar below the BIPOLAR WRAP heading. This implementation follows the heading and plotted bipolar sequence.
- **Pulse probability with long gates.** Each physical track pulse makes a new decision in Pulse mode. That decision gates the current pulse, including a stretched gate. The manual explains probability targets but does not fully specify every long-gate interaction.
- **Accumulator combinations.** The p.102 trigger matrix is matched with ordinary gates and ratchets. Pulse accumulation uses each eligible track-clock pulse for MULTI; Ratchet accumulation follows emitted ratchet events. Interactions with pulse-count division, probability, stretch, or live setting changes need more reference evidence before claiming exact firmware behavior.
- **Gate duration.** Gate tests reproduce the printed 50% ratchet examples and 75% stretch examples. Gated-ratchet behavior at other gate lengths is inferred. The nominal minimum high duration is 333 microseconds, capped below the ratchet interval to preserve edges at unusually fast external clocks.
- **Clock input.** One edge is a sixteenth note. Hardware input-mode/PPQN options and swing are absent. The first external period is estimated from internal tempo until two edges arrive.
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
destinations. Version 2.7.0 adds banks and queued loading; chains remain open.
Its 12-second, 48 kHz Rack recording passes both stereo signal checks: RMS
0.261/0.435 V, peaks 1.579/1.600 V, DC below 0.0005 V. These measurements
confirm the patched signal path; they do not rate the musical result.
Reproduce with `scripts/test-ui.ps1 -Accumulator` after closing Rack.

## Next feature work

The remaining hardware MOD/AUX destinations; remaining CTRL destinations; ALT+AUX selective randomization; stage pitch and gate-length overrides; Loopy; swing; full scale selection; additional slide models; and preset chaining are still open. MIDI, MX, and firmware backup interchange are also absent. These should be implemented with their own manual examples and host tests, not inferred from the existence of similarly named controls.

## Reproduce

Run the commands in README. Test reports and renderer images are written to `tmp/qa`; the earlier audio report and recording go to `tmp/audio-qa`. MOD evidence goes to `tmp/mod-qa`; the new hardware demo's audio report, recording, 100 Hz CV trace (`mod-cv.csv`), and rendered patch image go to `tmp/hardware-qa`. Rack logs are in `tmp/rack-qa/log.txt`. The release package contains no QA module, environment-variable test entry point, or test recorder.
