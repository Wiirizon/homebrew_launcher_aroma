# Builds only public homebrew sources. No Cafe SDK is needed.
param([string]$DevkitPro = $env:DEVKITPRO)
$ErrorActionPreference = 'Stop'
if (!$DevkitPro) { $DevkitPro = 'C:/devkitPro' }
$env:DEVKITPRO = $DevkitPro.Replace('\', '/')
$env:DEVKITPPC = "$env:DEVKITPRO/devkitPPC"
$env:PATH = "$DevkitPro/devkitPPC/bin;$DevkitPro/tools/bin;$DevkitPro/msys2/usr/bin;$env:PATH"
$make = Join-Path $DevkitPro 'msys2/usr/bin/make.exe'
# Prefer Git for Windows over the devkitPro MSYS git.
$git = Get-Command git.exe -ErrorAction Stop | Select-Object -ExpandProperty Source
$windowsGit = Join-Path $env:ProgramFiles 'Git/cmd/git.exe'
if (Test-Path -LiteralPath $windowsGit) { $git = $windowsGit }
$dependency = Join-Path $PSScriptRoot '.deps/librpxloader'
$revision = '7b055f15f2c0e31af5b0a4ceda44ecb1ec39cc34'
if (!(Test-Path -LiteralPath $dependency)) {
    New-Item -ItemType Directory -Force -Path (Split-Path $dependency) | Out-Null
    & $git clone https://github.com/wiiu-env/librpxloader.git $dependency
    if ($LASTEXITCODE -ne 0) { throw 'Could not download librpxloader' }
    & $git -C $dependency checkout --detach $revision
    if ($LASTEXITCODE -ne 0) { throw 'Could not check out librpxloader' }
}
$actual = & $git -C $dependency rev-parse HEAD
if ($LASTEXITCODE -ne 0 -or $actual -ne $revision) {
    throw "Expected librpxloader revision $revision; inspect $dependency before rebuilding."
}
& $make -C $dependency -j2
if ($LASTEXITCODE -ne 0) { throw 'librpxloader build failed' }
& $make -C $PSScriptRoot "RPXLOADER=$($dependency.Replace('\', '/'))" -j2
if ($LASTEXITCODE -ne 0) { throw 'Aroma build failed' }
Write-Output "Built $(Join-Path $PSScriptRoot 'homebrew_launcher_aroma.wuhb')"
