# Clock, reset and tuning — 2.20.0

SETUP → Global Setup opens the hardware configuration fields. Turn the encoder
to browse, press to edit a field, then hold and drag up/down to change it.
Right-click SETUP, the display or encoder for direct choices with Rack undo.

**Clock Src** offers Internal and External from manual p.49, plus VCV Auto.
Auto selects the patched CLK input, otherwise the internal tempo, preserving
older patches. Internal ignores a patched CLK input. External waits for clock
edges even if the jack is unpatched. One edge is one sixteenth-note pulse;
MIDI clock and alternate input resolutions remain separate work.

**Clk/Rset Out** offers Always and Running. Always keeps the divided/offset
CLK output going while notes and MOD lanes are stopped; no preset-chain or
musical state advances. Running suppresses global clock/reset outputs while
stopped. VCV legacy preserves previous builds: CLK runs with transport, while
global Reset triggers remain available when stopped. Existing patches and new
modules default to this legacy behavior. Track First/Last/New Stage events
are separate from this global output policy.

**Reset Input** changes the RESET jack's function:

| Mode | High gate | Low gate |
| --- | --- | --- |
| Reset | Rising trigger resets enabled tracks | Normal RUN-button operation |
| Run | Resets enabled tracks and starts | Stops playback |
| Run-C | Continues from the current sequence position | Pauses playback |

Run and Run-C override the RUN button while a cable is connected. The green
RUN light follows actual transport. Unplugging restores the saved RUN-button
choice. Both use Rack's Schmitt trigger with a 1.5 V rising threshold and
0.1 V rearm threshold. The panel RESET button remains available in every mode.
The Run Btn setting selects restarting or continuing for button transport.

ALT → TRK → **Global Reset** determines whether the panel RESET button and
RESET-jack reset triggers reset that track. Off retains the track's traversal,
divider, accumulated pitches and Loopy state. MOD lanes and global clock/phrase
timing still reset. Hold a TRK button while pressing RESET to force that track
to reset even when Global Reset is Off. Targeted AUX/MOD Reset remains available
independently. New modules and older presets default to On.

Global Reset is saved with musical presets. Preset format 8 appends two values
after the frozen 490-value format 7 snapshot. Formats 1–7 load with Global Reset
On. Global clock, reset-input and tuner settings remain Rack-persistent global
settings; recalling a musical preset does not overwrite them. The original
719 parameter IDs are unchanged; eight configuration parameters append to them.

**Bottom Pitch** chooses C–B or Root for the bottom of pitch sliders. A fixed
note absent from the scale uses the first in-scale note above it, as described
on p.49. The top still spans the selected number of octaves. Root preserves
previous behavior. Root performance offsets still transpose the resulting
pitch; the fixed anchor uses the programmed scale root. Pitch overrides remain
independent of this slider setting.

**Tuner** opens from Global Setup. Both pitch outputs emit the selected C octave,
−5 V through +5 V, with C0 = 0 V. Held vertical dragging changes the octave;
encoder presses toggle High and Clock gates. High sustains both gates at 5 V.
Clock uses the undivided sequence pulse while running and remains low while
stopped, even when Clk/Rset Out is Always. EXIT or another screen restores normal
sequence outputs. Tuner mode itself does not reopen with a saved patch; octave
and gate preference persist. Musical transport continues underneath the tuner.

[Clock-reset-performance.vcv](../examples/Clock-reset-performance.vcv) provides
a sequenced left voice and a right percussion voice driven by divided CLK.
It opens stopped with Always selected. After choosing AUDIO, the percussion
demonstrates clock-only playback; RUN adds the melodic voice. Switch Running
to stop the clock with transport. TRK 2 ignores global reset; compare its
continuing stage position with TRK 1, then force reset with the held TRK gesture.

The engine tests cover three sample rates, all clock-output divisions, all
21 traversal orders and fixed-bottom endpoints for all 49 factory scales,
12 roots and four octave ranges. Native Rack checks exercise gate transport,
protected/forced reset, real tuner ports and legacy preset migration. Widget
tests check editing, undo, held gestures and native save/reopen. This is a
manual-based implementation; timing collisions still need hardware-owner feedback.
