# Feature status — 2.16.0 public alpha

Assessment date: 2026-10-13. The reference is the
[Metropolix firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
This implementation has not been compared with a physical module.

2.16.0 adds all A/B functions, velocity, per-track slider range/direction, three CTRL and two AUX/MOD destinations; see [Output routing](OUTPUT-ROUTING.md).
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
The broad completeness estimates below remain unchanged; they are not a
hardware-fidelity score. See the [chain guide](PRESET-CHAINS.md).

Our engineering estimate is **75–85% of the core sequencing functions** and
**45–55% of the full hardware feature set**. These are broad judgment ranges,
not a measured count or a prediction of remaining development time. We weight
functional areas by their musical scope; individual scales, menu entries, and
repeated parameter instances are not counted as equally sized features.
Physical installation, calibration, and firmware-update procedures are outside
this software comparison. Visual resemblance does not establish behavior.

By core sequencing we mean creating two note sequences from the eight shared
stages, controlling traversal/timing, and applying gate/stage variations. The
full comparison also includes the hardware's performance controls, routing,
configuration, preset workflows, and external interfaces.

| Area | Available now | Principal remaining work |
| --- | --- | --- |
| Eight-stage, two-track sequencing | Shared pitch/pulse/gate controls; independent track settings; pitch/gate outputs; per-stage pitch/gate overrides | Additional track options |
| Traversal and timing | All 21 play orders, reverse, length, offset, clock/pulse division, fixed pulse lengths, Swing, reset, internal/external clock, global CLK output division/offset | Hardware clock/reset modes and configuration |
| Gate/stage variation | Four gate types; three ratchet modes; stretching; Stage/Pulse probability; skip; Tempo/Analog/Acid slides; Bipolar/Unipolar/Gate CV with Step/sRamp/pRamp; gate scaling and length Hold | Hardware comparison of live edits and mixed timing |
| Accumulators | Stage-local and track-wide accumulation, four orders, both polarities, separate limits, three triggers, Manual/Auto reset | Remaining reset/routing options; hardware comparison of mixed sources and live changes |
| Pitch/scales | 49 factory scales/chords, 100 user scales with roots, scale locks, eight shortcuts, per-track Scale (User) routing, transpose/range and pitch pre/post/offset modulation | Hardware comparison of modulation thresholds and mixed routes; legacy preset selector/global difference |
| MOD lanes | All eight lanes, Step/Ramp, bipolar/unipolar/gate voltage, independent traversal/timing and mute | Twelve internal destinations implemented; many further destinations and destination-specific display units remain |
| A/B and performance routing | All 16 A/B function types; track/lane assignment, confirmation, velocity, slider range/direction; AUX X/Y/Z with twelve destinations; 28 CTRL destinations | Additional AUX/MOD destinations, ALT+AUX selective randomization; Accum Dir remains unavailable as listed in [CTRL guide](CTRL-KNOBS.md) |
| Performance gestures | Run/reset, track/lane selection, mutes, tap tempo, Loopy, Stage Player, latching | Loopy AUX/MOD routing and Queue options, hardware copy/paste and selective randomization workflows |
| Display and desktop editing | Four HOME views, current edit diagrams, hover help, context menus, vertical dragging, numeric entry, undo, optional grids | Some secondary screens, long-press gestures, glyphs/animations, exact material/detail matching |
| Saving and integration | Rack persistence, parameter automation, 64 internal presets, queued loads, Total Pulses, 32-link chains, repeats/mutes, eight saved chains and Quick Chain | Custom Init, pickup, MIDI, MX expansion and SysEx interchange |

The current 713 parameters include repeated stage/lane controls; that number is
not a count of distinct hardware features. Implemented behavior can still
differ in edge cases or where the manual leaves details unspecified. See
[validation and known interpretations](VALIDATION.md).

## Readiness for feedback

This is a useful **early alpha for Windows testers**. The engine suites, Rack
host/persistence checks, widget callback tests, 79 display states, and recorded
stereo/CV paths pass on the documented setup. The musical baseline also received
an initial user playtest. Broad testing on other computers, long sessions,
different zoom/DPI settings, and interactions with other plugins is still needed.

2.4.0 adds AUX inputs and their hardware-positioned controls/display. The broad
completion estimates above remain unchanged; this is one routing feature area.
It is suitable for experimental patches and
feedback, with saved copies of important work. It is not a complete or
hardware-verified Metropolix replacement.
