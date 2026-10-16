"""Inspect a Rack package without executing/extracting its contents."""
import argparse,hashlib,io,json,re,struct,subprocess,tarfile
from pathlib import Path

parser=argparse.ArgumentParser()
parser.add_argument('--platform',required=True,choices=['win-x64','lin-x64','mac-x64','mac-arm64'])
parser.add_argument('--package',required=True,type=Path)
parser.add_argument('--zstd',default='zstd')
parser.add_argument('--report',type=Path)
args=parser.parse_args();root=Path(__file__).resolve().parents[1]
raw=subprocess.run([args.zstd,'-d','-c',str(args.package)],check=True,capture_output=True).stdout
manifest=json.loads((root/'plugin.json').read_text());extension='dll' if args.platform=='win-x64' else 'so' if args.platform=='lin-x64' else 'dylib'
with tarfile.open(fileobj=io.BytesIO(raw)) as archive:
    members=archive.getmembers();names=[m.name for m in members if m.isfile()]
    assert len(names)==len(set(names)),'duplicate package paths'
    for member in members:
        path=Path(member.name)
        assert path.parts[0]=='MetropolixLab' and '..' not in path.parts and not path.is_absolute(),member.name
        assert member.isfile() or member.isdir(),member.name
        assert not any(part in member.name for part in ('RESUME','tmp/','checkpoints/','.pdf','AudioQA')),member.name
        if member.isfile() and member.name!=f'MetropolixLab/plugin.{extension}':
            assert archive.extractfile(member).read()==(root/path.relative_to('MetropolixLab')).read_bytes(),member.name
    assert json.load(archive.extractfile('MetropolixLab/plugin.json'))==manifest
    assert [m['slug'] for m in manifest['modules']]==['Metropolix','MX']
    for required in ['docs/MANUAL-COVERAGE.md','docs/MIDI.md','docs/MX.md','docs/UI-FIDELITY.md','docs/BACKUPS.md','docs/MAINTENANCE.md','docs/RUNTIME-VALIDATION.md','docs/ORDER-SHORTCUTS.md','docs/PRESET-STORAGE.md','docs/MOD-GROUP-MUTE.md','docs/TIMING-SHORTCUTS.md','docs/STAGE-PAIRS.md','docs/STAGE-PATTERNS.md','docs/PLATFORMS.md','examples/MX-performance.vcv','examples/Backup-example.json','examples/Backup-bank-8.json']:
        assert 'MetropolixLab/'+required in names,required
    binary=archive.extractfile(f'MetropolixLab/plugin.{extension}').read()
    assert not re.search(rb'METROPOLIX_[A-Z0-9_]*QA',binary),'QA environment marker in production binary'
    assert b'Internal audio test' not in binary and b'QA COMPLETE' not in binary,'QA recorder/report in production binary'
    if args.platform=='lin-x64':
        assert binary[:5]==b'\x7fELF\x02' and struct.unpack_from('<H',binary,18)[0]==62,'ELF architecture'
    elif args.platform.startswith('mac-'):
        assert binary[:4]==b'\xcf\xfa\xed\xfe','Mach-O64 magic'
        assert struct.unpack_from('<I',binary,4)[0]==(0x100000c if args.platform=='mac-arm64' else 0x1000007),'Mach-O architecture'
        assert struct.unpack_from('<I',binary,12)[0]==6,'Mach-O dylib'
    else:
        assert binary[:2]==b'MZ';offset=struct.unpack_from('<I',binary,60)[0]
        assert binary[offset:offset+4]==b'PE\0\0' and struct.unpack_from('<H',binary,offset+4)[0]==0x8664,'PE x64 architecture'
report=dict(version=manifest['version'],platform=args.platform,sha256=hashlib.sha256(args.package.read_bytes()).hexdigest(),binarySha256=hashlib.sha256(binary).hexdigest(),files=names)
if args.report:args.report.write_text(json.dumps(report,indent=2)+'\n')
print('PASS package contents, source bytes, public models, architecture and absent QA code:',args.platform,report['sha256'])
