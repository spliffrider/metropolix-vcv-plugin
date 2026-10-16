# Demonstration video

The newer [Pulse Pressure performance](PULSE-PRESSURE.md) demonstrates the
2.70.0 plugin with MX-driven drums, accumulation, ratchets, swing and native
preset chaining. Its dedicated capture/edit commands are in that guide.

[Download the 1080p MP4](https://github.com/spliffrider/metropolix-vcv-plugin/releases/download/v2.4.1/Metropolix-Lab-Demo-1080p.mp4).
The 80-second showcase plays **Night Current / Variations** at 108 BPM,
covering its 32-bar arrangement and return. Captions identify the independent
tracks, octave lifts, passing-note shifts, bass breakdown, and articulation.
The instrument generates pitch, gates, and modulation; VCV Free provides the
patched voices, filters, envelopes, mixing, and delays.

## Reproduce on Windows

Requires the repository's configured MSYS2/Rack SDK toolchain, Rack Free,
VCV Free installed in `tmp/rack-user`, Python 3 with Pillow, FFmpeg on PATH,
and Windows Segoe UI fonts. Close Rack after saving your session. The capture
opens an isolated profile and exits when finished; reopen your session afterward.

```powershell
.\scripts\capture-demo-video.ps1
$capture = (Get-Content tmp/latest-video-capture.txt -Raw).Trim()
python scripts/render-demo-video.py $capture --output checkpoints/demo-video/Metropolix-Lab-Demo-1080p.mp4 --proofs-only
# Inspect the generated video-proofs before encoding.
python scripts/render-demo-video.py $capture --output checkpoints/demo-video/Metropolix-Lab-Demo-1080p.mp4
```

Use `-Seconds 8` for a short capture check. `-RackPath` selects another Rack
executable. The renderer's `--font-dir` can select another Windows fonts folder.
The camera framing assumes an 1800 × 1050 Rack window; inspect the proofs on
systems with different display scaling.

The script builds a separate QA DLL, copies the example into a fresh profile,
moves its sound generators below the camera, and taps the actual stereo AUDIO
inputs. It records 48 kHz audio and timestamps Rack front-buffer screenshots
against the audio sample count. Presentation changes select existing display
pages and MOD lanes without changing the musical settings. The capture starts
RUN once the window is ready. The QA DLL must never be distributed as the plugin.

The edit places those frames on a 30 fps timeline, holding frames between
captures (approximately 10 unique frames/second on the recording machine).
It uses captions, crops, a magnified display, a progress line, fixed 2× audio
gain, and short boundary fades. It does not synthesize replacement panel
animation or substitute a separate soundtrack. A brief blank frame at a Rack
zoom transition is omitted while holding the preceding frame; the audio
timeline remains intact. The JSON sidecar records any discarded frames.

Outputs include MP4, poster PNG, SRT captions, edit metadata, and proof images.
Captures and renders remain in ignored `tmp/` and `checkpoints/` directories;
the public MP4 is a release asset, not part of the plugin package. Validation
includes capture signal checks, proof inspection, a complete MP4 decode, and
format/audio-level checks. Listening feedback is still welcome.
