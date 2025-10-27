@echo off
setlocal enabledelayedexpansion
set SERVICE_NAME=HassMeService
set EXE_NAME=hassme.exe
set TARGET_DIR=%ProgramFiles%\%SERVICE_NAME%
set SOURCE_DIR=%~dp0x64\Release
set CONFIG_FILE=%~dp0config.json

echo.
echo === Installing or Updating %SERVICE_NAME% ===

REM Проверка прав администратора
net session >nul 2>&1
if !errorLevel! neq 0 (
    echo [ERROR] Please run this script as Administrator.
    echo Right-click the file and select "Run as administrator".
    echo.
    pause
    exit /b 1
)

REM Останавливаем службу, если запущена
sc query "%SERVICE_NAME%" | find /i "RUNNING" >nul
if !errorLevel!==0 (
    echo Stopping existing service...
    sc stop "%SERVICE_NAME%" >nul
    set /a wait_time=0
    :wait_stop
    sc query "%SERVICE_NAME%" | find /i "STOPPED" >nul
    if !errorLevel! neq 0 (
        if !wait_time! lss 10 (
            timeout /t 1 >nul
            set /a wait_time+=1
            goto wait_stop
        )
    )
)

REM Создаём целевую папку
if not exist "%TARGET_DIR%" (
    echo Creating target directory: "%TARGET_DIR%"
    mkdir "%TARGET_DIR%"
)

REM Копируем файлы
echo Copying files from "%SOURCE_DIR%" to "%TARGET_DIR%" ...
xcopy "%SOURCE_DIR%\%EXE_NAME%" "%TARGET_DIR%\" /Y >nul
xcopy "%SOURCE_DIR%\*.dll" "%TARGET_DIR%\" /Y >nul

REM Копируем конфиг в ProgramData
if exist "%CONFIG_FILE%" (
    echo Copying config.json to ProgramData...
    mkdir "%ProgramData%\%SERVICE_NAME%" >nul 2>&1
    xcopy "%CONFIG_FILE%" "%ProgramData%\%SERVICE_NAME%\" /Y >nul
) else (
    echo [INFO] config.json not found in project root. Skipping.
)

REM Проверяем, существует ли служба
sc query "%SERVICE_NAME%" >nul 2>&1
if !errorLevel! neq 0 (
    echo Creating new service "%SERVICE_NAME%"...
    sc create "%SERVICE_NAME%" binPath= "\"%TARGET_DIR%\%EXE_NAME%\"" start= auto DisplayName= "HassMe Windows Service" >nul
) else (
    echo Service "%SERVICE_NAME%" already exists.
)

REM Запускаем службу
echo Starting service...
sc start "%SERVICE_NAME%" >nul
timeout /t 2 >nul

sc query "%SERVICE_NAME%" | find /i "RUNNING" >nul
if !errorLevel!==0 (
    echo [OK] Service "%SERVICE_NAME%" is running.
) else (
    echo [WARN] Service did not start automatically. Check logs in ProgramData.
)

echo.
echo Done.
pause
endlocal
