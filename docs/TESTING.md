# Metropolix Lab 2.14.0 — Windows alpha quick start

This is an independent VCV Rack sequencer in development, not an official
Intellijel product. Feedback on playability, confusing controls, and bugs is
welcome. Some hardware controls are still placeholders; see Known limitations.

## Downloads

- [Windows x64 plugin](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.14.0/MetropolixLab-2.14.0-win-x64.vcvplugin)
- [Stage CV demo](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.14.0/Stage-CV-performance.vcv)
- [Ready-patched two-voice demo](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Aux-inputs-demo.vcv)
- [Release page and checksums](https://github.com/spliffrider/metropolix-vcv-plugin/releases/tag/v2.14.0)

Download the `.vcvplugin` file from **Assets**. GitHub's automatic “Source code”
ZIP is for building the plugin yourself; it is not the installable plugin.

Only **Windows x64** has a downloadable plugin in this release. Tested with
**VCV Rack Free 2.6.6** and **VCV Free 2.6.4**. Other Rack 2 versions and Rack Pro
are unverified; macOS/Linux plugin binaries are not provided. The cloud C++
engine tests on Linux do not establish Linux plugin compatibility.

## Install

1. In Rack, choose **Help → Open user folder** and keep that folder open.
2. Save your patch, then close Rack.
3. Copy `MetropolixLab-2.14.0-win-x64.vcvplugin` into the user folder's
   **`plugins-win-x64`** directory. Keep the file intact; Rack extracts it.
4. Start Rack and search for **Metropolix Lab** in the module browser.

This follows the [official third-party plugin installation procedure](https://vcvrack.com/manual/Installing#installing-plugins-not-available-on-the-vcv-library).
There is no separate installer or GitHub account requirement to download.

## Hear something immediately

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

Download [Preset-performance.vcv](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.7.1/Preset-performance.vcv).
The red bank has four related patterns. Select a filled slot on LOAD and let the
current eight-bar phrase finish, or use Queue Pulses = 16/32 for quicker changes.
Check that melody, MOD and gate changes start together. Try cancellation,
save/overwrite, undo, and saving/reopening your Rack patch. Compare Pitch/Switch
Off with **Recall complete pattern**. Full instructions: [Preset guide](PRESETS.md).

## Try the performance knobs

Download [CTRL-performance.vcv](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.6.0/CTRL-performance.vcv).
CTRL 1 changes Track 2 pulse counts; CTRL 2 adds ratchet fills. Both start at
noon, preserving the original evolving arrangement. Right-click either for
assignments and exact centre. See the [CTRL guide](CTRL-KNOBS.md) for ranges,
direct parameter ownership, and five unavailable destinations.

## Compare accumulator modes

Download [Accumulator-modes.vcv](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.5.0/Accumulator-modes.vcv).
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

**Currently inactive or incomplete:** additional AUX/CTRL destinations, Loopy AUX/MOD routing, most remaining MOD
destinations, hardware bank interchange,
remaining accumulator reset/routing options, MIDI, MX, and SysEx.

**Fixed options:** /1 external input clock
division. Each incoming clock edge is one sixteenth note. Some internal MOD
gains are implementation choices pending hardware comparison. The front panel
is not a promise that every printed function is implemented.

No physical hardware comparison or broad multi-computer stability test has been
performed. Native mouse/focus behavior needs tester feedback in addition to the
passing automated widget checks. See the
[feature assessment](https://github.com/spliffrider/metropolix-vcv-plugin/blob/main/docs/FEATURE-STATUS.md)
and [validation record](https://github.com/spliffrider/metropolix-vcv-plugin/blob/main/docs/VALIDATION.md).

## Send feedback

[Open a tester feedback issue](https://github.com/spliffrider/metropolix-vcv-plugin/issues/new?template=test-feedback.md)
or send the same information to the person who shared this build:

- Plugin version, Rack version, Windows version, zoom/display scaling, and sample rate.
- What you tried, what you expected, and what happened instead.
- The smallest reproducible sequence of actions and whether it happens every time.
- A minimal patch, screenshot, or short video when helpful. For a crash, the
  relevant part of Rack's `log.txt` can help; remove personal paths/details first.
- Whether you own the hardware; include a manual page or hardware observation
  when reporting a behavior mismatch.

Confusing interactions and missing explanations are useful feedback even if
nothing crashes. Known unimplemented functions are already tracked in the
feature assessment.

## Loopy — new in 2.11.0

Try momentary loops, mouse-selected ranges, Auto versus fixed counts, reverse,
latching/unlatching and stopped Stage Player. Check that release restores the
current original pattern. Try both target settings and save/reopen.
[Operation and comparison limits](LOOPY.md); [portable demo](../examples/Loopy-performance.vcv).

## Scales — new in 2.14.0

Try the [scale guide](SCALES.md): factory banks, custom note/root editing,
lock/search/apply, eight held/menu shortcuts, undo and saving/reopening.
The Scale (User) MOD/AUX/CTRL destination moves inside the locked ten-slot bank.
Feedback on fractional-voltage boundaries from hardware owners is especially useful.
