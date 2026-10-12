# Validation and fidelity status — 2.0.1

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
| Rack host | All 194 parameters round-trip through Rack serialization; UI state restores; legacy order/limit migration; internal/external transport, Schmitt edges, and reset priority |
| Rack UI | Shared parameter bindings, independent tracks, undo/redo, toggle handling, rectangular fader hit area, both widths, collision avoidance, and patch restoration |
| Real signal path | 12-second, 48 kHz stereo recording inside Rack: sequencer → VCV oscillators and envelopes → VCAs; non-silent finite output, bounded peaks, and low DC on both channels |

The automated suites contain 40,091 base-engine assertions, 64,738 manual/order/gate assertions, and 73,157 accumulator/probability assertions. Many are repeated sample or boundary checks; these numbers do not represent that many separate test scenarios. Host, editor, and audio checks run separately.

Actual Rack renderer images of both views were inspected. Windows Computer Use capture repeatedly failed with `window capture timed out: timed out waiting on channel`; native capture attempts were stopped. Editor tests dispatch Rack widget events from a dedicated test build, and image capture uses Rack's own renderer. Physical mouse/drag behavior and operating-system focus handling remain unverified in that session.

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
- **Persistence.** Rack saves configuration, not a running transport snapshot or the random generator state. Loading starts a fresh traversal and accumulator state.

## Next feature work

MOD lanes and routing; AUX/CTRL assignment; stage pitch and gate-length overrides; Loopy; swing; full scale selection; track-wide accumulation; additional slide models; and preset chaining are still open. MIDI, MX, and firmware backup interchange are also absent. These should be implemented with their own manual examples and host tests, not inferred from the existence of similarly named controls.

## Reproduce

Run the commands in README. Test reports and renderer images are written to `tmp/qa`; the audio report and recording go to `tmp/audio-qa`. Rack logs are in `tmp/rack-qa/log.txt`. The release package contains no QA module, environment-variable test entry point, or test recorder.
