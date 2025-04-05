#pragma once

#include <stdbool.h>
#include <filesystem>

struct ConfigData
{
    bool bEnableSplitScreenViaAccountLinkButton;
    bool bEnable4PlayersCoop;
    bool bPlayer1UseKeyboard;
    bool bHalo1FreezeFix;
    bool bEnableHalo3MainMenu;
    bool bDisableLoadScreen;
    bool bEnablePauseMenuHalo3;
    bool bEnablePauseMenuHalo3ODST;
    bool bEnablePauseMenuHaloReach;
    bool bEnablePauseMenuHalo4;
    struct SplitScreenPlayerConfig_
    {
        bool PlayerInvertY;
        bool PlayerCrouchToggle;
    }
    SplitScreenPlayerConfig[3]; // For players 2 - 4 because UE4 client settings menu is broken
    bool IsHalo3MainMenuLevelExist(bool isOdst = false) const // Main Menu patch, verify if `mainmenu.map` exists
    {
        if (bEnableHalo3MainMenu)
        {
            std::filesystem::path cwd = std::filesystem::current_path();
            wchar_t mainmenu_file[128]{};
            _snwprintf_s(mainmenu_file, _countof(mainmenu_file), L"%s/maps/mainmenu.map", !isOdst ? L"halo3" : L"halo3odst");
            std::filesystem::path mainmenu_map = cwd / mainmenu_file;
            bool exist = std::filesystem::exists(mainmenu_map);
            if (!exist)
            {
                wchar_t msg[1024]{};
                _snwprintf_s(msg, _countof(msg), L""
                             "Mainmenu file\n"
                             "`%s`\n"
                             "does not exist but `bEnableHalo3MainMenu` is enabled!\n"
                             "Please compile mainmenu map from source and place it in the above path.", mainmenu_map.u16string().c_str());
                wMSGW(msg, _PROJECT_NAME);
            }
            return bEnableHalo3MainMenu && exist;
        }
        return false;
    }
};

static ConfigData ConfigSettings{
    .bEnableSplitScreenViaAccountLinkButton = true,
    .bEnable4PlayersCoop = true,
    .bHalo1FreezeFix = true,
    .bDisableLoadScreen = true,
};
