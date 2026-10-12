# Validation and fidelity status — 2.2.0

Checked on Windows x64 with Rack Free 2.6.6 and VCV Free 2.6.4. Reference: [Metropolix manual 1.6 (2025-09-24)](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf). Page numbers below are the printed page numbers; the PDF cover adds one to the file page index.

## Evidence

| Area | Check performed |
| --- | --- |
| Play orders, pp.71–74 | Literal forward sequences for all 17 deterministic orders; reversed sequences; three complete cycles; all 21 orders across lengths 1–8, offset wrapping, and all-skipped tracks |
| Random orders, p.73 | Shuffle visits every stage and repeats its permutation until reset; Quasi Random avoids consecutive repeats where required; fixed-seed Brownian distribution checks |
| Sequence length, pp.75–76 | Literal fixed-pulse examples, including restart partway through a stage; Auto does not clear accumulated transpositions |
| Gates, pp.94–97, 166–167 | Thirteen literal timing diagrams sampled across pulse-count division, stretching, and all three ratchet modes; partial groups truncate before the next stage |
| Accumulator, pp.128–133 | Literal unipolar/bipolar wrap and pendulum sequences; positive/negative Hold limits; Random direction proportions and step bounds; large increments inside narrow limits |
| Accumulator triggers, p.102 | All twelve combinations of four gate types and Stage/Pulse/Ratchet trigger, through the complete sequencer at 48 kHz |
| Probability, pp.98, 102 | Stage decisions remain consistent across a stage; Pulse decisions produce partially played stages; rejected pulses do not accumulate |
| Reset, p.134 | Manual retains accumulation across a fixed-length automatic restart; Auto clears it |
| MOD lanes, pp.137–145 | Literal stepped voltages, divided gates, full-stage ramps, unipolar clipping, 1.5 V gate threshold, independent timing, mute/stop/reset, offset wrapping, fixed pulse length, reverse, and all eight A/B sources at 44.1/48/96 kHz |
| MOD routing, pp.147, 157–162 | Base settings preserved; pre/post/continuous pitch paths, octave thresholds, track assignment, mute toggling, probability suppression, gate-length rest, emitted ratchet edges, multiple-lane summing, and route removal |
| Rack host | All 404 parameters round-trip through Rack serialization; note/MOD UI state restores; legacy order/limit migration; internal/external transport, Schmitt edges, reset priority, unipolar numeric entry, and actual assignable A/B voltages |
| Rack UI | Shared parameter bindings, independent tracks and MOD lanes, undo/redo, toggle handling, rectangular fader hit area, both widths, collision avoidance, stage/MOD editor switching, and patch restoration |
| Real signal path | 12-second, 48 kHz stereo recording inside Rack: sequencer → VCV oscillators and envelopes → VCAs; non-silent finite output, bounded peaks, and low DC on both channels |
| MOD signals in Rack | The modulation demo's A/B cables recorded by the test module: A reaches -2 and +2 V; B reaches 0 and +5 V with continuous ramps; both voices remain audible in the captured signal |

The automated suites contain 40,091 base-engine assertions, 64,738 manual/order/gate assertions, 73,157 accumulator/probability assertions, and 119 MOD assertions. Many are repeated sample or boundary checks; these numbers do not represent that many separate test scenarios. Host, editor, and audio checks run separately.

Actual Rack renderer images of both views were inspected. Windows Computer Use capture repeatedly failed with `window capture timed out: timed out waiting on channel`; native capture attempts were stopped. Editor tests dispatch Rack widget events from a dedicated test build, and image capture uses Rack's own renderer. Physical mouse/drag behavior and operating-system focus handling remain unverified in that session.

On 2026-10-12, the user reported completing their testing and finding the current build functional and satisfactory for now, with no issues reported. This establishes 2.1.0 as the accepted working baseline. Individual manual test cases were not enumerated; this feedback does not establish equivalence with physical Metropolix hardware.

## Hardware UI pass — 2.2.0

The musical engine headers are unchanged from the accepted 2.1.0 baseline. The UI now follows the supplied 34 HP panel reference. All 404 parameter IDs and nine working port IDs remain unchanged; discrete switch quantities now snap when dragged. Schema 5 also saves screen/field/edit selection. Temporary ALT state clears on load.

The updated Rack widget suite passes: 24 pitch/pulse/gate controls, compact 34 HP and expanded 66 HP widths, collision avoidance, up/down drag direction, numeric button and encoder editing, toggle click versus held drag, current-value tooltips, direct numeric entry, named ORDER choices, undo/redo, independent track/lane bindings, editor switching, and save/load. EDIT → function → stage selection and Slide toggling follow manual pp.85–87. Leaving the function chooser through BPM or track selection restores normal editing. Slide/Skip indicators use the manual's red/orange colors.

`scripts/test-ui.ps1 -Hardware` passed using the new hardware demo through actual Rack cables at 48 kHz for 12 seconds. Channel 1 measured RMS 0.419756 V, peak 1.59967 V, DC 0.000037 V; channel 2 RMS 0.499084 V, peak 1.59992 V, DC -0.000244 V. A measured -2 to +2 V and B 0 to +5 V. The Rack integration suite passed again with all parameters and the new display state round-tripped.

Rendered compact, expanded MOD, numeric-menu, and audio-demo screenshots were inspected. Automated UI checks invoke the actual Rack callbacks; physical mouse/focus testing of this new interface remains for user playtesting. The first panel pass matches the arrangement, not every photographic detail. Hardware home views, long-press gestures, secondary functions, and component typography/artwork need more work. `HARDWARE-UI.md` records control limitations; visually represented unfinished features are not counted as implemented engine functions.

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

## Next feature work

The remaining hardware MOD destinations; AUX/CTRL assignment; stage pitch and gate-length overrides; Loopy; swing; full scale selection; track-wide accumulation; additional slide models; and preset chaining are still open. MIDI, MX, and firmware backup interchange are also absent. These should be implemented with their own manual examples and host tests, not inferred from the existence of similarly named controls.

## Reproduce

Run the commands in README. Test reports and renderer images are written to `tmp/qa`; the earlier audio report and recording go to `tmp/audio-qa`. MOD evidence goes to `tmp/mod-qa`; the new hardware demo's audio report, recording, 100 Hz CV trace (`mod-cv.csv`), and rendered patch image go to `tmp/hardware-qa`. Rack logs are in `tmp/rack-qa/log.txt`. The release package contains no QA module, environment-variable test entry point, or test recorder.
