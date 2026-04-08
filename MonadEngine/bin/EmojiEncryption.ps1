##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################


if ($args.length -ne 5)
{
	Write-Error "${MyInvocation.MyCommand.Name}: Incorrect number of parameters, should be: <Salt> <Source Mask> <Destinate Folder> <Output Extension>"
	[System.Console]::Beep()
	exit 1
}

. ($PSScriptRoot + '\Tools.ps1')

Create-MonadEncryptedFiles $args[0] $args[1] $args[2] $args[3] $args[4]