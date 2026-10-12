# Metropolix Lab 2.4.1 — Windows alpha quick start

This is an independent VCV Rack sequencer in development, not an official
Intellijel product. Feedback on playability, confusing controls, and bugs is
welcome. Some hardware controls are still placeholders; see Known limitations.

## Downloads

- [Windows x64 plugin](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/MetropolixLab-2.4.1-win-x64.vcvplugin)
- [Ready-patched two-voice demo](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Aux-inputs-demo.vcv)
- [Release page and checksums](https://github.com/spliffrider/metropolix-vcv-plugin/releases/tag/v2.4.1)

Download the `.vcvplugin` file from **Assets**. GitHub's automatic “Source code”
ZIP is for building the plugin yourself; it is not the installable plugin.

Only **Windows x64** has a downloadable plugin in this release. Tested with
**VCV Rack Free 2.6.6** and **VCV Free 2.6.4**. Other Rack 2 versions and Rack Pro
are unverified; macOS/Linux plugin binaries are not provided. The cloud C++
engine tests on Linux do not establish Linux plugin compatibility.

## Install

1. In Rack, choose **Help → Open user folder** and keep that folder open.
2. Save your patch, then close Rack.
3. Copy `MetropolixLab-2.4.1-win-x64.vcvplugin` into the user folder's
   **`plugins-win-x64`** directory. Keep the file intact; Rack extracts it.
4. Start Rack and search for **Metropolix Lab** in the module browser.

This follows the [official third-party plugin installation procedure](https://vcvrack.com/manual/Installing#installing-plugins-not-available-on-the-vcv-library).
There is no separate installer or GitHub account requirement to download.

## Hear something immediately

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

## A useful 15-minute test

- Move pitch sliders, pulse-count switches, and gate switches. Check that the
  sequence reacts and both tracks remain independent where appropriate.
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
- Save a new patch, close/reopen it, and confirm the programmed settings survive.
  Playback position/random state starts afresh by design.

[Full controls guide](https://github.com/spliffrider/metropolix-vcv-plugin/blob/main/docs/HARDWARE-UI.md)

## Known limitations

**Currently inactive or incomplete:** additional AUX destinations, CTRL knob
assignments, Loopy, swing, stage pitch/gate-length overrides, most remaining MOD
destinations, the full scale library/user scales, hardware preset banks/chains,
track-wide accumulator options, MIDI, MX, and SysEx.

**Fixed options:** Tempo slide, 100% gate scale, and /1 external input clock
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
