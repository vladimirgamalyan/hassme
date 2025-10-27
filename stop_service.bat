@echo off
setlocal enabledelayedexpansion
set SERVICE_NAME=HassMeService

echo.
echo === Stopping %SERVICE_NAME% ===

REM Проверяем права администратора
net session >nul 2>&1
if !errorLevel! neq 0 (
    echo [ERROR] Please run this script as Administrator.
    echo.
    pause
    exit /b 1
)

REM Проверяем, установлена ли служба
sc query "%SERVICE_NAME%" >nul 2>&1
if !errorLevel! neq 0 (
    echo [WARN] Service "%SERVICE_NAME%" is not installed.
    echo.
    pause
    exit /b 0
)

REM Проверяем, запущена ли служба
sc query "%SERVICE_NAME%" | find /i "RUNNING" >nul
if !errorLevel!==0 (
    echo Stopping service...
    sc stop "%SERVICE_NAME%" >nul

    set /a wait_time=0
    :wait_loop
    for /f "tokens=3 delims=: " %%A in ('sc query "%SERVICE_NAME%" ^| find "STATE"') do (
        set state=%%A
    )
    if /i "!state!" NEQ "STOPPED" (
        if !wait_time! lss 15 (
            timeout /t 1 >nul
            set /a wait_time+=1
            goto wait_loop
        )
    )

    if /i "!state!"=="STOPPED" (
        echo [OK] Service "%SERVICE_NAME%" stopped successfully.
    ) else (
        echo [WARN] Service did not stop within timeout.
        echo Last known state: !state!
    )
) else (
    echo [INFO] Service "%SERVICE_NAME%" is not running.
)

echo.
pause
endlocal
