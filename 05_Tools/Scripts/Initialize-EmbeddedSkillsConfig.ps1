$ErrorActionPreference = 'Stop'

$toolsRoot = Split-Path -Path $PSScriptRoot -Parent
$repoRoot = Split-Path -Path $toolsRoot -Parent
$examplePath = Join-Path $repoRoot '05_Tools/Config/embeddedskills.config.example.json'
$configDirectory = Join-Path $repoRoot '.embeddedskills'
$configPath = Join-Path $configDirectory 'config.json'

if (Test-Path -LiteralPath $configPath) {
    Write-Host '.embeddedskills/config.json 已存在，未作修改。'
    return
}

if (-not (Test-Path -LiteralPath $configDirectory -PathType Container)) {
    New-Item -ItemType Directory -Path $configDirectory | Out-Null
}

if (Test-Path -LiteralPath $configPath) {
    Write-Host '.embeddedskills/config.json 已存在，未作修改。'
    return
}

[System.IO.File]::Copy($examplePath, $configPath, $false)
Write-Host '已从项目示例创建 .embeddedskills/config.json。'
