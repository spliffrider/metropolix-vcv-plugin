"""Render a real Rack sidechain A/B capture, keeping one gain for every section."""
import argparse
import importlib.util
import json
import math
import subprocess
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

spec = importlib.util.spec_from_file_location('pulse', Path(__file__).with_name('render-pulse-pressure-video.py'))
pulse = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pulse)
BAR = 240 / 138
END = 24 * BAR
BG, PANEL, INK, MUTED, GRID = pulse.BG, pulse.PANEL, pulse.INK, pulse.MUTED, pulse.GRID
GREEN, RED, PRE = pulse.LEAD, pulse.KICK, '#aeb9c8'
label, paste = pulse.label, pulse.paste


def verify(take, output):
    s = take.signals
    assert all(np.max(s[k]) >= 4.9 for k in ['gate1', 'gate2', 'kick', 'snare', 'hat'])
    assert np.max(s['bass_peak']) > .1 and np.max(s['lead_peak']) > .1
    # The three last telemetry channels are pre/post music peaks and B control.
    change = -20 * np.log10(np.maximum(s['lead_peak'], .00001) / np.maximum(s['bass_peak'], .00001))
    expected = [False, True, False, True, True, True]
    report = []
    for i, enabled in enumerate(expected):
        q = ((s['seconds'] > i * 4 * BAR + .3) & (s['seconds'] < (i + 1) * 4 * BAR - .1)
             & (s['bass_peak'] > .05))
        assert np.all(s['drum_peak'][q] == (5 if enabled else 0))
        reduction = change[q]
        if enabled:
            assert np.percentile(reduction, 90) > 4
        else:
            assert abs(float(np.median(reduction))) < .2
        report.append(dict(section=i + 1, sidechain=enabled,
            median_peak_change_db=float(np.median(reduction)),
            p90_peak_change_db=float(np.percentile(reduction, 90)),
            max_peak_change_db=float(np.max(reduction))))
    tail = s[s['seconds'] > END + .1]
    assert all(not np.any(tail[k] > 1) for k in ['gate1', 'gate2', 'kick', 'snare', 'hat'])
    (output.parent / 'sidechain-check.json').write_text(json.dumps(report, indent=2) + '\n')
    print('PASS actual A/B control and music attenuation, active drums, end mute', flush=True)


def dashboard(canvas, take, t, enabled):
    d = ImageDraw.Draw(canvas)
    row = take.row(t)
    items = [(48, 420, 'KICK / DRUMS', RED), (496, 420, 'MUSIC / BEFORE', PRE),
             (944, 420, 'MUSIC / AFTER', GREEN), (1392, 480, 'SIDECHAIN CONTROL', GREEN)]
    for x, w, title, color in items:
        d.rounded_rectangle((x, 924, x + w, 1034), radius=10, fill=PANEL)
        label(d, (x + 16, 933), title, 18, color, True)
    take.history(d, t, 'kick', (64, 973, 325, 44), 0, 5, RED)
    d.ellipse((414, 979, 441, 1006), fill=RED if take.hit(t, 'kick') > .1 else GRID)
    for key, x, color in [('bass_peak', 512, PRE), ('lead_peak', 960, GREEN)]:
        take.history(d, t, key, (x, 973, 325, 44), 0, 1.5, color)
        label(d, (x + 277, 932), f'{row[key]:.2f} V', 19, INK, True)
    label(d, (1408, 969), 'ON' if enabled else 'OFF', 28, GREEN if enabled else MUTED, True)
    label(d, (1510, 972), 'MOD 8 → B → ratio CV', 20, MUTED)


def frame(take, t):
    raw = take.shot(t)
    enabled = take.row(t)['drum_peak'] > 2.5
    bar = min(24, int(t / BAR) + 1)
    title = 'SIDECHAIN ON' if enabled else 'SIDECHAIN OFF'
    subtitle = 'The kick ducks the music bus.' if enabled else 'Same groove. No gain reduction.'
    wide = not enabled or t >= END
    if t >= END:
        title, subtitle = 'LET IT RING', 'Bass, lead and effects breathe around the kick.'
    canvas = Image.new('RGB', (1920, 1080), BG)
    d = ImageDraw.Draw(canvas)
    label(d, (48, 18), 'SENSIBURNER / METROPOLIX LAB / PULSE PRESSURE', 21, MUTED, True)
    label(d, (1350, 18), '138 BPM / SIDECHAIN A–B', 21, GREEN, True)
    label(d, (48, 54), title, 37, GREEN if enabled else INK, True)
    label(d, (650, 66), subtitle, 26, MUTED)
    label(d, (1715, 66), f'BAR {bar:02} / 24', 18, INK, True)
    if wide:
        paste(canvas, raw, (15, 49, 1755, 810), (64, 120, 1792, 784))
    else:
        label(d, (48, 126), 'THE SEQUENCER + MUSIC VOICES', 20, GREEN, True)
        paste(canvas, raw, (15, 49, 1185, 429), (48, 164, 1280, 416))
        label(d, (1400, 126), 'PRESSOR / ACTUAL METER', 20, GREEN, True)
        paste(canvas, raw, (1185, 49, 1410, 429), (1400, 164, 435, 735))
        d = ImageDraw.Draw(canvas)
        label(d, (48, 609), 'KICK AUDIO → SIDECHAIN DETECTOR', 25, RED, True)
        label(d, (48, 648), 'Bass + lead + delay + reverb → compressor audio inputs', 23, INK)
        label(d, (48, 684), 'Drums join the mix afterward. No makeup gain between A/B sections.', 21, MUTED)
        label(d, (48, 737), 'MUSIC BUS / MEASURED 10 ms PEAKS', 18, MUTED, True)
        take.history(d, t, 'bass_peak', (48, 783, 1280, 106), 0, 1.5, PRE, 2)
        take.history(d, t, 'lead_peak', (48, 783, 1280, 106), 0, 1.5, GREEN, 2)
        label(d, (880, 733), 'BEFORE', 18, PRE, True)
        label(d, (1110, 733), 'AFTER', 18, GREEN, True)
    dashboard(canvas, take, t, enabled)
    d = ImageDraw.Draw(canvas)
    for i in range(6):
        x = 48 + i * 304
        d.rectangle((x, 1046, x + 290, 1050), fill=GREEN if i <= int(t / (4 * BAR)) else GRID)
    label(d, (48, 1057), 'Actual Rack audio + footage • PRESSOR supplies compression • Metropolix supplies notes, drums and A/B control', 15, MUTED)
    if t > take.duration - .6:
        canvas = Image.blend(canvas, Image.new('RGB', canvas.size, BG), (t - (take.duration - .6)) / .6)
    return canvas


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('capture', type=Path)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--proofs-only', action='store_true')
    args = p.parse_args()
    args.output.parent.mkdir(exist_ok=True, parents=True)
    proofs = args.output.parent / 'proofs'
    proofs.mkdir(exist_ok=True)
    take = pulse.Take(args.capture, expected_duration=45)
    verify(take, args.output)
    for t in [2, 7.5, 10, 16, 22, 30, 37, 43]:
        frame(take, t).save(proofs / f'{t:04.1f}.png')
    frame(take, 7.5).save(args.output.with_suffix('.png'))
    print('Proofs ready', flush=True)
    if args.proofs_only:
        return
    audio, measured = pulse.master_audio(take, args.output)
    command = ['ffmpeg', '-hide_banner', '-loglevel', 'warning', '-y', '-f', 'rawvideo',
        '-pixel_format', 'rgb24', '-video_size', '1920x1080', '-framerate', '30', '-i', '-',
        '-i', str(audio), '-map', '0:v', '-map', '1:a', '-c:v', 'libx264', '-preset', 'fast',
        '-crf', '18', '-threads', '4', '-pix_fmt', 'yuv420p', '-c:a', 'aac', '-b:a', '320k',
        '-ar', '48000', '-t', '45', '-movflags', '+faststart',
        '-metadata', 'title=Metropolix Lab — Pulse Pressure Sidechain A-B', str(args.output)]
    with (args.output.parent / 'video-encode.log').open('w') as log:
        encoder = subprocess.Popen(command, stdin=subprocess.PIPE, stderr=log)
        try:
            for n in range(45 * 30):
                encoder.stdin.write(frame(take, n / 30).tobytes())
                if n % 300 == 0:
                    print(f'Editing {n / 30:.0f} / 45 seconds', flush=True)
        finally:
            encoder.stdin.close()
        assert encoder.wait() == 0, 'Check video-encode.log'
    args.output.with_suffix('.json').write_text(json.dumps(dict(duration=45, fps=30,
        width=1920, height=1080, capture_frames=len(take.frames),
        max_frame_gap=float(max(np.diff(take.times))), audio_source='Actual stereo Rack recording',
        mastering=measured, on_off_source='Actual MOD 8 voltage from output B'), indent=2) + '\n')
    print(args.output, flush=True)


if __name__ == '__main__':
    main()
