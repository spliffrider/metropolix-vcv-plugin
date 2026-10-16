# Pulse Pressure: sidechain A/B

[Watch the 45-second comparison](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Metropolix-Lab-Pulse-Pressure-Sidechain.mp4)
or [download its Rack patch](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.70.0/Pulse-Pressure-Sidechain.vcv).
The original [Pulse Pressure video](PULSE-PRESSURE.md) remains available.

Metropolix Lab plays the notes, MOD lanes and MX drum triggers. A separate
**Bogaudio PRESSOR** provides the sidechain compression. Install Bogaudio in
addition to the [original patch's required plugins](PULSE-PRESSURE.md).

Kickall's actual audio output feeds PRESSOR's sidechain detector. Bass, lead,
delay and stereo reverb pass through its stereo audio inputs. The drum bus
joins the mix after the compressor, preserving the kick's level and punch.
Detector mix is fully external; input and output gain remain at unity.

The comparison repeats the same programmed four-bar pattern:

| Time | Setting |
| --- | --- |
| 0:00 | Compression off: ratio CV = 0 V, effective ratio 1:1 |
| 0:07 | Compression on: ratio CV = 5 V, effective ratio about 1.8:1 |
| 0:14 | Off again |
| 0:21 | On again |
| 0:28 | On, with denser lead accents and extra percussion |
| 0:42 | Voices mute; effects tail |

Metropolix MOD 8 drives output B and PRESSOR's ratio CV input. This control
changes automatically with the native preset chain, so the playable patch
reproduces the comparison without the video recorder. PRESSOR stays active
in the off sections at 1:1; this is not Rack's module bypass switch.

PRESSOR uses a **−14 dB threshold, 2 ms attack, 180 ms release, peak detector
and soft knee**. Its ratio knob is fully clockwise; the connected 5 V CV scales
it to an effective ratio of about 1.8:1. See the official
[PRESSOR implementation](https://github.com/bogaudio/BogaudioModules/blob/master/src/Pressor.cpp)
for its CV scaling and detector behavior.

The captured music peaks fall by up to about 6.4 dB during the on sections.
The video shows PRESSOR's actual gain-reduction meter and measured pre/post
music-bus peaks. Those traces are 10 ms peak windows, not a separately inferred
sample-accurate gain-reduction meter. The edit applies one overall gain and
peak limiter across the complete recording; individual A/B sections are not
loudness-normalized separately.

## Reproduce

Save your session and close Rack, then use the same configured SDK/toolchain,
installed plugins and Python/FFmpeg dependencies as the original capture.

```powershell
python scripts/create-sidechain-demo.py
.\scripts\capture-demo-video.ps1 -Example Pulse-Pressure-Sidechain -Seconds 45
$capture = (Get-Content tmp/latest-video-capture.txt -Raw).Trim()
python scripts/render-sidechain-video.py $capture --output exports/Pulse-Pressure-Sidechain/Metropolix-Lab-Pulse-Pressure-Sidechain.mp4 --proofs-only
python scripts/render-sidechain-video.py $capture --output exports/Pulse-Pressure-Sidechain/Metropolix-Lab-Pulse-Pressure-Sidechain.mp4
```

The generator rebuilds the unchanged original patch before deriving this
companion. Capture remains isolated from the normal Rack profile. The renderer
verifies recorded A/B voltage states, attenuation, drum activity and the final
mute, then saves proof images, a stereo WAV master, MP4 and edit metadata.
The existing plugin packages and original demonstration are unchanged.
