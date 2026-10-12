# Metropolix Lab for VCV Rack 2

[![Engine tests](https://github.com/spliffrider/metropolix-vcv-plugin/actions/workflows/engine-tests.yml/badge.svg)](https://github.com/spliffrider/metropolix-vcv-plugin/actions/workflows/engine-tests.yml)

Public source repository: **[spliffrider/metropolix-vcv-plugin](https://github.com/spliffrider/metropolix-vcv-plugin)**. See **[Development and version control](docs/DEVELOPMENT.md)** for branches, imported release tags, and automated checks.

A playable, two-track sequencer based on the Metropolix 1.6 manual, with eight independent modulation lanes, three AUX inputs, a hardware-layout main panel, and optional stage and modulation editors. Version **2.4.1 — public alpha**, for **Windows x64 / Rack Free 2.6.6**. This is an independent implementation in progress, not an official Intellijel product or a complete reproduction of its firmware.

**[Download the Windows plugin](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/MetropolixLab-2.4.1-win-x64.vcvplugin)** · **[AUX demo patch](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Aux-inputs-demo.vcv)** · **[Install and test](docs/TESTING.md)** · **[Release notes](https://github.com/spliffrider/metropolix-vcv-plugin/releases/tag/v2.4.1)**

2.4.0 made X/Y/Z patchable, with attenuverters, eight destinations, independent track assignments, live display meters, and desktop menus. See the [AUX guide](docs/AUX-INPUTS.md). macOS/Linux plugin downloads are not available yet. The [feature assessment](docs/FEATURE-STATUS.md) distinguishes implemented behavior from unfinished hardware functions.

2.4.1 replaces the standalone manufacturer logo with **sensiburner <3 intellijel**,
using matching bold lowercase lettering and red accents. Rack identifies
sensiburner as the creator. This is a visual/metadata update; musical behavior
and demo patches are unchanged.

Version 2.3.0 refines the supplied hardware panel's lettering, components, markings, and logo. Its 128 × 64 pixel display now includes all four HOME views and manual-based editing diagrams. Hold and drag upward to increase values; downward to decrease. Hover help, context menus, numeric entry, and Rack undo/redo work alongside the hardware layout. See **[docs/HARDWARE-UI.md](docs/HARDWARE-UI.md)** for operation and current limits. The exact visual target remains **[docs/UI-TARGET.md](docs/UI-TARGET.md)**; this revision is closer, with further visual and firmware details still to implement.

## Start here

To try the plugin, download the `.vcvplugin` asset using the link above; no compiler or source checkout is required. In Rack choose **Help → Open user folder**, save/close Rack, place the file in **`plugins-win-x64`**, and restart. Search for **Metropolix Lab**. See the [tester guide](docs/TESTING.md) for the demo and feedback instructions.

For a local development build, the package is `dist/MetropolixLab-2.4.1-win-x64.vcvplugin`. After building it, close Rack and run:

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
4. Click **EDIT**, choose the printed function on a stage button, then choose a stage. Drag the encoder, screen value, or held stage button to edit it. Slide/Skip stage clicks toggle the chosen stage. Stage pitch/gate-length overrides are still unimplemented.
5. Right-click **EDIT** to open the stage grid. Numeric and toggle cells accept vertical dragging; a short click toggles ON/OFF. Right-click a stage button for its current value, function selection, and probability/accumulator trigger settings.
6. Click **BPM**, **SCALE**, **ORDER**, **LEN**, **DIV**, **SLIDE**, or **GATE** for its display page. These value buttons also accept a held vertical drag. The encoder/display click switches fields where applicable. **EXIT** returns home.
7. Click **MOD** for modulation. Right-click it to choose MOD 1–8, change lane settings/routing, or open the MOD grid. Each lane retains independent values, transitions, timing, destination, and mute.
8. **ALT**, then **A** or **B**, selects the corresponding output assignment on the display. The optional MOD grid also exposes both output sources directly. A lane can drive an internal destination and a physical output simultaneously.

9. Right-click an **AUX X/Y/Z assignment button** below its knob to choose a destination and target tracks. Drag the knob up/down to attenuvert. Click the button for the AUX display; encoder clicks switch destination/track fields. See the [AUX guide](docs/AUX-INPUTS.md), especially the gate-inversion behavior of Mute.

Opening an optional grid changes the width from 34 HP to 66 HP. Rack finds an available position if that would overlap a neighbor. Closing returns it to 34 HP. View state and musical parameters are saved with the Rack patch. The previous 404 parameter IDs and nine existing patchable ports are preserved; AUX appends nine parameters and three inputs. Earlier development patches migrate their reverse-order setting and shared accumulation limit and start with AUX unassigned.

## Connections

| Port | Behavior |
| --- | --- |
| PITCH 1 / 2 | Quantized 1 V/oct pitch; 0 V is C4 |
| X / Y / Z | Assignable mono CV/gate input; attenuverter per input; Track 1/2/Both targeting |
| GATE 1 / 2 | 5 V gate for the corresponding track |
| CLOCK IN | Automatically selects external clock; one edge is one sixteenth note |
| RESET | Rising edge resets playback and accumulators; a 1 ms clock guard follows |
| CLK OUT | 5 V / 1 ms pulse on each base clock tick |
| OUT A / OUT B | Assignable to Track 1 CV, Track 2 CV, or MOD 1–8; default to the two tracks' CV |

With CLOCK IN connected, RUN arms playback and the next eligible clock edge begins the sequence. With no cable, the internal 0.1–400 BPM clock runs. Track clock division applies after the base clock. Stopping lowers gates and stage CV, while pitch holds. RUN starts a fresh sequence.

MOD lanes advance one stage per divided base-clock pulse, independently of the note tracks' pulse counts. Bipolar lanes span -5 to +5 V, unipolar lanes span 0 to +5 V, and gate lanes output 0 or +5 V. A ramp travels from the previous voltage to the current stage's target across the whole stage. Muted lanes keep advancing but contribute no voltage or internal modulation. Stop silences all MOD outputs; reset restarts their traversal.

The eight available internal destinations are Octave, Pitch Pre, Pitch Post, Pitch Offset, Gate Length, Probability, Ratchets, and Mute. Modulation offsets the saved base settings. The selected track's panel settings remain unchanged, so removing or muting a route restores their effect. See the validation document for voltage scaling and inferred behavior.

## Implemented

- Two independent tracks, with length, offset, all 21 play orders and reverse direction, division 1–64, fixed pulse length 1–64 or Auto, transpose, gate length, and mute.
- Pulse-count division 1–8; gate stretching; Multiply, Pulse, and Gated ratchet modes; ratchet counts 1–8.
- Per-stage probability with Stage/Pulse targeting; skip; a linear, tempo-relative slide; bipolar CV.
- Per-stage accumulation in scale degrees, with Stage/Pulse/Ratchet triggers, separate positive/negative limits, unipolar/bipolar ranges, Wrap/Pendulum/Random/Hold orders, and Manual/Auto reset.
- Seven scales: chromatic, major, minor, Dorian, Mixolydian, major and minor pentatonic; chromatic root; slider range 1–4 octaves.
- Eight independent eight-stage MOD lanes, per-stage Step/Ramp, bipolar/unipolar/gate voltage, independent timing and play order, eight internal destinations, track assignment, and assignable A/B outputs.
- Three AUX CV/gate inputs, attenuverters, the same eight destinations, per-input track assignments, live meters, and logical gate inversion.
- Rack parameter automation, numeric entry, undo/redo, and patch/preset serialization.

## What remains

This build does **not** yet include the remaining hardware MOD/AUX destinations, CTRL assignments, ALT+AUX selective randomization, Loopy, swing, per-stage pitch/gate-length overrides, track-wide accumulator mode, AUX/MOD-only accumulator reset, acid/analog slide models, the full scale library or user scales, hardware preset banks/chains, MIDI, MX, or SysEx compatibility.

The manual is the behavioral reference. No physical module was available for comparison. Slider quantization, the linear slide, random-number sequences, and some combinations that the manual does not fully specify remain implementation choices. See **docs/VALIDATION.md** for tested examples, assumptions, and remaining uncertainty.

## Build and test

```powershell
.\scripts\setup-windows.ps1
.\scripts\build.ps1 -Test -Package
.\scripts\test-rack.ps1
.\scripts\test-ui.ps1
.\scripts\test-ui.ps1 -Aux
.\scripts\test-ui.ps1 -Display
.\scripts\test-ui.ps1 -Audio
.\scripts\test-ui.ps1 -Modulation
.\scripts\test-ui.ps1 -Hardware
```

The UI test commands launch an isolated Rack test profile and close it automatically. They require Rack to be closed. They build a separate instrumented DLL; test hooks and the recorder are absent from the release plugin. The hardware test uses the default demo and records both synthesizer voices and A/B through real Rack cables. The display test checks navigation and captures 20 screen states in `tmp/display-qa`.

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
