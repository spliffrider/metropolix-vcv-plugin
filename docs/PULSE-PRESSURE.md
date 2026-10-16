# Pulse Pressure

[Watch the 1080p video](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Metropolix-Lab-Pulse-Pressure.mp4)
or [download the Rack patch](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Pulse-Pressure.vcv).

An original **138 BPM, D minor** performance: 64 bars followed by an effects
tail, about 115 seconds in total. Every note, gate and drum trigger originates
in Metropolix Lab. The preset chain performs the arrangement automatically;
the video soundtrack is recorded from the actual Rack patch.

The [45-second sidechain companion](SIDECHAIN-DEMO.md) adds Bogaudio PRESSOR
and compares the same groove with and without kick-driven ducking.

### Just the opening groove

[Download the opening loop](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Pulse-Pressure-Opening-Loop.vcv)
to keep playing the original video's opening bass, kick, snare and closed hats.
Chain playback and Total Pulses automatic resets are disabled, so it stays on
that groove while you edit. The original stored presets and chain are retained.
Select your output device in AUDIO; RUN is already enabled.

For the later sidechain video's opening with bass and lead together, use
[its opening loop](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Pulse-Pressure-Sidechain-Opening-Loop.vcv).
PRESSOR remains at 1:1 with its existing zero-volt ratio CV, as at that video's
start. Dependencies are the same as each full patch. Regenerate both files with
`python scripts/create-opening-loops.py`.

## Play it

Install Metropolix Lab **2.70.0 or newer**, VCV Free, Audible Instruments,
Befaco and Valley. The patch uses Macro Oscillator 2, Liquid Filter, Kickall,
Plateau, VCA Mix, Delay, Scope and AUDIO. No samples or plugin binaries are
included in the patch.

Save your current session, open `Pulse-Pressure.vcv`, and choose your audio
device on AUDIO. Start at a comfortable listening level. The patch starts
running when opened. Stop RUN, press RESET, then start RUN to replay it from
the first section. After 64 bars it reaches a silent tail preset for eight
bars before the chain repeats. Remove the ninth chain link to loop the eight
musical sections immediately instead.

## What it demonstrates

| Time | Bars | Change |
| --- | --- | --- |
| 0:00 | 1–8 | Kick, snare, closed hats and rolling bass |
| 0:14 | 9–16 | Seven-stage lead against the bass's 16-pulse phrase |
| 0:28 | 17–24 | Bipolar stage accumulation changes the lead |
| 0:42 | 25–32 | Lead ratchets, probability, swing and extra percussion |
| 0:56 | 33–40 | Bass/drum breakdown with a rising lead filter |
| 1:10 | 41–48 | Full bass and drum groove returns |
| 1:23 | 49–56 | New lead order and track accumulation |
| 1:37 | 57–64 | Stage offset and denser accents in the final phrase |
| 1:51 | Tail | Voices mute; delay and reverb decay |

TRK 1 uses the physical pulse-count and gate-type switches. TRK 2 uses seven
stages, a uniform single pulse and a uniform Multi gate source. This lets the
tracks share pitches while their phrases cycle at different lengths. Tempo
slides join selected stages, so the pitch traces include intermediate voltages.

| Source | Destination |
| --- | --- |
| TRK 1 pitch/gate | Bass oscillator and its internal lowpass gate |
| TRK 2 pitch/gate | Lead oscillator and its internal lowpass gate |
| MOD 1 → MX 1 | Kickall trigger |
| MOD 2 → MX 2 | Snare trigger |
| MOD 3 → MX 3 | Closed-hat trigger |
| MOD 4 → MX 4 | Seven-step percussion trigger |
| MOD 5 → MX 5 | Five-step open-hat trigger |
| MOD 6 → A | Lead filter frequency modulation |
| MOD 7 → B | Bass filter frequency modulation |

MX uses **Mod Trig** mode. Track voices run through separate resonant filters;
the lead also feeds dotted-eighth delay and stereo reverb. The kick and bass
remain centered. Accumulation, probability and independent phrase lengths
make individual passes vary.

## Reproduce the recording

The builder is self-contained, using checked-in default module states exported
through Rack's public module API. It creates the patch and an arrangement plan
in `tmp/`. Capture requires the configured Windows Rack SDK/MSYS2 toolchain,
Rack Free 2.6.6, VCV Free in the development profile, and the other required
plugins installed in the standard Rack user folder. Rendering requires Python,
Pillow, numpy, FFmpeg on PATH and Windows Segoe UI fonts.

```powershell
python scripts/create-pulse-pressure-demo.py
# Save your work and close Rack before capture.
.\scripts\capture-demo-video.ps1 -Example Pulse-Pressure -Seconds 115
$capture = (Get-Content tmp/latest-video-capture.txt -Raw).Trim()
python scripts/render-pulse-pressure-video.py $capture --output exports/Pulse-Pressure/Metropolix-Lab-Pulse-Pressure.mp4 --proofs-only
# Inspect exports/Pulse-Pressure/proofs before encoding.
python scripts/render-pulse-pressure-video.py $capture --output exports/Pulse-Pressure/Metropolix-Lab-Pulse-Pressure.mp4
```

Capture uses an isolated Rack profile and a separate QA-only recorder. It
records the stereo AUDIO inputs at 48 kHz and timestamps actual Rack frames
against the same audio sample count. The camera selects existing pages for
presentation; the native preset chain drives the music. The QA DLL is never
installed as the production plugin or included with the patch.

The edit crops the real panel and display. The output is 1920 × 1080 at 30 fps;
captured Rack frames are held between updates. Signal overlays use measured
pitch/gate voltages and 10 ms peak windows, making short MX triggers visible.
Mastering removes subsonic DC/rumble, normalizes loudness and fades the effects
tail. It does not replace the original Rack audio with another soundtrack.

The renderer also saves a poster, proof images, a 24-bit stereo WAV master,
encode/mastering logs and an edit-metadata JSON file. Raw captures and exports
are ignored by Git. The public video is a release asset separate from the
unchanged 2.70.0 plugin packages.
