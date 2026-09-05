[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$libraryRoot = Split-Path -Parent $PSScriptRoot
$requiredFiles = @(
    '00-治理/素材许可证总表.csv',
    '00-治理/素材使用记录模板.csv',
    '30-制作规范/大学PPT生产规范.md',
    '50-技能/技能清单.md'
)

$missing = foreach ($relativePath in $requiredFiles) {
    $path = Join-Path $libraryRoot $relativePath
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { $relativePath }
}

if ($missing) {
    throw "资源库缺少必要文件：$($missing -join '；')"
}

$assets = Get-ChildItem -LiteralPath $libraryRoot -Recurse -File -Include '*.svg', '*.ttf', '*.otf' |
    Where-Object { $_.FullName -notmatch '\\.git\\' }
$licenses = Get-ChildItem -LiteralPath $libraryRoot -Recurse -File -Include 'LICENSE*', 'OFL.txt' |
    Where-Object { $_.FullName -notmatch '\\.git\\' }

if ($assets.Count -eq 0) {
    throw '尚未发现 SVG、TTF 或 OTF 素材。请先完成首批收集。'
}
if ($licenses.Count -eq 0) {
    throw '发现素材但未发现许可证文件。不能将其视为可复用素材。'
}

[PSCustomObject]@{
    asset_files = $assets.Count
    license_files = $licenses.Count
    checked_at = (Get-Date).ToString('yyyy-MM-dd HH:mm:ss K')
} | Format-List
