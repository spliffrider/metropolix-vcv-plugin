param([string]$RackDirectory = 'C:\Program Files\VCV\Rack2Free')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$binary = Join-Path $projectRoot 'dist\MetropolixLab\plugin.dll'
if (!(Test-Path -LiteralPath $binary)) { throw 'Package the production plugin with scripts/build.ps1 -Package first.' }
$previousPath = $env:PATH
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
Push-Location -LiteralPath $projectRoot
try {
    & "$env:LOCALAPPDATA\MetropolixDev\msys64\usr\bin\bash.exe" -lc 'make production-test-build'
    if ($LASTEXITCODE -ne 0) { throw 'Production test host build failed.' }
    $env:PATH = "$RackDirectory;$previousPath"
    & (Join-Path $projectRoot 'build\tests\plugin-load.exe') $binary (Join-Path $projectRoot 'examples\Backup-example.json')
    if ($LASTEXITCODE -ne 0) { throw "Production runtime soak failed ($LASTEXITCODE)" }
} finally {
    $env:PATH = $previousPath
    Pop-Location
}
