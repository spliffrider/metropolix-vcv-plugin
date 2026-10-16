"""Run actual Rack widget/renderer/audio callbacks in an isolated Linux profile."""
import argparse,json,os,shutil,subprocess
from pathlib import Path
from PIL import Image,ImageChops

p=argparse.ArgumentParser();p.add_argument('--rack',required=True,type=Path)
p.add_argument('--mode',required=True,choices=['Performance','Output','Chain','Pending','Scale','CtrlEditor','StagePattern','ScaleCopy','StagePair','Timing','Group','Storage','Order','Fidelity','Backup','Polish','Display','MxAudio','MidiAudio'])
args=p.parse_args();root=Path(__file__).resolve().parents[1];rack=args.rack.resolve()
profile=root/'tmp/linux-rack-qa';output=root/f'tmp/linux-{args.mode.lower()}-qa'
output.mkdir(parents=True,exist_ok=True);plugin=profile/'plugins-lin-x64/MetropolixLab';plugin.mkdir(parents=True,exist_ok=True)
shutil.copy2(root/'build/native-ui-test/plugin.so',plugin/'plugin.so')
for name in ['res']:shutil.copytree(root/name,plugin/name,dirs_exist_ok=True)
manifest=json.loads((root/'plugin.json').read_text());manifest['modules'].append(dict(slug='AudioQA',name='Internal audio test'))
(plugin/'plugin.json').write_text(json.dumps(manifest));shutil.copy2(root/'README.md',plugin/'README.md')
(profile/'settings.json').write_text(json.dumps(dict(windowSize=[1600,950],windowPos=[0,0],sampleRate=48000,showTipsOnLaunch=False,autoCheckUpdates=False)))
flags={'Performance':'PERFORMANCE_UI','Output':'OUTPUT_UI','Chain':'CHAIN_UI','Pending':'PENDING_UI','Scale':'SCALE_UI','CtrlEditor':'CTRLEDITOR_UI','StagePattern':'STAGEPATTERN_UI','ScaleCopy':'SCALECOPY_UI','StagePair':'STAGEPAIR_UI','Timing':'TIMING_UI','Group':'GROUP_UI','Storage':'STORAGE_UI','Order':'ORDER_UI','Fidelity':'FIDELITY_UI','Backup':'BACKUP_UI','Polish':'POLISH','Display':'DISPLAY','MxAudio':'MX_AUDIO','MidiAudio':'AUDIO'}
env=os.environ.copy();env['METROPOLIX_QA_DIR']=str(output);env['METROPOLIX_'+flags[args.mode]+'_QA']='1'
# The distributed Linux executable resolves libRack.so from its launch directory.
# Explicitly use the application library, rather than the separate SDK host library.
env['LD_LIBRARY_PATH']=str(rack.parent)+(os.pathsep+env['LD_LIBRARY_PATH'] if env.get('LD_LIBRARY_PATH') else '')
source='MX-performance.vcv' if args.mode in ['Fidelity','MxAudio'] else 'MIDI-performance.vcv' if args.mode=='MidiAudio' else 'Hardware-panel-demo.vcv' if args.mode in ['Performance','Pending','Scale','Display','Polish','Order','Storage','Group','Timing','StagePair','ScaleCopy','CtrlEditor','StagePattern'] else 'Hybrid-demo.vcv'
patch=json.loads((root/'examples'/source).read_text())
if args.mode in ['Display','Polish']:
    patch['modules']=[m for m in patch['modules'] if m['plugin']=='MetropolixLab'];patch['cables']=[];patch['zoom']=2.;patch['gridOffset']=[0,0]
if args.mode in ['MxAudio','MidiAudio']:
    env['METROPOLIX_AUDIO_QA']='1'
    for m in patch['modules']:
        if m['plugin']=='MetropolixLab' and m['model']=='Metropolix':
            for parameter in m['params']:
                if parameter['id']==1:parameter['value']=1
    patch['modules'].append(dict(id=100,plugin='MetropolixLab',model='AudioQA',pos=[100,1]))
    for channel in range(2):
        tap=[c for c in patch['cables'] if c['inputModuleId']==9 and c['inputId']==channel];assert len(tap)==1
        patch['cables'].append(dict(id=100+channel,outputModuleId=tap[0]['outputModuleId'],outputId=tap[0]['outputId'],inputModuleId=100,inputId=channel,color='#777777'))
    if args.mode=='MxAudio':
        for port in range(8):patch['cables'].append(dict(id=102+port,outputModuleId=120,outputId=port,inputModuleId=100,inputId=2+port,color='#e0ac36'))
        for cable,module,port in [(110,19,10),(111,131,11)]:patch['cables'].append(dict(id=cable,outputModuleId=module,outputId=0,inputModuleId=100,inputId=port,color='#777777'))
fixture=output/'test.vcv';fixture.write_text(json.dumps(patch));(output/'report.txt').write_text('')
def run(path,name):
    previous=profile/'log.txt'
    if previous.exists():shutil.move(str(previous),str(output/f'previous-{name}.log'))
    with (output/f'console-{name}.log').open('w') as log:
        try:
            result=subprocess.run([str(rack),'-u',str(profile),'-s',str(rack.parent),str(path)],env=env,cwd=rack.parent,stdout=log,stderr=subprocess.STDOUT,timeout=120)
        finally:
            if previous.exists():shutil.copy2(previous,output/f'rack-{name}.log')
    if result.returncode:
        print((output/f'console-{name}.log').read_text()[-6000:]);raise RuntimeError(f'Rack exited {result.returncode}')
    report=(output/'report.txt').read_text();assert 'FAIL' not in report,report
    return report
report=run(fixture,'initial')
marker={'Performance':'PERFORMANCE UI','Output':'OUTPUT UI','Chain':'CHAIN UI','Pending':'PENDING UI','Scale':'SCALE UI','CtrlEditor':'CTRLEDITOR UI','StagePattern':'STAGEPATTERN UI','ScaleCopy':'SCALECOPY UI','StagePair':'STAGEPAIR UI','Timing':'TIMING UI','Group':'GROUP UI','Storage':'STORAGE UI','Order':'ORDER UI','Fidelity':'FIDELITY UI','Backup':'BACKUP UI','Polish':'UI POLISH','Display':'DISPLAY','MxAudio':'AUDIO','MidiAudio':'AUDIO'}[args.mode]
assert 'PASS '+marker+' QA COMPLETE' in report,report
if args.mode in ['Performance','Output','Chain','Pending','Scale','CtrlEditor','StagePattern','ScaleCopy','StagePair','Timing','Group','Storage','Order','Fidelity','Backup']:
    env[f'METROPOLIX_{args.mode.upper()}_REOPEN_QA']='1';report=run(output/f'{args.mode.lower()}-roundtrip.vcv','reopen');assert f'PASS {args.mode.upper()} REOPEN QA COMPLETE' in report,report
if args.mode=='Backup':
    env['METROPOLIX_FACTORY_REOPEN_QA']='1';report=run(output/'maintenance-disarmed.vcv','maintenance-reopen');assert 'PASS FACTORY REOPEN QA COMPLETE' in report,report
if args.mode=='MxAudio':assert 'PASS MX AUDIO SIGNALS' in report,report
captures=list(output.glob('*.png'));assert captures,'No rendered scene captures'
for capture in captures:
    frame=Image.open(capture).convert('RGB')
    assert max(high for low,high in frame.getextrema())>20,f'Blank rendering: {capture}'
    red,green,blue=frame.split()
    assert sum(ImageChops.subtract(blue,red).histogram()[21:])>20,f'No blue OLED pixels: {capture}'
if args.mode=='Polish':
    off=Image.open(output/'bloom-off.png').convert('RGB').crop((465,100,795,275))
    on=Image.open(output/'bloom-on.png').convert('RGB').crop((465,100,795,275))
    assert ImageChops.difference(off,on).getbbox(),'Pitch light Bloom had no rendered effect'
print('PASS populated real scene/OLED captures:',len(captures))
print('PASS actual Linux Rack callbacks/render/save/audio:',args.mode)
