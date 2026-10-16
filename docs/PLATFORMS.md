# Platform packages — 2.70.0

Choose the `.vcvplugin` matching the computer and the Rack application you run:

| Package suffix | Rack installation | Plugin folder inside Rack's user folder |
| --- | --- | --- |
| win-x64 | Windows x64 | plugins-win-x64 |
| lin-x64 | Linux x64 | plugins-lin-x64 |
| mac-x64 | Intel Mac, or Intel Rack under Rosetta | plugins-mac-x64 |
| mac-arm64 | Native Apple Silicon Rack | plugins-mac-arm64 |

Use **Help → Open user folder** in Rack. Save your patch, close Rack, copy the
intact package into that plugin folder and restart Rack. Find **Metropolix Lab**
and **MX** in the browser. A package for the other CPU architecture cannot load.
There is no separate installer or GitHub account requirement.

The new Linux/Mac packages use the official Rack 2.6.6 SDK; the established
Windows build retains its Rack 2.3.0 SDK and runs in Rack Free 2.6.6. All use
Rack 2's plugin ABI. They share the same
789 parameter IDs, settings schema 35, format 12 musical banks, backup formats and
examples. Patches and portable backups can move between these builds; physical
audio/MIDI device choices remain local and may need reselection.

## Verification

Windows has the existing local Rack widget, renderer, persistence, CV/MIDI and
audio-path coverage. Linux x64, Mac x64 and Mac ARM64 jobs run the full native
engine/persistence/file/MIDI/MX tests against their SDK libraries. A separate
host loads the actual stripped production binary and exercises both public
models for six virtual minutes, with 56,664 edits, exact state restores, fresh
instances, three sample rates and MX reconnects. Windows runs this same host
locally against the installed Rack library. See [runtime checks](RUNTIME-VALIDATION.md). Package audits compare
included files with the source, check binary architecture and exclude QA code.
SDK downloads have fixed versions and verified SHA-256 hashes.

Linux also runs the actual Rack 2.6.6 application with a software renderer under
Xvfb. Linux scene captures use Rack's framebuffer renderer because Mesa's
default front buffer can be empty under Xvfb; image checks reject blank scenes,
require visible blue OLED pixels and compare pitch-light Bloom. Its callback tests cover all A/B source confirmations, external-edit conflicts and menu undo, chain insertion/deletion/clear and bank/repeat undo, raw CV/MOD/CLK edit/click order, dependent type/range changes and confirmation cancellation, destination-controlled MOD types and the OLED TYPE page, zero-movement values, effective ALT range, stationary-click external-write/toggle/menu history, long-reset origin/atomic history and pending-edit conflicts/native value actions, user-scale entry/exit/shortcuts, CTRL confirmation/clear and active-drag ownership takeover, Slide/Skip/Ratchet patterns, SCALE copy/paste, PROB/ACCUM paired/ALT gestures, LEN/DIV shortcuts and resets, independent MOD group mute, musical preset storage, ORDER shortcuts, panel refinement, backups, hover/drag/readout
polish, display captures, save/reopen, MX gates/percussion and MIDI-CV voices.
This complements the native host check; it is not an OS mouse/focus playtest.

The Linux build/test environment is Ubuntu 22.04 x64; older distributions are
unverified. Mac jobs use macOS 15 on Intel and Apple Silicon; other Mac versions
and a human Mac GUI/audio playtest are unverified. The SDK applies ad hoc signing
to Mac binaries; these are not independently notarized releases. Rack Pro and
other Rack versions still need tester feedback.

## Reporting a platform problem

Include the package filename, Rack version, OS/CPU, your patch and Rack's
`log.txt`. Say whether the module failed to appear, loading failed, drawing was
wrong or playback failed. The [tester guide](TESTING.md) covers musical checks;
[Feature status](FEATURE-STATUS.md) records hardware fidelity limits.

Build instructions follow the [official Rack build guide](https://vcvrack.com/manual/Building).
The workflow retains native logs, package audits and Linux captures beside its
artifacts. Public release checksums cover the actual downloadable packages.
