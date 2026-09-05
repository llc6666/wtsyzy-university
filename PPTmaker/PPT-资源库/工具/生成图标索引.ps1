[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$libraryRoot = Split-Path -Parent $PSScriptRoot
$iconRoot = Join-Path $libraryRoot '10-图标'
$outputPath = Join-Path $libraryRoot '90-索引/图标索引.csv'

if (-not (Test-Path -LiteralPath $iconRoot)) {
    throw "图标目录不存在：$iconRoot"
}

$entries = Get-ChildItem -LiteralPath $iconRoot -Recurse -File -Filter '*.svg' |
    ForEach-Object {
        $relativePath = $_.FullName.Substring($libraryRoot.Length + 1).Replace('\', '/')
        $topFolder = $relativePath.Split('/')[1]
        [PSCustomObject]@{
            source = $topFolder
            name = $_.BaseName
            relative_path = $relativePath
            bytes = $_.Length
        }
    } |
    Sort-Object source, name

$entries | Export-Csv -LiteralPath $outputPath -NoTypeInformation -Encoding utf8BOM
Write-Host "已写入 $($entries.Count) 个 SVG 图标：$outputPath"
