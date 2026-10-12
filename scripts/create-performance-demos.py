"""Two full-rack performances using installed Audible Instruments and Befaco.

Source mappings: VCVRack/AudibleInstruments and VCVRack/Befaco, v2 sources.
No samples or third-party binaries are redistributed with these patches.
"""
import json
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def create(broken=False):
    modules, cables = [], []
    bpm = 100 if broken else 124
    name = 'Broken-Orbit' if broken else 'Neon-Transit'
    versions = {'MetropolixLab': '2.4.1', 'Fundamental': '2.6.4',
                'Core': '2.6.6', 'AudibleInstruments': '2.0.0', 'Befaco': '2.4.0'}

    def mod(id, model, pos, params=None, plugin='Fundamental', data=None):
        entry = dict(id=id, plugin=plugin, model=model, version=versions[plugin],
                     pos=pos, params=[dict(id=k,value=v) for k,v in sorted((params or {}).items())])
        if data is not None:
            entry['data'] = data
        modules.append(entry)

    def cable(src, out, dst, inp, color='#53cfc6'):
        cables.append(dict(id=len(cables)+1, outputModuleId=src,outputId=out,
                           inputModuleId=dst,inputId=inp,color=color))

    notes = [0,7,10,12,3,7,5,10] if broken else [0,0,7,10,12,7,3,5]
    pulses = [2,1,1,3,1,2,1,5] if broken else [2,2,1,3,2,2,3,1]
    p = {0:bpm,1:1,3:0 if broken else 9,4:3 if broken else 2,5:2,402:2,403:3}
    for s in range(8):
        p.update({6+s:notes[s]/24,14+s:pulses[s],22+s:2 if s in [0,3,5] else 1})
    for track,length,division,order,transpose in [(0,4 if broken else 2,2,0,-24),
                                                (1,8,1 if broken else 2,3 if broken else 2,0)]:
        b=30+track*57
        p.update({b:length,b+1:0,b+2:division,b+3:order,b+4:transpose,b+5:42 if track==0 else 58,b+6:20})
        for s in range(8):
            for row,val in enumerate([1,100,0,1 if track==0 and s==1 else 0,0,0]):
                p[b+9+row*8+s]=val
    lanes = [
        (2,0,2,0,[5,0,0,5,0,0,5,0] if broken else [5,0,5,0,5,0,5,0]),
        (2,0,2,0,[0,0,5,0,0,0,5,0]),
        (32,2,0,2,[0,0,-2/12,-2/12,3/12,3/12,-5/12,-5/12]),
        (16,1,1,1,[0,0,0,1,0,0,1,0]),
        (4,7,1,1,[0,0,0,0,0,5/7,0,5/7]),
        (16,8,2,1,[0,0,0,5,0,0,0,0]),
        (6,2,0,1,[0,0,2/12,0,-2/12,0,3/12,0]),
        (32,8,2,0,[0,0,0,0,5,5,0,0]),
    ]
    for l,(division,destination,typ,assignment,values) in enumerate(lanes):
        b=194+l*26
        p.update({b:8,b+2:division,b+6:typ,b+7:destination,b+8:assignment})
        for s,v in enumerate(values):
            p[b+10+s]=v
            p[b+18+s]=0
    mod(1,'Metropolix',[0,0],p,'MetropolixLab',dict(schemaVersion=7,expanded=False,
        modEditor=False,selectedTrack=1,uiPage=0,uiHome=1,uiField=0,uiMod=False,uiEdit=False))

    # Top row: sequencer -> bass / pitch scope -> contrasting lead / audio scope.
    mod(3,'Plaits',[36,0],{2:0,3:.2,4:.25,5:.25,9:.35,10:.32},
        'AudibleInstruments',dict(model=1 if broken else 0,lowCpu=False))
    mod(2,'Scope',[49,0],{0:2,1:2,2:1,3:-2.5,4:-1,7:1})
    if broken:
        mod(6,'Plaits',[63,0],{2:-1,3:.23,4:.3,5:.38,9:.42,10:.43},
            'AudibleInstruments',dict(model=2,lowCpu=False))
    else:
        # Rings' frequency knob at 48 yields MIDI C4 at zero V/oct input.
        mod(6,'Rings',[63,0],{2:48,3:.32,4:.48,5:.48,6:.36},
            'AudibleInstruments',dict(polyphony=1,model=2,easterEgg=False))
    mod(4,'Scope',[78,0],{0:2,2:2,4:6,7:1})
    mod(15,'VCMixer',[92,0],{0:.55,1:.55,2:.48,3:.7,4:.15},data=dict(chExp=False,mixExp=False))
    mod(9,'AudioInterface2',[104,0],{0:1},'Core')

    # Bottom row: actual triggered drum voices and visible drum bus.
    mod(20,'Kickall',[0,1],{0:48 if broken else 52,2:.03,3:.34,4:.035,5:.27},'Befaco')
    mod(21,'Plaits',[10,1],{2:-.4,3:.55,4:.45,5:.28,10:.2},
        'AudibleInstruments',dict(model=14,lowCpu=False))
    mod(22,'Plaits',[24,1],{2:0,3:.55,4:.56,5:.1,10:.12},
        'AudibleInstruments',dict(model=15,lowCpu=False))
    mod(23,'VCMixer',[38,1],{0:.75,1:.85,2:.43,3:.28,4:0},data=dict(chExp=False,mixExp=False))
    mod(24,'Scope',[50,1],{0:2,2:2,4:4,7:1})
    mod(16,'VCMixer',[64,1],{0:.55,1:.55,2:.45,3:.7,4:.2},data=dict(chExp=False,mixExp=False))
    for id,x,t in [(12,76,1.5),(13,88,2.5)]:
        mod(id,'Delay',[x,1],{0:math.log10(t*1000)/4,1:.32 if broken else .28,2:.4,3:1})
        cable(6,0,id,4)
        cable(1,4,id,5,'#ac8ddd')
    mod(25,'Scope',[100,1],{0:2,2:2,4:-1,7:1})
    for out,inp in [(0,7),(1,5)]:
        cable(1,out,3,inp,'#e4ac68')
    cable(1,2,6,7 if broken else 6)
    cable(1,3,6,5)
    cable(1,0,2,0,'#e4ac68'); cable(1,1,2,1,'#e4ac68')
    cable(3,0,4,0,'#e4ac68'); cable(6,0,4,1)
    cable(1,5,20,0,'#ff7085'); cable(1,6,21,5,'#8eb8ff'); cable(1,4,22,5,'#d3be69')
    for src,inp,color in [(20,1,'#ff7085'),(21,2,'#8eb8ff'),(22,3,'#d3be69')]:
        cable(src,0,23,inp,color)
    cable(20,0,24,0,'#ff7085'); cable(23,0,24,1,'#8eb8ff')
    cable(1,5,25,0,'#ff7085'); cable(1,6,25,1,'#8eb8ff')
    for mix,delay in [(15,12),(16,13)]:
        for src,out,inp in [(3,0,1),(6,0,2),(23,0,3),(delay,1,4)]:
            cable(src,out,mix,inp)
    cable(15,0,9,0); cable(16,0,9,1)
    patch=dict(version='2.6.6',zoom=1,gridOffset=[-1,-.05],modules=modules,cables=cables)
    path=ROOT/'examples'/f'{name}.vcv'
    path.write_text(json.dumps(patch,indent=2)+'\n')
    print(f'{name}: {bpm} BPM, {len(modules)} modules, {len(cables)} cables')


create()
create(True)
