@echo off
reg delete HKCR\.appcs_cf /f
reg delete HKCR\AppChat-server_ConfigFiles /f

ver | find /i "version 10.0.1" > nul && if %errorlevel%==0 goto winten
 
ie4uinit.exe -ClearIconCache
goto end

:winten
ie4uinit.exe -show
goto end
 
:end
cls