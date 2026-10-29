# Prepare native distribution files; never upload or stage arbitrary directories.
param([string]$Version = 'v0.1.0-alpha.2')
$ErrorActionPreference = 'Stop'
if ($Version -notmatch '^v[0-9]+\.[0-9]+\.[0-9]+(?:-[a-zA-Z0-9.]+)?$') { throw 'Invalid version' }
$projectRoot = Split-Path $PSScriptRoot
$git = (Get-Command git.exe -ErrorAction Stop).Source
$windowsGit = Join-Path $env:ProgramFiles 'Git/cmd/git.exe'
if (Test-Path -LiteralPath $windowsGit) { $git = $windowsGit }
Push-Location $projectRoot
try {
    $dirty = & $git status --porcelain --untracked-files=normal
    if ($LASTEXITCODE -ne 0 -or $dirty) { throw 'Package only a clean committed checkout' }
    node aroma/tests/verify-build.mjs
    if ($LASTEXITCODE -ne 0) { throw 'Executable verification failed' }
    $revision = & $git rev-parse HEAD
    $dependency = Join-Path $PSScriptRoot '.deps/librpxloader'
    $dependencyRevision = & $git -C $dependency rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $dependencyRevision -ne '7b055f15f2c0e31af5b0a4ceda44ecb1ec39cc34') { throw 'Unexpected loader source' }
    $dependencyDirty = & $git -C $dependency status --porcelain --untracked-files=normal
    if ($LASTEXITCODE -ne 0 -or $dependencyDirty) { throw 'Loader source has local modifications' }
    $destination = Join-Path $PSScriptRoot "build/distribution-$Version"
    if (Test-Path -LiteralPath $destination) { throw 'Distribution directory already exists; inspect it before repackaging' }
    $binary = Join-Path $destination 'binary'
    $source = Join-Path $destination 'source'
    New-Item -ItemType Directory -Path (Join-Path $binary 'wiiu/apps'),$source -Force | Out-Null
    Copy-Item -LiteralPath aroma/homebrew_launcher_aroma.wuhb -Destination (Join-Path $binary 'wiiu/apps/homebrew_launcher_aroma.wuhb')
    Copy-Item -LiteralPath aroma/README.md,LICENSE,THIRD_PARTY_NOTICES.md,UNOFFICIAL.md,RELEASE_AUDIT.md -Destination $binary
    Copy-Item -LiteralPath licenses -Destination $binary -Recurse
    $sourceNote = "Simple Aroma Launcher $Version`nNot a direct HBL port; no ELF support.`nSource revision: $revision`nRebuild/relink instructions: THIRD_PARTY_NOTICES.md`nCorresponding source is in the sibling simple-aroma-launcher-$Version-source.zip release asset.`n"
    Set-Content -LiteralPath (Join-Path $binary 'SOURCE.txt') -Value $sourceNote -Encoding utf8
    $projectArchive = Join-Path $destination 'project-source.zip'
    & $git archive --format=zip "--output=$projectArchive" HEAD LICENSE THIRD_PARTY_NOTICES.md UNOFFICIAL.md RELEASE_AUDIT.md licenses aroma/Makefile aroma/Dockerfile aroma/README.md aroma/build.ps1 aroma/package.ps1 aroma/source aroma/scripts aroma/tests
    if ($LASTEXITCODE -ne 0) { throw 'Project source archive failed' }
    Expand-Archive -LiteralPath $projectArchive -DestinationPath $source
    $dependencyArchive = Join-Path $destination 'loader-source.zip'
    & $git -C $dependency archive --format=zip "--output=$dependencyArchive" HEAD LICENSE README.md Makefile include source
    if ($LASTEXITCODE -ne 0) { throw 'Loader source archive failed' }
    Expand-Archive -LiteralPath $dependencyArchive -DestinationPath (Join-Path $source 'third_party/librpxloader')
    $binaryZip = Join-Path $destination "simple-aroma-launcher-$Version.zip"
    $sourceZip = Join-Path $destination "simple-aroma-launcher-$Version-source.zip"
    Compress-Archive -Path "$binary/*" -DestinationPath $binaryZip
    Compress-Archive -Path "$source/*" -DestinationPath $sourceZip
    $checksums = foreach ($asset in @($binaryZip,$sourceZip)) {
        "{0}  {1}" -f (Get-FileHash -LiteralPath $asset -Algorithm SHA256).Hash.ToLowerInvariant(),(Split-Path $asset -Leaf)
    }
    Set-Content -LiteralPath (Join-Path $destination 'SHA256SUMS.txt') -Value $checksums -Encoding ascii
    Write-Output "Prepared $destination"
} finally { Pop-Location }
