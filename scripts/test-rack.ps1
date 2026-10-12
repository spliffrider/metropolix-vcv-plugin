param([string]$RackDirectory = 'C:\Program Files\VCV\Rack2Free')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
Push-Location -LiteralPath $projectRoot
$previousPath = $env:PATH
try {
    & "$env:LOCALAPPDATA\MetropolixDev\msys64\usr\bin\bash.exe" -lc 'make rack-test-build'
    if ($LASTEXITCODE -ne 0) { throw 'Integration test build failed.' }
    $env:PATH = "$RackDirectory;$previousPath"
    & (Join-Path $projectRoot 'build\tests\rack-integration.exe')
    if ($LASTEXITCODE -ne 0) { throw "Rack integration tests failed ($LASTEXITCODE)" }
} finally {
    $env:PATH = $previousPath
    Pop-Location
}
