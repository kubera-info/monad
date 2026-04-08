##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

if ($args.length -lt 2)
{
	Write-Error "($MyInvocation.MyCommand.Name): Incorrect number of parameters, should be: <Output File> <Input Folder>"
	[System.Console]::Beep()
	exit 1
}

. ($PSScriptRoot + '\Tools.ps1')
$Exe = $PSScriptRoot + '\texassemble.exe'
$ExeConv = $PSScriptRoot + '\texconv.exe'
$params = $args
$Emo = Get-ChildItem -Path ($args[1]) –Name
| Where-Object{($_.Length -eq 15) -and ($_ -imatch '\.(dds)$')}
| ForEach-Object { $params[1] + '\' + $_ }
| Sort-Object

$Emo | Out-File 'TexList.txt'

$newParams = @('array', '-o', $args[0], '-fl', '12.0', '-dx10', '-y', '-flist', 'TexList.txt')
& $Exe $newParams
& $ExeConv @('-m', '3', '-y', $args[0], '-o', $args[1])

Remove-Item -Path 'TexList.txt'

#$XML = '<Monad>'
#for($Index = 0; $Index -lt $Emo.Length; ++$Index)
#{
#	$XML += '<Slice id="' + $Emo[$Index].Replace($args[1] + '\', '').Replace('.dds', '') + '" position="' + $Index + '"/>'; 	
}
#$XML += '</Monad>'
#$XML | Out-File 'emoji.xml'
