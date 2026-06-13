param(
    [Parameter(Mandatory = $true)]
    [string]$WinDeployQt,

    [Parameter(Mandatory = $true)]
    [string]$InstalledExePath,

    [Parameter(Mandatory = $true)]
    [string]$QmlDir,

    [Parameter(Mandatory = $true)]
    [string]$DeployMode
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $InstalledExePath)) {
    throw "Installed executable not found: $InstalledExePath"
}

$installRoot = Split-Path -Path (Split-Path -Path $InstalledExePath -Parent) -Parent

$deployArgs = @(
    "--$DeployMode"
    "--force"
    "--qmldir"
    $QmlDir
    $InstalledExePath
)

& $WinDeployQt @deployArgs
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed with exit code $LASTEXITCODE"
}
