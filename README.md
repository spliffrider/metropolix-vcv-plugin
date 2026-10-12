# Metropolix Lab for VCV Rack 2

A playable, two-track sequencer based on the Metropolix 1.6 manual, with eight independent modulation lanes, a hardware-style main panel, and expanded stage and modulation editors. Version **2.1.0**, built and tested on **Windows x64 / Rack Free 2.6.6**. This is an independent implementation in progress, not an official Intellijel product or a complete reproduction of its firmware.

## Start here

The ready-to-install package is `dist/MetropolixLab-2.1.0-win-x64.vcvplugin`. Close Rack and run:

```powershell
.\scripts\install.ps1
```

This copies the package to this computer's Rack user plugin folder. On the next launch, search for **Metropolix Lab** in the module browser. For another computer, use Rack's **Help > Open user folder**, place the package in `plugins-win-x64` (Rack 2.4 or later), and restart Rack.

Open **examples/Modulation-demo.vcv** for the new test patch. The first Scope shows A stepping from -2 V to +2 V and B ramping from 0 V to +5 V. Other MOD lanes change track 1's octave, track 2's ratchets, and both tracks' gate length. The adjacent Scope monitors track 1 pitch and gate. See **[docs/MOD-TEST.md](docs/MOD-TEST.md)** for the complete playtest guide.

The patch includes two synthesizer voices using only VCV Free and VCV Core: a triangle voice on the left and a slower sine voice on the right. It starts running, but its AUDIO module has no device selected. Select your output device there to hear it. The VCA levels are set to 32%. Metropolix Lab itself outputs pitch, gates, and modulation; the patched oscillators turn those signals into sound.

**examples/Two-voice-demo.vcv** retains the earlier instrument patch without MOD routing. **examples/Hybrid-demo.vcv** is a simpler pitch/gate Scope patch. **examples/Modulation-preview.wav** and **examples/Two-voice-preview.wav** are 12-second recordings made inside Rack; probability can make subsequent runs differ.

To try an example in a separate development profile:

```powershell
.\scripts\preview.ps1
.\scripts\preview.ps1 -Example Two-voice-demo
.\scripts\preview.ps1 -Example Hybrid-demo
```

Rack must be closed first because its Windows version allows one running instance. The preview profile lives in `tmp/rack-user`.

## Playing and editing

1. **RUN** starts or stops playback. **RESET** returns to the starting position and clears accumulated transpositions.
2. Drag the eight pitch sliders. Drag **PULSE COUNT** values to set stage durations from 1 to 8 pulses. Click a **GATE TYPE** to select REST, SINGLE, MULTI, or HOLD.
3. Select **TRACK 1** or **TRACK 2**. Pitches, pulse counts, and gate types are shared; track settings and stage edits are independent.
4. Click a numbered stage button. The selector below the transport chooses RATCHETS, PROBABILITY, ACCUM, SLIDE, SKIP, or CV; the adjacent value edits that stage.
5. **OPEN STAGE EDITOR** shows every stage value for the selected track. Drag numeric cells vertically, right-click for numeric entry/reset, and click ON/OFF cells to toggle. Both views use the same Rack parameters and support undo/redo.
6. **TRACK SETUP** exposes direction, pulse-count division, fixed pulse length, gate stretching, ratchet mode, and accumulator options. **ORDER** offers all 21 play orders.
7. Right-click a numbered stage button for that stage's **probability target** (Stage/Pulse) and **accumulator trigger** (Stage/Pulse/Ratchet). These options are also available through the module's context menu.
8. **MOD LANES** opens the modulation editor. Select MOD 1–8, set its eight voltages, and click STEP/RAMP to choose each stage's transition. Each lane has its own length, offset, division, play order, and mute. Right-click a MOD selector for reverse direction and fixed pulse length.
9. Choose a MOD lane's **internal destination** and target track(s). Choose **OUTPUT A SOURCE** and **OUTPUT B SOURCE** independently: either track's stage CV or any MOD lane. A lane can drive both an internal destination and a physical output.

Expanding changes the width from 40 HP to 72 HP. Rack finds an available position if the extra width would overlap a neighbor. Closing returns it to 40 HP. View state and musical parameters are saved with the Rack patch. Earlier development patches migrate their reverse-order setting and shared accumulation limit.

## Connections

| Port | Behavior |
| --- | --- |
| PITCH 1 / 2 | Quantized 1 V/oct pitch; 0 V is C4 |
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
- Rack parameter automation, numeric entry, undo/redo, and patch/preset serialization.

## What remains

This build does **not** yet include the remaining hardware MOD destinations, AUX X/Y/Z and CTRL assignments, Loopy, swing, per-stage pitch/gate-length overrides, track-wide accumulator mode, AUX/MOD-only accumulator reset, acid/analog slide models, the full scale library or user scales, hardware preset banks/chains, MIDI, MX, or SysEx compatibility.

The manual is the behavioral reference. No physical module was available for comparison. Slider quantization, the linear slide, random-number sequences, and some combinations that the manual does not fully specify remain implementation choices. See **docs/VALIDATION.md** for tested examples, assumptions, and remaining uncertainty.

## Build and test

```powershell
.\scripts\setup-windows.ps1
.\scripts\build.ps1 -Test -Package
.\scripts\test-rack.ps1
.\scripts\test-ui.ps1
.\scripts\test-ui.ps1 -Audio
.\scripts\test-ui.ps1 -Modulation
```

The three UI test commands launch an isolated Rack test profile and close it automatically. They require Rack to be closed. They build a separate instrumented DLL; test hooks and the recorder are absent from the release plugin. The modulation test records both synthesizer voices and measures A/B through real Rack cables.

Setup installs portable MSYS2/MinGW under `%LOCALAPPDATA%\MetropolixDev` and the Rack 2.3.0 SDK under `.tools/Rack-SDK`. It does not modify the system PATH. The SDK preserves Rack 2 ABI compatibility; execution has been checked against installed Rack 2.6.6. Other platforms have not been built or tested.

The engine uses fixed-size state and performs no allocation, file I/O, or UI work in `process()`. UI presentation state and meters are atomic. Musical edits use Rack parameter quantities.

## Source and references

- `src/Sequencer.hpp`, `PlayOrder.hpp`, `GateTiming.hpp`, `Accumulator.hpp`, `Modulation.hpp`: independent musical engine.
- `src/Metropolix.hpp`: Rack parameters, timing adapters, ports, and persistence.
- `src/Metropolix.cpp`: main panel and expanded editor.
- `tests/`: musical, manual-example, host integration, editor, and audio checks.
- [VCV Rack manual](https://vcvrack.com/manual/), read excluding Licensing as requested.
- [Metropolix manual 1.6, 2025-09-24](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf), cached locally in `reference/`.
