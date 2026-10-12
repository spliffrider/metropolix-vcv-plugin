"""Verify and archive the 2.3.0 milestone; existing session backups are preserved."""
import hashlib
import json
from pathlib import Path
import shutil
import zipfile

root = Path(__file__).resolve().parents[1]
checkpoint = root / 'checkpoints/2026-10-12-ui-refinement'
checkpoint.mkdir(exist_ok=True)
assert json.loads((root / 'plugin.json').read_text())['version'] == '2.3.0'
evidence = []

def passed(message):
    evidence.append('PASS ' + message)
    print(evidence[-1])

with zipfile.ZipFile(root / 'checkpoints/2026-10-12-modulation/MetropolixLab-2.1.0-source.zip') as previous:
    for name in ['Sequencer.hpp', 'PlayOrder.hpp', 'GateTiming.hpp', 'Accumulator.hpp', 'Modulation.hpp']:
        matches = [n for n in previous.namelist() if n.replace('\\', '/').endswith('src/' + name)]
        assert len(matches) == 1 and previous.read(matches[0]) == (root / 'src' / name).read_bytes(), name
passed('Five musical engine headers byte-identical to accepted 2.1.0')

binary = (root / 'dist/MetropolixLab/plugin.dll').read_bytes()
for marker in (b'METROPOLIX_QA_DIR', b'METROPOLIX_AUDIO_QA', b'METROPOLIX_DISPLAY_QA', b'DISPLAY QA COMPLETE', b'AUDIO QA COMPLETE', b'Internal audio test'):
    assert marker not in binary, marker
assert len(json.loads((root / 'dist/MetropolixLab/plugin.json').read_text())['modules']) == 1
for path in (root / 'res').iterdir():
    assert path.read_bytes() == (root / 'dist/MetropolixLab/res' / path.name).read_bytes()
passed('Release contains UI assets and no QA entry points or recorder')

original = json.loads((checkpoint / 'before-refinement-autosave.json').read_text())
saved = json.loads((checkpoint / 'Before-refinement.vcv').read_text())
original.pop('path', None)
assert saved == original
passed('Pre-refinement user patch preserved except original source path')

baseline = json.loads((root / 'examples/Modulation-demo.vcv').read_text())
demo = json.loads((root / 'examples/Hardware-panel-demo.vcv').read_text())
assert baseline['cables'] == demo['cables'] and len(baseline['modules']) == len(demo['modules'])
for old, new in zip(baseline['modules'], demo['modules']):
    assert old['id'] == new['id'] and old.get('params') == new.get('params')
passed('Demo retains every accepted musical parameter and cable')

for folder, target, completion in [('qa', 'ui-report.txt', 'PASS UI QA COMPLETE'), ('display-qa', 'display-report.txt', 'PASS DISPLAY QA COMPLETE'), ('hardware-qa', 'audio-cv-report.txt', 'PASS AUDIO QA COMPLETE')]:
    report = root / 'tmp' / folder / 'report.txt'
    assert completion in report.read_text() and 'FAIL' not in report.read_text()
    shutil.copy2(report, checkpoint / target)
    passed(completion.removeprefix('PASS '))

copies = {
    'dist/MetropolixLab-2.3.0-win-x64.vcvplugin': 'MetropolixLab-2.3.0-win-x64.vcvplugin',
    'examples/Hardware-panel-demo.vcv': 'Hardware-panel-demo.vcv',
    'tmp/display-qa/reference-pose.png': 'panel-reference-pose.png',
    'tmp/display-qa/display-contact-sheet.png': 'display-contact-sheet.png',
    'tmp/hardware-qa/mod-cv.csv': 'mod-cv.csv',
}
for source, target in copies.items():
    shutil.copy2(root / source, checkpoint / target)
for path in (root / 'tmp/hardware-qa').glob('*.wav'):
    shutil.copy2(path, checkpoint / path.name)

paths = [root / n for n in ['.gitignore', 'Makefile', 'plugin.json', 'README.md', 'reference/metropolix-panel-user-reference.png']]
for directory in ['src', 'tests', 'scripts', 'docs', 'examples', 'res']:
    paths.extend(p for p in (root / directory).rglob('*') if p.is_file())
archive = checkpoint / 'MetropolixLab-2.3.0-source.zip'
with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as output:
    for path in sorted(paths):
        output.write(path, path.relative_to(root).as_posix())
with zipfile.ZipFile(archive) as saved_source:
    assert saved_source.testzip() is None
    for path in paths:
        assert saved_source.read(path.relative_to(root).as_posix()) == path.read_bytes()
passed(f'Source archive: {len(paths)} files verified byte-for-byte')
(checkpoint / 'verification.txt').write_text('\n'.join(evidence) + '\n', encoding='utf-8')
lines = [hashlib.sha256(p.read_bytes()).hexdigest().upper() + '  ' + p.name
         for p in sorted(checkpoint.iterdir()) if p.is_file() and p.name != 'SHA256SUMS.txt']
(checkpoint / 'SHA256SUMS.txt').write_text('\n'.join(lines) + '\n', encoding='utf-8')
