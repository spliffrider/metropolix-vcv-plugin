# Slide models — 2.13.0

Select TRK 1 or TRK 2, then SLIDE. Drag the encoder, display value, or SLIDE
button vertically to change the amount. Click the encoder to cycle Analog →
Tempo → Acid. Right-click SLIDE or the encoder to choose a named model. Each
track has its own amount and model; MOD lanes do not have this note-slide page.

Click EDIT → SLIDE and click stages to mark where slides occur. The models use
those marks differently, following the firmware 1.6 manual, pp.80–81:

| Model | Slide location | Amount controls |
| --- | --- | --- |
| Tempo | Into a marked stage | Transition time as a percentage of one track pulse; equal time for small and large intervals |
| Analog | Into a marked stage | Slew rate in volts per second; larger intervals take longer |
| Acid | From a marked stage into the next played stage | Analog slew rate; the marked stage also uses Hold gates across all its pulses |

Acid replaces the marked stage's physical GATE TYPE with Hold, including a
physical Rest switch. Explicit stage gate-length Rest, Gate Scale zero, mute
and probability rejection still suppress its gate. Its accumulation triggers
therefore follow Hold behavior rather than the replaced switch's Single/Multi
behavior. Skipped stages are not played and do not arm Acid; the previous
actually played stage determines the following glide. Reset has no previous
note to glide from.

**Hold SLIDE and click stage 1–8** for 0 / 14 / 28 / 42 / 57 / 71 / 85 / 100%.
With one mouse, right-click a stage on the SLIDE page for the same shortcut.
Hold the encoder for one second, or choose Reset slide in its menu, to restore
25% / Tempo. Type and amount reset together with one Rack undo. A CTRL knob
assigned to Slide Amount retains ownership of the amount; the reset still
changes the type. CTRL assignments and shortcuts otherwise retain their
existing direct-control behavior.

While RUN is stopped, Loopy's Stage Player applies **Analog only** to a marked
stage, as specified on p.44. Its target and rate are sampled on press and frozen
until the next press. Tempo and Acid audition immediately. Release drops the
audition gate/CV and restores the underlying track; audition does not advance
its position or accumulator.

Both models are saved with the Rack patch and internal presets/chains.
Parameter IDs 685–686 append the two types without changing earlier IDs.
Settings schema 16 and preset format 4 add two fields to the existing compact
458-field snapshot. Formats 1–3 still load; their absent models become Tempo,
preserving the earlier plugin's behavior. Custom scale definitions and shortcuts
remain global. As of 2.33, format10 appends Scale Lock and both Loopy settings
after the frozen format9 prefix; see [Preset storage](PRESET-STORAGE.md).

## Calibration and comparison limits

The manual's Analog wording includes both “constant time” and an explicit
example in which close notes finish sooner than distant notes. This version
follows that example using a constant linear voltage slew. Amount zero is
instant. Nonzero amounts map logarithmically from approximately 1 ms to 1 second
per volt (one octave), with 100% = 1 second per octave. Hover help includes the
current time per octave. Tempo continues using its original linear pulse-time
interpolation.

The rate curve, endpoints, default and eight shortcut values are implementation
choices: the manual does not specify their exact calibration. Analog/Acid
curves and Hold/accumulator/probability interactions still need comparison with
a physical module. These modes reproduce the documented musical distinctions,
not a measured hardware slew circuit.

## Try it

Open [Slide-performance.vcv](../examples/Slide-performance.vcv), select your
AUDIO device and press RUN. It starts stopped and uses only Core and VCV Free.
Four related presets chain through Tempo, Analog, Acid, then mixed models on
the two voices. Disable the chain to edit a single pattern; compare small and
large pitch intervals, pulse counts, marked/skipped stages and gate types.

`scripts/test-ui.ps1 -Slides` checks controls, grouped undo, CTRL ownership,
independent tracks, native save/reopen and preset storage. `-SlideAudio` records
twelve seconds of actual stereo playback and observes all three models on both
tracks. The dedicated engine suite checks rates at 44.1/48/96 kHz and
60/120/240 BPM, Acid gates/skips and stopped audition.
