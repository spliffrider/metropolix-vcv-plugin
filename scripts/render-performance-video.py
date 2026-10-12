"""Edit two Rack performances with signal-driven annotations and close-ups.

Requires Pillow, numpy, FFmpeg and Segoe UI. Overlays use recorded voltages;
the underlying panel, scopes, and sound are all captured in Rack.
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

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('neon',type=Path)
p.add_argument('broken',type=Path)
p.add_argument('--output',type=Path,required=True)
p.add_argument('--proofs-only',action='store_true')
a=p.parse_args()
a.output.parent.mkdir(parents=True,exist_ok=True)
proofs=a.output.parent/'performance-proofs'
proofs.mkdir(exist_ok=True)
BG='#10171d'; INK='#eff4f5'; MUTED='#a8b9c3'
COLORS=['#e4ac68','#53cfc6','#ff7085','#8eb8ff','#d3be69']
fonts={}
def font(n,b=False):
    key=n,b
    if key not in fonts:
        fonts[key]=ImageFont.truetype('C:/Windows/Fonts/'+('segoeuib.ttf' if b else 'segoeui.ttf'),n)
    return fonts[key]

class Take:
    def __init__(self,path,title,bpm,index):
        self.path,self.title,self.bpm,self.index=path,title,bpm,index
        with wave.open(str(path/'two-track-demo.wav')) as w:
            assert w.getframerate()==48000 and w.getnchannels()==2
            self.duration=w.getnframes()/48000
        self.frames=list(csv.DictReader((path/'frames.csv').open()))
        self.times=[float(r['seconds']) for r in self.frames]
        assert self.times==sorted(self.times) and self.times[0]==0
        assert max(np.diff(self.times))<.5 and self.duration-self.times[-1]<.5
        self.signals=np.genfromtxt(path/'signals.csv',delimiter=',',names=True)
        assert np.isfinite(self.signals.view(float)).all()
        assert all(np.max(self.signals[k])>=4.9 for k in ['gate1','gate2','kick','snare','hat'])
        assert all(np.max(self.signals[k])>.05 for k in ['bass_peak','lead_peak','drum_peak'])
        self.shot_index=-1
    def shot(self,t):
        index=max(0,bisect.bisect_right(self.times,t)-1)
        if index!=self.shot_index:
            self.image=Image.open(self.path/self.frames[index]['file']).convert('RGB')
            assert self.image.size==(1800,1050)
            self.shot_index=index
        return self.image
    def row(self,t):
        # Samples cover the preceding 10 ms, so never show future activity.
        return self.signals[max(0,min(len(self.signals)-1,int(t*100)-1))]
    def hit(self,t,key):
        end=max(0,min(len(self.signals)-1,int(t*100)-1))
        start=max(0,end-12)
        active=np.flatnonzero(self.signals[key][start:end+1]>1)
        return max(0,1-(end-start-active[-1])/12) if len(active) else 0

takes=[Take(a.neon,'NEON TRANSIT',124,0),Take(a.broken,'BROKEN ORBIT',100,1)]
duration=sum(x.duration for x in takes)

def text(d,xy,s,size=24,color=INK,bold=False):
    d.text(xy,s,font=font(size,bold),fill=color)

def pitch_name(v):
    midi=round(60+12*v)
    return ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'][midi%12]+str(midi//12-1)

def history(d,take,t,key,rect,lo,hi,color,seconds=2):
    x,y,w,h=rect
    first=max(0,int((t-seconds)*100)); last=min(len(take.signals),int(t*100))
    points=[]
    for i in range(first,last):
        px=x+w*(take.signals['seconds'][i]-(t-seconds))/seconds
        v=float(take.signals[key][i])
        py=y+h*(1-max(0,min(1,(v-lo)/(hi-lo))))
        points.append((px,py))
    for q in range(4):
        yy=y+h*q/3
        d.line((x,yy,x+w,yy),fill='#2a353e',width=1)
    if len(points)>1:d.line(points,fill=color,width=2)

def dashboard(canvas,take,t):
    d=ImageDraw.Draw(canvas)
    row=take.row(t)
    for i,x in enumerate([48,510]):
        col=COLORS[i]; key='pitch'+str(i+1); gate='gate'+str(i+1)
        d.rounded_rectangle((x,923,x+438,1034),radius=9,fill='#1b262f')
        text(d,(x+16,932),f'TRK {i+1} → '+('BASS' if i==0 else 'MELODY'),18,col,True)
        text(d,(x+260,930),f'{pitch_name(row[key])}  {row[key]:+.2f} V',18,INK,True)
        history(d,take,t,key,(x+16,966,338,54),-3 if i==0 else -1,0 if i==0 else 3,col)
        strength=take.hit(t,gate)
        d.ellipse((x+376,978,x+407,1009),fill=col if strength>.05 else '#3c4852')
        text(d,(x+371,1013),'GATE',11,MUTED)
    for i,(key,label,x) in enumerate([('kick','A → KICK',976),('snare','B → SNARE',1280),('hat','CLK → HAT',1584)]):
        col=COLORS[i+2]; strength=take.hit(t,key)
        d.rounded_rectangle((x,923,x+286,1034),radius=9,fill='#1b262f')
        text(d,(x+14,932),label,18,col,True)
        history(d,take,t,key,(x+14,972,213,48),0,5,col)
        d.ellipse((x+239,975,x+267,1003),fill=col if strength>.12 else '#3c4852')

def map_full(x,y):
    return (80+(x-15)*1760/1695,117+(y-49)*790/761)

def frame(take,t):
    raw=take.shot(t)
    canvas=Image.new('RGB',(1920,1080),BG)
    d=ImageDraw.Draw(canvas)
    bar=t/(240/take.bpm)
    section=min(3,int(bar//4))
    view=['full','melody','drums','full'][section]
    title=(['Two tracks. Three drum voices.','Follow the pitches into the voices.',
            'The bass rests. The drum lanes keep going.','The bass returns to a shifting melody.'] if take.index==0 else
           ['A different pulse. A different palette.','Waveshaped bass meets an FM melody.',
            'A broken kick pattern, with snare and hats.','Different cycles keep the phrase changing.'])[section]
    text(d,(48,20),'METROPOLIX LAB / PATCHES IN MOTION',24,INK,True)
    text(d,(1170,22),f'{take.index+1:02}  {take.title}  /  {take.bpm} BPM',23,COLORS[1],True)
    text(d,(48,59),title,29,INK,True)
    text(d,(1710,65),f'BAR {int(bar)%16+1:02}/16',18,MUTED)
    if view=='full':
        canvas.paste(raw.crop((15,49,1710,810)).resize((1760,790),Image.Resampling.LANCZOS),(80,117))
        d=ImageDraw.Draw(canvas)
        # Mark the real source/destination ports when their recorded gates fire.
        routes=[('gate1',0,(186,146),(575,377)),('gate2',1,(223,146),(985 if take.index==0 else 980,377)),
                ('kick',2,(39,95),(31,474)),('snare',3,(76,95),(187,760)),('hat',4,(113,95),(397,760))]
        for key,i,src,dst in routes:
            if take.hit(t,key)>.25:
                for pt in (src,dst):
                    x,y=map_full(*pt)
                    d.ellipse((x-11,y-11,x+11,y+11),outline=COLORS[i],width=3)
        # Small labels identify actual modules without obscuring their controls.
        for label,col,x,y in [('BASS',0,555,53),('MELODY',1,960,53),('KICK',2,16,433),('SNARE',3,166,433),('HATS',4,376,433)]:
            xx,yy=map_full(x,y)
            d.rectangle((xx,yy,xx+95,yy+21),fill=BG)
            text(d,(xx+6,yy),label,14,COLORS[col],True)
    elif view=='melody':
        canvas.paste(raw.crop((15,49,1170,429)).resize((1770,582),Image.Resampling.LANCZOS),(75,133))
        d=ImageDraw.Draw(canvas)
        text(d,(75,750),'PITCH CV: THE NOTES BEING SENT',19,COLORS[0],True)
        history(d,take,t,'pitch1',(75,788,820,94),-3,0,COLORS[0],4)
        history(d,take,t,'pitch2',(995,788,850,94),-1,3,COLORS[1],4)
        text(d,(75,721),'TRK 1 → Macro Oscillator 2',22,COLORS[0],True)
        text(d,(995,721),'TRK 2 → '+('Resonator' if take.index==0 else 'Macro Oscillator 2 / FM'),22,COLORS[1],True)
        text(d,(1320,750),'Last four seconds • actual CV',18,MUTED)
    else:
        canvas.paste(raw.crop((15,429,960,810)).resize((1340,540),Image.Resampling.LANCZOS),(40,185))
        canvas.paste(raw.crop((15,49,525,429)).resize((460,343),Image.Resampling.LANCZOS),(1420,185))
        canvas.paste(raw.crop((1515,471,1700,617)).resize((400,316),Image.Resampling.LANCZOS),(1450,558))
        d=ImageDraw.Draw(canvas)
        text(d,(40,133),'A → KICKALL       B → SNARE       CLK → CLOSED HATS',27,INK,True)
        text(d,(1420,141),'THE SEQUENCER',20,COLORS[1],True)
        text(d,(1420,530),'A / B — ACTUAL SCOPE',18,MUTED)
        for key,col,x,label in [('kick',2,40,'KICK'),('snare',3,440,'SNARE'),('hat',4,840,'HATS')]:
            strength=take.hit(t,key)
            text(d,(x,763),label,24,COLORS[col],True)
            d.rounded_rectangle((x,811,x+340,847),radius=5,fill='#293540')
            if strength>0:
                d.rounded_rectangle((x,811,x+max(8,340*strength),847),radius=5,fill=COLORS[col])
        text(d,(40,876),'Hit indicators and traces follow recorded cable voltages.',18,MUTED)
    dashboard(canvas,take,t)
    d=ImageDraw.Draw(canvas)
    text(d,(48,1048),'Actual Rack audio + UI • overlays follow recorded signals',16,MUTED)
    text(d,(1130,1048),'sensiburner • Independent project, not affiliated with Intellijel',16,MUTED)
    return canvas

for take in takes:
    for t in [2,9,17,25,33]:
        if t<take.duration:
            frame(take,t).save(proofs/f'{take.index}-{t:02}.png')
frame(takes[0],2).save(a.output.with_suffix('.png'))
print('Proofs ready',flush=True)
if a.proofs_only:raise SystemExit(0)

# Two original Rack recordings, with a brief fade at the musical scene cut.
pcm=[]
for take in takes:
    with wave.open(str(take.path/'two-track-demo.wav')) as w:
        sound=np.frombuffer(w.readframes(w.getnframes()),dtype='<i2').reshape(-1,2).astype(float)
    fade=round(.06*48000); end=round(.32*48000)
    sound[:fade]*=np.linspace(0,1,fade)[:,None]
    sound[-end:]*=np.linspace(1,0,end)[:,None]
    pcm.append(sound)
audio=a.output.parent/'performance-edit.wav'
with wave.open(str(audio),'wb') as w:
    w.setnchannels(2);w.setsampwidth(2);w.setframerate(48000)
    w.writeframes(np.vstack(pcm).astype('<i2').tobytes())
# Standard video mastering, not a substitute soundtrack. Measure first.
analysis=subprocess.run(['ffmpeg','-hide_banner','-i',str(audio),'-af',
    'highpass=f=25,loudnorm=I=-18:TP=-1.5:LRA=11:print_format=json','-f','null','-'],capture_output=True,text=True,check=True)
measured=json.JSONDecoder().raw_decode(analysis.stderr[analysis.stderr.rfind('{'):])[0]
audio_filter=('highpass=f=25,loudnorm=I=-18:TP=-1.5:LRA=11:linear=true:'+
    ':'.join(f'{out}={measured[key]}' for out,key in [('measured_I','input_i'),('measured_TP','input_tp'),
    ('measured_LRA','input_lra'),('measured_thresh','input_thresh'),('offset','target_offset')]))
cmd=['ffmpeg','-hide_banner','-loglevel','warning','-y','-f','rawvideo','-pixel_format','rgb24',
     '-video_size','1920x1080','-framerate','30','-i','-','-i',str(audio),'-map','0:v','-map','1:a',
     '-c:v','libx264','-preset','fast','-crf','19','-threads','4','-pix_fmt','yuv420p',
     '-af',audio_filter,'-ar','48000','-c:a','aac','-b:a','256k','-t',str(duration),
     '-movflags','+faststart','-metadata','title=Metropolix Lab — Patches in Motion',
     '-metadata','artist=sensiburner',str(a.output)]
with (a.output.parent/'performance-encode.log').open('w') as log:
    enc=subprocess.Popen(cmd,stdin=subprocess.PIPE,stderr=log)
    try:
        for take in takes:
            for n in range(round(take.duration*30)):
                enc.stdin.write(frame(take,n/30).tobytes())
                if n%300==0:print(f'{take.title}: {n/30:.0f}/{take.duration:.0f}s',flush=True)
    finally:
        enc.stdin.close()
    if enc.wait()!=0:raise RuntimeError('Encoding failed; inspect performance-encode.log')
a.output.with_suffix('.json').write_text(json.dumps(dict(duration=duration,width=1920,height=1080,
    fps=30,measured_audio=measured,scenes=[dict(name=x.title,bpm=x.bpm,duration=x.duration,
    capture_frames=len(x.frames),max_frame_gap=float(max(np.diff(x.times)))) for x in takes]),indent=2)+'\n')
print(a.output,flush=True)
