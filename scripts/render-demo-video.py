"""Edit timestamped Rack captures into a captioned 1080p demonstration.

Requires Python 3 + Pillow and FFmpeg on PATH. The audio and UI come from the
same capture; captions, close-ups, level adjustment and boundary fades are edits.
"""
import argparse
import bisect
import csv
import json
import math
import subprocess
import wave
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageStat

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('capture', type=Path)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('--proofs-only', action='store_true')
parser.add_argument('--font-dir', type=Path, default=Path('C:/Windows/Fonts'))
args = parser.parse_args()
args.output.parent.mkdir(parents=True, exist_ok=True)
proofs = args.output.parent / 'video-proofs'
proofs.mkdir(exist_ok=True)
W, H, FPS = 1920, 1080, 30
BAR = 240 / 108
TEAL, GOLD = '#71cec4', '#e4ac68'
INK, MUTED = '#f0f3f4', '#a6b1ba'
BG, LINE = '#12191f', '#30404b'
fonts = {}


def font(size, bold=False):
    key = size, bold
    if key not in fonts:
        fonts[key] = ImageFont.truetype(str(args.font_dir / ('segoeuib.ttf' if bold else 'segoeui.ttf')), size)
    return fonts[key]


chapters = [
    (0, 'NIGHT CURRENT', 'Night\nCurrent', 'A 32-bar performance\nbuilt inside VCV Rack.', '108 BPM  /  A MINOR'),
    (4*BAR, 'TWO INDEPENDENT TRACKS', 'Two independent\ntracks', 'Bass: 2 stages at /4\nMelody: 8 stages at /2\nPendulum play order', 'SHARED SLIDERS / DIFFERENT PATHS'),
    (8*BAR, 'AN OCTAVE ANSWER', 'Raise the melody', 'MOD 4 adds an octave in bars 9–12.', 'OCTAVE → TRACK 2'),
    (12*BAR, 'MELODIC MOVEMENT', 'A melody that keeps moving', 'MOD 3 shifts notes before quantization on its own three-bar cycle.', 'PITCH PRE → TRACK 2'),
    (16*BAR, 'THE BREAKDOWN', 'Make room for a breakdown', 'MOD 8 rests the bass for four bars. The melody and delay carry on.', 'MUTE → TRACK 1'),
    (20*BAR, 'THE RETURN', 'Bring the\nbass back', 'Low notes return beneath\nthe octave-raised melody.', 'BASS + MELODY / REUNITED'),
    (24*BAR, 'RHYTHM AND EXPRESSION', 'Articulation\nin motion', 'Double-note fills.\nChanging gate lengths.\nBrief slides and rests.', 'RATCHETS / GATES / SLIDE'),
    (28*BAR, 'EIGHT MODULATION LANES', 'Eight lanes.\nAn evolving\nperformance.', 'Pitch, register, rhythm,\nfilter movement and space.', 'BUILT WITH THE MODULE’S MOD LANES'),
    (32*BAR, 'TRY THE PUBLIC ALPHA', 'Make it\nyour own', 'Plugin and demo patch:\ngithub.com/spliffrider/\nmetropolix-vcv-plugin', 'SENSIBURNER / VCV RACK 2'),
]
with wave.open(str(args.capture / 'two-track-demo.wav'), 'rb') as wav:
    assert wav.getframerate() == 48000 and wav.getnchannels() == 2
    duration = wav.getnframes() / wav.getframerate()
rows = list(csv.DictReader((args.capture / 'frames.csv').open()))
assert rows and float(rows[0]['seconds']) == 0
discarded = []
usable = []
for row in rows:
    # A zoom cut can expose one intermediate Rack layout frame. Hold the last
    # complete frame over that brief transition, preserving the audio timeline.
    origin = (25,61) if row['wide'] == '1' else (34,73)
    with Image.open(args.capture / row['file']) as shot:
        probe = shot.crop((origin[0]+5,origin[1]+5,origin[0]+120,origin[1]+80))
        if sum(ImageStat.Stat(probe).mean)/3 <= 100:
            discarded.append(row)
            continue
    usable.append(row)
rows = usable
assert len(discarded) <= 8, 'Camera drift exceeds a brief layout transition'
assert rows and float(rows[0]['seconds']) == 0
times = [float(row['seconds']) for row in rows]
assert times == sorted(times)
assert duration - times[-1] < .5, 'Capture ended too early for its soundtrack'
assert max(b-a for a,b in zip(times,times[1:])) < .5, 'Unexpected gap in the capture'


def chapter(t):
    return max(i for i, item in enumerate(chapters) if item[0] <= t)


def wrapped(draw, text, x, y, width, size, color=INK, bold=False, spacing=1.35):
    for paragraph in text.split('\n'):
        line = ''
        for word in paragraph.split():
            candidate = (line + ' ' + word).strip()
            if draw.textlength(candidate, font=font(size, bold)) > width and line:
                draw.text((x,y), line, font=font(size,bold), fill=color)
                y += round(size*spacing)
                line = word
            else:
                line = candidate
        draw.text((x,y), line, font=font(size,bold), fill=color)
        y += round(size*spacing)
    return y


def compose(index, ch):
    row = rows[index]
    source = Image.open(args.capture / row['file']).convert('RGB')
    assert source.size == (1800,1050), f'Unexpected Rack capture dimensions: {source.size}'
    wide = row['wide'] == '1'
    crop = (25,61,1659,688) if wide else (34,73,1181,928)
    panel = source.crop(crop)
    # The silver panel must stay in frame, including after Rack zoom changes.
    probe = panel.crop((5,5,min(120,panel.width),min(80,panel.height)))
    assert sum(ImageStat.Stat(probe).mean)/3 > 100, f'Camera lost panel in {row["file"]}'
    canvas = Image.new('RGB', (W,H), BG)
    d = ImageDraw.Draw(canvas)
    d.text((56,28), 'METROPOLIX LAB', font=font(31,True), fill=INK)
    d.text((1448,36), 'VCV RACK 2  /  PUBLIC ALPHA', font=font(21), fill=MUTED)
    d.line((56,87,1864,87), fill=LINE, width=2)
    _, kicker, title, description, detail = chapters[ch]
    if wide:
        d.text((56,104), kicker, font=font(20,True), fill=TEAL)
        d.text((56,140), title.replace('\n',' '), font=font(49,True), fill=INK)
        d.text((56,207), description.replace('\n',' '), font=font(27), fill=MUTED)
        panel = panel.resize((1808,694), Image.Resampling.LANCZOS)
        canvas.paste(panel, (56,266))
    else:
        panel = panel.resize((1120,835), Image.Resampling.LANCZOS)
        canvas.paste(panel, (56,124))
        d.text((1240,132), f'{ch+1:02d}  /  {kicker}', font=font(19,True), fill=TEAL)
        bottom = wrapped(d, title, 1240, 187, 610, 66, bold=True, spacing=1.08)
        bottom = wrapped(d, description, 1240, bottom+31, 610, 29, MUTED, spacing=1.5)
        wrapped(d, detail, 1240, bottom+25, 610, 18, GOLD, True)
        # Magnify the same live OLED visible on the module. No redrawing.
        d.text((1240,651), 'LIVE DISPLAY', font=font(18,True), fill=MUTED)
        oled = source.crop((327,377,537,487)).resize((588,308), Image.Resampling.NEAREST)
        canvas.paste(oled, (1240,684))
    return canvas


def decorate(canvas, t):
    d = ImageDraw.Draw(canvas)
    end = 32*BAR
    progress = min(t/end,1)
    y = 1013
    d.line((56,y,1864,y), fill=LINE, width=4)
    d.line((56,y,56+1808*progress,y), fill=TEAL, width=4)
    for number in [0,8,16,24,32]:
        x=56+1808*number/32
        d.line((x,y-5,x,y+5), fill=TEAL if number/32<=progress else LINE, width=2)
    d.text((56,1033), 'Independent sensiburner project. Not affiliated with Intellijel.', font=font(18), fill=MUTED)
    label = f'BAR {int(t/BAR)%32+1:02d} / 32' if t < end else 'PLUGIN + PATCH ON GITHUB'
    d.text((900,1033), label, font=font(18,True), fill=TEAL)
    d.text((1280,1033), 'Audio: VCV Free voices  /  Sequence: Metropolix Lab', font=font(18), fill=MUTED)
    if t > duration-.6:
        canvas = Image.blend(canvas, Image.new('RGB',canvas.size,BG), min(1,(t-duration+.6)/.6))
    return canvas


def image_at(t):
    index = min(len(rows)-1, max(0,bisect.bisect_right(times,t)-1))
    return decorate(compose(index, chapter(t)),t)


proof_times = [t for t in [2,11,20,30,38,47,57,66,75] if t<duration]
for t in proof_times:
    image_at(t).save(proofs / f'{t:02d}s.png')
image_at(min(2,duration/2)).save(args.output.with_suffix('.png'))
print(f'Prepared {len(proof_times)} proof frames', flush=True)
if args.proofs_only:
    raise SystemExit(0)

# Keep the preview's dynamics. Apply a fixed, bounded gain for video playback.
# The capture test already rejects peaks >= 4 V (0.4 of WAV full scale).
# 2x is therefore <= 0.8 full scale before AAC; final output is checked separately.
command = ['ffmpeg','-hide_banner','-loglevel','warning','-y',
           '-f','rawvideo','-pixel_format','rgb24','-video_size','1920x1080',
           '-framerate',str(FPS),'-i','-', '-i',str(args.capture/'two-track-demo.wav'),
           '-map','0:v:0','-map','1:a:0','-c:v','libx264','-preset','fast','-crf','19',
           '-threads','4','-pix_fmt','yuv420p','-c:a','aac','-b:a','256k',
           '-af',f'volume=2,afade=t=in:st=0:d=0.04,afade=t=out:st={duration-1.6}:d=1.6',
           '-t',str(duration),'-movflags','+faststart',
           '-metadata','title=Metropolix Lab — Night Current',
           '-metadata','artist=sensiburner',str(args.output)]
with (args.output.parent/'video-encode.log').open('w') as log:
    encoder = subprocess.Popen(command, stdin=subprocess.PIPE, stderr=log)
    last_key, base = None, None
    try:
        for frame in range(round(duration*FPS)):
            t=frame/FPS
            index=min(len(rows)-1,max(0,bisect.bisect_right(times,t)-1))
            key=index,chapter(t)
            if key!=last_key:
                base=compose(*key)
                last_key=key
            encoder.stdin.write(decorate(base.copy(),t).tobytes())
            if frame%(10*FPS)==0:
                print(f'Edited {t:.0f}/{duration:.0f} seconds',flush=True)
    finally:
        encoder.stdin.close()
    if encoder.wait()!=0:
        raise RuntimeError('FFmpeg failed; see video-encode.log')


def stamp(seconds):
    ms=round(seconds*1000)
    return f'{ms//3600000:02}:{ms//60000%60:02}:{ms//1000%60:02},{ms%1000:03}'


subtitles=[]
for i,(start,kicker,title,description,detail) in enumerate(chapters):
    if start>=duration:
        break
    end=min(duration, chapters[i+1][0] if i+1<len(chapters) else duration)
    subtitles.append(f'{i+1}\n{stamp(start)} --> {stamp(end)}\n{title.replace(chr(10)," ")}\n{description}\n')
args.output.with_suffix('.srt').write_text('\n'.join(subtitles),encoding='utf-8')
metadata=dict(duration=duration,width=W,height=H,fps=FPS,capture_frames=len(rows),
              discarded_transition_frames=discarded,
              max_capture_gap=max(b-a for a,b in zip(times,times[1:])),
              sample_rate=48000,audio_gain=2,
              chapters=[dict(start=c[0],title=c[1]) for c in chapters if c[0]<duration])
args.output.with_suffix('.json').write_text(json.dumps(metadata,indent=2)+'\n',encoding='utf-8')
print(f'Created {args.output}',flush=True)
