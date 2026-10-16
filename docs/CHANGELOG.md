# Release history

Historical notes describe what changed in each listed release. For the current
implemented features, missing functions and verification limits, use the
[README](../README.md#feature-status) and [feature status](FEATURE-STATUS.md).
Earlier milestones are recorded in [development history](DEVELOPMENT.md#imported-milestones).
Download packages from [GitHub Releases](https://github.com/spliffrider/metropolix-vcv-plugin/releases).

## 2.71.2

Leaves the bottom panel credit area blank, removing the “sensiburner <3 intellijel” wordmark. The **Metropolism** title and all controls remain in place.

## 2.71.1

Renames the panel title to **Metropolism**, keeping its existing typography and layout. The Rack browser name and internal identifiers remain unchanged, so existing patches continue to load.

## 2.71.0

Adds an optional [Desktop performance view](DESKTOP-PERFORMANCE.md): held number-row/numpad stage keys, two-boundary keyboard latching, mouse range drags, visible track/MOD mutes, transport and live pitch/gate/accumulator feedback. Keyboard control is explicitly armed and releases temporary inputs on focus/view changes. The view is saved, but input ownership and holds never are. Hardware panel, parameter/port IDs and musical preset formats remain unchanged.

## 2.70.0

Adds the firmware 1.6.5 [Mod Trigs outputs](OUTPUT-ROUTING.md), [MX Mod Gate/Mod Trig and MIDI MX](MX.md), including MIDI operation without an expander. The MX screen follows the new manual. Older patches and banks import; files saved in the new formats require 2.70.

## 2.69.0

Refines the [stage editing display](UI-FIDELITY.md): native binary circles, pitch/gate bars, stacked ratchets, probability/accumulator indicators, clearer CV/MOD curves and CV Type. Existing controls, sequencing and saved formats stay unchanged.

## 2.68.0

Refines all four [HOME views](UI-FIDELITY.md), fixes fixed-Bottom-Pitch note names, adds pitch-override dots and the Pre degree cue, and shows remembered panel values and live CV/MOD gauges in the measured layout. Sequencing and saved formats stay unchanged.

## 2.67.0

Adds the [physical pitch-slider popup](UI-FIDELITY.md) and fixes complete Undo for recalled pitch/CTRL value actions. The popup preserves the underlying page and consumes its first dismissal gesture. Saved formats stay unchanged.

## 2.66.0

Refines the [performance fields, gauges and action footer](UI-FIDELITY.md), preserving editing and saved formats.

## 2.65.0

Refines the [settings menu layout](UI-SETUP.md), with native spacing and margins, complete bottom-row descenders and correct right alignment.

## 2.64.0

Refines the [CTRL ownership symbols and LIM captions](UI-FIDELITY.md), preserving existing editing and saved formats.

## 2.63.0

Refines the [scale display and editor](SCALES.md), and confirmed browsing now follows the same hardware bank order as immediate browsing.

## 2.62.0

Refines the [chain display](PRESET-CHAINS.md), with native-reference rows, selections, mutes and Quick Chain repeat feedback.

## 2.61.0

Refines [performance values and encoder cues](UI-FIDELITY.md), with reference-based bold/large lettering and pending tempo feedback.

## 2.60.0

Refines [menu lettering](UI-FIDELITY.md), with proportional glyphs based on native OLED images and complete pending/descender rendering.

## 2.59.0

Refines the [accumulator/LIM display](HARDWARE-UI.md), following its three-row reference while retaining field cycling, CTRL locks and saved formats.

## 2.58.0

Refines [Backup and maintenance lists](BACKUPS.md), preserving file scopes, Cancel-first confirmation and saved formats.

## 2.57.0

Refines [Presets Setup](PRESETS.md), including manual order, browse/edit selection, pending readouts and preserved saved logical setting IDs.

## 2.56.0

Refines [track/MOD/Global/UI/MIDI lists](UI-SETUP.md), their selection/readouts and the Shared octave preview without changing saved formats.

## 2.55.0

Adds the [compact primary SETUP list](UI-SETUP.md), with deliberate settings-schema 34 migration and preserved native shortcuts.

## 2.54.0

Corrects [MOD lane navigation on ORDER/LEN/DIV](MOD-TEST.md), preserving stage voltages/gates, active fields and pending-edit isolation.

## 2.53.0

Corrects [native-style value actions and Undo](UI-SETUP.md), including dormant values, menu binding and locked preferences.

## 2.52.0

Corrects the [locked MOD type grid menu](MOD-TEST.md), preserving unrelated accumulator settings and keeping its lane settings bound to the clicked lane.

## 2.51.0

Protects [paired-page drag Undo](UI-SETUP.md), preserving independent changes to adjacent fields while retaining whole-page long resets.

## 2.50.0

Corrects [MOD effective CV types and OLED TYPE editing](MOD-TEST.md), with destination-controlled ranges, ramps, gates and MIDI while preserving saved preferences.

## 2.49.0

Protects [dependent value edits and confirmation](UI-SETUP.md), preserving raw Undo across type changes and consuming obsolete held previews.

## 2.48.0

Extends [raw drag Undo](UI-SETUP.md) to MOD voltage and masked clock offsets, and corrects effective ALT MOD offsets.

## 2.47.0

Corrects [tiny CV drag history and the effective ALT range](STAGE-CV.md), preserving dormant raw values, normal clicks and confirmation.

## 2.46.0

Corrects [stationary-click history](UI-SETUP.md), preserving external values and genuine toggle/menu actions while retaining small continuous drags.

## 2.45.0

Corrects [long-press reset history](UI-SETUP.md), with an immediate single-action receipt, current raw/scope capture and consumed movement/release.

## 2.44.0

Protects [live CTRL ownership during held edits](CTRL-KNOBS.md), including encoder/OLED/grid cancellation, fresh-drag recovery and preserved partial resets.

## 2.43.0

Protects [A/B previews against external source edits](OUTPUT-ROUTING.md), preserving valid confirmation, menus and undo.

## 2.42.0

Corrects [chain insertion/clear/deletion history](PRESET-CHAINS.md), preserving single-action bank and repeat drags.

## 2.41.0

Corrects [pending-edit conflicts and preview-drag history](UI-SETUP.md), including bulk stage edits and native menus.

## 2.40.0

Refines [user-scale editor entry/exit](SCALES.md), including Root guards, explicit desktop entry and cancellation.

## 2.39.0

Corrects [CTRL assignment confirmation and long clear](CTRL-KNOBS.md), preserving live performance knobs and exact undo.

## 2.38.0

Adds [Slide/Skip pattern and Ratchet gestures](STAGE-PATTERNS.md), including bulk editing/reset, display feedback and exact undo.

## 2.37.0

Adds [SCALE copy/paste](SCALES.md), including user roots, panel/keyboard Alt and paired undo.

## 2.36.0

Adds [PROB/ACCUM paired and ALT gestures](STAGE-PAIRS.md), reset, confirmation and exact undo.

## 2.35.0

Adds [LEN/DIV shortcuts](TIMING-SHORTCUTS.md), field-preserving holds and whole-screen encoder resets.

## 2.34.0

Adds [independent MOD group mute](MOD-GROUP-MUTE.md), preserving individual lane choices and stored group state.

## 2.33.0

Corrects [Scale Lock and Loopy preset storage](PRESET-STORAGE.md), with legacy full/selective bank migration.

## 2.32.0

Adds [global ORDER shortcuts](ORDER-SHORTCUTS.md), paired reset, held gestures and desktop undo.

## 2.31.0

Adds [actual production runtime stress coverage](RUNTIME-VALIDATION.md) on all four platforms.

## 2.30.0

Adds [confirmed maintenance reset/clear](MAINTENANCE.md), bank preservation, native undo and disarmed reopen.

## 2.29.0

Adds [selective Filled and bank backups](BACKUPS.md), preserving unrelated slots/chains with native undo.

## 2.28.0

Adds [Linux and both Mac packages with native host tests](PLATFORMS.md), plus actual Linux Rack rendering/control/audio checks.

## 2.27.0

Refines [panel interaction, MOD display units and EXIT/MX workflows](UI-FIDELITY.md).

## 2.26.0

Adds [portable backups](BACKUPS.md), scoped restore and native undo.

## 2.25.0

Adds [MX gates/clocks and stage assignments](MX.md), including Loopy Stage Player, swap and mute.

## 2.24.0

Adds [MIDI notes/CCs, clock/transport and incoming control](MIDI.md).

## 2.23.0

Adds [preset pickup](PRESET-PICKUP.md), with physical/sounding state, native undo and persistence.

## 2.22.0

Adds [UI setup, pending confirmation, input clock division and output delay](UI-SETUP.md).

## 2.21.0

Adds [editing workflows, custom Init and copy/paste](EDITING-WORKFLOWS.md).

## 2.20.0

Adds [clock/reset configuration, protected tracks, Bottom Pitch and tuner](CLOCK-RESET.md).

## 2.19.0

Completed all 38 AUX/MOD destinations, including Loopy and targeted reset; see [Complete routing](COMPLETE-ROUTING.md).

## 2.18.0

Added all 29 CTRL destinations and six accumulator AUX/MOD routes; see [Accumulator routing](ACCUMULATOR-ROUTING.md).

## 2.17.0

Added ALT → TRK, pulse/gate sources, Rest Pitch, full transpose and track initialization; see [Track options](TRACK-OPTIONS.md).

## 2.16.0

Added all A/B functions, velocity, per-track slider range/direction, three CTRL and two AUX/MOD destinations; see [Output routing](OUTPUT-ROUTING.md).

## 2.15.0

Added global clock output division and offset; see the [clock guide](CLOCK-OUTPUT.md).

## 2.14.0

Added full Bipolar/Unipolar/Gate stage CV, Step/sRamp/pRamp transitions and ALT pattern workflows; see the [CV guide](STAGE-CV.md).

## 2.13.0

Added independent Tempo/Analog/Acid slides, shortcuts, reset and Analog Stage Player; see the [slide guide](SLIDES.md).

## 2.12.0

Added the 49 factory scales/chords, 100 user scales, locks, shortcuts and Scale (User) routing; see the [scale guide](SCALES.md).

## 2.11.0

Added Loopy, Stage Player, latching and performance mutes; see the [Loopy guide](LOOPY.md).

## 2.10.0

Added per-stage pitch/gate overrides, Gate Scale and the track gate Hold endpoint; see the [override guide](STAGE-OVERRIDES.md).

## 2.9.0

Added per-track Swing, a CTRL destination and AUX/MOD offsets; see the [Swing guide](SWING.md).

## 2.8.0

Added 32-link preset chains, repeats, per-link mutes and eight saved chains.

## 2.7.0

Added 64 internal presets, selective recall, queued loading and Total Pulses.

## 2.6.0

Added both CTRL performance knobs with 23 supported destinations.

## 2.5.0

Added Stage/Track accumulator mode and a LIM options display.

