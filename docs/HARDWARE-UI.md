# Hardware panel and desktop interaction — 2.66.0

**2.71.0:** [Desktop performance](DESKTOP-PERFORMANCE.md) adds an optional
mouse/keyboard view alongside the original panel. Shift+P opens it while
hovering over this module; click to arm keyboard playing.

MOD ALT + TYPE opens the OLED CV TYPE page. Internal destinations fix its type; see [MOD types](MOD-TEST.md).

Tiny CV drags preserve [raw Undo and normal click order](STAGE-CV.md); ALT editing uses the effective CV range when MIDI velocity forces Unipolar.

User-scale note editing requires [SCALE to be selected](SCALES.md) before holding the encoder; the desktop note-editor menu still opens directly from ROOT.

CTRL routing now uses [mandatory confirmation and long-encoder clear](CTRL-KNOBS.md), while performance knobs stay live.

PROB and ACCUM support [paired resets and whole-pattern ALT gestures](STAGE-PAIRS.md), including confirmation and exact undo.

LEN and DIV now support [held stage shortcuts and whole-screen encoder resets](TIMING-SHORTCUTS.md), preserving the active field and Rack undo.

Open `examples/Hardware-panel-demo.vcv`. Its musical settings and signal connections come from the accepted MOD demo, with a new 34 HP front panel. Select an output device in the AUDIO module to hear its two voices.

The panel follows the supplied photograph at 34 HP. This revision refines lettering, component proportions, slider grips, button colors, metal sockets, secondary legends, mounting details, and the logo. The display uses a native 128 × 64 pixel canvas with lettering and diagrams based on the manual's screen images. Further comparison is still needed before calling it an exact recreation.

The bottom credit reads **sensiburner <3 intellijel** at the user's request.
sensiburner uses matching bold lowercase lettering and a red i dot; the red
<3 separates the creator's name from the original hardware maker. Rack's brand
and author metadata also identify sensiburner. This is an independent homage.

## Using the controls

- **Drag upward to increase; downward to decrease.** This applies to pitch faders, pulse-count/gate switches, the display's active value, the encoder, numeric buttons such as BPM/LEN/DIV, and values in the optional editors. Integer and toggle settings snap to valid values. Ctrl-drag provides fine adjustment. Release ends one undoable edit.
- **Hover for help.** Controls identify their value and purpose. Ports retain Rack cable behavior and tooltips. Controls for unfinished features explain their status.
- **Read values while dragging.** A compact popup follows the edited parameter on the panel, display, encoder and optional grids. It stays visible if the pointer leaves the control while held. Rack's Tooltips preference controls both hover help and adjustment readouts.
- **Gentler sliders.** At Rack's default linear sensitivity, pitch takes approximately 400 mouse pixels across its range; pulse-count and gate switches take about 24 pixels per step. Rack's global sensitivity and fine-adjustment modifiers still apply. Pitch-slider lights follow **View → Bloom** and remain illuminated when room brightness is reduced.
- **Right-click for direct access.** Numeric controls provide Rack value entry and reset; named settings such as ORDER offer their choices. TRK 1/2 expose track settings. MOD offers lane selection and its expanded editor. EDIT opens the stage editor; EXIT offers either editor, a close action and Sync to Panel.
- **HOME:** turn the encoder with a vertical drag, or click the encoder/display, to browse four views: stage positions, base notes, panel values, and CV/MOD activity. EXIT steps back through Setup submenus and otherwise returns HOME. Hold EXIT for Sync to Panel ([guide](UI-FIDELITY.md)). The note view excludes accumulation and modulation from its stage notes; the pre/post readouts show the corresponding live routes.
- **Click a settings button** to put its setting on the display. Drag the encoder or display to change its active value. Click the encoder/display to switch root/scale, stages/pulses, or clock/pulse-count division. On LEN, ALT selects offset; on SCALE, ALT switches between scale and root.
- **Encoder presses** toggle Forward/Reverse on ORDER, Stage/Pulse on probability, Stage/Pulse/Ratchet on accumulation, and Step/Ramp on a MOD stage. Turning still edits the corresponding main value. These changes support Rack undo.
- **BPM:** the first click opens the tempo page. Subsequent BPM clicks tap an internal tempo. External clock shows measured tempo after two edges; the existing input convention remains one edge per sixteenth note.
- **Select TRK 1 or TRK 2** before editing note-track settings. Select MOD for modulation; MOD → EDIT → a stage button chooses that MOD lane. Right-click MOD also provides direct lane selection.
- **Stage editing:** press EDIT, choose the printed function with a stage button, then select a stage. Drag the encoder or hold and drag a stage button to edit its value. Slide/Skip stage clicks toggle that stage. Right-click a stage button for direct editing and related choices.
- **ALT** enables the secondary stage-button actions. LOAD/SAVE open the internal 8 × 8 preset banks ([guide](PRESETS.md)); A/B select output assignment on the display. EXIT returns to the overview.
- **Optional grids** remain available through menus. They share parameters with the panel and preserve undo/redo. The hardware panel is 34 HP; opening a grid expands the module to 66 HP. Rack moves it to available space if needed. An adjacent MX follows the new right edge while the main panel stays anchored.

## Stage overrides and Gate Scale

2.10.0 activates EDIT → PITCH/GATE for each track, with Sync, ALT editing,
long-press clear, colored stage lights, menus and one-step undo. Track GATE
now has Length/Scale fields and Hold. See the [override guide](STAGE-OVERRIDES.md).

## Swing

2.9.0 also activates SWING independently for both note tracks, with percentage
display, shortcuts, encoder long-press reset, and desktop editing. CTRL sets
its base amount; AUX and MOD offset it. See [Swing operation](SWING.md).

## Scale display

The [scale screen/editor](SCALES.md) uses the native keyboard, frames, captions
and symbols. Root and note previews keep their confirmation feedback. Confirmed
scale browsing now follows the same bank order as immediate browsing.

## Chain display

The [chain table](PRESET-CHAINS.md) follows the native row spacing, filled
selection, mute symbols and captions. Quick Chain shows a repeat-plus cue.
Existing drag, menu, playback and Undo/Redo workflows are preserved.

## Performance readouts

Performance fields use [reference-based bold and large values](UI-FIDELITY.md),
with measured encoder cues, stage units and pending tempo feedback. Existing
confirmation preferences and desktop input gestures are preserved.

## Menu lettering

Lists and LIM use the [proportional menu font](UI-FIDELITY.md), based on native
OLED glyphs. Four-row selection includes the complete descenders; labels may
shorten for long pending values while hover help keeps their full names.

## LIM display — 2.59.0

Press **ALT → LIM** (stage button 7) for **LIMITS + SETUP**. Click the encoder
or display to cycle Positive Limit, Negative Limit, Mode, Order, Polarity and
Reset; drag vertically to edit the selected value. The first three and last
three settings retain their existing groups. Three mixed-case rows, a full
selection bar and the track/ACCUMULATOR footer follow the manual's p126 image.

Positive/Negative Limit show a signed magnitude from 1–28 scale degrees.
With Enc. Confirm On, a drag previews the value with `?`; click to apply with
one Undo. Confirmation retains the field; a later click selects the next one.
EXIT cancels an unconfirmed preview. Native menus and numeric entry stay
immediate, and ordinary edits retain their existing Undo/Redo behavior.

CTRL-owned Mode and Polarity show the owning knob number and dim both their
name and value. Dragging or double-clicking cannot overwrite the knob's
setting. Clicking still selects the next field; right-click opens the owning
controller's assignment menu. These behaviors already worked before this
display pass and are retained. No new LIM long-press reset gesture is added.

The reference image's +7/-7 values are an example, not a default specification.
The plugin retains its +9/-9 defaults. DSP, all 787 parameter IDs, settings
schema34 and format11/522 musical snapshots remain unchanged. Exact glyph and
physical-module equivalence remain unverified.

## Accumulator modes

Select **TRK 1** or **TRK 2**, click **ALT**, then **LIM** (stage button 7).
Click the encoder twice to select **Mode**, then drag upward for **Track** or
downward for **Stage**. The display itself is also draggable; right-click the
active value for named choices. Alternatively, right-click a track button and
choose **Accumulator mode**. Each track has its own mode and Rack saves it.

Encoder clicks cycle Positive Limit, Negative Limit, Mode, Order, Polarity,
and Reset. The display scrolls between two groups of three settings. These
edits support undo. EXIT returns to HOME. Accumulator *mode* is different from
the per-stage Stage/Pulse/Ratchet *trigger* edited through EDIT → ACCUM.

`examples/Accumulator-modes.vcv` compares the manual's four-note C-major
example: Track 1 uses Stage mode and Track 2 uses Track mode. Only stage 1 has
an increment (+1). With Stage mode, only that note changes each pass; Track
mode shifts the entire phrase. Limits are +3/-3 for a short repeating test.
The two Scopes show each track's pitch/gate. Select an AUDIO device to hear it.

Try skipping stage 1 before RESET: neither track should accumulate. Unskip it
and reset, let the phrase rise, then skip it during playback: Track mode holds
the transposition already reached. Try moving the +1 to a different stage,
changing pulse counts, or choosing a generative order. These are comparison
exercises, not claims of hardware-verified behavior. Please include the patch,
mode, order, skip positions, and a hardware recording when reporting a mismatch.

Mode edits take effect on the next track-clock pulse. The two modes retain
separate runtime counters until reset; returning to a mode resumes its stored
state. Loading a patch restarts both. Older patches default to Stage mode.
The manual does not fully specify live switching or multiple accumulator
sources; see [the documented interpretations](VALIDATION.md).

## Compatibility and remaining controls

The established 770 parameter IDs and all ports retain their identities;
sixteen ORDER shortcut values and independent MOD group mute are appended, for 787 parameters. Settings schema34
includes global ORDER shortcuts and group mute. Musical format11/522 adds group
mute after format10’s 521-value prefix (including Scale Lock and Loopy settings). Existing patch
and bank migrations remain in place. Temporary ALT state clears on load;
the tuner does not activate on reopen. See [Feature status](FEATURE-STATUS.md)
and the current guides for compatibility details.

AUX X/Y/Z now accept cables. Their knobs attenuvert; their black buttons open destination/track settings, with a live CV or gate meter on the display. Right-click either for named routing choices; encoder clicks switch fields. See the [AUX guide](AUX-INPUTS.md). Both CTRL knobs now work; see the [CTRL guide](CTRL-KNOBS.md). ALT+AUX now randomizes, morphs and restores the current screen; see [Editing workflows](EDITING-WORKFLOWS.md). All seven [MX modes](MX.md) are implemented; ALT → MX selects mode and assignment.

All four HOME views and editing pages for current parameters are represented. The display shows filled/current stages, thick starting stages, skipped stages, inactive dots, muted hatching, the scale keyboard, ratchet stacks, probability/accumulator bars, and CV/MOD graphs. Selection corners pulse and the selected numeric stage button flashes. MOD footers show A/B assignment where present.

Slide offers Tempo, Analog and Acid; see the [slide guide](SLIDES.md). Note-track ALT → TYPE and EDIT → CV now offer the three CV types, transitions and ALT pattern edits; see the [stage CV guide](STAGE-CV.md). ALT → CLK edits global output division/offset; click switches fields and a long encoder press resets both. See [Clock output](CLOCK-OUTPUT.md). External clock input division is adjustable from /1–/256 ([guide](UI-SETUP.md)). CTRL dials in the panel-value HOME view show the live knob positions. MOD values show destination-specific offsets; voltage-output lanes stay in volts ([guide](UI-FIDELITY.md)). A/B offers all sixteen hardware function types with track/MOD choices and pending confirmation; see [Output routing](OUTPUT-ROUTING.md). SETUP now opens a selectable menu with Preset Settings and [Preset Chains](PRESET-CHAINS.md); holding SETUP opens chains, with encoder/stage holds for deletion/saving. [MIDI](MIDI.md), [backup files](BACKUPS.md), [UI setup](UI-SETUP.md), [global settings](CLOCK-RESET.md) and [editing workflows](EDITING-WORKFLOWS.md) have dedicated guides. Remaining fidelity work is tracked in [Feature status](FEATURE-STATUS.md). Numeric editing and context menus are intentional computer conveniences.

## Verification

The Rack widget test exercises held upward/downward drags, discrete values, numeric entry, menus, hover text, undo/redo, track/lane rebinding, optional editor sizing, and save/load. The separate display test exercises HOME/LIM navigation and renders 181 screen states for inspection. The hardware and accumulator demos also run through the stereo audio recorder. These callback tests complement a physical mouse playtest; they do not certify every operating-system input/focus interaction.

## Loopy performance

The ∞ control now provides temporary loops, stopped audition and performance mutes.
Mouse range menus and a latched-loop display complement the original stage buttons.
See [Loopy operation, saving and hardware-comparison limits](LOOPY.md).

## Scales — 2.12.0

All 49 factory scales/chords and 100 custom definitions are available from SCALE.
Encoder browsing uses the hardware bank order while legacy numerical IDs stay fixed.
Long encoder presses enter/exit note editing, ALT selects Root, clicks lock/apply
or toggle notes. Hold SCALE + stage to recall a shortcut; long stage press saves.
Right-click menus provide the same operations. See [Scales](SCALES.md).

Portable validation adds 1,609,670 checks. Rack host, custom editing, lock/search,
held shortcuts, grouped undo, native saving/reopening, 49 display states and
actual stereo chord playback pass. Audio measured RMS .360/.363 V, peaks
1.730/1.733 V, DC approximately .00084 V at 48 kHz over twelve seconds.
Fractional-voltage truncation, default C-major custom slots and root-only empty
mask fallback are implementation choices; hardware comparison remains necessary.
Internal presets recall Scale/Root settings, as the manual describes on p.112;
the earlier claim that their recall differs from hardware was incorrect. Custom
definitions remain global. As of 2.33, Scale Lock follows the preset storage
column on p.169; see [Preset storage](PRESET-STORAGE.md).

## Track menu — 2.17.0

ALT → TRK opens browsing/editing with toggles, long reset and Init Trk Defaults.
Independent pulse/gate sources and Rest Pitch complement slider range/direction
and velocity; transpose now spans −60…+60. See [Track options](TRACK-OPTIONS.md).

SCALE copy/paste now follows the panel ALT gestures and has keyboard Alt and
right-click equivalents; see [Scale copying](SCALES.md#copy-and-paste-a-scale).

EDIT Slide/Skip now supports ALT rotate/invert/reset; Ratchet supports ALT
bulk count edits and selected/all long resets. See [Stage patterns](STAGE-PATTERNS.md).

## Controller markers — 2.64.0

Circled CTRL numbers now use the native-reference outline and seven-row digits.
LIM also uses measured caption lettering and footer alignment. Editing, ownership
and saved formats retain their existing behavior; see [visual evidence and its
limits](UI-FIDELITY.md#ctrl-ownership-symbols-and-lim-captions--2640).

## Menu spacing — 2.65.0

Settings menus use native row spacing, headings and margins. The bottom row
keeps lowercase tails visible, and desktop file/status screens retain compact
rows. Gestures and saving are unchanged; see [menu layout](UI-SETUP.md).

## Performance field refinement — 2.66.0

Performance headings, frames, action labels and CTRL/AUX/MOD gauges now follow
the native references. Confirmation and desktop gestures remain available; see
[display coverage and interpretations](UI-FIDELITY.md).
