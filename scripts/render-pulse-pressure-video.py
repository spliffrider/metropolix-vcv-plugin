"""Edit the Pulse Pressure Rack capture into a signal-synchronized demo.

Usage: python scripts/render-pulse-pressure-video.py CAPTURE --output VIDEO.mp4
Requires Pillow, numpy, FFmpeg and Windows Segoe UI fonts. No replacement
soundtrack, animated panel replica, or invented signal data is used.
"""
import argparse
import bisect
import csv
import json
import math
import subprocess
import wave
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFont

BPM = 138
BAR = 240 / BPM
END = 64 * BAR
BG = '#0b111a'
PANEL = '#15212e'
GRID = '#273748'
INK = '#f1f5f8'
MUTED = '#9cafc1'
BASS, LEAD, KICK, SNARE, HAT = '#efa95e', '#55d7c4', '#ff687f', '#b5a0f4', '#8bbbf5'
FONTS = {}

SECTIONS = [
    ('FOUNDATION', 'Kick. Bass. Offbeat hats.', 'wide',
     'TRK 1 follows the pulse-count switches.', 'MX 1–3 trigger the kick, snare and hats.'),
    ('CROSSING PHRASES', 'Two tracks. Different phrase lengths.', 'voices',
     'Bass: 16 pulses. Lead: 7 single-pulse stages.', 'The phrases keep moving against each other.'),
    ('ACCUMULATOR', 'The lead changes with each pass.', 'focus',
     'Stage accumulation moves notes through D minor.', 'Bipolar limits keep the changes within a musical range.'),
    ('RATCHETS + SWING', 'Extra hits. A looser groove.', 'focus',
     'Two ratcheted stages and 85% probability accents.', '56% lead swing; 7-step percussion and 5-step open hats.'),
    ('BREAKDOWN', 'Strip it back. Open the filter.', 'focus',
     'Bass and drums mute for eight bars.', 'MOD 6 raises the lead filter through output A.'),
    ('THE DROP', 'Bring the whole patch back.', 'wide',
     'Five MX-triggered drum voices return with the bass.', 'The lead keeps its ratchets and accumulation.'),
    ('CHAIN IN MOTION', 'Stored patterns. One continuous performance.', 'focus',
     'Preset chaining changes the lead order and accumulator.', 'The arrangement runs inside Metropolix Lab.'),
    ('FINAL LAP', 'One more turn of the machine.', 'wide',
     'A stage offset and denser accents change the final phrase.', 'Then the voices mute and the effects ring out.'),
]


def font(size, bold=False):
    key = size, bold
    if key not in FONTS:
        FONTS[key] = ImageFont.truetype(
            'C:/Windows/Fonts/' + ('segoeuib.ttf' if bold else 'segoeui.ttf'), size)
    return FONTS[key]


def label(draw, xy, message, size=24, color=INK, bold=False):
    draw.text(xy, message, font=font(size, bold), fill=color)


def note(volts):
    midi = round(60 + 12 * volts)
    return ['C', 'C♯', 'D', 'D♯', 'E', 'F', 'F♯', 'G', 'G♯', 'A', 'A♯', 'B'][midi % 12] + str(midi // 12 - 1)


class Take:
    def __init__(self, path, expected_duration=115):
        self.path = path
        with wave.open(str(path / 'two-track-demo.wav')) as audio:
            assert (audio.getframerate(), audio.getnchannels(), audio.getsampwidth()) == (48000, 2, 2)
            self.duration = audio.getnframes() / 48000
        self.frames = list(csv.DictReader((path / 'frames.csv').open()))
        self.times = [float(r['seconds']) for r in self.frames]
        assert self.times == sorted(self.times) and self.times[0] == 0
        assert max(np.diff(self.times)) < .6 and self.duration - self.times[-1] < .6
        self.signals = np.genfromtxt(path / 'signals.csv', delimiter=',', names=True)
        assert np.isfinite(self.signals.view(float)).all()
        assert math.isclose(self.duration, expected_duration), f'Capture the complete {expected_duration}-second arrangement.'
        self.shot_index = -1

    def shot(self, t):
        i = max(0, bisect.bisect_right(self.times, t) - 1)
        if i != self.shot_index:
            self.image = Image.open(self.path / self.frames[i]['file']).convert('RGB')
            assert self.image.size == (1800, 1050)
            self.shot_index = i
        return self.image

    def row(self, t):
        return self.signals[max(0, min(len(self.signals) - 1, int(t * 100) - 1))]

    def hit(self, t, key):
        end = max(0, min(len(self.signals) - 1, int(t * 100) - 1))
        start = max(0, end - 14)
        active = np.flatnonzero(self.signals[key][start:end + 1] > 1)
        return max(0, 1 - (end - start - active[-1]) / 14) if len(active) else 0

    def history(self, draw, t, key, rect, lo, hi, color, seconds=3):
        x, y, w, h = rect
        for q in range(3):
            yy = y + h * q / 2
            draw.line((x, yy, x + w, yy), fill=GRID)
        first, last = max(0, int((t - seconds) * 100)), min(len(self.signals), int(t * 100))
        points = []
        for i in range(first, last):
            px = x + w * (self.signals['seconds'][i] - (t - seconds)) / seconds
            v = float(self.signals[key][i])
            py = y + h * (1 - max(0, min(1, (v - lo) / (hi - lo))))
            points.append((px, py))
        if len(points) > 1:
            draw.line(points, fill=color, width=3)


def validate_arrangement(take, output):
    """Check recorded outputs, not just the intended preset programming."""
    with wave.open(str(take.path / 'two-track-demo.wav')) as audio:
        pcm = np.frombuffer(audio.readframes(audio.getnframes()), dtype='<i2').reshape(-1, 2) / 32768.
    assert not np.any(np.abs(pcm) >= .999), 'Clipped source recording.'
    checks = []
    for section in range(8):
        start, end = section * 8 * BAR, (section + 1) * 8 * BAR
        q = take.signals[(take.signals['seconds'] >= start + .1) & (take.signals['seconds'] < end - .1)]
        sound = pcm[int(start * 48000):int(end * 48000)]
        row = dict(section=SECTIONS[section][0], rms_db=float(20 * np.log10(np.sqrt(np.mean(sound * sound)))))
        for key in ['gate1', 'gate2', 'kick', 'snare', 'hat']:
            high = q[key] > 1
            row[key + '_hits'] = int(np.sum(high & ~np.roll(high, 1)))
        checks.append(row)
    assert checks[0]['gate2_hits'] == 0
    assert all(checks[i]['gate2_hits'] > 20 for i in range(1, 8))
    assert all(checks[i]['kick_hits'] >= 30 for i in [0, 1, 2, 3, 5, 6, 7])
    assert all(checks[4][k + '_hits'] == 0 for k in ['gate1', 'kick', 'snare', 'hat'])
    assert checks[5]['gate1_hits'] > 20
    # The lead must remain audible in the breakdown rather than disappear
    # beneath a mix accidentally calibrated only around the kick.
    assert checks[5]['rms_db'] - checks[4]['rms_db'] < 16
    tail = take.signals[take.signals['seconds'] > END + .1]
    assert all(not np.any(tail[k] > 1) for k in ['gate1', 'gate2', 'kick', 'snare', 'hat'])
    (output.parent / 'arrangement-check.json').write_text(json.dumps(checks, indent=2) + '\n')
    print('PASS recorded arrangement: foundation, lead entry, breakdown, drop, end mute, headroom', flush=True)


def paste(canvas, raw, crop, box, pixels=False):
    x, y, w, h = box
    canvas.paste(raw.crop(crop).resize((w, h), Image.Resampling.NEAREST if pixels else Image.Resampling.LANCZOS), (x, y))


def dashboard(canvas, take, t):
    d = ImageDraw.Draw(canvas)
    row = take.row(t)
    for i, x in enumerate([48, 496]):
        color = [BASS, LEAD][i]
        key, gate = f'pitch{i + 1}', f'gate{i + 1}'
        d.rounded_rectangle((x, 924, x + 420, 1028), radius=10, fill=PANEL)
        label(d, (x + 16, 932), f'TRK {i + 1} / ' + ['BASS', 'LEAD'][i], 18, color, True)
        label(d, (x + 286, 930), note(row[key]), 23, INK, True)
        take.history(d, t, key, (x + 16, 969, 333, 43), -3 if i == 0 else -1.5, 0 if i == 0 else .75, color)
        level = take.hit(t, gate)
        d.ellipse((x + 369, 976, x + 395, 1002), fill=color if level > .08 else GRID)
    for key, title, x, color in [
        ('kick', 'MX 1 / KICK', 944, KICK),
        ('snare', 'MX 2 / SNARE', 1256, SNARE),
        ('hat', 'MX 3 / HATS', 1568, HAT),
    ]:
        d.rounded_rectangle((x, 924, x + 304, 1028), radius=10, fill=PANEL)
        label(d, (x + 16, 932), title, 18, color, True)
        take.history(d, t, key, (x + 16, 974, 228, 38), 0, 5, color)
        level = take.hit(t, key)
        d.ellipse((x + 263, 978, x + 287, 1002), fill=color if level > .08 else GRID)


def render_frame(take, t):
    raw = take.shot(t)
    bar = t / BAR
    section = min(7, int(bar // 8))
    title, subtitle, view, explanation, detail = SECTIONS[section]
    local = t - section * 8 * BAR
    if section in [3, 5] and local >= 4 * BAR:
        view = 'drums'
    if t >= END:
        view, title, subtitle = 'wide', 'PULSE PRESSURE', '64 bars. One sequencer. An entire patch in motion.'
    canvas = Image.new('RGB', (1920, 1080), BG)
    d = ImageDraw.Draw(canvas)
    label(d, (48, 17), 'SENSIBURNER / METROPOLIX LAB', 21, MUTED, True)
    label(d, (1190, 17), '138 BPM   /   D MINOR   /   LIVE RACK PERFORMANCE', 20, LEAD, True)
    label(d, (48, 52), title, 37, INK, True)
    label(d, (650, 65), subtitle, 24, MUTED)
    label(d, (1715, 62), f'BAR {min(64, int(bar) + 1):02} / 64', 18, INK, True)
    # A four-beat indicator follows the same audio-clock timeline as the patch.
    beat = min(255, int(t * BPM / 60)) % 4
    for i in range(4):
        x = 1738 + i * 31
        d.rounded_rectangle((x, 95, x + 22, 100), radius=2, fill=LEAD if beat == i and t < END else GRID)

    if view == 'wide':
        paste(canvas, raw, (15, 49, 1725, 810), (64, 120, 1792, 797))
    elif view == 'voices':
        label(d, (64, 127), 'TRK 1 → BASS     /     TRK 2 → LEAD     /     A + B → FILTERS', 20, LEAD, True)
        paste(canvas, raw, (15, 49, 1185, 429), (64, 164, 1792, 582))
        d = ImageDraw.Draw(canvas)
        label(d, (64, 769), explanation, 25, INK, True)
        label(d, (64, 807), detail, 23, MUTED)
        label(d, (64, 863), 'The traces below show the actual pitch CV and trigger signals.', 19, MUTED)
    elif view == 'focus':
        label(d, (48, 127), 'THE RUNNING SEQUENCER + MX', 20, LEAD, True)
        paste(canvas, raw, (15, 49, 585, 429), (48, 164, 930, 620))
        label(d, (1080, 127), 'ACTUAL DISPLAY / ENLARGED', 20, LEAD, True)
        # The photographed pixels are taken from this exact Rack frame.
        paste(canvas, raw, (145, 184, 238, 234), (1080, 164, 651, 350), pixels=True)
        d = ImageDraw.Draw(canvas)
        lines = {
            2: ['Stage accumulation moves the notes', 'through the selected scale.'],
            3: ['Ratcheted notes add energy.', 'Probability changes individual passes.'],
            4: ['The rhythm drops away.', 'MOD 6 pushes the lead filter upward.'],
            6: ['The next stored pattern takes over.', 'The clock and performance continue.'],
        }.get(section, [explanation, detail])
        for i, line in enumerate(lines):
            label(d, (1080, 548 + 40 * i), line, 27, INK, True)
        label(d, (1080, 654), {
            2: 'Bipolar accumulation / scale quantization',
            3: '2× ratchets / 85% accents / 56% lead swing',
            4: 'MOD 6 → A → lead filter FM',
            6: 'Native preset chain / 8 sections + effect tail',
        }.get(section, ''), 21, MUTED)
        label(d, (48, 811), 'TRK 2 / ACTUAL PITCH CV', 18, LEAD, True)
        take.history(d, t, 'pitch2', (48, 850, 930, 44), -1.5, .75, LEAD, 4)
        label(d, (1080, 731), 'LEAD VOICE / ACTUAL OUTPUT ENVELOPE', 18, MUTED, True)
        take.history(d, t, 'lead_peak', (1080, 776, 752, 118), 0, 5, LEAD, 4)
    else:
        label(d, (48, 127), 'MX 1–5 → KICK / SNARE / CLOSED HATS / PERCUSSION / OPEN HATS', 20, KICK, True)
        paste(canvas, raw, (15, 429, 825, 810), (48, 168, 1320, 621))
        label(d, (1420, 127), 'EIGHT MOD LANES → MX', 19, LEAD, True)
        paste(canvas, raw, (525, 49, 585, 429), (1420, 168, 96, 608))
        d = ImageDraw.Draw(canvas)
        for i, (key, txt, color) in enumerate([('kick', 'KICK', KICK), ('snare', 'SNARE', SNARE), ('hat', 'HATS', HAT)]):
            y = 190 + 195 * i
            label(d, (1570, y), txt, 28, color, True)
            level = take.hit(t, key)
            d.rounded_rectangle((1570, y + 55, 1856, y + 101), radius=7, fill=PANEL)
            if level > 0:
                d.rounded_rectangle((1570, y + 55, 1570 + max(8, 286 * level), y + 101), radius=7, fill=color)
            label(d, (1570, y + 119), 'Recorded trigger voltage', 17, MUTED)
        label(d, (48, 815), 'The drums are played by the sequencer’s MOD lanes through the MX expander.', 25, INK, True)
        label(d, (48, 858), '7-step percussion and 5-step open hats move around the four-on-the-floor kick.', 23, MUTED)

    dashboard(canvas, take, t)
    d = ImageDraw.Draw(canvas)
    # Eight short sections make the arrangement visible, including the break/drop.
    for i in range(8):
        x = 48 + i * 228
        d.rectangle((x, 1044, x + 216, 1048), fill=LEAD if i <= section else GRID)
    label(d, (48, 1055), 'Original Rack audio + live module footage • indicators follow recorded voltages', 15, MUTED)
    label(d, (1280, 1055), 'Independent project • not affiliated with Intellijel', 15, MUTED)
    if t > take.duration - .6:
        canvas = Image.blend(canvas, Image.new('RGB', canvas.size, BG), min(1, (t - (take.duration - .6)) / .6))
    return canvas


def master_audio(take, output):
    audio = output.parent / 'Pulse-Pressure-master.wav'
    log = output.parent / 'audio-mastering.log'
    base = f'highpass=f=25,afade=t=out:st={take.duration - .9:.3f}:d=0.9'
    analysis = subprocess.run([
        'ffmpeg', '-hide_banner', '-i', str(take.path / 'two-track-demo.wav'),
        '-af', base + ',loudnorm=I=-12:TP=-1:LRA=7:print_format=json', '-f', 'null', '-'
    ], capture_output=True, text=True, check=True)
    raw_measurement = json.JSONDecoder().raw_decode(analysis.stderr[analysis.stderr.rfind('{'):])[0]
    measured = {key: value for key, value in raw_measurement.items() if key.startswith('input_')}
    # Keep constant gain across the arrangement, preserving the breakdown/drop
    # contrast. Oversampled peak limiting catches resonant transients without
    # a loudness rider turning the quiet section back up.
    gain_db = -13 - float(measured['input_i'])
    normalization = (f'volume={gain_db:.5f}dB,aresample=192000,'
                     'alimiter=limit=0.841395:attack=5:release=80:level=false:latency=true,'
                     'aresample=48000')
    result = subprocess.run([
        'ffmpeg', '-hide_banner', '-y', '-i', str(take.path / 'two-track-demo.wav'),
        '-af', base + ',' + normalization, '-ar', '48000', '-c:a', 'pcm_s24le', str(audio)
    ], capture_output=True, text=True, check=True)
    measured['applied_gain_db'] = gain_db
    measured['mastering'] = 'Constant gain, 4x oversampled -1.5 dBFS peak limiter; no loudness riding'
    log.write_text(analysis.stderr + f'\n\nCONSTANT GAIN {gain_db:.5f} dB\nMASTER\n' + result.stderr, encoding='utf-8')
    return audio, measured


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('capture', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--proofs-only', action='store_true')
    args = parser.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    proofs = args.output.parent / 'proofs'
    proofs.mkdir(exist_ok=True)
    take = Take(args.capture)
    validate_arrangement(take, args.output)
    for t in [2, 17, 31, 45, 51, 60, 72, 79, 88, 101, 112]:
        render_frame(take, t).save(proofs / f'{t:03}.png')
    render_frame(take, 72).save(args.output.with_suffix('.png'))
    print('Proofs ready', flush=True)
    if args.proofs_only:
        return
    audio, measured = master_audio(take, args.output)
    command = [
        'ffmpeg', '-hide_banner', '-loglevel', 'warning', '-y',
        '-f', 'rawvideo', '-pixel_format', 'rgb24', '-video_size', '1920x1080',
        '-framerate', '30', '-i', '-', '-i', str(audio), '-map', '0:v', '-map', '1:a',
        '-c:v', 'libx264', '-preset', 'fast', '-crf', '18', '-threads', '4',
        '-pix_fmt', 'yuv420p', '-c:a', 'aac', '-b:a', '320k', '-ar', '48000',
        '-t', str(take.duration), '-movflags', '+faststart',
        '-metadata', 'title=Metropolix Lab — Pulse Pressure',
        '-metadata', 'artist=sensiburner', str(args.output)]
    with (args.output.parent / 'video-encode.log').open('w') as log:
        encoder = subprocess.Popen(command, stdin=subprocess.PIPE, stderr=log)
        try:
            for n in range(round(take.duration * 30)):
                encoder.stdin.write(render_frame(take, n / 30).tobytes())
                if n % 300 == 0:
                    print(f'Editing {n / 30:.0f} / {take.duration:.0f} seconds', flush=True)
        finally:
            encoder.stdin.close()
        if encoder.wait() != 0:
            raise RuntimeError('Encoding failed; inspect video-encode.log')
    args.output.with_suffix('.json').write_text(json.dumps(dict(
        duration=take.duration, width=1920, height=1080, fps=30, bpm=BPM,
        musical_end=END, audio_source='Actual Rack AUDIO input, 48 kHz stereo',
        capture_frames=len(take.frames), max_frame_gap=float(max(np.diff(take.times))),
        audio_measurement=measured,
        sections=[dict(seconds=i * 8 * BAR, title=entry[0]) for i, entry in enumerate(SECTIONS)]), indent=2) + '\n')
    print(args.output, flush=True)


if __name__ == '__main__':
    main()
