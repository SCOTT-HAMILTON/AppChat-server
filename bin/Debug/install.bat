@echo off
set APP=%1
set ICON=%2
echo app = %APP%
reg add HKCR\.appcs_cf /f
reg add HKCR\AppChat-server_ConfigFiles /f
reg add HKCR\.appcs_cf /ve /t REG_SZ /d "AppChat-server_ConfigFiles" /f
reg add HKCR\AppChat-server_ConfigFiles\DefaultIcon  /f
reg add HKCR\AppChat-server_ConfigFiles\DefaultIcon /ve /t REG_SZ /d %ICON% /f
reg add HKCR\AppChat-server_ConfigFiles\shell  /f
reg add HKCR\AppChat-server_ConfigFiles\shell\open  /f
reg add HKCR\AppChat-server_ConfigFiles\shell\open\command  /f
reg add HKCR\AppChat-server_ConfigFiles\shell\open\command /ve /t REG_SZ /d %APP% /f
ver | find /i "version 10.0.1" > nul && if %errorlevel%==0 goto winten
 
ie4uinit.exe -ClearIconCache
goto end

:winten
ie4uinit.exe -show
goto end
 
:end

