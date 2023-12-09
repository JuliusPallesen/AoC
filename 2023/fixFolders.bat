@echo off
setlocal enabledelayedexpansion

for /l %%i in (1, 1, 25) do (
    set "num=%%i"
    set "num=0!num!"
    set "num=!num:~-2!"
    ren "Day%%i" "Day!num!"
)

endlocal