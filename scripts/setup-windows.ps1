$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$toolRoot = Join-Path $env:LOCALAPPDATA 'MetropolixDev'
$sdkRoot = Join-Path $projectRoot '.tools'
New-Item -ItemType Directory -Force -Path $toolRoot, $sdkRoot | Out-Null

$bash = Join-Path $toolRoot 'msys64\usr\bin\bash.exe'
if (!(Test-Path -LiteralPath $bash)) {
    $archive = Join-Path $toolRoot 'msys2-base.exe'
    Invoke-WebRequest -Uri 'https://repo.msys2.org/distrib/x86_64/msys2-base-x86_64-20260927.sfx.exe' -OutFile $archive
    $process = Start-Process -FilePath $archive -ArgumentList @('-y', "-o$toolRoot") -WindowStyle Hidden -Wait -PassThru
    if ($process.ExitCode -ne 0) { throw "MSYS2 extraction failed ($($process.ExitCode))" }
}
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
# The first pass may terminate its own shell when updating the runtime.
& $bash -lc 'pacman --noconfirm -Syuu'
& $bash -lc 'pacman --noconfirm -Syu --needed mingw-w64-x86_64-gcc make jq zstd'
if ($LASTEXITCODE -ne 0) { throw "MSYS2 package setup failed ($LASTEXITCODE)" }

if (!(Test-Path -LiteralPath (Join-Path $sdkRoot 'Rack-SDK\include\rack.hpp'))) {
    $sdkArchive = Join-Path $sdkRoot 'Rack-SDK-2.3.0-win-x64.zip'
    Invoke-WebRequest -Uri 'https://vcvrack.com/downloads/Rack-SDK-2.3.0-win-x64.zip' -OutFile $sdkArchive
    Expand-Archive -LiteralPath $sdkArchive -DestinationPath $sdkRoot
}
Write-Output 'Ready. Run scripts/build.ps1 -Test -Package.'
