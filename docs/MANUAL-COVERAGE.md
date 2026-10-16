# Manual workflow coverage — firmware 1.6.5 / plugin 2.70.0

Reference: [Metropolix firmware 1.6 manual, 2025-09-24](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
Page numbers below are printed page numbers. This is a map of implemented
software workflows and their test entry points, not a hardware equivalence
score. The table identifies implemented scope and representative tests; it does not claim that every hardware edge case has an independent oracle.

The table below retains the supplied1.6 manual's printed page numbers for its
existing workflows. This build also implements the newer [1.6.5 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6.5_2026.09.16.pdf):
A/B Mod Trigs (p117), MX Mod Gate/Mod Trig (p124) and independent MIDI MX channel
and CC/Note output (pp125/182–183). [Firmware165RackTests.hpp](../tests/Firmware165RackTests.hpp),
[Mx165WidgetQa.hpp](../tests/Mx165WidgetQa.hpp), [OutputTriggerTests.hpp](../tests/OutputTriggerTests.hpp),
[MxModGateTests.hpp](../tests/MxModGateTests.hpp) and [MidiMxTests.hpp](../tests/MidiMxTests.hpp)
cover the new bindings, native image, controls, MIDI lifecycle and migrations.
See [MX](MX.md), [MIDI](MIDI.md) and [routing](OUTPUT-ROUTING.md).

Native tests run through `scripts/test-rack.ps1`; portable engine suites run
through `scripts/build.ps1 -Test` or the 25 engine CI jobs. Windows application
groups below use their named switches, for example `scripts/test-ui.ps1 -Tracks`
or `scripts/test-ui.ps1 -Pending`. The `-Group` switch specifically tests MOD
group mute; it is not a placeholder for another group name. All packages also load both
public models in the stripped production host. Linux additionally runs the
eighteen application groups listed in its test script; a Windows group being
listed here does not imply a separate Linux GUI run for that group.

| Manual area | Software workflow | Primary evidence / application group | Guide |
| --- | --- | --- | --- |
| Panel, sliders and switches, pp.17–27 | 34 HP controls, shared stages, four gate types, encoder, EXIT and desktop dragging | [ManualConformanceTests.cpp](../tests/ManualConformanceTests.cpp), [WidgetQa.hpp](../tests/WidgetQa.hpp), [UiPolishQa.hpp](../tests/UiPolishQa.hpp); Display, Polish, Hardware | [Panel](HARDWARE-UI.md) |
| Four HOME views, pp.28–31 | Positions, notes, remembered panel values, pickup/override indicators and MOD activity | [HomeDisplayWidgetQa.hpp](../tests/HomeDisplayWidgetQa.hpp), [HomeDisplayReference.hpp](../tests/HomeDisplayReference.hpp), [DisplayQa.hpp](../tests/DisplayQa.hpp); Pending, Display, Fidelity | [Display](UI-FIDELITY.md) |
| Track/lane selection and menus, pp.32–37 | Independent track settings, pulse/gate sources, rest pitch, initialization and MOD lane settings | [TrackOptionsRackTests.hpp](../tests/TrackOptionsRackTests.hpp), [TrackOptionsWidgetQa.hpp](../tests/TrackOptionsWidgetQa.hpp), [SecondaryMenuWidgetQa.hpp](../tests/SecondaryMenuWidgetQa.hpp); Tracks, Pending, Fidelity | [Track options](TRACK-OPTIONS.md) |
| Run/reset, pp.38–39 | Internal/external transport, reset timing and configurable run/reset modes | [ClockConfigTests.cpp](../tests/ClockConfigTests.cpp), [ClockConfigRackTests.hpp](../tests/ClockConfigRackTests.hpp); Config, ConfigAudio | [Clock/reset](CLOCK-RESET.md) |
| Loopy, Stage Player and mutes, pp.39–46 | One/two-boundary loops, stopped audition, latching, note/MOD mutes and group mute | [LoopyRackTests.hpp](../tests/LoopyRackTests.hpp), [ModGroupRackTests.hpp](../tests/ModGroupRackTests.hpp); Loopy, LoopyAudio, Group | [Loopy](LOOPY.md), [MOD mute](MOD-GROUP-MUTE.md) |
| Global/preset/MIDI/advanced settings, pp.47–58 | Setup navigation, timing source/options, load behavior, Encoder Confirm, pickup and display/UI settings | [SetupRackTests.hpp](../tests/SetupRackTests.hpp), [SetupNavigationRackTests.hpp](../tests/SetupNavigationRackTests.hpp), [PrimarySetupWidgetQa.hpp](../tests/PrimarySetupWidgetQa.hpp), [SecondaryMenuWidgetQa.hpp](../tests/SecondaryMenuWidgetQa.hpp), [PresetRackTests.hpp](../tests/PresetRackTests.hpp), [MidiRackTests.hpp](../tests/MidiRackTests.hpp); SetupOptions, SetupAudio, Pending | [Setup](UI-SETUP.md), [Pickup](PRESET-PICKUP.md) |
| Factory menu/reset, pp.59/189 | Scoped custom Init, factory settings reset and confirmed preset clearing | [FactoryRackTests.hpp](../tests/FactoryRackTests.hpp), [BackupWidgetQa.hpp](../tests/BackupWidgetQa.hpp); Backup, maintenance reopen | [Maintenance](MAINTENANCE.md) |
| BPM, p.60–61 | Internal BPM, tap tempo, external input division and clock-source selection | [ClockConfigRackTests.hpp](../tests/ClockConfigRackTests.hpp), [SetupRackTests.hpp](../tests/SetupRackTests.hpp), [DisplayQa.hpp](../tests/DisplayQa.hpp); Config, SetupOptions, Display | [Clock/reset](CLOCK-RESET.md) |
| Scale selection/shortcuts/lock/user editing, pp.62–68 | Factory/user scales, roots, shortcuts, locked-bank modulation, copy/paste and editor entry/exit | [ScaleRackTests.hpp](../tests/ScaleRackTests.hpp), [ScaleWidgetQa.hpp](../tests/ScaleWidgetQa.hpp), [ScaleCopyWidgetQa.hpp](../tests/ScaleCopyWidgetQa.hpp); Scales, ScaleAudio, ScaleCopy | [Scales](SCALES.md) |
| ORDER, pp.70–74 | Twenty-one traversals, reverse, global shortcuts and paired reset | [ManualConformanceTests.cpp](../tests/ManualConformanceTests.cpp), [OrderRackTests.hpp](../tests/OrderRackTests.hpp), [OrderWidgetQa.hpp](../tests/OrderWidgetQa.hpp); Order | [ORDER](ORDER-SHORTCUTS.md) |
| LEN/DIV, pp.75–78 | Stages, offset, pulse length, clock/pulse division, held shortcuts and reset | [TimingWidgetQa.hpp](../tests/TimingWidgetQa.hpp), [SequencerTests.cpp](../tests/SequencerTests.cpp); Timing | [Timing](TIMING-SHORTCUTS.md) |
| Swing, p.79 | Divided-track swing, division interaction, persistence and controller ownership | [SwingTests.cpp](../tests/SwingTests.cpp), [SwingRackTests.hpp](../tests/SwingRackTests.hpp); Swing, SwingAudio | [Swing](SWING.md) |
| Slide, pp.80–81 | Tempo/Analog/Acid behavior, per-stage switches and routing | [SlideTests.cpp](../tests/SlideTests.cpp), [SlideRackTests.hpp](../tests/SlideRackTests.hpp); Slides, SlideAudio | [Slides](SLIDES.md) |
| Track Gate, pp.82–84 | Length, Hold, Gate Scale, held shortcuts, paired reset and partial CTRL ownership | [OverrideRackTests.hpp](../tests/OverrideRackTests.hpp), [OverrideWidgetQa.hpp](../tests/OverrideWidgetQa.hpp), [LiveCtrlWidgetQa.hpp](../tests/LiveCtrlWidgetQa.hpp); Overrides, OverrideAudio, CtrlEditor | [Overrides](STAGE-OVERRIDES.md) |
| Slide/Skip stage editing, pp.85–88 | Selection, binary editing, ALT rotation/inversion, whole-pattern reset | [StagePatternWidgetQa.hpp](../tests/StagePatternWidgetQa.hpp), [ManualConformanceTests.cpp](../tests/ManualConformanceTests.cpp), [BinaryStageWidgetQa.hpp](../tests/BinaryStageWidgetQa.hpp); StagePattern, Pending | [Patterns](STAGE-PATTERNS.md) |
| Pitch/Gate overrides, pp.89–92 | Selected/ALT relative editing, Sync, special inherited/rest/hold values and undo | [OverrideTests.cpp](../tests/OverrideTests.cpp), [OverrideWidgetQa.hpp](../tests/OverrideWidgetQa.hpp), [BarStageWidgetQa.hpp](../tests/BarStageWidgetQa.hpp); Overrides, OverrideAudio, Pending | [Overrides](STAGE-OVERRIDES.md) |
| Ratchets/stretching, pp.93–97/166–168 | Three modes, stretch interactions, ALT relative edits and reset | [ManualConformanceTests.cpp](../tests/ManualConformanceTests.cpp), [StagePatternWidgetQa.hpp](../tests/StagePatternWidgetQa.hpp); StagePattern | [Patterns](STAGE-PATTERNS.md) |
| Probability/accumulation stages, pp.98–102 | Stage/Pulse probability, three accumulation triggers, paired resets and ALT editing | [AccumulatorTests.cpp](../tests/AccumulatorTests.cpp), [StagePairWidgetQa.hpp](../tests/StagePairWidgetQa.hpp), [BarStageWidgetQa.hpp](../tests/BarStageWidgetQa.hpp); StagePair, Accumulator, Pending | [Stage pairs](STAGE-PAIRS.md) |
| Stage CV/TYPE, pp.103–106/136 | Bipolar/unipolar/gate values, Step/sRamp/pRamp, velocity source and gate-pattern editing | [StageCvTests.cpp](../tests/StageCvTests.cpp), [StageCvRackTests.hpp](../tests/StageCvRackTests.hpp), [CurveStageWidgetQa.hpp](../tests/CurveStageWidgetQa.hpp), [BinaryStageWidgetQa.hpp](../tests/BinaryStageWidgetQa.hpp); StageCv, CvAudio, Pending | [Stage CV](STAGE-CV.md) |
| LOAD/SAVE and storage scope, pp.107–114/169–170 | Sixty-four presets, global/musical scope, queue timing, physical pickup, Filled/Bank8/All clearing and custom Init | [PresetRackTests.hpp](../tests/PresetRackTests.hpp), [PresetStorageRackTests.hpp](../tests/PresetStorageRackTests.hpp), [PickupRackTests.hpp](../tests/PickupRackTests.hpp); Presets, PresetAudio, Storage, Pickup, Backup | [Presets](PRESETS.md), [Storage](PRESET-STORAGE.md) |
| A/B, pp.115–119 | Seventeen source families / 40 packed choices, assignment, mandatory previews and external-edit conflicts | [OutputRoutingTests.cpp](../tests/OutputRoutingTests.cpp), [OutputRoutingRackTests.hpp](../tests/OutputRoutingRackTests.hpp), [OutputPreviewWidgetQa.hpp](../tests/OutputPreviewWidgetQa.hpp); Outputs, OutputAudio, OutputArrangement | [Outputs](OUTPUT-ROUTING.md) |
| CLK, pp.120–121 | Output division/offset, running/always scope and reset | [ClockOutputTests.cpp](../tests/ClockOutputTests.cpp), [ClockOutputRackTests.hpp](../tests/ClockOutputRackTests.hpp); Clock, ClockAudio | [Clock out](CLOCK-OUTPUT.md) |
| MX, pp.122–125 | Four HP software expander, nine modes and independent MIDI MX, eight software output jacks and reciprocal links | [MxTests.cpp](../tests/MxTests.cpp), [MxRackTests.hpp](../tests/MxRackTests.hpp); Mx, MxAudio, Fidelity | [MX](MX.md) |
| LIM/accumulator options, pp.126–135 | Stage/Track accumulation, limits, four orders, polarity, Auto/Manual reset and routed changes | [AccumulatorTests.cpp](../tests/AccumulatorTests.cpp), [AccumulatorRoutingRackTests.hpp](../tests/AccumulatorRoutingRackTests.hpp); AccumRouting, AccumAudio, Display | [Accumulator routing](ACCUMULATOR-ROUTING.md) |
| MOD lanes, pp.137–145 | Eight lanes, independent traversal/timing, Step/Ramp/gates, internal/output assignments and destination units | [ModulationTests.cpp](../tests/ModulationTests.cpp), [ExtendedRoutingRackTests.hpp](../tests/ExtendedRoutingRackTests.hpp), [FidelityWidgetQa.hpp](../tests/FidelityWidgetQa.hpp); Modulation, Fidelity, Group | [Routing](COMPLETE-ROUTING.md), [Display](UI-FIDELITY.md) |
| CTRL, pp.146–153 | Twenty-nine destinations plus None, direct/offset semantics, preview/confirmation, clear and active ownership takeover | [ControlTests.cpp](../tests/ControlTests.cpp), [RackIntegrationTests.cpp](../tests/RackIntegrationTests.cpp), [CtrlEditorWidgetQa.hpp](../tests/CtrlEditorWidgetQa.hpp), [LiveCtrlWidgetQa.hpp](../tests/LiveCtrlWidgetQa.hpp); Controls, CtrlEditor | [CTRL](CTRL-KNOBS.md) |
| AUX/MOD destinations, pp.154–164 | Three AUX inputs, attenuation/inversion/assignment and all thirty-eight destinations | [AuxTests.cpp](../tests/AuxTests.cpp), [ExtendedRoutingTests.cpp](../tests/ExtendedRoutingTests.cpp), [ExtendedRoutingRackTests.hpp](../tests/ExtendedRoutingRackTests.hpp); Aux, ExtendedRouting, RoutingAudio | [AUX](AUX-INPUTS.md), [Routing](COMPLETE-ROUTING.md) |
| Copy/paste and randomization, pp.164–165/179 | Stage/track/lane copy, ALT randomize/morph/restore and independent scale clipboard | [WorkflowRackTests.hpp](../tests/WorkflowRackTests.hpp), [WorkflowWidgetQa.hpp](../tests/WorkflowWidgetQa.hpp), [ScaleCopyWidgetQa.hpp](../tests/ScaleCopyWidgetQa.hpp); Workflows, WorkflowsAudio, ScaleCopy | [Workflows](EDITING-WORKFLOWS.md) |
| Preset chaining, pp.171–178 | Thirty-two links, repeats/mutes, eight saved chains, Quick Chain, follow, editing and complete undo | [ChainTests.cpp](../tests/ChainTests.cpp), [ChainRackTests.hpp](../tests/ChainRackTests.hpp), [ChainHistoryWidgetQa.hpp](../tests/ChainHistoryWidgetQa.hpp); Chains, ChainAudio | [Chains](PRESET-CHAINS.md) |
| MIDI, pp.180–185 | Host MIDI note/velocity/CC/clock/transport/root/program changes, learning, panic and loopback | [MidiTests.cpp](../tests/MidiTests.cpp), [MidiRackTests.hpp](../tests/MidiRackTests.hpp), [MidiWidgetQa.hpp](../tests/MidiWidgetQa.hpp); Midi, MidiAudio | [MIDI](MIDI.md) |

## What remains outside the software checks

- Output/clock event timing, undocumented route gains, simultaneous-source
  combinations, random-order behavior and Analog/Acid slew calibration require
  a physical reference. The current interpretations are in VALIDATION.md.
- Display evidence has explicit boundaries. Complete native masks cover selected
  scale, chain, LIM, performance, popup, binary/bar-stage and CV Type screens.
  Filtered HOME and curve pictures support only the documented clear regions.
  Unobserved glyphs, animation phases and photographic material appearance
  remain interpretations. See [display evidence](UI-FIDELITY.md).
- Full-scene screenshots captured in one frame can lag a state change. Keep
  direct OLED captures separate and schedule frame-delayed scene captures when
  showing a new page (the chain evidence has this limitation for some early
  screenshots; its following view was inspected).
- Hardware calibration, power/expander installation, USB backpack/solo wiring
  and firmware update procedures are hardware-only. Software MX/MIDI behavior
  is covered separately; hardware SysEx transfer still needs a verified payload
  specification. Portable JSON backups are intentionally their own format.
- Mac GUI/audio human playtests, long sessions, unusual zoom/DPI/Rack versions
  and other-plugin interaction still require tester feedback.

## How to use this map

Start at the guide for a workflow, then run the listed application switch or
portable suite. Tests of implementation invariants and tests using independent
manual image/reference data are different kinds of evidence; a passing suite
does not establish an undocumented hardware rule. Historical check counts in
the guides describe their dated builds, while current CI and release receipts
describe the downloadable build.

Hardware installation/power material (pp5–14), calibration (pp186–188), firmware
updates (p190), firmware history (pp191–203) and electrical specifications
(p204) do not create equivalent procedures inside Rack. Reset (p189) has the
software maintenance workflow listed above. MIDI uses host ports; the MX
connection uses Rack's adjacent-module expander link. Portable JSON backup
is deliberately separate from hardware SysEx.
