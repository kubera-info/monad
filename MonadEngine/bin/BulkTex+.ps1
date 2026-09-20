##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

if ($args.length -lt 3)
{
	Write-Error "($MyInvocation.MyCommand.Name): Incorrect number of parameters, should be: <Format> <MipMaps> <Output Folder> <Input Folders..0 or more>"
	[System.Console]::Beep()
	exit 1
}

. (Join-Path $PSScriptRoot 'Tools.ps1')
$Exe = (Join-Path $PSScriptRoot '..\..\vcpkg_installed\x64\x64-windows-static-md\tools\directxtex\texconv.exe')
$List = ''
# This loop processes packs of graphical files in theirs folders, rather than single files.
For ($FolderNumber = 3; $FolderNumber -lt $args.length; ++$FolderNumber)
{
	ForEach ($FileToConvert in (Get-ChildItem -Path ($args[$FolderNumber]) –Name)
	| Where-Object{$_ -imatch '\.(bmp|jpg|jpeg|png|dds|ddx|tga|hdr|tif|tiff|wdp|hdp|jxr|ppm|pfm|heif)$'})
	{
		$SourceFile = Join-Path $args[$FolderNumber] $FileToConvert
		$ObjectFile = Join-Path $args[2] ([System.IO.Path]::GetFileNameWithoutExtension($FileToConvert) + '.dds')
		if (Test-MonadFileOutdated $SourceFile $ObjectFile)
		{
			if (-not [string]::IsNullOrEmpty($List))
			{
				$List += "`n"
			}
			$List += $SourceFile
		}
	}
	if (-not [string]::IsNullOrEmpty($List))
	{
		$FileList = (Join-Path $args[2] 'Conv.$$$')
		$List | Out-File -NoNewline -FilePath $FileList
		& $Exe -nologo -y -gpu 0 -f $args[0] -flist "$FileList" -srgb -m $args[1] -o $args[2]
		& $Exe -nologo -y -gpu 0 -f $args[0] -srgb -m $args[1] -o $args[2] $SourceFile
		Remove-Item -Path $FileList -Force
	}
}