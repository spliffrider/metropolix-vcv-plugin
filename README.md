# Metropolix Lab for VCV Rack 2

[![Engine tests](https://github.com/spliffrider/metropolix-vcv-plugin/actions/workflows/engine-tests.yml/badge.svg)](https://github.com/spliffrider/metropolix-vcv-plugin/actions/workflows/engine-tests.yml)

Public source repository: **[spliffrider/metropolix-vcv-plugin](https://github.com/spliffrider/metropolix-vcv-plugin)**. See **[Development and version control](docs/DEVELOPMENT.md)** for branches, imported release tags, and automated checks.

A playable, two-track sequencer based on the Metropolix 1.6 manual, with eight independent modulation lanes, three AUX inputs, a hardware-layout main panel, and optional stage and modulation editors. Version **2.24.0 — public alpha**, for **Windows x64 / Rack Free 2.6.6**. This is an independent implementation in progress, not an official Intellijel product or a complete reproduction of its firmware.

**[Download the Windows plugin](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.24.0/MetropolixLab-2.24.0-win-x64.vcvplugin)** · **[MIDI demo](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.24.0/MIDI-performance.vcv)** · **[Install and test](docs/TESTING.md)** · **[Release notes](https://github.com/spliffrider/metropolix-vcv-plugin/releases/tag/v2.24.0)**

2.24.0 adds [MIDI](docs/MIDI.md): notes/velocity, track and MOD CCs, clock/transport, root and queued program changes, with a real MIDI-CV demo.

2.23.0 adds [preset pickup](docs/PRESET-PICKUP.md): saved pitch/CTRL values, individual takeover, visible targets, exact undo and persistence.

2.22.0 adds [UI setup and confirmation](docs/UI-SETUP.md), external input clock division, shared output delay, MOD assignment/menu gestures and corrected held selection resets.

2.21.0 adds [editing workflows](docs/EDITING-WORKFLOWS.md): ALT+AUX randomize/morph/restore, held and menu copy/paste, custom Init presets and AUX long clear, with native undo.

2.20.0 adds [clock/reset configuration and tuning](docs/CLOCK-RESET.md): explicit clock sources, stopped clock output, Reset/Run/Run-C inputs, per-track Global Reset, Bottom Pitch and tuner.

2.19.0 completed **all 38 AUX/MOD destinations**, including hands-free Loopy, track reset, clock/pulse, slider and slide routing. See [Complete routing](docs/COMPLETE-ROUTING.md).

2.18.0 added **CTRL Accum Dir and six AUX/MOD accumulator gates**, including pause, reverse, invert and targeted reset. See [Accumulator routing](docs/ACCUMULATOR-ROUTING.md).

2.17.0 added **the ALT → TRK menu**, independent uniform pulse/gate sources,
Rest Pitch Hold/Update, transpose −60…+60 and track initialization with undo.
See [Track options](docs/TRACK-OPTIONS.md) and its changing performance demo.

2.16.0 added **all A/B signal functions**, pending-choice confirmation, velocity,
per-track slider range/direction and three CTRL destinations. Its demo patches
velocity into the bass and an independent MOD clock into percussion. See the
[output routing guide](docs/OUTPUT-ROUTING.md).

2.15.0 added **global CLK output division `/1`–`/256` and pulse offset**,
with the hardware ALT → CLK display, mouse editing, menus and paired reset.
See the [clock output guide](docs/CLOCK-OUTPUT.md).

2.14.0 added the **three stage-CV types and Step/sRamp/pRamp transitions**,
with the hardware TYPE screen, whole-pattern ALT edits, gate inversion/rotation,
stored-value undo and a filter performance demo. See the [stage CV guide](docs/STAGE-CV.md).

2.13.0 added **Tempo, Analog and Acid slides**, independent track models,
shortcuts, grouped reset and Analog stopped Stage Player. The new chained demo
compares the three models across two voices. See the [slide guide](docs/SLIDES.md).

2.12.0 added **all 49 factory scales/chords, 100 user scales, scale locks, eight
shortcuts and Scale (User) routing** through MOD, AUX and CTRL. The chord demo
uses stored notes and roots to move through C / Am / F / G. See the
[scale guide](docs/SCALES.md).

2.11.0 added **Loopy live loops, stopped Stage Player, latching and performance
mutes**, with a manual-style display and mouse range menus. The original sequence
keeps advancing underneath temporary loops. Global settings persist; held gestures
clear on reopen. See the [Loopy guide](docs/LOOPY.md).

2.10.0 adds **per-stage pitch and gate-length overrides** for both tracks,
including slider Sync, ALT edits/clears, manual-style OLEDs, and one-step undo.
The track GATE page now edits **Gate Scale** and includes a **Hold** endpoint.
Overrides are stored in presets and chains; older patches keep their sound.
See the [stage override guide](docs/STAGE-OVERRIDES.md).

2.9.0 added **Swing** independently for both note tracks, from 50–78%, with a
manual-style OLED, stage shortcuts, long-press reset, desktop editing and undo.
Swing is also available as a direct CTRL destination and an AUX/MOD offset.
Old patches and preset banks keep straight timing. The new demo alternates
straight and swung phrases and starts stopped. See the [Swing guide](docs/SWING.md).

2.8.0 adds **preset chaining**: 32 links, 1–64 repeats, per-link track/MOD
mutes, eight saved chains, Quick Chain, an OLED list/follow view, desktop menus
and undo. Hold SETUP to open it. The new 32-bar demo changes patterns and mute
states automatically. See the [chain guide](docs/PRESET-CHAINS.md).

2.7.1 refines the desktop controls following tester feedback: gentler sliders,
live parameter readouts during drags, complete OLED descenders, and pitch-slider
lights that follow Rack's Bloom setting. Sequencing and saved patch formats are
unchanged. See [UI validation](docs/VALIDATION.md#ui-polish--271).

2.7.0 adds **64 internal presets**, hardware-style **ALT → LOAD / SAVE** pages,
selective recall, Queue/Total pulse boundaries, and Rack undo. Banks persist
inside each saved Rack patch. Choose **Recall complete pattern** in the preset
menu to include slider/switch positions. See the [preset guide](docs/PRESETS.md).
The 2.8.0 chain workflow builds on these banks.

2.6.0 activates **CTRL 1 and CTRL 2** with 23 destinations, independent track
assignments, direct panel control and bipolar offsets, an OLED assignment page,
hover help, menus, and Rack undo. Older patches keep both knobs unassigned.
See the [CTRL guide](docs/CTRL-KNOBS.md) and try the
[CTRL performance patch](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.6.0/CTRL-performance.vcv).

2.5.0 added **Track accumulation** alongside the original Stage mode. Select a
track, then **ALT → LIM → Mode**, or right-click **TRK 1/2 → Accumulator mode**.
The LIM display also exposes limits, order, polarity, and reset. Old patches
retain Stage mode and their existing parameter/cable IDs. This earlier release
focuses on accumulation and skip interactions.
See [accumulator operation and comparison](docs/HARDWARE-UI.md#accumulator-modes).

2.4.0 made X/Y/Z patchable, with attenuverters, eight destinations, independent track assignments, live display meters, and desktop menus. See the [AUX guide](docs/AUX-INPUTS.md). macOS/Linux plugin downloads are not available yet. The [feature assessment](docs/FEATURE-STATUS.md) distinguishes implemented behavior from unfinished hardware functions.

2.4.1 replaces the standalone manufacturer logo with **sensiburner <3 intellijel**,
using matching bold lowercase lettering and red accents. Rack identifies
sensiburner as the creator. This is a visual/metadata update; musical behavior
and demo patches are unchanged.

Version 2.3.0 refines the supplied hardware panel's lettering, components, markings, and logo. Its 128 × 64 pixel display now includes all four HOME views and manual-based editing diagrams. Hold and drag upward to increase values; downward to decrease. Hover help, context menus, numeric entry, and Rack undo/redo work alongside the hardware layout. See **[docs/HARDWARE-UI.md](docs/HARDWARE-UI.md)** for operation and current limits. The exact visual target remains **[docs/UI-TARGET.md](docs/UI-TARGET.md)**; this revision is closer, with further visual and firmware details still to implement.

## Start here

**[Watch Patches in Motion (72 seconds, 1080p)](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Metropolix-Lab-Patches-in-Motion.mp4)**
shows Metropolix driving complete synth and drum patches. Live pitch traces,
gate indicators, scopes, and close-ups follow the recorded signals. Two
contrasting performances use **Audible Instruments**, **Befaco**, and **VCV Free**:
[Neon Transit](examples/Neon-Transit.vcv) (124 BPM) and
[Broken Orbit](examples/Broken-Orbit.vcv) (100 BPM).
See [patch routing and requirements](docs/PERFORMANCE-PATCHES.md).

**[Watch the 80-second demonstration (1080p MP4)](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Metropolix-Lab-Demo-1080p.mp4)**
— actual Rack playback, with captions, display close-ups, and the MOD editor.
The soundtrack uses the evolving performance below. [Capture and editing workflow](docs/DEMO-VIDEO.md).

For the evolving performance, open **[Night Current / Variations](examples/Musical-variations.vcv)**.
It uses the same voices at 108 BPM, with A natural minor quantization and
Am–F–C–G root movement every two bars. The melody runs forward and backward
at /2 against the bass at /4; its 54-tick cycle moves across bar lines.
Passing-note shifts run on a separate three-bar cycle. The result develops
across phrases instead of repeating the same one-bar melody.

The 32-bar arrangement lasts about 71 seconds: bars 1–8 introduce the voices,
9–12 lift the melody an octave, 13–16 return it, 17–20 drop the bass for a
break, 21–24 bring bass and high melody back, and 25–32 settle down. Half-bar
melody gaps, double ratchets, gate-length changes, and a brief slide into stage
5 add variation throughout. The arrangement repeats, while the different
melodic cycle lengths continue changing its details.

**[Download the performance patch](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Musical-variations.vcv)**
· **[Listen to the full 80-second preview](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Musical-variations-preview.wav)**.
These are additional demo files for the existing 2.4.1 plugin; the plugin
download itself has not changed. The preview has only short boundary fades.

Select **MOD** to explore what each lane does:

| Lane | Role | Destination |
| --- | --- | --- |
| 1 | Eight-bar filter swell | Output A → melody VCF |
| 2 | Am/F/C/G movement, two bars per root | Pitch Pre, both tracks |
| 3 | Three-bar passing-note pattern | Pitch Pre, Track 2 |
| 4 | Register lifts in the 32-bar arrangement | Octave, Track 2 |
| 5 | Double-note fills in alternate bars | Ratchet, Track 2 |
| 6 | Shorter and longer articulation | Gate Length, Track 2 |
| 7 | Half-bar breathing spaces | Mute, Track 2 |
| 8 | Four-bar bass breakdown | Mute, Track 1 |

The public patches start running after loading. Select your AUDIO device;
press RESET to hear the arrangement from the start. Mute individual MOD lanes
in the modulation editor to hear their contribution.

**Night Current** is the new musical demo: **108 BPM, A minor pentatonic**,
with a repeating A1/E2 bass, a plucked answering melody, an eight-bar filter
movement, and quiet stereo echoes. Open **[examples/Musical-demo.vcv](examples/Musical-demo.vcv)**
and select your device in AUDIO. It uses only Metropolix Lab 2.4.1, VCV Free,
and Core. This is an original composition, not a transcription or recreation
of a hardware factory preset.

Listen to the **[24-second Rack recording](examples/Musical-preview.wav)**.
Only a 5 ms start fade and 150 ms end fade were added to the recording.

The bass repeats A–A–A–E on quarter notes. The melody plays
A–A–A–E–A–G–E–C–D, followed by a rest, every bar. Both tracks share the pitch
sliders; Track 1 loops stages 1–2 at /4, while Track 2 uses all eight at /1.
MOD 1 slowly opens the melody filter through output A. The two delays follow
CLK at three and five sixteenth-note intervals, so they follow tempo changes.
There are no random notes, probability changes, or accumulating transpositions.

For listening adjustments, the two top-row VCA MIX modules control left and
right output: channel 1 is bass, 2 is melody, and 3 is echo. Change channel 1
equally on both mixers to keep the bass centered. Raise the second VCF's cutoff
for a brighter melody, or lower channel 3 for less echo. Save your variations
under a new name. The older examples remain available for feature testing.

To try the plugin, download the `.vcvplugin` asset using the link above; no compiler or source checkout is required. In Rack choose **Help → Open user folder**, save/close Rack, place the file in **`plugins-win-x64`**, and restart. Search for **Metropolix Lab**. See the [tester guide](docs/TESTING.md) for the demo and feedback instructions.

For a local development build, the package is `dist/MetropolixLab-2.24.0-win-x64.vcvplugin`. After building it, close Rack and run:

```powershell
.\scripts\install.ps1
```

This copies the package to this computer's Rack user plugin folder. On the next launch, search for **Metropolix Lab** in the module browser. For another computer, use Rack's **Help > Open user folder**, place the package in `plugins-win-x64` (Rack 2.4 or later), and restart Rack.

Open **examples/Hardware-panel-demo.vcv** for the new panel with the accepted modulation demo's musical settings. The first Scope shows A stepping from -2 V to +2 V and B ramping from 0 V to +5 V. Other MOD lanes change track 1's octave, track 2's ratchets, and both tracks' gate length. The adjacent Scope monitors track 1 pitch and gate. See **[docs/MOD-TEST.md](docs/MOD-TEST.md)** for musical checks.

The patch includes two synthesizer voices using only VCV Free and VCV Core: a triangle voice on the left and a slower sine voice on the right. It starts running, but its AUDIO module has no device selected. Select your output device there to hear it. The VCA levels are set to 32%. Metropolix Lab itself outputs pitch, gates, and modulation; the patched oscillators turn those signals into sound.

**examples/Two-voice-demo.vcv** retains the earlier instrument patch without MOD routing. **examples/Hybrid-demo.vcv** is a simpler pitch/gate Scope patch. **examples/Modulation-preview.wav** and **examples/Two-voice-preview.wav** are 12-second recordings made inside Rack; probability can make subsequent runs differ.

To try an example in a separate development profile:

```powershell
.\scripts\preview.ps1
.\scripts\preview.ps1 -Example Two-voice-demo
.\scripts\preview.ps1 -Example Hybrid-demo
.\scripts\preview.ps1 -Example Aux-inputs-demo
```

Rack must be closed first because its Windows version allows one running instance. The preview profile lives in `tmp/rack-user`.

## Playing and editing

1. **RUN** starts or stops playback. **RESET** returns to the starting position and clears accumulated transpositions.
2. Drag the pitch faders, **PULSE COUNT** switches (1–8), and **GATE TYPE** switches (Rest, Single, Multiple, Hold) vertically. Right-click for direct entry or named choices.
3. Select **TRK 1** or **TRK 2**. Pitches, pulse counts, and gate types are shared; track settings and stage edits are independent. Right-click a track button for timing, ratchet, and accumulator settings.
4. Click **EDIT**, choose the printed function on a stage button, then choose a stage. Drag the encoder, screen value, or held stage button to edit it. Slide/Skip stage clicks toggle the chosen stage. PITCH/GATE edit the independent overrides for that track.
5. Right-click **EDIT** to open the stage grid. Numeric and toggle cells accept vertical dragging; a short click toggles ON/OFF. Right-click a stage button for its current value, function selection, and probability/accumulator trigger settings.
6. Click **BPM**, **SCALE**, **ORDER**, **LEN**, **DIV**, **SLIDE**, or **GATE** for its display page. These value buttons also accept a held vertical drag. The encoder/display click switches fields where applicable. **EXIT** returns home.
7. Click **MOD** for modulation. Right-click it to choose MOD 1–8, change lane settings/routing, or open the MOD grid. Each lane retains independent values, transitions, timing, destination, and mute.
8. **ALT**, then **A** or **B**, selects the corresponding output assignment on the display. The optional MOD grid also exposes both output sources directly. A lane can drive an internal destination and a physical output simultaneously.

9. Right-click an **AUX X/Y/Z assignment button** below its knob to choose a destination and target tracks. Drag the knob up/down to attenuvert. Click the button for the AUX display; encoder clicks switch destination/track fields. See the [AUX guide](docs/AUX-INPUTS.md), especially the gate-inversion behavior of Mute.

Opening an optional grid changes the width from 34 HP to 66 HP. Rack finds an available position if that would overlap a neighbor. Closing returns it to 34 HP. View state and musical parameters are saved with the Rack patch. Existing parameter and port IDs are preserved as features append new controls. Earlier development patches migrate their reverse-order setting and shared accumulation limit and start with AUX unassigned.

## Connections

| Port | Behavior |
| --- | --- |
| PITCH 1 / 2 | Quantized 1 V/oct pitch; 0 V is C4 |
| X / Y / Z | Assignable mono CV/gate input; attenuverter per input; Track 1/2/Both targeting |
| GATE 1 / 2 | 5 V gate for the corresponding track |
| CLOCK IN | Automatically selects external clock; one edge is one sixteenth note |
| RESET | Rising edge resets playback and accumulators; a 1 ms clock guard follows |
| CLK OUT | 5 V / 1 ms master clock triggers, global division /1–/256 and offset 0–division−1 |
| OUT A / OUT B | All sixteen routing functions; defaults A = Track 1 CV, B = Track 1 First Pulse; see [Output routing](docs/OUTPUT-ROUTING.md) |

With CLOCK IN connected, RUN arms playback and the next eligible clock edge begins the sequence. With no cable, the internal 0.1–400 BPM clock runs. Track clock division applies after the base clock. Stopping lowers gates and stage CV, while pitch holds. RUN starts a fresh sequence.

MOD lanes advance one stage per divided base-clock pulse, independently of the note tracks' pulse counts. Bipolar lanes span -5 to +5 V, unipolar lanes span 0 to +5 V, and gate lanes output 0 or +5 V. A ramp travels from the previous voltage to the current stage's target across the whole stage. Muted lanes keep advancing but contribute no voltage or internal modulation. Stop silences all MOD outputs; reset restarts their traversal.

The original twelve internal destinations are Octave, Pitch Pre, Pitch Post, Pitch Offset, Gate Length, Probability, Ratchets, Mute, Swing, Scale (User), Velocity and Track Out Swap. Version 2.18.0 also adds Accumulate, Accum Rev, Accum Invert, Accum Mode, Accum Polar and Accum Reset; see [the accumulator routing guide](docs/ACCUMULATOR-ROUTING.md). Version 2.19.0 completes the other twenty [hardware destinations](docs/COMPLETE-ROUTING.md). Modulation offsets the saved base settings. The selected track's panel settings remain unchanged, so removing or muting a route restores their effect. See the validation document for voltage scaling and inferred behavior.

## Implemented

- Two independent tracks, with length, offset, all 21 play orders and reverse direction, division 1–64, fixed pulse length 1–64 or Auto, transpose, gate length, and mute.
- Pulse-count division 1–8; gate stretching; Multiply, Pulse, and Gated ratchet modes; ratchet counts 1–8.
- Per-stage probability with Stage/Pulse targeting; skip; Tempo/Analog/Acid slides; Bipolar/Unipolar/Gate stage CV with Step/sRamp/pRamp transitions.
- Stage-local or track-wide accumulation in scale degrees, with Stage/Pulse/Ratchet triggers, separate positive/negative limits, unipolar/bipolar ranges, Wrap/Pendulum/Random/Hold orders, and Manual/Auto reset.
- All 49 factory scales/chords, 100 custom scales with stored roots, locking, eight Scale/Root shortcuts, and Scale (User) modulation; slider range 1–4 octaves.
- Eight independent eight-stage MOD lanes, per-stage Step/Ramp, bipolar/unipolar/gate voltage, independent timing and play order, 38 internal destinations, track assignment, and assignable A/B outputs.
- Three AUX CV/gate inputs, attenuverters, the same 38 destinations, per-input track assignments, live meters, and logical gate inversion.
- Independent 50–78% Swing per note track, with CTRL control and AUX/MOD offsets, shortcuts, reset, and desktop editing.
- Rack parameter automation, numeric entry, undo/redo, and patch/preset serialization.

## What remains

MIDI notes/CCs, clock/transport, root and program change are available. MX expansion, hardware SysEx interchange and final display/detail polishing remain in development. See [MIDI behavior and manual ambiguities](docs/MIDI.md).

The manual is the behavioral reference. No physical module was available for comparison. Slider quantization, Analog/Acid rate calibration, random-number sequences, and some combinations that the manual does not fully specify remain implementation choices. See **docs/VALIDATION.md** for tested examples, assumptions, and remaining uncertainty.

## Build and test

```powershell
.\scripts\setup-windows.ps1
.\scripts\build.ps1 -Test -Package
.\scripts\test-rack.ps1
.\scripts\test-ui.ps1
.\scripts\test-ui.ps1 -Aux
.\scripts\test-ui.ps1 -Tracks
.\scripts\test-ui.ps1 -TrackAudio
.\scripts\test-ui.ps1 -Outputs
.\scripts\test-ui.ps1 -OutputAudio
.\scripts\test-ui.ps1 -Outputs
.\scripts\test-ui.ps1 -OutputAudio
.\scripts\test-ui.ps1 -StageCv
.\scripts\test-ui.ps1 -CvAudio
.\scripts\test-ui.ps1 -Slides
.\scripts\test-ui.ps1 -SlideAudio
.\scripts\test-ui.ps1 -Display
.\scripts\test-ui.ps1 -Controls
.\scripts\test-ui.ps1 -Workflows
.\scripts\test-ui.ps1 -WorkflowsAudio
.\scripts\test-ui.ps1 -Audio
.\scripts\test-ui.ps1 -Modulation
.\scripts\test-ui.ps1 -Hardware
.\scripts\test-ui.ps1 -Musical
.\scripts\test-ui.ps1 -Variations
```

Regenerate the musical patch with `python scripts/create-musical-demo.py`.
Add `--variations` to regenerate the evolving version. `-Variations` records
80 seconds in Rack to cover the complete 32-bar arrangement and its return.
The musical check records 24 seconds from the actual stereo mix in Rack at
48 kHz, covering the eight-bar filter cycle, and checks signal levels,
DC offset, and finite samples. These are technical checks; listening feedback
is still needed to judge the composition and timbre.

The UI test commands launch an isolated Rack test profile and close it automatically. They require Rack to be closed. They build a separate instrumented DLL; test hooks and the recorder are absent from the release plugin. The hardware test uses the default demo and records both synthesizer voices and A/B through real Rack cables. The display test checks navigation and captures 89 screen states in `tmp/display-qa`.

Setup installs portable MSYS2/MinGW under `%LOCALAPPDATA%\MetropolixDev` and the Rack 2.3.0 SDK under `.tools/Rack-SDK`. It does not modify the system PATH. The SDK preserves Rack 2 ABI compatibility; execution has been checked against installed Rack 2.6.6. Other platforms have not been built or tested.

The engine uses fixed-size state and performs no allocation, file I/O, or UI work in `process()`. UI presentation state and meters are atomic. Musical edits use Rack parameter quantities.

## Source and references

- `src/Sequencer.hpp`, `PlayOrder.hpp`, `GateTiming.hpp`, `Accumulator.hpp`, `Modulation.hpp`: independent musical engine.
- `src/Metropolix.hpp`: Rack parameters, timing adapters, ports, and persistence.
- `src/Metropolix.cpp`, `HardwareUI.hpp`, `HardwareLayout.hpp`: hardware artwork, interaction, and optional editors.
- `src/HardwareDisplay.hpp`, `OledCanvas.hpp`: native-pixel screen views, diagrams, and lettering. `res/README.txt` records artwork/font provenance.
- `tests/`: musical, manual-example, host integration, editor, and audio checks.
- [VCV Rack manual](https://vcvrack.com/manual/), read excluding Licensing as requested.
- [Metropolix manual 1.6, 2025-09-24](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf), cached locally in `reference/`.

For Loopy changes, run `scripts/test-ui.ps1 -Loopy` and
`scripts/test-ui.ps1 -LoopyAudio` with Rack closed. Reports and captures go to
`tmp/loopy-ui-qa` and `tmp/loopy-audio-qa`.
