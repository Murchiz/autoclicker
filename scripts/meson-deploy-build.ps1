param(
    [Parameter(Mandatory = $true)]
    [string]$WinDeployQt,

    [Parameter(Mandatory = $true)]
    [string]$BuiltExePath,

    [Parameter(Mandatory = $true)]
    [string]$BuiltPdbPath,

    [Parameter(Mandatory = $true)]
    [string]$DeployDir,

    [Parameter(Mandatory = $true)]
    [string]$QmlDir,

    [Parameter(Mandatory = $true)]
    [string]$DeployMode,

    [Parameter(Mandatory = $true)]
    [string]$StampPath
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $BuiltExePath)) {
    throw "Built executable not found: $BuiltExePath"
}

if (Test-Path -LiteralPath $DeployDir) {
    Remove-Item -LiteralPath $DeployDir -Recurse -Force
}

New-Item -ItemType Directory -Path $DeployDir | Out-Null

$deployedExePath = Join-Path $DeployDir ([IO.Path]::GetFileName($BuiltExePath))
Copy-Item -LiteralPath $BuiltExePath -Destination $deployedExePath -Force

if (Test-Path -LiteralPath $BuiltPdbPath) {
    Copy-Item -LiteralPath $BuiltPdbPath -Destination (Join-Path $DeployDir ([IO.Path]::GetFileName($BuiltPdbPath))) -Force
}

$deployArgs = @(
    "--$DeployMode"
    "--force"
    "--qmldir"
    $QmlDir
    $deployedExePath
)

& $WinDeployQt @deployArgs
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed with exit code $LASTEXITCODE"
}

Set-Content -LiteralPath $StampPath -Value $deployedExePath -NoNewline
