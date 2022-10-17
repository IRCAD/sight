cd %~dp0
set ok=1
for /f %%t in ('%1 --list') do (
    %1 %%t || set ok=0
)
if %ok%==0 exit 1
