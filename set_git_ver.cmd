for /f %%a in ('git rev-parse HEAD') do set "GIT_COMMIT=%%a"
for /f %%a in ('git branch --show-current') do set "GIT_VER=%%a"
for /f %%a in ('git rev-list HEAD --count') do set "GIT_NUM=%%a"

echo #define GIT_COMMIT "Git Commit: %GIT_COMMIT%" > ..\..\include\git_ver.h
echo #define GIT_VER "Git Branch: %GIT_VER%" >> ..\..\include\git_ver.h
echo #define GIT_NUM "Git Version: %GIT_NUM%" >> ..\..\include\git_ver.h
type ..\..\include\git_ver.h
