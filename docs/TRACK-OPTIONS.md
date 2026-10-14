# Track menu and source options — 2.17.0

Press ALT then TRK 1/2 to open its hardware-style menu (manual pp.33–37).
Drag the encoder or OLED to browse. Press to edit a multi-choice field; press
again to return to browsing. Binary choices toggle immediately. Hold the
encoder to restore that item’s default. Right-click for desktop menus or numeric
entry. Existing CTRL ownership still locks directly controlled settings.

The menu currently includes Transpose, Slider Octaves/Direction, Pulse Count,
Gate Type, Gate Stretching, Ratchets, Rest Pitch, Velocity and Init Trk Defaults.
Global Reset and MIDI choices are available; see [Clock/reset](CLOCK-RESET.md)
and [MIDI](MIDI.md). The optional editor
and right-click track menu also expose the settings.

| Setting | Behavior |
| --- | --- |
| Transpose | −60…+60 chromatic semitones after scale quantization |
| Pulse Count: Switch | Shared physical switches determine each stage’s duration |
| Pulse Count: 1…8 | Every stage on this track uses that many pulses; CTRL offsets still apply |
| Gate Type: Switch | Shared physical gate switches determine articulation |
| Single / Multi / Hold | Every stage uses the selected gate mode, including physical rests |
| Single+Rest / Multi+Rest / Hold+Rest | Change non-rest stages while retaining physical rests |
| Rest Pitch: Hold | Physical Rest stages keep the preceding pitch |
| Rest Pitch: Update | Rest stages change pitch despite sending no gate; long envelope tails can reveal ghost notes or slides |

Each track has independent sources. Shared sliders/switches remain programmed
and immediately become effective again when Switch is selected. Gate stretching,
ratchets, Gate Scale, probability, skip, stage overrides and accumulation continue
to operate on the effective stage. Rest Pitch Update does not itself generate a
gate or accumulate through a rest. Probability rejection and mute still hold
pitch. Existing Acid slide behavior can hold the gate on a slid stage; stage gate
overrides retain their separate articulation semantics.

Init Trk Defaults resets only the selected track’s settings and stage edits.
It leaves shared pitch/pulse/gate controls, the other track, MOD lanes, global
routes and preset bank intact. CTRL-owned parameters stay under their controller.
Initialization, toggles, edits and long resets support Rack undo.

Open [Track-options-performance.vcv](../examples/Track-options-performance.vcv),
select AUDIO and press RUN. Four chained scenes compare switch-based and uniform
pulse/gate sources, Rest Pitch modes, velocity accents and independent percussion.
Disable the chain to edit one scene; ALT → TRK opens its source settings.

Preset format7 appends six fields after format6’s 484-entry prefix. Older banks
and patches migrate to Switch sources and Rest Pitch Hold, preserving their sound.
All earlier parameter/cable IDs stay fixed. The transpose range extension retains
old values. Source changes take effect at the existing engine boundaries; exact
live-edit precedence and hardware comparison remain open to tester feedback.

## Compact display — 2.56.0

The track menu now follows the four-row p33 reference, with mixed-case text,
inverted browsing and a separate value highlight. Slider Octaves retains the
Rack Shared extension, and correctly shows a pending 1–4 octave choice with `?`.
See [UI lists](UI-SETUP.md) for compact labels and confirmation behavior.
