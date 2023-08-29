$sourceFolder = ".\res\**"
$exeFolder = ".\build\exe\main\main.exe"
$destinationFolder = ".\publish\"
$newFileName = "DefenceIY.exe"

#Remove-Item -Path $destinationFolder -Recurse -Force
Copy-Item -Force $exeFolder -Destination $destinationFolder
Copy-Item -Path $sourceFolder -Destination "$destinationFolder" -Recurse
Rename-Item -Force -Path ".\publish\main.exe" -NewName $newFileName

#$items = Get-ChildItem $sourceFolder -Recurse
#foreach($item in $items)
#{
#    $relPath = $item.FullName.Replace($sourceFolder,"")
#    $targetPath = Join-Path -Path $destinationFolder -ChildPath $relPath
#
#
#    if($item.PSIsContainer)
#    {
#        New-Item -ItemType Directory -Force -Path $targetPath | Out-Null
#    }
#    else
#    {
#        Copy-Item -Force $item.FullName -Destination $targetPa\th
#    }
#}
