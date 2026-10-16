# Metropolix Lab for VCV Rack 2

[![Engine tests](https://github.com/spliffrider/metropolix-vcv-plugin/actions/workflows/engine-tests.yml/badge.svg)](https://github.com/spliffrider/metropolix-vcv-plugin/actions/workflows/engine-tests.yml)

An independent sequencer by **sensiburner**, inspired by Intellijel Metropolix:
two note tracks, eight modulation lanes and an optional MX expander, with a
hardware-layout panel and desktop editing. This is **not an official Intellijel
product**. It outputs pitch, gates, modulation and MIDI; connect voice modules
or load a demo patch to hear it.

**Current release: [2.70.0 — public alpha](https://github.com/spliffrider/metropolix-vcv-plugin/releases/tag/v2.70.0).** Core sequencing and performance
workflows are implemented. Hardware SysEx and the original factory preset bank
are missing, and equivalence with a physical Metropolix has not been verified.
The behavioral reference is the [firmware 1.6.5 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6.5_2026.09.16.pdf).

[Feature status](#feature-status) · [Download and install](#download-and-install) ·
[Demos](#watch-or-play-a-demo) · [Report feedback](#report-a-bug-or-hardware-difference) ·
[Release history](docs/CHANGELOG.md)

## Feature status

### Implemented in 2.70.0

These features are available for testing. “Implemented” describes the software;
the hardware-comparison and platform limits below still apply.

| Area | Available now |
| --- | --- |
| Two-track sequencing | Eight shared pitch/pulse-count/gate-type stages; independent length, offset, division, transpose, direction and all 21 play orders. [Track guide](docs/TRACK-OPTIONS.md) |
| Groove and stage edits | Skip, probability, ratchets, gate stretching, pitch/gate overrides, independent Swing, Tempo/Analog/Acid slides and stage CV ramps/gates. [Stage controls](docs/HARDWARE-UI.md) |
| Accumulators | Stage and Track modes, trigger choices, positive/negative limits, orders, polarities and reset/routing options. [Accumulator guide](docs/HARDWARE-UI.md#accumulator-modes) |
| Scales | 49 factory scales/chords, 100 custom scales with roots, locks and shortcuts. [Scales](docs/SCALES.md) |
| Modulation and performance | Eight MOD lanes, three AUX inputs, both CTRL knobs, 38 AUX/MOD destinations and 29 CTRL destinations; Loopy, Stage Player, latching, mutes, copy/paste and randomize/morph/restore. [Routing](docs/COMPLETE-ROUTING.md), [Loopy](docs/LOOPY.md) |
| Presets and chains | 64 user presets, selective/queued recall, pickup, Total Pulses, 32-link chains, repeats/mutes, eight saved chains, Quick Chain and plugin JSON backups. [Presets](docs/PRESETS.md), [chains](docs/PRESET-CHAINS.md) |
| Clock, MIDI and MX | Internal/external clock and reset options; assignable A/B including CV Mod Trigs; MIDI notes/CC/clock/transport/control; nine MX modes and independent MIDI MX output. [Clock](docs/CLOCK-RESET.md), [MIDI](docs/MIDI.md), [MX](docs/MX.md) |
| Rack interaction | Hover help, live adjustment readouts, click-and-drag editing, right-click menus, numeric entry, undo/redo, parameter automation and patch persistence. [Controls](docs/HARDWARE-UI.md) |

### Missing functions

- **Hardware SysEx preset/bank import and export:** not supported. JSON backups
  belong to this plugin and cannot be exchanged with the physical module.
- **The hardware's original factory preset bank:** not included. The supplied
  musical demos are original patterns; saving and recalling your own presets works.

Hardware power, calibration, firmware updates and physical USB/MX wiring are
outside the Rack workflow. Host MIDI ports and the virtual MX provide the
software connections.

### Implemented, but still needs verification

- **Hardware behavior:** no physical module was available for comparison.
  Timing, skip/accumulator combinations, live preset changes, modulation gains,
  random behavior, slider thresholds and Analog/Acid slide calibration need
  hardware-owner feedback. Visual resemblance does not establish equivalence.
- **Conflicting references:** some 1.6.5 trigger details differ between the PDF
  manual and firmware history. This build's choices are recorded in
  [feature status](docs/FEATURE-STATUS.md#hardware-fidelity-and-reference-choices).
- **Platforms and stability:** Windows and Linux have actual Rack display and
  signal-path checks; both Mac architectures have native loading/engine tests,
  with human Mac GUI/audio testing still open. Rack Free 2.6.6 is the tested
  baseline. Rack Pro, other Rack/OS versions, unusual DPI/zoom settings and long
  sessions with other plugins need broader testing. [Platform details](docs/PLATFORMS.md)

For detailed scope and test evidence, see [feature status](docs/FEATURE-STATUS.md),
[manual coverage](docs/MANUAL-COVERAGE.md) and [validation](docs/VALIDATION.md).
Please report unexpected behavior in an implemented feature as a possible bug.

## Download and install

**GitHub download only: Metropolix Lab is not currently in the VCV Library.**
It cannot yet be installed or updated through Rack's Library menu. Library
distribution remains separate work; use the packages below for now.

No compiler or GitHub account is needed. Download the **`.vcvplugin`** for your
OS and the CPU architecture of your Rack application. GitHub's “Source code”
ZIP/TAR downloads are for developers, not installation.

| Rack platform | Plugin download | Folder inside Rack's user folder |
| --- | --- | --- |
| Windows x64 | [Windows package](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MetropolixLab-2.70.0-win-x64.vcvplugin) | `plugins-win-x64` |
| Linux x64 | [Linux package](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MetropolixLab-2.70.0-lin-x64.vcvplugin) | `plugins-lin-x64` |
| Intel Mac / Intel Rack under Rosetta | [Intel Mac package](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MetropolixLab-2.70.0-mac-x64.vcvplugin) | `plugins-mac-x64` |
| Native Apple Silicon Rack | [Apple Silicon package](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MetropolixLab-2.70.0-mac-arm64.vcvplugin) | `plugins-mac-arm64` |

1. In Rack, choose **Help → Open user folder** and keep that folder open.
2. Save your patch and close Rack.
3. Put the intact `.vcvplugin` in the matching folder above. **Do not extract it.**
4. Restart Rack and search for **Metropolix Lab** or **MX** in the module browser.

Rack extracts and loads the package on startup, following the
[official manual installation procedure](https://vcvrack.com/manual/Installing#installing-plugins-not-available-on-the-vcv-library).
For a future GitHub update, repeat these steps with the newer package.
[Release notes and checksums](https://github.com/spliffrider/metropolix-vcv-plugin/releases/tag/v2.70.0) ·
[Installation help and test guide](docs/TESTING.md)

## Watch or play a demo

| Demo | Video / patch | Other plugins required |
| --- | --- | --- |
| Simple scale progression | [Rack patch](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Scale-progression.vcv) | VCV Free and Rack Core |
| Pulse Pressure — 138 BPM, bass, lead, drums, breakdown and drop | [115-second video](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Metropolix-Lab-Pulse-Pressure.mp4) · [Rack patch](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Pulse-Pressure.vcv) | VCV Free, Audible Instruments, Befaco, Valley |
| Pulse Pressure — sidechain off/on comparison | [45-second video](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Metropolix-Lab-Pulse-Pressure-Sidechain.mp4) · [Rack patch](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Pulse-Pressure-Sidechain.vcv) | The Pulse Pressure plugins, plus Bogaudio |

Install a demo's dependencies, open its `.vcv` file, select your audio device in
**AUDIO**, and enable **RUN** if stopped. Some patches start running on load.
The demos use separate voice, drum and effect modules; Bogaudio PRESSOR supplies
the sidechain compression. [Pulse Pressure routing](docs/PULSE-PRESSURE.md) ·
[Sidechain routing](docs/SIDECHAIN-DEMO.md) · [More example patches](examples/)

## Playing and editing

- **RUN** starts/stops; **RESET** restarts the sequence and accumulators.
- Drag sliders and value fields **up to increase, down to decrease**. Hover for
  help or right-click for direct entry and named options.
- Select **TRK 1**, **TRK 2** or **MOD** before editing that track/lane. Use
  **EDIT** and a stage function for per-stage settings; **ALT** selects secondary
  functions. Click the encoder/display to switch fields, then **EXIT** to return.
- Right-click **EDIT/MOD** for optional stage/MOD grids. Save the Rack patch to
  keep its patterns, internal presets and chains.

[Panel and control guide](docs/HARDWARE-UI.md) · [CTRL knobs](docs/CTRL-KNOBS.md) ·
[AUX inputs](docs/AUX-INPUTS.md) · [Setup](docs/UI-SETUP.md)

## Report a bug or hardware difference

[Open a feedback issue](https://github.com/spliffrider/metropolix-vcv-plugin/issues/new?template=test-feedback.md).
You do not need to own the hardware to report a problem. Check the feature
status above, then include:

- Plugin version, Rack version and Free/Pro, OS/CPU, sample rate and zoom/DPI.
- Steps to reproduce, what you expected and what happened.
- A small `.vcv` patch; for crashes, attach Rack's `log.txt` if available.
- For hardware differences, the firmware version, manual page or a recording
  of the observed behavior, if you have them.

Skip/accumulator interactions and preset chaining are implemented, so a
repeatable problem there is useful bug feedback. Missing functions above are
known feature gaps; hardware mismatches and uncertain behavior are also welcome.

## Build and test

Developers can use the configured Windows toolchain:

```powershell
.\scripts\setup-windows.ps1
.\scripts\build.ps1 -Test -Package
.\scripts\test-rack.ps1
.\scripts\test-production.ps1
.\scripts\test-ui.ps1
```

Application tests use isolated profiles and require Rack to be closed.
See [development and platform builds](docs/DEVELOPMENT.md), the
[manual-to-test map](docs/MANUAL-COVERAGE.md) and [validation](docs/VALIDATION.md)
for targeted checks and evidence.
[Source code](https://github.com/spliffrider/metropolix-vcv-plugin/tree/main/src) ·
[Tests](https://github.com/spliffrider/metropolix-vcv-plugin/tree/main/tests) ·
[Release history](docs/CHANGELOG.md) · [Artwork/font provenance](res/README.txt)
