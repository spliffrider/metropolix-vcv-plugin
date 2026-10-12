# Patches in Motion

[Watch/download the 72-second video](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Metropolix-Lab-Patches-in-Motion.mp4).
It shows the full rack, melodic signal paths, and drum close-ups across two
original performances. The accompanying traces show measured pitch and gate
voltages, with short persistence on hit indicators so fast triggers are visible.
The knobs retain their actual positions; incoming CV does not move their base values.

## Patches and requirements

| Patch | Tempo and scale | Sound |
| --- | --- | --- |
| [Neon Transit](../examples/Neon-Transit.vcv) | 124 BPM, A minor | Classic-waveform bass, resonator melody, four-on-the-floor kick |
| [Broken Orbit](../examples/Broken-Orbit.vcv) | 100 BPM, C Dorian | Waveshaped bass, FM melody, syncopated kick |

Both use Metropolix Lab 2.4.1, VCV Free 2.6.4, Audible Instruments 2.0.0,
Befaco 2.4.0, and Rack Core. Tested in Rack Free 2.6.6 on Windows x64.
Install those plugins through the VCV Library before loading. The patches
include their settings and cables, not third-party plugin binaries or samples.
Choose your output device in AUDIO. RUN starts/stops the sequencer; RESET
returns to the beginning. Both portable patches start with RUN enabled.

## Follow the connections

| Metropolix signal | Destination | What it does |
| --- | --- | --- |
| Track 1 pitch + gate | Audible Instruments Macro Oscillator 2 | Bass notes and internal low-pass-gate triggers |
| Track 2 pitch + gate | Resonator / Macro Oscillator 2 | Strummed resonator / triggered FM melody |
| A, assigned to MOD 1 | Befaco Kickall | Kick rhythm |
| B, assigned to MOD 2 | Macro Oscillator 2, snare model | Backbeat |
| CLK | Macro Oscillator 2, hi-hat model | Sixteenth-note hats |
| CLK | Both VCV Delays | Tempo-following echoes |

The first two MOD lanes use gate voltages, eight stages, and /2 division.
Neon Transit's kick pattern is `1 0 1 0 1 0 1 0`; Broken Orbit uses
`1 0 0 1 0 0 1 0`. Both snare patterns are `0 0 1 0 0 0 1 0`.
The remaining lanes shift both tracks' pitch every two bars, lift the melody,
add ratchet fills, leave melodic rests, change passing notes on a three-bar
cycle, and rest the bass during bars 9–12. The bass returns at bar 13.

Four real Scope modules show bass pitch/gate, voice waveforms, drum audio,
and A/B gates. The video also adds color-coded traces from the same captured
signals. Pitch names in those traces use the 1 V/oct convention of C4 at 0 V;
a voice's own transpose setting can shift the audible octave.

## Recreate the video

Save and close Rack before capturing. The capture script copies only the
required installed plugins from the local Rack profile into a fresh test
profile. Existing profiles remain unchanged. Python needs Pillow and numpy;
FFmpeg and Windows Segoe UI are required for editing.

```powershell
python scripts/create-performance-demos.py
.\scripts\capture-demo-video.ps1 -Example Neon-Transit -Seconds 32
$neon = (Get-Content tmp/latest-video-capture.txt -Raw).Trim()
.\scripts\capture-demo-video.ps1 -Example Broken-Orbit -Seconds 40
$broken = (Get-Content tmp/latest-video-capture.txt -Raw).Trim()
python scripts/render-performance-video.py $neon $broken --output checkpoints/performance-video/Metropolix-Lab-Patches-in-Motion.mp4 --proofs-only
# Inspect performance-proofs, then omit --proofs-only to encode.
```

Audio and screenshots share the QA recorder's sample clock. Additional taps
record pitch values and 10 ms gate/voice peaks; the invisible QA taps do not
change audio routing. The video editor places captured frames on a 30 fps
timeline and draws signal overlays at that rate. The original UI capture is
approximately 7–10 frames/second on the recording machine. It records actual
Rack scopes and meters, not simulated module behavior.

The two soundtracks receive short boundary fades, a 25 Hz high-pass filter,
and measured loudness adjustment targeting −18 LUFS / −1.5 dBTP for the
video. No replacement music is added. Both recordings pass signal-level,
finite-value, and DC checks; the edited MP4 is decoded and checked separately.

Parameter/port references: the official
[Audible Instruments source](https://github.com/VCVRack/AudibleInstruments/tree/v2/src),
[Befaco Kickall source](https://github.com/VCVRack/Befaco/blob/v2/src/Kickall.cpp),
and [VCV Free source](https://github.com/VCVRack/Fundamental/tree/v2/src).
The generator uses the models present in the tested installed versions.
