if(($args.length -ne 2) -and ($args.length -ne 3))
{
	Write-Output 'Build''s speech misconfigured'
	[System.Console]::Beep()
	exit
}
#$mutex = New-Object System.Threading.Mutex($false, "MonadMutexTTS")
#$dummy = $mutex.WaitOne()
if($args.length -eq 2)
{
	$res = $args[1]
}
else
{
	$res = $args[1] + ' for ' + $args[2]
}
Write-Output ('Project has been built: ' + $res)
(Get-Date).toString("yyyy-MM-dd HH:mm:ss")