# Feature status — 2.4.1 public alpha

Assessment date: 2026-10-12. The reference is the
[Metropolix firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
This implementation has not been compared with a physical module.

2.4.1 changes the creator wordmark and Rack brand/author metadata only. Its
functionality and completeness estimates remain those of 2.4.0.

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
| Eight-stage, two-track sequencing | Shared pitch/pulse/gate controls; independent track settings; pitch/gate outputs | Stage pitch and gate-length overrides; additional track options |
| Traversal and timing | All 21 play orders, reverse, length, offset, clock/pulse division, fixed pulse lengths, reset, internal/external clock | Swing; hardware clock/reset modes and configuration |
| Gate/stage variation | Four gate types; three ratchet modes; stretching; Stage/Pulse probability; skip; slide; stage CV | Other slide models; gate scaling; full stage-CV type behavior |
| Accumulators | Stage-local accumulation, four orders, both polarities, separate limits, three triggers, Manual/Auto reset | Track-wide mode and remaining reset/routing options |
| Pitch/scales | Seven scales, root, transpose, slider range, pitch pre/post/offset modulation | Full scale library, user scales, scale shortcuts/locking and associated workflows |
| MOD lanes | All eight lanes, Step/Ramp, bipolar/unipolar/gate voltage, independent traversal/timing and mute | Eight internal destinations implemented; many further destinations and destination-specific display units remain |
| A/B and performance routing | A/B can carry either track CV or any MOD lane; AUX X/Y/Z attenuverters and eight destinations; routes can target either/both tracks | Full A/B signal choices, additional AUX destinations, ALT+AUX selective randomization and CTRL knob assignments |
| Performance gestures | Run/reset, track/lane selection, mutes, tap tempo | Loopy, stage-player/latching workflows, hardware copy/paste and selective randomization workflows |
| Display and desktop editing | Four HOME views, current edit diagrams, hover help, context menus, vertical dragging, numeric entry, undo, optional grids | Some secondary screens, long-press gestures, glyphs/animations, exact material/detail matching |
| Saving and integration | Rack patch/module-preset persistence and parameter automation | Hardware preset banks/chains, MIDI, MX expansion and SysEx interchange |

The current 413 parameters include repeated stage/lane controls; that number is
not a count of 413 distinct hardware features. Implemented behavior can still
differ in edge cases or where the manual leaves details unspecified. See
[validation and known interpretations](VALIDATION.md).

## Readiness for feedback

This is a useful **early alpha for Windows testers**. The engine suites, Rack
host/persistence checks, widget callback tests, 22 display states, and recorded
stereo/CV paths pass on the documented setup. The musical baseline also received
an initial user playtest. Broad testing on other computers, long sessions,
different zoom/DPI settings, and interactions with other plugins is still needed.

2.4.0 adds AUX inputs and their hardware-positioned controls/display. The broad
completion estimates above remain unchanged; this is one routing feature area.
It is suitable for experimental patches and
feedback, with saved copies of important work. It is not a complete or
hardware-verified Metropolix replacement.
