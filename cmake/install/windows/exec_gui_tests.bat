@echo off
setlocal enabledelayedexpansion

cd %~dp0
set ok=0

for /f %%t in ('%1 --list') do (
    call %1 %%t
    if !errorlevel! neq 0 set ok=!errorlevel!
)

exit !ok!

endlocal
