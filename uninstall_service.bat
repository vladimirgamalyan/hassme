@echo off
setlocal enabledelayedexpansion
set SERVICE_NAME=HassMeService
set EXE_NAME=hassme.exe
set TARGET_DIR=%ProgramFiles%\%SERVICE_NAME%
set DATA_DIR=%ProgramData%\%SERVICE_NAME%

echo.
echo === Uninstalling %SERVICE_NAME% ===

REM Проверка прав администратора
net session >nul 2>&1
if !errorlevel! neq 0 (
    echo [ERROR] Please run this script as Administrator.
    echo Right-click and select "Run as administrator".
    echo.
    pause
    exit /b 1
)

REM Проверяем наличие службы
sc query "%SERVICE_NAME%" >nul 2>&1
if !errorlevel! neq 0 (
    echo Service "%SERVICE_NAME%" is not installed.
) else (
    REM Останавливаем службу, если запущена
    sc query "%SERVICE_NAME%" | find /i "RUNNING" >nul
    if !errorlevel!==0 (
        echo Stopping service...
        sc stop "%SERVICE_NAME%" >nul

        set /a wait_time=0
        :wait_stop
        sc query "%SERVICE_NAME%" | find /i "STOPPED" >nul
        if !errorlevel! neq 0 (
            if !wait_time! lss 10 (
                timeout /t 1 >nul
                set /a wait_time+=1
                goto wait_stop
            )
        )
    )

    echo Deleting service...
    sc delete "%SERVICE_NAME%" >nul
)

REM Убиваем зависшие процессы
for /f "usebackq delims=" %%p in (`tasklist /fi "imagename eq %EXE_NAME%" ^| find /i "%EXE_NAME%"`) do (
    echo Killing leftover process...
    taskkill /im "%EXE_NAME%" /f >nul 2>&1
)

REM Удаляем файлы из Program Files
if exist "%TARGET_DIR%" (
    echo Removing "%TARGET_DIR%" ...
    attrib -r -h -s "%TARGET_DIR%\*" /s /d >nul 2>&1
    rmdir /s /q "%TARGET_DIR%" 2>nul
    if exist "%TARGET_DIR%" (
        echo [WARN] Could not remove "%TARGET_DIR%". Close any apps locking files and try again.
    ) else (
        echo [OK] Program Files directory removed.
    )
) else (
    echo Program Files directory not found: "%TARGET_DIR%"
)

REM Проверяем параметр purge отдельно (без скобок в if)
if /i "%~1"=="purge" goto :purge_data
goto :done

:purge_data
if exist "%DATA_DIR%" (
    echo PURGE mode: removing data directory "%DATA_DIR%" ...
    attrib -r -h -s "%DATA_DIR%\*" /s /d >nul 2>&1
    rmdir /s /q "%DATA_DIR%" 2>nul
    if exist "%DATA_DIR%" (
        echo [WARN] Could not remove "%DATA_DIR%". Check permissions/locks.
    ) else (
        echo [OK] ProgramData directory removed.
    )
) else (
    echo ProgramData directory not found: "%DATA_DIR%"
)
goto :done

:done
echo.
echo Done.
pause
endlocal
