param([string]$UserFolder = (Join-Path $env:LOCALAPPDATA 'Rack2'))
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
if (Get-Process Rack -ErrorAction SilentlyContinue) { throw 'Close Rack before installing the package.' }
if (!(Test-Path -LiteralPath (Join-Path $UserFolder 'settings.json'))) {
    throw 'Rack user folder not found. Pass the folder shown by Rack > Help > Open user folder.'
}
$manifest = Get-Content -LiteralPath (Join-Path $projectRoot 'plugin.json') -Raw | ConvertFrom-Json
$package = Join-Path $projectRoot ("dist\MetropolixLab-$($manifest.version)-win-x64.vcvplugin")
if (!(Test-Path -LiteralPath $package)) { throw 'Package missing. Run scripts/build.ps1 -Test -Package first.' }
$destination = Join-Path $UserFolder 'plugins-win-x64'
if (!(Test-Path -LiteralPath $destination)) { throw 'This installer expects the Rack 2.4+ Windows plugin folder.' }
Copy-Item -LiteralPath $package -Destination $destination -Force
Write-Output "Installed package: $destination"
Write-Output 'Restart Rack and search for Metropolix Lab in the module browser.'
