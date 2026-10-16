"""Create Pulse Pressure: a 64-bar, 138 BPM native Metropolix performance.

Uses installed Audible Instruments, Befaco, Valley and VCV Free. No samples or
third-party binaries are bundled. Every rhythmic trigger comes from Metropolix.
"""
import copy,json,math
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
META=json.loads((ROOT/'scripts/fixtures/pulse-pressure-defaults.json').read_text())['modules']
DEFS={(m['plugin'],m['model']):m for m in META}
VERSION=json.loads((ROOT/'plugin.json').read_text())['version']
BPM=138;BAR=240/BPM
modules=[];cables=[]
def module(id,plugin,model,pos,params=None,data=None):
 definition=DEFS.get((plugin,model),{})
 d=dict(id=id,plugin=plugin,model=model,version=VERSION if plugin=='MetropolixLab' else definition.get('version','2.6.6'),pos=pos,params=[dict(id=k,value=v) for k,v in sorted((params or {}).items())])
 state=copy.deepcopy(definition.get('data')) if data is None else data
 if state is not None:d['data']=state
 modules.append(d);return d
def cable(src,out,dst,inp,color='#62bfc1'):
 assert not any(c['inputModuleId']==dst and c['inputId']==inp for c in cables),(dst,inp)
 cables.append(dict(id=len(cables)+1,outputModuleId=src,outputId=out,inputModuleId=dst,inputId=inp,color=color))
metro=DEFS['MetropolixLab','Metropolix'];base={int(k):v for k,v in metro['defaultParams'].items()}
# D natural minor. Bass reads shared pulse/gate switches; lead traverses seven
# stages at one pulse each, so their phrases continually move against each other.
base.update({0:BPM,1:1,3:2,4:2,5:2,402:7,403:8,415:0,416:0,418:0,419:0,421:128,422:128,423:0,424:0,425:0,426:0,427:0,429:50,430:54,728:1,729:0,733:0,735:2,767:8,768:0,769:0})
notes=[0,7,10,12,0,3,5,7];pulses=[2,1,1,2,2,1,3,4];gates=[0,1,2,1,2,1,2,3]
for s in range(8):base[6+s]=notes[s]/24;base[14+s]=pulses[s];base[22+s]=gates[s]
for t in range(2):
 b=30+t*57;base.update({b:8 if t==0 else 7,b+1:0,b+2:1,b+3:0,b+4:-24 if t==0 else -12,b+5:45 if t==0 else 35,b+6:18,b+7:3,b+8:0})
 for s in range(8):
  for row,val in enumerate([1,100,0,0,0,0]):base[b+9+row*8+s]=val
 base[685+t]=1 # Tempo slide
 base[713+t*3]=0 if t==0 else 1
 base[714+t*3]=0 if t==0 else 3
 base[736+t*5]=0
for l in range(8):base[746+l*2]=0
# Programmed voltage patterns become eight real five-millisecond MX outputs.
patterns=[(8,1,[5,0,0,0,5,0,0,0]),(8,1,[0,0,0,0,5,0,0,0]),
 (8,1,[0,0,5,0,0,0,5,0]),(7,1,[0,5,0,0,0,0,5,0]),
 (5,2,[0,0,5,0,0,0,0,0]),(8,16,[.1,.4,.8,1.45,2.2,1.1,.5,.2]),
 (8,16,[.1,.2,.4,.6,.8,.4,.2,.1]),(8,1,[0,5,0,5,0,5,0,5])]
for l,(length,division,values) in enumerate(patterns):
 b=194+l*26;base.update({b:length,b+1:0,b+2:division,b+3:0,b+4:0,b+5:0,b+6:1,b+7:0,b+8:2,b+9:0})
 for s,v in enumerate(values):base[b+10+s]=v;base[b+18+s]=int(l in [5,6])
# Musical snapshots use the released format12 tuple, not a made-up bank format.
preset_ids=list(range(422))+list(range(429,465))+list(range(685,705))+list(range(707,719))+[722,723]+list(range(736,762))+[467,465,466,786]
assert len(preset_ids)==522 and len(set(preset_ids))==522
slots=[]
sections=[('FOUNDATION','Kick, rolling bass and offbeat hats'),('CROSSING PHRASES','Two tracks: 16 pulses against 7 stages'),('ACCUMULATOR','Scale-degree accumulation changes the lead'),('RATCHETS + SWING','Ratchets, probability and polymetric percussion'),('BREAKDOWN','Bass and kick leave room for the rising lead'),('THE DROP','The complete MX-driven drum pattern returns'),('CHAIN IN MOTION','New stored pattern, same performance clock'),('FINAL LAP','Denser accents and a resolved finish')]
for section in range(9):
 p=copy.deepcopy(base)
 p[95]=int(section in [0,8]) # Track2 mute
 if section in range(1,8):
  p[120+2]=1;p[120+5]=1;p[63+6]=1
 if section>=2 and section!=8:
  p[112+2]=1;p[112+5]=-1;p[94]=3;p[159]=3;p[160]=1
 if section in [3,5,6,7]:
  for s in [1,4]:p[96+s]=2
  for s in [3,6]:p[104+s]=85
  p[430]=56
 if section==6:p[90]=3;p[414]=1;p[112+2]=1;p[112+5]=0;p[94]=2
 if section==7:
  p[6+7]=0;p[88]=1;p[96+6]=3;p[104+6]=72;p[30+9+6]=2
 if section==4:
  p[38]=1
  for l in [0,1,2,3,4,7]:p[194+l*26+9]=1
  p[194+5*26+10]=.1
  for s in range(8):p[194+5*26+10+s]=.2+s*.36
 if section<3:
  for l in [3,4,7]:p[194+l*26+9]=1
 if section==8:
  p[38]=p[95]=1
  for l in range(8):p[194+l*26+9]=1
 for v in p.values():assert math.isfinite(v)
 slots.append(p)
state=copy.deepcopy(metro['data']);state.update(schemaVersion=35,presetFormat=12,uiPage=0,uiHome=1,uiField=0,uiMod=False,expanded=False,modEditor=False,selectedTrack=0,selectedLane=0,selectedRow=3)
state['presets']=[[p[i] for i in preset_ids] for p in slots]+[None]*55
links=[dict(slot=i,repeats=1,mutes=0) for i in range(9)]
state['chains']=dict(working=links,saved=[copy.deepcopy(links)]+[[] for _ in range(7)],enabled=True,loaded=0,follow=True,cursor=1)
module(1,'MetropolixLab','Metropolix',[0,0],slots[0],state)
module(2,'MetropolixLab','MX',[34,0])
# Main voices: virtual analogue oscillators through resonant analogue filters.
module(3,'AudibleInstruments','Plaits',[38,0],{2:0,3:.18,4:.58,5:.25,9:.12,10:.2},dict(model=0,lowCpu=False))
module(4,'AudibleInstruments','Ripples',[50,0],{0:.27,1:math.log2(360),2:.45})
module(6,'AudibleInstruments','Plaits',[58,0],{2:0,3:.62,4:.68,5:.42,9:.34,10:.24},dict(model=1,lowCpu=False))
module(7,'AudibleInstruments','Ripples',[70,0],{0:.68,1:math.log2(620),2:.72})
module(10,'Fundamental','Scope',[78,0],{0:2,1:0,2:2,3:0,4:3,7:1})
# VCV VCA Mix channel faders apply squared gain; keep the pitched voices
# near unity so the kick does not bury the sequencer's musical variations.
module(15,'Fundamental','VCMixer',[91,0],{0:.5,1:1.2,2:.95,3:.74,4:.60})
module(16,'Fundamental','VCMixer',[100,0],{0:.5,1:1.2,2:.95,3:.74,4:.66})
module(9,'Core','AudioInterface2',[109,0],{0:1})
# Drum row. A seven-step percussion lane and five-step open hat keep turning.
module(20,'Befaco','Kickall',[0,1],{0:55,2:.16,3:.40,4:.045,5:.42})
module(21,'AudibleInstruments','Plaits',[6,1],{2:-.35,3:.65,4:.56,5:.42,10:.22},dict(model=14,lowCpu=False))
module(22,'AudibleInstruments','Plaits',[18,1],{2:.1,3:.76,4:.58,5:.22,10:.10},dict(model=15,lowCpu=False))
module(26,'AudibleInstruments','Plaits',[30,1],{2:-.25,3:.62,4:.42,5:.65,10:.43},dict(model=15,lowCpu=False))
module(27,'AudibleInstruments','Plaits',[42,1],{2:-1.5,3:.30,4:.4,5:.48,10:.16},dict(model=13,lowCpu=False))
module(23,'Fundamental','VCMixer',[54,1],{0:.70,1:.95,2:.60,3:.52,4:.80})
module(28,'Fundamental','VCMixer',[63,1],{0:.7,1:.60,2:.60,3:.50,4:0})
module(30,'Valley','Plateau',[72,1],{0:0,1:.6,2:.015,3:7.8,4:6.7,5:.3,6:8,7:.60,8:6.1,9:7.2,10:.18,12:.4})
module(12,'Fundamental','Delay',[88,1],{0:math.log10((60/BPM)*.75*1000)/4,1:.30,2:.45,3:.25})
module(25,'Fundamental','Scope',[97,1],{0:2,1:0,2:2,3:0,4:-1,7:1})
# Colored cables identify what the real module is actually driving.
BASS='#efa95e';LEAD='#55d7c4';DRUM='#ff687f';PERC='#a995ed';CV='#89baf5'
for out,dst,inp,col in [(0,3,7,BASS),(1,3,5,BASS),(2,6,7,LEAD),(3,6,5,LEAD),(5,7,2,CV),(6,4,2,CV)]:cable(1,out,dst,inp,col)
cable(3,0,4,3,BASS);cable(6,0,7,3,LEAD)
for out,dst,inp in [(0,20,0),(1,21,5),(2,22,5),(3,27,5),(4,26,5)]:cable(2,out,dst,inp,DRUM if out<3 else PERC)
for src,inp in [(20,1),(21,2),(22,3),(28,4)]:cable(src,0,23,inp,DRUM)
cable(26,0,28,1,PERC);cable(27,0,28,2,PERC)
cable(7,2,12,4,LEAD);cable(7,2,30,0,LEAD);cable(28,0,30,1,PERC)
for mix,wet in [(15,0),(16,1)]:
 for src,out,inp,col in [(4,2,1,BASS),(7,2,2,LEAD),(23,0,3,DRUM),(30,wet,4,PERC)]:cable(src,out,mix,inp,col)
# Put a little echo on one stereo side without losing a solid mono bass/kick.
cable(12,1,28,3,LEAD)
cable(15,0,9,0);cable(16,0,9,1)
cable(1,0,10,0,BASS);cable(1,2,10,1,LEAD)
cable(2,0,25,0,DRUM);cable(23,0,25,1,PERC)
patch=dict(version='2.6.6',zoom=1,gridOffset=[-1,-.05],modules=modules,cables=cables)
path=ROOT/'examples/Pulse-Pressure.vcv';path.write_text(json.dumps(patch,indent=2)+'\n',newline='\n')
plan=dict(title='PULSE PRESSURE',bpm=BPM,key='D minor',bars=64,bar_seconds=BAR,musical_end=64*BAR,capture_seconds=115,sections=[dict(start_bar=i*8+1,start_seconds=i*8*BAR,title=t,detail=d) for i,(t,d) in enumerate(sections)],plugins=sorted({m['plugin'] for m in modules}),notes='All note, gate and drum events originate in Metropolix Lab. Chain slots 0–7 arrange 64 bars; slot 8 clears voices for effect tails.')
(ROOT/'tmp').mkdir(exist_ok=True)
(ROOT/'tmp/pulse-pressure-plan.json').write_text(json.dumps(plan,indent=2)+'\n')
print(path);print(BPM,'BPM,64bars,',len(modules),'modules,',len(cables),'cables,',round(64*BAR,3),'seconds plus tails')
