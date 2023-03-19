SET task=/TN "DC_Updater"
schtasks.exe /End %task%
schtasks.exe /Delete %task% /F