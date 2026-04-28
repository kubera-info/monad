##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

function Write-MonadHr
{
	Write-Output '---------------------------'
}

function Write-MonadOutput(
	[string]$TextMessage
)
{
	Write-Output "Monad(a) Script: $TextMessage"
}

function Test-MonadFileOutdated(
	[string]$SourceFile, 
	[string]$ObjectFile
)
{
	(-not (Test-Path $ObjectFile -PathType Leaf)) -or (Test-Path $ObjectFile -OlderThan (Get-Item $SourceFile).LastWriteTime)
}

function Create-MonadIfMissed(
	[string]$Folder
)
{
	if (-not (Test-Path $Folder -PathType Container))
	{
		New-Item -ItemType Directory -Path $Folder -Force|Out-Null
	}
}

# Copies file if it is outdated or missing
function Copy-MonadFile(
	[string]$SourceFile, 
	[string]$DestinateFile
)
{
	Create-MonadIfMissed (Split-Path -Path $DestinateFile -Parent)
	if (Test-MonadFileOutdated $SourceFile $DestinateFile)
	{
		Copy-Item -Path $SourceFile -Destination $DestinateFile -Force
		Write-MonadOutput "copy $SourceFile => $DestinateFile"
		Write-MonadHr
	}
}

function Create-MonadEncryptedFile(
	[string]$Salt,
	[string]$SourceFile,
	[string]$DestinateFile
)
{
	Create-MonadIfMissed (Split-Path -Path $DestinateFile -Parent)
	if (Test-MonadFileOutdated $SourceFile $DestinateFile)
	{
		& (Join-Path $PSScriptRoot 'Encryption.exe') $Salt $SourceFile $DestinateFile
	}
}

function Create-MonadEncryptedFiles(
	[string]$Salt,
	[string]$SourceFolder,
	[string]$SourceWildcard,
	[string]$DestinateFolder,
	[string]$DestinateExtension
)
{
	Write-MonadOutput $DestinateExtension
	Get-ChildItem $SourceFolder -File -Filter $SourceWildcard
	| ForEach-Object {
		$Name = $_.FullName
		$Dest = Join-Path $DestinateFolder ([System.IO.Path]::GetFileNameWithoutExtension($_.Name) + '.' + $DestinateExtension)
		Create-MonadEncryptedFile $Salt $Name $Dest		
	}
}

Set-Variable WindowsX64Platform -Option ReadOnly -Value '' -Scope Global
Set-Variable WindowsArm64Platform -Option ReadOnly -Value 'ARM64' -Scope Global