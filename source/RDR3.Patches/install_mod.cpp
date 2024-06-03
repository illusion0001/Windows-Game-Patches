#include "mod_data.hpp"
#include "mod_enum.h"
#include <direct.h>
#include <stdio.h>
#include <io.h>

#define BASE_PATH ".\\x64"
#define BASE_DATA_PATCH_PATH BASE_PATH "\\data"
#define BASE_MENU_PATCH_PATH BASE_PATH "\\main_and_legal_menu_skip_data"

void Install_NoLogo_MainMenu_Patch(int mode)
{
    switch (mode)
    {
    case ModInstallEnable:
    {
        if (_access(BASE_DATA_PATCH_PATH "\\startup.meta", 0) == 0 ||
            _access(BASE_MENU_PATCH_PATH "\\legal_screen_patch.ymt", 0) == 0 ||
            _access(BASE_MENU_PATCH_PATH "\\main_menu_patch.ymt", 0) == 0)
        {
            printf_s("File exists\n");
        }

        if (_access(BASE_DATA_PATCH_PATH "\\startup_disable.meta", 0) == 0)
        {
            printf_s("startup_disable exists\n");
            rename(BASE_DATA_PATCH_PATH "\\startup_disable.meta", BASE_DATA_PATCH_PATH "\\startup.meta");
            break;
        }

        printf_s("no file exists, creating it now\n");

        _mkdir(BASE_PATH);
        _mkdir(BASE_DATA_PATCH_PATH);
        _mkdir(BASE_MENU_PATCH_PATH);

        FILE* startup_fp{};
        fopen_s(&startup_fp, BASE_DATA_PATCH_PATH "\\startup.meta", "w");
        if (startup_fp)
        {
            fputs(STARTUP_META_DATA, startup_fp);
            fclose(startup_fp);
        }

        FILE* legal_fp{};
        fopen_s(&legal_fp, BASE_MENU_PATCH_PATH "\\legal_screen_patch.ymt", "w");
        if (legal_fp)
        {
            fputs(LEGAL_YMT_DATA, legal_fp);
            fclose(legal_fp);
        }

        FILE* mainmenu_fp{};
        fopen_s(&mainmenu_fp, BASE_MENU_PATCH_PATH "\\main_menu_patch.ymt", "w");
        if (mainmenu_fp)
        {
            fputs(MAIN_MENU_DATA, mainmenu_fp);
            fclose(mainmenu_fp);
        }
        break;
    }
    case ModDisable:
    {
        if (_access(BASE_DATA_PATCH_PATH "\\startup.meta", 0) == 0)
        {
            printf_s("startup exists\n");
            rename(BASE_DATA_PATCH_PATH "\\startup.meta", BASE_DATA_PATCH_PATH "\\startup_disable.meta");
        }
        break;
    }
    }
}
