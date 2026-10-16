# Feature status — 2.71.0 public alpha

Assessment date: 2026-10-16. The reference is the
[Metropolix firmware 1.6.5 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6.5_2026.09.16.pdf).
This implementation has not been compared with a physical module.

Release-by-release notes have moved to the [changelog](CHANGELOG.md).

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
| Eight-stage, two-track sequencing | Shared pitch/pulse/gate controls; independent pulse/gate sources, Rest Pitch, full transpose, track menu/init and per-track Global Reset; pitch/gate outputs and stage overrides | Hardware timing and MIDI comparisons |
| Traversal and timing | All 21 play orders, reverse, eight global order/direction shortcuts, paired reset, LEN/DIV held shortcuts and whole-screen resets, length, offset, clock/pulse division, fixed pulse lengths, Swing, reset, internal/external clock with input division, global CLK division/offset and Always/Running output, explicit clock source, Reset/Run/Run-C | Hardware clock/timing comparisons |
| Gate/stage variation | Four gate types; three ratchet modes; stretching; Stage/Pulse probability; skip; Tempo/Analog/Acid slides; Bipolar/Unipolar/Gate CV with Step/sRamp/pRamp; gate scaling and length Hold | Hardware comparison of live edits and mixed timing |
| Accumulators | Stage-local and track-wide accumulation, four orders, both polarities, separate limits, three triggers, Manual/Auto reset; pause/reverse/invert/mode/polarity/targeted reset routing | Hardware comparison of mixed sources and live changes |
| Pitch/scales | 49 factory scales/chords, 100 user scales with roots, scale locks, eight shortcuts, per-track Scale (User) routing, Bottom Pitch, tuner, transpose/range and pitch pre/post/offset modulation | Hardware comparison of modulation thresholds and mixed routes |
| MOD lanes | All eight lanes, Step/Ramp, bipolar/unipolar/gate voltage, independent traversal/timing, individual mute and a separate group mute; all 38 hardware destinations; gate On/Off readouts and global scope | Hardware comparisons of modulation gains and live edits |
| A/B and performance routing | All 17 A/B function types, including 5 ms CV Mod Trigs; track/lane assignment, confirmation, velocity, slider range/direction; AUX X/Y/Z with all 38 destinations; all 29 CTRL destinations | Hardware comparisons |
| Performance gestures | Run/reset, track/lane selection, mutes, tap tempo, Loopy, Stage Player, latching, Loopy routing, ALT Slide/Skip pattern, Ratchet and PROB/ACCUM edits and resets, ALT randomize/morph/restore and stage/track/lane copy/paste | Hardware comparisons of copy/paste and randomization |
| Display and desktop editing | Optional [desktop performance view](DESKTOP-PERFORMANCE.md) with armed keyboard stages, mouse range selection, visible mutes and live pitch/gate/accumulator readouts; four HOME views, current edit diagrams, hover/pending help, destination units, context menus, vertical dragging, numeric entry, undo, submenu back, long EXIT Sync and optional grids with adjacent MX | Unobserved glyphs/animation phases, full hardware behavior and photographic material comparisons |
| Saving and integration | Rack persistence, parameter automation, 64 internal presets with Scale Lock and Loopy settings, queued loads, Total Pulses, 32-link chains, repeats/mutes, eight saved chains, Quick Chain, custom Init, pitch/CTRL Thru pickup, MIDI notes/CCs/clock/transport/root/program change, nine MX expander modes and independent MIDI MX CC/Note output, scoped JSON backups | Hardware SysEx interchange |

The current 789 parameters include repeated stage/lane controls; that number is
not a count of distinct hardware features. Implemented behavior can still
differ in edge cases or where the manual leaves details unspecified. See
[validation and known interpretations](VALIDATION.md), and the [manual coverage map](MANUAL-COVERAGE.md).

## Missing or outside the Rack workflow

- **Hardware SysEx interchange is not implemented.** Plugin JSON backups cannot
  be loaded into the physical module, and hardware SysEx banks cannot be imported.
- **The original hardware factory preset bank is not included.** Supplied demo
  patterns are original; 64 user slots and the preset/chaining workflows work.
- **VCV Library distribution is not available.** Install the GitHub packages
  manually using the [quick start](../README.md#download-and-install).
- Physical power, calibration, firmware flashing and USB/expander wiring are
  hardware procedures. Rack uses host MIDI ports and an adjacent virtual MX.

## Hardware fidelity and reference choices

No physical Metropolix was available for comparison. Implemented features can
still differ in timing, skip/accumulator interactions, live preset transitions,
modulation gains, random sequences, slider thresholds and Analog/Acid slew rates.
The panel/display follows the references, with unobserved details still inferred.
These are verification limits, not a list of absent controls.

The firmware 1.6.5 references also conflict. The
[PDF manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6.5_2026.09.16.pdf)
describes 5 ms Mod Trigs and A/B Track CV assignment; the
[official firmware history](https://github.com/intellijel/firmware/blob/master/docs/Metropolix.html)
describes 10 ms triggers and A/B MOD-lane assignment, plus changed stopped
Stage Num behavior. Version 2.70.0 follows the PDF-based trigger/routing
implementation and retains the documented stopped Stage Player behavior.
Hardware-owner comparisons are needed to resolve these differences. See
[A/B routing](OUTPUT-ROUTING.md#mod-trigs--firmware-165), [MX](MX.md) and
[MIDI interpretations](MIDI.md#clock-transport-and-incoming-control).

The project does not use a percentage-complete score: implemented functions,
hardware equivalence and platform stability are separate questions.

## Readiness for feedback

This is a useful **early alpha for Windows, Linux and Mac testers**. Windows
and Linux have actual Rack callback, 181-state display and stereo/CV-path checks.
All four packages pass native engine/persistence checks and a six-virtual-minute
soak of the actual stripped production binary, including repeated state restores
and fresh instances. This is single-threaded simulation; long human sessions
and cross-plugin interactions still need feedback.
Mac GUI/audio interaction still needs human feedback. See [Platform packages](PLATFORMS.md). The musical baseline also received
an initial user playtest. Broad testing on other computers, long sessions,
different zoom/DPI settings, and interactions with other plugins is still needed.
