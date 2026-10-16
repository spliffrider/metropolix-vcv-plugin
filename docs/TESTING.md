# Metropolix Lab 2.70.0 — public alpha quick start

This is an independent VCV Rack sequencer in development, not an official
Intellijel product. Feedback on playability, confusing controls, and bugs is
welcome. See Known limitations for remaining fidelity checks.

For the short current feature list, missing functions and verification limits,
start with [README feature status](../README.md#feature-status). Version history
is in the [changelog](CHANGELOG.md). Metropolix Lab is currently distributed
through GitHub, with manual installation below; it is not in the VCV Library.

## Try the new firmware 1.6.5 features

1. ALT → stage 6 opens MX. Drag up/down to select Mod Gate or Mod Trig and click
   the encoder to apply it. These modes expose one gate/trigger per MOD lane.
2. For MIDI without an expander, choose a MIDI output device from the module's
   right-click MIDI menu. On the MX screen, ALT selects MIDI CH; drag to 1–16,
   then press the encoder. Right-click for CC or Note output. SETUP exposes
   these same global settings.
3. Give a MOD lane alternating low/high stage values and run the sequence.
   CC 102–109 or notes 35–42 follow the eight MX signals. Check that stopping or
   changing channel releases held messages. Use Release MIDI notes for panic.
4. ALT → A/B, select Mod Trigs and Track 1/2, and confirm. Patch that output to a
   gate destination. It follows rising track CV with 5 ms pulses, independently
   of the ordinary note gate. Older saved A/B routes keep their meaning.

Current backups require 2.70; earlier banks and settings still import. See
[MX](MX.md), [MIDI](MIDI.md) and [compatibility](BACKUPS.md).

## Downloads

- [Windows x64 plugin](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MetropolixLab-2.70.0-win-x64.vcvplugin)
- [Linux x64 plugin](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MetropolixLab-2.70.0-lin-x64.vcvplugin)
- [Intel Mac plugin](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MetropolixLab-2.70.0-mac-x64.vcvplugin)
- [Apple Silicon plugin](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MetropolixLab-2.70.0-mac-arm64.vcvplugin)
- [MX guide](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MX.md) · [MIDI guide](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MIDI.md)
- [MX percussion demo](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/MX-performance.vcv)
- [Example Bank 8 backup](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Backup-bank-8.json) — replaces Bank 8, including five empty slots
- [Example full backup](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Backup-example.json)
- [Ready-patched two-voice demo](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Aux-inputs-demo.vcv)
- [Release page and checksums](https://github.com/spliffrider/metropolix-vcv-plugin/releases/tag/v2.70.0)

Download the `.vcvplugin` file from **Assets**. GitHub's automatic “Source code”
ZIP is for building the plugin yourself; it is not the installable plugin.

Choose the package matching your OS **and Rack's CPU architecture**. On Apple
Silicon, Intel Rack under Rosetta uses `mac-x64`; native Rack uses `mac-arm64`.
Windows and Linux have actual Rack Free 2.6.6 rendering/audio-path checks.
Both Mac packages pass native engine, persistence and production loading tests;
a human Mac GUI/audio playtest, other Rack versions and Rack Pro remain open.
See [Platform packages](PLATFORMS.md) for precise coverage.

## Install

1. In Rack, choose **Help → Open user folder** and keep that folder open.
2. Save your patch, then close Rack.
3. Copy the matching `.vcvplugin` into **`plugins-win-x64`**, **`plugins-lin-x64`**,
   **`plugins-mac-x64`** or **`plugins-mac-arm64`** inside that user folder.
   Keep the file intact; Rack extracts it.
4. Start Rack and search for **Metropolix Lab** in the module browser.

This follows the [official third-party plugin installation procedure](https://vcvrack.com/manual/Installing#installing-plugins-not-available-on-the-vcv-library).
There is no separate installer or GitHub account requirement to download.

## Hear something immediately

Open **[Track-options-performance.vcv](../examples/Track-options-performance.vcv)**,
select AUDIO and press RUN. Four scenes vary pulse/gate sources and Rest Pitch.
ALT → TRK opens the hardware menu; disable the chain before editing. See
[Track options](TRACK-OPTIONS.md).

Open **[Output-routing-performance.vcv](../examples/Output-routing-performance.vcv)**,
select your AUDIO device and press RUN. A drives bass velocity; B clocks a
percussion envelope independently. Four chained scenes add variations and a
break. Try ALT → A/B: a question mark stays pending until encoder confirmation.
See the [output routing guide](OUTPUT-ROUTING.md).

Open **[Stage-CV-performance.vcv](../examples/Stage-CV-performance.vcv)**, select
your AUDIO device and press RUN. A/B drive the two filters through voltage
ramps and gate patterns. Try the [stage CV guide](STAGE-CV.md) for TYPE,
transitions, ALT edits, gate rotation/inversion and undo.

For slides, open **[Slide-performance.vcv](../examples/Slide-performance.vcv)**, select your
AUDIO device and press RUN. Four chained patterns compare Tempo, Analog and Acid
on two voices. Try the [slide guide](SLIDES.md) for amount/type controls,
shortcuts, resets and stopped Analog audition.

For custom chords, open **[Scale-progression.vcv](../examples/Scale-progression.vcv)**, select your
AUDIO device and press RUN. MOD 1 moves both voices through C / Am / F / G
custom triads. CTRL 1 adds a live scale offset. See the [scale guide](SCALES.md).

For live looping, try **[Loopy-performance.vcv](../examples/Loopy-performance.vcv)**
and the [Loopy guide](LOOPY.md).

For stage overrides, open **[Stage-overrides-performance.vcv](../examples/Stage-overrides-performance.vcv)**,
choose your AUDIO device and level, then press RUN. Select a track and click
EDIT → PITCH/GATE; try Sync, ALT changes, clear, Rest and Hold, Gate Scale
and undo. The four presets change melody and articulation over shared sliders.
See the [override guide](STAGE-OVERRIDES.md).

For the automatic arrangement, open **[Chain-performance.vcv](../examples/Chain-performance.vcv)**,
select your AUDIO device, and watch the OLED follow seven links over 32 bars.
Hold SETUP to open Preset Chains. Disable the chain before editing or loading
individual presets. Try repeats, per-link mutes, pause/resume, reset, undo and
saving/reopening the patch. See the [chain guide](PRESET-CHAINS.md).

For the evolving version, open [`Musical-variations.vcv`](../examples/Musical-variations.vcv).
Its 32-bar arrangement demonstrates all eight MOD lanes: changing roots and
melody, octave answers, ratchet fills, articulation, filter movement, and breaks.
Press RESET and allow about 71 seconds to hear the whole arrangement.

For a more structured musical example, open
[`Musical-demo.vcv`](../examples/Musical-demo.vcv): **Night Current**, an original
108 BPM A-minor-pentatonic groove with bass, melody, and stereo delay. It works
with the existing 2.4.1 plugin and the same free dependencies. Select an AUDIO
device to hear it. This patch is provided separately from the original 2.4.1
package; no plugin update is required. The README explains its controls.

1. Ensure **VCV Free** modules are installed; the demo uses VCV Free and Core.
2. Download and open `Aux-inputs-demo.vcv` using Rack's **File → Open**.
3. In its AUDIO module, select your audio driver, device, and output channels.
   The portable demo deliberately has no device selected.
4. The demo starts running. Begin at a comfortable listening level. Save a copy
   under a new name before experimenting.

Metropolix Lab generates pitch, gates, and modulation, not audio by itself. The
demo's oscillators, envelopes, and VCAs turn its signals into two audible voices.
The A/B Scope shows a stepped signal from -2 to +2 V and a ramp from 0 to +5 V.
In the AUX demo, A also feeds X to transpose Track 1, B feeds Y to transpose
Track 2 downward, and Track 1's gate feeds Z to mask Track 2. Turn X/Y to noon
or clear Z's assignment to hear the difference. See the
[AUX guide](https://github.com/spliffrider/metropolix-vcv-plugin/blob/main/docs/AUX-INPUTS.md).

If there is no sound, check AUDIO's device/output selection, RUN, and the Scope.
Moving Scope traces with silent audio generally point to audio-device or
downstream patch routing rather than a stopped sequencer.

## Try the preset banks

Download [Preset-performance.vcv](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Preset-performance.vcv).
The red bank has four related patterns. Select a filled slot on LOAD and let the
current eight-bar phrase finish, or use Queue Pulses = 16/32 for quicker changes.
Check that melody, MOD and gate changes start together. Try cancellation,
save/overwrite, undo, and saving/reopening your Rack patch. Compare Pitch/Switch
Off with **Recall complete pattern**. Full instructions: [Preset guide](PRESETS.md).

## Try Presets Setup — 2.70.0

Open SETUP → Presets Setup. Browse the six hardware settings, then the two
RACK settings; browsing should leave their values unchanged. Click Queue
Pulses to edit, drag up/down, and try Undo/Redo. Enable Enc. Confirm in UI
Setup, then preview an Init slot: the pending bank/slot and `?` should be
visible. EXIT should cancel the pending value. Try a selected encoder hold
to reset only that preference. Save and reopen a patch on this screen: the
same setting should be selected in browse mode. See [Presets](PRESETS.md).

## Try the performance knobs

Download [CTRL-performance.vcv](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/CTRL-performance.vcv).
CTRL 1 changes Track 2 pulse counts; CTRL 2 adds ratchet fills. Both start at
noon, preserving the original evolving arrangement. Right-click either for
assignments and exact centre. See the [CTRL guide](CTRL-KNOBS.md) for ranges,
direct parameter ownership, and all 29 destinations.

## Compare accumulator modes

Download [Accumulator-modes.vcv](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Accumulator-modes.vcv).
It presents Stage mode on Track 1 and Track mode on Track 2, with the same
four-note phrase and a +1 increment on stage 1. Right-click TRK 1/2 to change
Accumulator mode, or use ALT → LIM. See the
[step-by-step comparison and skip exercises](HARDWARE-UI.md#accumulator-modes).

## A useful 15-minute test

- Move pitch sliders, pulse-count switches, and gate switches. Check that the
  sequence reacts and both tracks remain independent where appropriate.
- Try small slider adjustments: pulse/gate switches should move one position
  at a time. A held drag should show its current value until release. Check
  **View → Bloom** on the pitch lights and lettering such as **Ping Pong** or
  **Step** at your usual zoom. Report your Rack sensitivity and Windows scaling
  if controls still feel too fast or text looks broken.
- Hold BPM/LEN/DIV or a numeric stage value and drag up/down. Up should increase;
  down should decrease. Try fine dragging, right-click entry, and undo/redo.
- Select EDIT, choose a function, then select a stage. Try ratchets, probability,
  accumulation, slide, and skip. Hover controls when unsure.
- Click or vertically drag the encoder on HOME to visit all four views. Check
  readability at your normal Rack zoom and Windows scaling.
- Select MOD, choose a lane, and change a voltage or Step/Ramp setting. Watch
  the Scope and listen for the result.
- Stop, restart, reset, and try an external clock if you have a familiar patch.
- Right-click an AUX assignment button to change destination/track. Try
  positive/negative knob amounts, noon, unplugging, clear assignment, and undo.
- Try CTRL knob dragging, track assignment, clear assignment, and undo. Check
  that direct destinations show their owner and offsets return to normal at noon.
- Save a new patch, close/reopen it, and confirm the programmed settings survive.
  Playback position/random state starts afresh by design.

[Full controls guide](https://github.com/spliffrider/metropolix-vcv-plugin/blob/main/docs/HARDWARE-UI.md)

## Stage overrides — introduced in 2.10.0

Try EDIT → PITCH/GATE, Sync, ALT all-stage edits and clears, Rest with ACCUM,
track Gate Scale, and undo/redo. Save/reopen your patch. The downloadable
[stage override demo](../examples/Stage-overrides-performance.vcv) starts stopped.
See the [controls guide](STAGE-OVERRIDES.md); hardware comparisons are welcome.

## Known limitations

**Missing:** hardware SysEx import/export and the original hardware factory
preset bank. Plugin JSON backups and the supplied original demo patterns are
separate from hardware files. Presets/chains, MIDI, all nine MX modes, all 38
AUX/MOD destinations and all 29 CTRL destinations are implemented.

No physical hardware comparison has been performed. Some timing, route gains,
random behavior and live-edit interactions remain interpretations. Firmware
1.6.5 trigger references conflict; see the
[current reference choices](FEATURE-STATUS.md#hardware-fidelity-and-reference-choices).

Windows/Linux have actual Rack callback, display and signal-path checks. Mac
GUI/audio human playtests, Rack Pro/other Rack versions, unusual DPI settings,
long sessions and cross-plugin behavior still need feedback. See
[platform coverage](PLATFORMS.md), [feature status](FEATURE-STATUS.md) and
[validation](VALIDATION.md). An implemented feature behaving unexpectedly is
still a possible bug; please report a reproducible example.

## Send feedback

[Open a tester feedback issue](https://github.com/spliffrider/metropolix-vcv-plugin/issues/new?template=test-feedback.md)
or send the same information to the person who shared this build:

- Plugin version, Rack version and Free/Pro, OS/version and CPU architecture,
  zoom/display scaling, and sample rate.
- What you tried, what you expected, and what happened instead.
- The smallest reproducible sequence of actions and whether it happens every time.
- A minimal patch, screenshot, or short video when helpful. For a crash, the
  relevant part of Rack's `log.txt` can help; remove personal paths/details first.
- Whether you own the hardware; include a manual page or hardware observation
  when reporting a behavior mismatch.

Confusing interactions and missing explanations are useful feedback even if
nothing crashes. Known reference gaps and newer firmware additions are tracked in the
[manual coverage map](MANUAL-COVERAGE.md).

## Loopy — new in 2.11.0

Try momentary loops, mouse-selected ranges, Auto versus fixed counts, reverse,
latching/unlatching and stopped Stage Player. Check that release restores the
current original pattern. Try both target settings and save/reopen.
[Operation and comparison limits](LOOPY.md); [portable demo](../examples/Loopy-performance.vcv).

## Scales — new in 2.12.0

Try the [scale guide](SCALES.md): factory banks, custom note/root editing,
lock/search/apply, eight held/menu shortcuts, undo and saving/reopening.
The Scale (User) MOD/AUX/CTRL destination moves inside the locked ten-slot bank.
Feedback on fractional-voltage boundaries from hardware owners is especially useful.

## Clock output — new in 2.15.0

Open the supplied clock demo, select your audio device and press RUN. The right
voice uses CLK `/4` offset `2`; the left voice uses regular track gates.
Press ALT → CLK, try offsets 0–3 and other divisors, and verify that both note
tracks keep their existing timing. Encoder click selects the other field;
vertical dragging edits; long press resets both with one undo. Save/reopen
the patch and recall an internal preset: CLK settings should stay global.
See [Clock output](CLOCK-OUTPUT.md) for transport and live-edit choices.

## Accumulator gates — new in 2.18.0

Try CTRL Accum Dir at forward/noon/reverse positions and AUX/MOD Accumulate,
Rev, Invert, Mode, Polar and Reset. Check that pauses keep the gate rhythm and
that a brief reset waits for the next track pulse without restarting the pattern.
Use [the guide](ACCUMULATOR-ROUTING.md) and the new percussion arrangement.
Compare Rev versus Invert in unipolar mode, save/reopen and test undo.

## Complete routing — new in 2.19.0

Try [the twenty added routes](COMPLETE-ROUTING.md) on AUX and internal lanes.
Compare direction versus slider inversion; clock division versus pulse division;
track Reset versus Accum Reset. Gate-driven Loopy should return to the current
background pattern on release. Check the live BPM readout, Global assignment
fields, On/Off gate entry, undo and save/reopen. Gate Length CV now reaches Hold.

## Clock/reset and tuner — new in 2.20.0

Try [Global Setup](CLOCK-RESET.md): compare Auto/Internal/External and Always/Running/Legacy.
Check Reset versus Run versus Run-C, protected tracks and held-TRK forced reset.
Try Bottom Pitch outside the current scale, then tune both voices with High/Clock
gates. EXIT must restore their sequence. Save/reopen retains settings and presets
without activating the tuner. Older format 7 presets should load with Global Reset On.

## Editing workflows — new in 2.21.0

Try [ALT randomize/morph/restore and copy/paste](EDITING-WORKFLOWS.md). Compare
AUX Y variations with AUX Z, confirm LEN Pulses stays unchanged when randomizing,
copy stages and whole MOD lanes, and undo each paste. Choose a custom Init slot,
initialize, undo/redo and save/reopen. Long AUX encoder clear should restore both
fields on undo.

## MIDI — new in 2.24.0

Open the MIDI performance example, select AUDIO and press RUN. Both voice
pitch/gate paths go through Loopback 16 and Core MIDI-CV on separate channels.
Try ALT → TRK MIDI settings and ALT → MOD CC/channel settings. With a MIDI
monitor/DAW, check Slide Toggle/Amount, stopped CC learn, incoming Root/PC,
queued loading, Start/Continue/Stop and held-note release. Save/reopen and undo
the settings. See [the MIDI guide](MIDI.md), including the manual's clock
contradiction, voltage/note mapping and the limits of this software comparison.

## MX — new in 2.25.0

Open the MX performance example, choose AUDIO, and press RUN. MX sits at the
right of the sequencer and triggers kick/snare/hats from actual track stages.
Try ALT → stage 6 (MX), preview/confirm modes and track/MOD assignments, undo,
long default reset, mute, swap, Loopy Stage Player and save/reopen. The [MX
guide](MX.md) lists every output and the clock prediction interpretations.

## Backup files — new in 2.26.0

In a spare patch, export All from SETUP → MIDI → Backup files. Change the tempo
and a preset, import your file, then try Rack Undo and Redo. Settings and the
bank should return with RUN stopped; local MIDI devices and cables should stay
in place. Try Settings and Presets exports separately, and cancel a chooser.
The [backup guide](BACKUPS.md) describes scopes and the example JSON file.

For **2.29.0 selective backups**, export a sparse Filled collection and one bank.
Before importing, put different presets in another bank and create a chain.
Filled should update only the source's occupied addresses. Bank should replace
all eight addresses in that bank, clearing its empty slots. Unrelated banks,
chains and tempo should remain. Try Undo/Redo and save/reopen the selected
submenu. Use All Presets when sharing with 2.26–2.28 builds.

## Panel and display — new in 2.27.0

With Enc Confirm On, hold and drag LEN or TYPE: the popup should show the pending
value, and the encoder should apply it. Try EXIT cancellation, fine dragging,
numeric MOD entry in semitones/BPM/percent and one-step undo. ALT should mark all
MOD vertices. With pitch/CTRL Thru recall, long EXIT should sync visible positions
and undo should restore every pickup target. Open and close an optional editor
with MX beside it and check the link/cables. See [Panel refinement](UI-FIDELITY.md).

## Maintenance — new in 2.30.0

In a spare patch, open SETUP → Version / Factory. Test both actions first with
Cancel and EXIT: no preset or sound setting should change. Apply Factory Reset
and check that live defaults return but saved presets remain. Apply Clear All
Presets and check that all slots empty while live settings stay. Both stop RUN
and disable chains; Undo/Redo should recover the entire previous bank/settings.
Save with Apply selected in confirmation and reopen: the menu should return
with confirmation disarmed and the presets untouched. See [Maintenance](MAINTENANCE.md).
