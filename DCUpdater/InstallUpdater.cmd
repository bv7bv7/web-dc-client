SC.exe CONFIG Schedule start= auto
NET START Schedule
SET task=/TN "DC_Updater"
schtasks.exe /End %task%
schtasks.exe /Delete %task% /F
schtasks.exe /Create /RU "SYSTEM" /SC ONLOGON %task% /TR """"%~dp0DCUpdater.exe""""
schtasks.exe /Run %task%