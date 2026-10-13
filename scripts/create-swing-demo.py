"""An evolving straight/swing comparison using the portable chain arrangement."""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
patch = json.loads((root / "examples/Chain-performance.vcv").read_text())
module = next(m for m in patch["modules"] if m["plugin"] == "MetropolixLab")
module["version"] = json.loads((root / "plugin.json").read_text())["version"]
amounts = [(50, 50), (54, 62), (58, 66), (62, 75)]
for slot, preset in enumerate(module["data"]["presets"]):
    if preset is None:
        continue
    assert len(preset) == 422
    preset.extend(amounts[slot])
    # Spare lane 8 gently varies the melody's swing after its first pair.
    preset[376 + 2] = 8
    preset[376 + 7] = 9  # ModSwing, appended destination ID.
    preset[376 + 8] = 1  # Track 2 only.
    preset[376 + 9] = 1 if slot == 0 else 0  # First preset remains fully straight.
    preset[376 + 10:376 + 18] = [0, .25, -.25, .5, -.5, 0, .25, 0]
    preset[376 + 18:376 + 26] = [0] * 8
params = {p["id"]: p["value"] for p in module["params"]}
params.update(enumerate(module["data"]["presets"][0][:422]))
params.update({1: 0, 429: 50, 430: 50})  # Starts silent until RUN.
module["params"] = [dict(id=i, value=params.get(i, 0)) for i in range(431)]
module["data"].update(schemaVersion=12, presetFormat=2, uiPage=20, uiField=0,
                      selectedTrack=1, uiMod=False)
patch.pop("path", None)
assert all(m["plugin"] in {"Core", "Fundamental", "MetropolixLab"} for m in patch["modules"])
for m in patch["modules"]:
    if m["plugin"] == "Core" and "audio" in m.get("data", {}):
        assert m["data"]["audio"].get("deviceId", -1) == -1
output = root / "examples/Swing-performance.vcv"
output.write_text(json.dumps(patch, indent=2) + "\n")
print(output)
