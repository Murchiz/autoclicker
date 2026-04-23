param(
    [Parameter(Mandatory = $true)]
    [string]$WinDeployQt,

    [Parameter(Mandatory = $true)]
    [string]$InstalledExeRelativePath,

    [Parameter(Mandatory = $true)]
    [string]$QmlDir,

    [Parameter(Mandatory = $true)]
    [string]$DeployMode
)

$ErrorActionPreference = "Stop"

if ($env:MESON_INSTALL_DRY_RUN) {
    exit 0
}

$installRoot = $env:MESON_INSTALL_DESTDIR_PREFIX
if ([string]::IsNullOrWhiteSpace($installRoot)) {
    throw "MESON_INSTALL_DESTDIR_PREFIX is not set."
}

$exePath = Join-Path $installRoot $InstalledExeRelativePath
if (-not (Test-Path -LiteralPath $exePath)) {
    throw "Installed executable not found: $exePath"
}

$deployArgs = @(
    "--$DeployMode"
    "--force"
    "--qmldir"
    $QmlDir
    $exePath
)

& $WinDeployQt @deployArgs
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed with exit code $LASTEXITCODE"
}
