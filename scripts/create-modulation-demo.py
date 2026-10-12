"""Author the MOD example with named parameter mappings, preserving older demos."""
import json
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
patch = json.loads((ROOT / "examples/Two-voice-demo.vcv").read_text())
module = patch["modules"][0]
module["version"] = "2.1.0"
module["data"].update(schemaVersion=4, modEditor=True, selectedLane=0)
params = {p["id"]: p["value"] for p in module["params"]}
MOD_BASE, MOD_STRIDE = 194, 26
LENGTH, DIVISION, TYPE, DESTINATION, ASSIGNMENT = 0, 2, 6, 7, 8
VALUE, RAMP = 10, 18


def lane(index, values, division=4, destination=0, assignment=0, ramp=False, cv_type=0):
    base = MOD_BASE + index * MOD_STRIDE
    for prop, value in [(LENGTH, 8), (DIVISION, division), (TYPE, cv_type),
                        (DESTINATION, destination), (ASSIGNMENT, assignment)]:
        params[base + prop] = value
    for stage, value in enumerate(values):
        params[base + VALUE + stage] = value
        params[base + RAMP + stage] = int(ramp)


# MOD 1 -> A: stepped bipolar voltage. MOD 2 -> B: continuous unipolar ramps.
lane(0, [-2, -1, 0, 1, 2, 1, 0, -1])
lane(1, [0, 5, 0, 5, 0, 5, 0, 5], ramp=True, cv_type=1)
# Independent internal routes; external A/B assignments don't consume a lane.
lane(2, [0, 0, 1, 1, 0, 0, -1, -1], division=8, destination=1)
lane(3, [0, 0, 0, 0, 1.43, 0, 2.86, 0], destination=7, assignment=1)
lane(4, [0, -1, 0, 1, 0, -1, 0, 1], division=2, destination=5, assignment=2)
params[402], params[403] = 2, 3
module["params"] = [{"id": key, "value": value} for key, value in sorted(params.items())]
# First Scope shows A/B. The adjacent Scope retains note pitch/gate monitoring.
# Five seconds shows a complete eight-stage MOD cycle at this tempo.
# Parameter mapping: https://github.com/VCVRack/Fundamental/blob/v2/src/Scope.cpp
scope = next(item for item in patch["modules"] if item["id"] == 2)
scope["params"] = [{"id": key, "value": value} for key, value in
                   [(0, 1), (1, 0), (2, 1), (3, -2.5), (4, -math.log2(5)), (7, 1)]]
patch["cables"][0]["outputId"] = 5
patch["cables"][1]["outputId"] = 6
patch["modules"].append({"id": 10, "plugin": "Fundamental", "model": "Scope", "version": "2.6.4", "pos": [87, 0]})
for output, color in [(0, "#d76737"), (1, "#2d8c89")]:
    patch["cables"].append({"id": len(patch["cables"]) + 1,
                            "outputModuleId": 1, "outputId": output,
                            "inputModuleId": 10, "inputId": output, "color": color})
(ROOT / "examples/Modulation-demo.vcv").write_text(json.dumps(patch, indent=2) + "\n")
