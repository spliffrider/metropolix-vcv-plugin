# Preset chains — 2.62.0

Arrange the 64 internal presets into **up to 32 links**, each with **1–64
repeats** and independent **TRK 1 / TRK 2 / MOD** mute flags. The last link loops
back to the first. Eight chain storage slots, the working chain and its enabled
state are saved inside the Rack patch, alongside the preset banks.

## Chain display

The table uses [reference-based lettering and geometry](UI-FIDELITY.md), with
inverted row/field selection and solid/hollow mute symbols. Empty-chain actions
are hatched. Magenta appears as Mag when needed to keep the repeat column clear.
Quick Chain adds a plus beside the last repeat count; another matching stage
press can increase it. The track footer, playing repeat and follow status remain
visible. These display changes preserve the existing gestures and saved chains.

## Panel workflow

Click **SETUP**, drag the encoder upward to **Preset Chains…**, and click the
encoder. Holding SETUP for more than one second opens the same screen from
another page. The OLED lists link number, bank/slot, repeats and three mute
circles; solid means unmuted, hollow means muted.

1. Highlight **New Chain** or an insertion line and click the encoder.
2. With the preset field selected, drag the encoder to choose a bank colour,
   then press a stage button to choose its preset slot.
3. Click the encoder to edit repeats. Hold and drag upward to increase,
   downward to decrease. TRK 1, TRK 2 and MOD toggle this link's mute flags.
4. Click again to continue adding links. Press EXIT to finish editing; a second
   EXIT returns home. For an existing link, click it to edit the same fields.
5. Highlight **Enable Chain** and click. Press RUN to play. Clicking the encoder
   on the link list toggles **Chain Follow**, which follows the playing link.
   To disable, turn to **Disable Chain** or use the right-click menu.

Insertion lines alternate with links as you turn the encoder. Holding the
encoder for more than one second deletes the selected link while disabled.
Right-click the OLED or encoder for preset choices, repeats, mutes, insertion,
deletion, moving a link up/down, enable/disable and chain storage. Held repeat
drags show a live readout and create one undo action for the whole gesture.

With chain playback disabled and link editing finished, stage buttons represent
the eight chain storage slots. Bright white indicates a saved chain; dim white
is empty; the most recently loaded/saved slot flashes. Click a filled button to
load. Hold a button for more than two seconds to save. Overwriting a saved chain
opens a confirmation menu. **Clear Chain** only clears the working copy. Rack
undo/redo supports edits, storage changes, loading, clearing and enable/disable.

## Undo in Rack

Inserting a link, Clear Chain and a selected-link long deletion each create
one Rack undo action. Undo/Redo restores the entire working-chain state,
including its loaded-slot indicator, while preserving all eight saved chains.
Clicks and long holds record their own actions; they no longer also receive
the history entry intended for a bank/repeat drag.

Encoder and OLED bank/repeat drags still accumulate into one action. Cursor
browsing, a drag that returns to its original repeat count, clipped no-op
adjustments and a failed insertion into a full 32-link chain add no undo.
Playback must be disabled for insertion, clearing or deletion; a long hold
on an insertion line retains its ordinary short-click insertion action. Saved
formats stay unchanged.

## Quick Chain

On the chain screen, hold SETUP for more than one second and keep holding it.
This starts a new working chain. Select mute flags with TRK 1/TRK 2/MOD, choose
a bank with the encoder, then press stage buttons to add presets. Press the same
preset again with the same mute flags to increase its repeats. A different
preset or mute state creates a new link. Release SETUP to enable the chain.
Press EXIT before releasing to prevent automatic enabling. The desktop menu
provides **New Quick Chain** and **Finish Quick Chain** so you can enter the
same sequence with one mouse, without holding SETUP while clicking other buttons.

## Timing and transport

Each repeat lasts the playing preset's **Total Pulses**. For example, Total = 32
and Repeats = 4 plays 128 base clock pulses before the next link. Total resets
both tracks, MOD lanes and accumulators on every repeat, regardless of individual
track lengths, divisions, skipped stages or ratchets. The next link loads and
plays its first note/MOD values on the due base clock edge, preserving clock
phase. It follows Pitch/Switch/CTRL/AUX recall policies in Preset Settings; use
**Recall complete pattern** to recall slider and switch positions.

When Total is Off, Queue Pulses determines each repeat's duration. Individual
tracks continue their natural cycles until the next link loads. When both are
Off, the current link holds indefinitely. RESET, including the reset input,
returns to Link 1. Selecting internal/external clock also restarts the chain.

The chain menu's **Run button** option selects **Run/Stop** (default: stop and
restart at Link 1) or **Run/Pause** (silence gates while retaining position,
then resume on the next clock). This option applies to enabled chains. Ordinary
sequencing keeps the previous Run/Stop behavior.

Chain mutes add to the preset's own mute settings. Note-track link mutes silence
gates and stage CV while pitch holds; they cannot be undone by AUX/MOD mute
inversion. A MOD link mute silences all MOD outputs and internal MOD routes,
while their timing continues. Disabling the chain removes these transient link
mutes without changing stored mute parameters. An empty referenced preset keeps
the current pattern, applying that link's repeat/mute settings (manual p.178).

Preset saving/loading and chain editing/storage are disabled while chain mode
is enabled. Disable it before editing. Undoing an earlier preset-bank edit or
manual recall also disables the chain so the undo can take effect. Automatic
link changes do not add undo entries or overwrite preset banks.

Reopening an enabled patch restarts at Link 1; the in-flight clock position is
not persisted. The saved chain slots are local to this module in this Rack
patch, rather than shared globally across instances as in hardware flash.

## Demo and comparison

Open **[Chain-performance.vcv](../examples/Chain-performance.vcv)** and select
your AUDIO device. This original 108 BPM arrangement uses only Metropolix Lab,
VCV Free and Core. Seven links form 32 bars (about 71 seconds): eight bars of
baseline, then four bars each of higher melody, baseline, bass breakdown,
held/slide/ratchet answer, a MOD-muted variation, and the return. The OLED follows
the arrangement. Chain slot 1 holds its saved copy; red preset slots 1–4 contain
the patterns. Press RESET to restart, or disable the chain to experiment.

Based on the firmware 1.6 manual, printed pp.51–52, 57, 171–178, 197–198.
Physical hardware comparison remains outstanding. Combined preset/link mutes,
Queue-based repeat continuity and live clock-source changes are documented
software interpretations where the manual does not fully specify interactions.
MIDI program changes and portable JSON backups are available. Hardware SysEx
interchange and flash compatibility remain outside the verified implementation.
