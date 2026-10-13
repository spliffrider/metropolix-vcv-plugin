# Feature status — 2.23.0 public alpha

Assessment date: 2026-10-14. The reference is the
[Metropolix firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
This implementation has not been compared with a physical module.

2.23.0 adds [preset pickup](PRESET-PICKUP.md), with physical/sounding state, native undo and persistence.
2.22.0 adds [UI setup, pending confirmation, input clock division and output delay](UI-SETUP.md).
2.21.0 adds [editing workflows, custom Init and copy/paste](EDITING-WORKFLOWS.md).
2.20.0 adds [clock/reset configuration, protected tracks, Bottom Pitch and tuner](CLOCK-RESET.md).
2.19.0 completed all 38 AUX/MOD destinations, including Loopy and targeted reset; see [Complete routing](COMPLETE-ROUTING.md).
2.18.0 added all 29 CTRL destinations and six accumulator AUX/MOD routes; see [Accumulator routing](ACCUMULATOR-ROUTING.md).
2.17.0 added ALT → TRK, pulse/gate sources, Rest Pitch, full transpose and track initialization; see [Track options](TRACK-OPTIONS.md).
2.16.0 added all A/B functions, velocity, per-track slider range/direction, three CTRL and two AUX/MOD destinations; see [Output routing](OUTPUT-ROUTING.md).
2.15.0 added global clock output division and offset; see the [clock guide](CLOCK-OUTPUT.md).
2.14.0 added full Bipolar/Unipolar/Gate stage CV, Step/sRamp/pRamp transitions and ALT pattern workflows; see the [CV guide](STAGE-CV.md).
2.13.0 added independent Tempo/Analog/Acid slides, shortcuts, reset and Analog Stage Player; see the [slide guide](SLIDES.md).
2.12.0 added the 49 factory scales/chords, 100 user scales, locks, shortcuts and Scale (User) routing; see the [scale guide](SCALES.md).
2.11.0 added Loopy, Stage Player, latching and performance mutes; see the [Loopy guide](LOOPY.md).
2.10.0 added per-stage pitch/gate overrides, Gate Scale and the track gate Hold endpoint; see the [override guide](STAGE-OVERRIDES.md).
2.9.0 added per-track Swing, a CTRL destination and AUX/MOD offsets; see the [Swing guide](SWING.md).
2.8.0 added 32-link preset chains, repeats, per-link mutes and eight saved chains.
2.7.0 added 64 internal presets, selective recall, queued loading and Total Pulses.
2.6.0 added both CTRL performance knobs with 23 supported destinations.
2.5.0 added Stage/Track accumulator mode and a LIM options display.
The feature table below is the current assessment. Earlier percentage estimates
predate several major features and should not be used to judge this build.
Physical installation, calibration and firmware-update procedures are outside
the software comparison. Visual resemblance does not establish behavior.

By core sequencing we mean creating two note sequences from the eight shared
stages, controlling traversal/timing, and applying gate/stage variations. The
full comparison also includes the hardware's performance controls, routing,
configuration, preset workflows, and external interfaces.

| Area | Available now | Principal remaining work |
| --- | --- | --- |
| Eight-stage, two-track sequencing | Shared pitch/pulse/gate controls; independent pulse/gate sources, Rest Pitch, full transpose, track menu/init and per-track Global Reset; pitch/gate outputs and stage overrides | Remaining track MIDI/interface options |
| Traversal and timing | All 21 play orders, reverse, length, offset, clock/pulse division, fixed pulse lengths, Swing, reset, internal/external clock with input division, global CLK division/offset and Always/Running output, explicit clock source, Reset/Run/Run-C | MIDI clock integration and hardware timing comparisons |
| Gate/stage variation | Four gate types; three ratchet modes; stretching; Stage/Pulse probability; skip; Tempo/Analog/Acid slides; Bipolar/Unipolar/Gate CV with Step/sRamp/pRamp; gate scaling and length Hold | Hardware comparison of live edits and mixed timing |
| Accumulators | Stage-local and track-wide accumulation, four orders, both polarities, separate limits, three triggers, Manual/Auto reset; pause/reverse/invert/mode/polarity/targeted reset routing | Hardware comparison of mixed sources and live changes |
| Pitch/scales | 49 factory scales/chords, 100 user scales with roots, scale locks, eight shortcuts, per-track Scale (User) routing, Bottom Pitch, tuner, transpose/range and pitch pre/post/offset modulation | Hardware comparison of modulation thresholds and mixed routes; legacy preset selector/global difference |
| MOD lanes | All eight lanes, Step/Ramp, bipolar/unipolar/gate voltage, independent traversal/timing and mute; all 38 hardware destinations; gate On/Off readouts and global scope | MIDI options, continuous destination-specific display units and hardware comparisons |
| A/B and performance routing | All 16 A/B function types; track/lane assignment, confirmation, velocity, slider range/direction; AUX X/Y/Z with all 38 destinations; all 29 CTRL destinations | Hardware comparisons |
| Performance gestures | Run/reset, track/lane selection, mutes, tap tempo, Loopy, Stage Player, latching, Loopy routing, ALT randomize/morph/restore and stage/track/lane copy/paste | Hardware comparisons of copy/paste and randomization |
| Display and desktop editing | Four HOME views, current edit diagrams, hover help, context menus, vertical dragging, numeric entry, undo, optional grids | Some secondary screens, long-press gestures, glyphs/animations, exact material/detail matching |
| Saving and integration | Rack persistence, parameter automation, 64 internal presets, queued loads, Total Pulses, 32-link chains, repeats/mutes, eight saved chains, Quick Chain, custom Init and pitch/CTRL Thru pickup | MIDI, MX expansion and SysEx interchange |

The current 736 parameters include repeated stage/lane controls; that number is
not a count of distinct hardware features. Implemented behavior can still
differ in edge cases or where the manual leaves details unspecified. See
[validation and known interpretations](VALIDATION.md).

## Readiness for feedback

This is a useful **early alpha for Windows testers**. The engine suites, Rack
host/persistence checks, widget callback tests, 126 display states, and recorded
stereo/CV paths pass on the documented setup. The musical baseline also received
an initial user playtest. Broad testing on other computers, long sessions,
different zoom/DPI settings, and interactions with other plugins is still needed.
