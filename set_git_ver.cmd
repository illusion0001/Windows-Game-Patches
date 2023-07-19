@echo off

for /f %%a in ('git rev-parse HEAD') do set "GIT_COMMIT=%%a"
for /f %%a in ('git branch --show-current') do set "GIT_VER=%%a"
for /f %%a in ('git rev-list HEAD --count') do set "GIT_NUM=%%a"

set "GIT_VER_FILE=.\include\git_ver.h"
if not exist "%GIT_VER_FILE%" (
    set "GIT_VER_FILE=..\..\include\git_ver.h"
)

echo #define GIT_COMMIT "Git Commit: %GIT_COMMIT%" > "%GIT_VER_FILE%"
echo #define GIT_VER "Git Branch: %GIT_VER%" >> "%GIT_VER_FILE%"
echo #define GIT_NUM "Git Version: %GIT_NUM%" >> "%GIT_VER_FILE%"
type "%GIT_VER_FILE%"
