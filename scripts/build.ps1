param([switch]$Test, [switch]$Package)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$bash = Join-Path $env:LOCALAPPDATA 'MetropolixDev\msys64\usr\bin\bash.exe'
if (!(Test-Path -LiteralPath $bash)) { throw 'Build tools missing. Run scripts/setup-windows.ps1 first.' }
if (!(Test-Path -LiteralPath (Join-Path $projectRoot '.tools\Rack-SDK\include\rack.hpp'))) {
    throw 'Rack SDK missing. Run scripts/setup-windows.ps1 first.'
}
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
Push-Location -LiteralPath $projectRoot
try {
    if ($Test) {
        & $bash -lc 'make test'
        if ($LASTEXITCODE -ne 0) { throw "Tests failed ($LASTEXITCODE)" }
    }
    & $bash -lc 'make -j4'
    if ($LASTEXITCODE -ne 0) { throw "Build failed ($LASTEXITCODE)" }
    if ($Package) {
        # SDK packaging replaces dist recursively. Verify that target before
        # invoking it, and reject junctions/symlinks that could escape the repo.
        $distPath = [IO.Path]::GetFullPath((Join-Path $projectRoot 'dist'))
        if ($distPath -ne ([IO.Path]::GetFullPath($projectRoot).TrimEnd('\') + '\dist')) {
            throw 'Unexpected package staging path.'
        }
        if ((Test-Path -LiteralPath $distPath) -and
            ((Get-Item -LiteralPath $distPath).Attributes -band [IO.FileAttributes]::ReparsePoint)) {
            throw 'Refusing to package through a dist junction or symlink.'
        }
        & $bash -lc 'make dist'
        if ($LASTEXITCODE -ne 0) { throw "Packaging failed ($LASTEXITCODE)" }
    }
} finally { Pop-Location }
