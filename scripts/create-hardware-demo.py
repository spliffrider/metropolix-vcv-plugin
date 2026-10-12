"""Present the accepted MOD demo in the hardware panel, preserving its engine settings."""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
patch = json.loads((root / 'examples/Modulation-demo.vcv').read_text())
patch['zoom'] = 1.15
for module in patch['modules']:
    if module['plugin'] == 'MetropolixLab':
        module['version'] = json.loads((root / 'plugin.json').read_text())['version']
        module['data'].update(schemaVersion=6, expanded=False, modEditor=False,
                              uiPage=0, uiField=0, uiHome=0, uiStageDetail=False,
                              uiMod=False, uiEdit=False)
    if module['id'] == 2:
        module['pos'] = [35, 0]
    if module['id'] == 10:
        module['pos'] = [48, 0]
(root / 'examples/Hardware-panel-demo.vcv').write_text(json.dumps(patch, indent=2) + '\n')
