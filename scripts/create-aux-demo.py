"""Add real AUX feedback cables to a portable two-voice hardware-panel patch."""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
patch = json.loads((root / 'examples/Hardware-panel-demo.vcv').read_text(encoding='utf-8'))
module = next(m for m in patch['modules'] if m['plugin'] == 'MetropolixLab')
module['version'] = json.loads((root / 'plugin.json').read_text(encoding='utf-8'))['version']
module['data'].update(schemaVersion=7, selectedAux=0, uiPage=13, uiField=0,
                      uiMod=False, uiEdit=False, uiChooseFunction=False)
module['params'] = [p for p in module['params'] if p['id'] < 404]
# X: +/-2V from A -> +/-0.5V chromatic transpose on track 1.
# Y: 0..5V ramp from B -> 0..-0.5V chromatic transpose on track 2.
# Z: track 1 gate -> inverted Mute on track 2 (plays during track 1 gates).
for input_index, values in enumerate(((.25, 3, 0), (-.1, 3, 1), (-1, 8, 1))):
    for field, value in enumerate(values):
        module['params'].append({'id': 404 + input_index * 3 + field, 'value': value})
for input_index, (output, color) in enumerate(((5, '#d76737'), (6, '#2d8c89'), (1, '#bda033'))):
    patch['cables'].append(dict(id=200+input_index, outputModuleId=module['id'],
        outputId=output, inputModuleId=module['id'], inputId=2+input_index, color=color))
patch.pop('path', None)
(root / 'examples/Aux-inputs-demo.vcv').write_text(json.dumps(patch, indent=2)+'\n', encoding='utf-8')
