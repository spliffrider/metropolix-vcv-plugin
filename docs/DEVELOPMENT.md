# Development and version control

Repository: https://github.com/spliffrider/metropolix-vcv-plugin

The working branch is `main`. New work should normally start on a short-lived
branch, with a commit describing the finished change and relevant validation.
Commit source, artwork, tests, documentation, and reusable demo patches together.

```powershell
git status
git switch -c codex/next-refinement
# Make changes and run the relevant checks from README.
git add src tests docs
git diff --cached
git commit -m "Describe the finished change"
git push -u origin codex/next-refinement
```

Use a pull request to review the diff before merging into `main`. A branch is
enough for everyday work; a separate worktree is optional. Avoid changing
parameter and port IDs: existing Rack patches depend on them.

## Imported milestones

Git history begins with source snapshots recovered from the local release
archives. These are reconstruction commits made when Git was introduced, not
the original development commits. The tags identify the corresponding saved
versions:

| Tag | Milestone |
| --- | --- |
| `v2.0.1` | Two-track instrument and timing/accumulator baseline |
| `v2.1.0` | Eight MOD lanes and assignable outputs; user-tested musical baseline |
| `v2.2.0` | First hardware-layout panel and desktop controls |
| `v2.3.0` | Refined panel, native-pixel display, and navigation |

The imported source/assets match their archives. Machine-specific continuation
notes (`docs/RESUME.md`) are excluded from every imported snapshot. Import
commits record each archive's SHA-256. The following repository-setup commit
adds ignore rules, consistent text handling, this guide, and automated tests.

To compare a milestone without replacing the current working files:

```powershell
git diff v2.2.0 v2.3.0 -- src
git show v2.1.0:src/Sequencer.hpp
```

## Local files and validation

Downloaded SDKs, build output, installed Rack profiles, logs, cached manuals,
session backups, and release checkpoint archives stay local and are ignored.
The repository retains the panel reference image, UI assets, source attribution,
demo patches, and generated example audio. GitHub stores the source history;
keep local backups of personal patches separately.

GitHub Actions runs the fourteen portable C++ engine suites on pushes and pull
requests. It does not run Rack, validate the Windows DLL, or exercise the UI.
Run the Windows build, Rack integration, and applicable UI/audio checks from
README for changes to plugin behavior or rendering. Test DLLs must never be
installed or published as production plugins.

For slider/readout/light/OLED changes, run `scripts/test-ui.ps1 -Polish` and
`python scripts/inspect-ui-polish.py` (Pillow required), then inspect
`tmp/polish-qa/comparison.png`. The test profile is isolated from the user's
Rack profile; previous test logs are archived alongside its reports.

The existing project metadata and third-party asset attribution are preserved.
Making the repository public does not change its declared software license.

## Public test releases

`v2.3.1` is the first downloadable Windows alpha, with the same runtime code as
2.3.0. GitHub Releases hosts the `.vcvplugin`, portable demo, tester guide, and
SHA-256 checksums. It is marked as a prerelease. Release assets are separate from
GitHub's automatic source archives.

`Makefile` lists distributable documents explicitly so local continuation notes
cannot enter a public package. Verify staged contents, runtime assets, absence
of test hooks, and the published download's checksum before sharing a build.
