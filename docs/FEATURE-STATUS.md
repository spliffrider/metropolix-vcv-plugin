# Feature status — 2.48.0 public alpha

Assessment date: 2026-10-14. The reference is the
[Metropolix firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
This implementation has not been compared with a physical module.

2.48.0 extends [raw drag Undo](UI-SETUP.md) to MOD voltage and masked clock offsets, and corrects effective ALT MOD offsets.
2.47.0 corrects [tiny CV drag history and the effective ALT range](STAGE-CV.md), preserving dormant raw values, normal clicks and confirmation.
2.46.0 corrects [stationary-click history](UI-SETUP.md), preserving external values and genuine toggle/menu actions while retaining small continuous drags.
2.45.0 corrects [long-press reset history](UI-SETUP.md), with an immediate single-action receipt, current raw/scope capture and consumed movement/release.
2.44.0 protects [live CTRL ownership during held edits](CTRL-KNOBS.md), including encoder/OLED/grid cancellation, fresh-drag recovery and preserved partial resets.
2.43.0 protects [A/B previews against external source edits](OUTPUT-ROUTING.md), preserving valid confirmation, menus and undo.
2.42.0 corrects [chain insertion/clear/deletion history](PRESET-CHAINS.md), preserving single-action bank and repeat drags.
2.41.0 corrects [pending-edit conflicts and preview-drag history](UI-SETUP.md), including bulk stage edits and native menus.
2.40.0 refines [user-scale editor entry/exit](SCALES.md), including Root guards, explicit desktop entry and cancellation.
2.39.0 corrects [CTRL assignment confirmation and long clear](CTRL-KNOBS.md), preserving live performance knobs and exact undo.
2.38.0 adds [Slide/Skip pattern and Ratchet gestures](STAGE-PATTERNS.md), including bulk editing/reset, display feedback and exact undo.
2.37.0 adds [SCALE copy/paste](SCALES.md), including user roots, panel/keyboard Alt and paired undo.
2.36.0 adds [PROB/ACCUM paired and ALT gestures](STAGE-PAIRS.md), reset, confirmation and exact undo.
2.35.0 adds [LEN/DIV shortcuts](TIMING-SHORTCUTS.md), field-preserving holds and whole-screen encoder resets.
2.34.0 adds [independent MOD group mute](MOD-GROUP-MUTE.md), preserving individual lane choices and stored group state.
2.33.0 corrects [Scale Lock and Loopy preset storage](PRESET-STORAGE.md), with legacy full/selective bank migration.
2.32.0 adds [global ORDER shortcuts](ORDER-SHORTCUTS.md), paired reset, held gestures and desktop undo.
2.31.0 adds [actual production runtime stress coverage](RUNTIME-VALIDATION.md) on all four platforms.
2.30.0 adds [confirmed maintenance reset/clear](MAINTENANCE.md), bank preservation, native undo and disarmed reopen.
2.29.0 adds [selective Filled and bank backups](BACKUPS.md), preserving unrelated slots/chains with native undo.
2.28.0 adds [Linux and both Mac packages with native host tests](PLATFORMS.md), plus actual Linux Rack rendering/control/audio checks.
2.27.0 refines [panel interaction, MOD display units and EXIT/MX workflows](UI-FIDELITY.md).
2.26.0 adds [portable backups](BACKUPS.md), scoped restore and native undo.
2.25.0 adds [MX gates/clocks and stage assignments](MX.md), including Loopy Stage Player, swap and mute.
2.24.0 adds [MIDI notes/CCs, clock/transport and incoming control](MIDI.md).
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
| Eight-stage, two-track sequencing | Shared pitch/pulse/gate controls; independent pulse/gate sources, Rest Pitch, full transpose, track menu/init and per-track Global Reset; pitch/gate outputs and stage overrides | Hardware timing and MIDI comparisons |
| Traversal and timing | All 21 play orders, reverse, eight global order/direction shortcuts, paired reset, LEN/DIV held shortcuts and whole-screen resets, length, offset, clock/pulse division, fixed pulse lengths, Swing, reset, internal/external clock with input division, global CLK division/offset and Always/Running output, explicit clock source, Reset/Run/Run-C | Hardware clock/timing comparisons |
| Gate/stage variation | Four gate types; three ratchet modes; stretching; Stage/Pulse probability; skip; Tempo/Analog/Acid slides; Bipolar/Unipolar/Gate CV with Step/sRamp/pRamp; gate scaling and length Hold | Hardware comparison of live edits and mixed timing |
| Accumulators | Stage-local and track-wide accumulation, four orders, both polarities, separate limits, three triggers, Manual/Auto reset; pause/reverse/invert/mode/polarity/targeted reset routing | Hardware comparison of mixed sources and live changes |
| Pitch/scales | 49 factory scales/chords, 100 user scales with roots, scale locks, eight shortcuts, per-track Scale (User) routing, Bottom Pitch, tuner, transpose/range and pitch pre/post/offset modulation | Hardware comparison of modulation thresholds and mixed routes |
| MOD lanes | All eight lanes, Step/Ramp, bipolar/unipolar/gate voltage, independent traversal/timing, individual mute and a separate group mute; all 38 hardware destinations; gate On/Off readouts and global scope | Hardware comparisons of modulation gains and live edits |
| A/B and performance routing | All 16 A/B function types; track/lane assignment, confirmation, velocity, slider range/direction; AUX X/Y/Z with all 38 destinations; all 29 CTRL destinations | Hardware comparisons |
| Performance gestures | Run/reset, track/lane selection, mutes, tap tempo, Loopy, Stage Player, latching, Loopy routing, ALT Slide/Skip pattern, Ratchet and PROB/ACCUM edits and resets, ALT randomize/morph/restore and stage/track/lane copy/paste | Hardware comparisons of copy/paste and randomization |
| Display and desktop editing | Four HOME views, current edit diagrams, hover/pending help, destination units, context menus, vertical dragging, numeric entry, undo, submenu back, long EXIT Sync and optional grids with adjacent MX | Some secondary screens, long-press gestures, glyphs/animations, exact material/detail matching |
| Saving and integration | Rack persistence, parameter automation, 64 internal presets with Scale Lock and Loopy settings, queued loads, Total Pulses, 32-link chains, repeats/mutes, eight saved chains, Quick Chain, custom Init, pitch/CTRL Thru pickup, MIDI notes/CCs/clock/transport/root/program change, seven MX expander modes, scoped JSON backups | Hardware SysEx interchange |

The current 787 parameters include repeated stage/lane controls; that number is
not a count of distinct hardware features. Implemented behavior can still
differ in edge cases or where the manual leaves details unspecified. See
[validation and known interpretations](VALIDATION.md).

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
