#include <yaml-cpp/yaml.h>

#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "function_ptr.h"

#include <shlobj.h>
#include <strsafe.h>
#include <io.h>
#include <filesystem>

#include <shellapi.h>

#include <werapi.h>
#include <guiddef.h>

#include "config.hpp"

#include "UserProfile.hpp"

#include "halo1.hpp"
#include "halo3.hpp"

#define DEBUG_IN_RELEASE 1

HMODULE baseModule = 0;

struct XalHandle_ // not sure
{
    void* ptr1;
    void* ptr2;
    int user_count;
    uint64_t xuid;
    char pad[0x18];
};

static_assert(sizeof(XalHandle_) == 0x38, "");

struct LoggedInUsers
{
    char pad[0xe8];
    XalHandle_ users[4];
};


// https://github.com/WinterSquire/AlphaRing/blob/3ac75be8bb6429fa6051ca2e9978816212e1bbbb/src/mcc/CDeviceManager.h#L38
namespace gamepad_device_manager
{
    struct this_
    {
        void* vtable;
        void* p_input_device[5];
    };
}

LoggedInUsers** g_LoggedInUsers = 0;
gamepad_device_manager::this_** g_DeviceManager = 0;

static void UploadNewVftable_(const void* pSrc, const void* pDst, uintptr_t* pOriginal, const wchar_t* nameSrc, const wchar_t* nameDst, const wchar_t* nameOriginal)
{
    if (pOriginal)
    {
        *pOriginal = *(uintptr_t*)pSrc;
    }
    Memory::PatchBytes((uintptr_t)pSrc, &pDst, sizeof(pDst));
    LOG(L"Uploaded '%s' 0x%p to '%s' 0x%p\n", nameSrc, pSrc, nameDst, pDst);
    LOG(L"Original '%s' at 0x%p\n", nameOriginal, pOriginal);
}

#define UploadNewVftable(pSrc,pDst,pOriginal) UploadNewVftable_((void*)pSrc,(void*)pDst,pOriginal, L"" #pSrc,L"" #pDst,L"" #pOriginal)

uiTYPEDEF_FUNCTION_PTR(void*, CreateUser, const void*);

const uint64_t SplitScreenMaxUsers = 3;

static uint64_t iUsers = 1;

static int onAccountLinkClick(void)
{
    static bool deleteMode = false;
    static XalHandle_ PreviousUserList[SplitScreenMaxUsers] = {};
    uint64_t iUserLocal = iUsers - 1;

    XalHandle_* CurrentUser = &g_LoggedInUsers[0]->users[iUsers];
    if (!CurrentUser->ptr1 && !CurrentUser->ptr2 &&
        !PreviousUserList[iUserLocal].ptr1 && !PreviousUserList[iUserLocal].ptr2)
    {
        // TODO: Find a way to generate a fake user for a valid handle and
        // xuid without needing to hook XalUserGetId and call whatever this is
        void* pad[2]{};
        CreateUser.ptr(&pad);
        LOG(L"Allocated user at %p %p\n", pad[0], pad[1]);
        PreviousUserList[iUserLocal].ptr1 = CurrentUser->ptr1 = pad[0];
        PreviousUserList[iUserLocal].ptr2 = CurrentUser->ptr2 = pad[1];
        iUsers++;
    }
    else if (deleteMode && CurrentUser->ptr1 && CurrentUser->ptr2)
    {
        LOG("Delete user %d\n", iUserLocal);
        CurrentUser->ptr1 = CurrentUser->ptr2 = 0;
        iUsers--;
    }
    else if (PreviousUserList[iUserLocal].ptr1 && PreviousUserList[iUserLocal].ptr2)
    {
        LOG("Readd user %d\n", iUserLocal);
        CurrentUser->ptr1 = PreviousUserList[iUserLocal].ptr1;
        CurrentUser->ptr2 = PreviousUserList[iUserLocal].ptr2;
        iUsers++;
    }
    if (iUsers > SplitScreenMaxUsers)
    {
        LOG("Delete user mode\n");
        iUsers = SplitScreenMaxUsers;
        deleteMode = true;
    }
    else if (iUsers < 1)
    {
        LOG("Readd user mode\n");
        iUsers = 1;
        deleteMode = false;
    }
    return 1;
}

// This function seems broken, it never returns a valid gamepad device
// So fix 343i stuff
static void* getPlayerGamepadDevice(void* param_1, uint32_t player_idx)
{
    return g_DeviceManager[0]->p_input_device[player_idx];
}

uiTYPEDEF_FUNCTION_PTR(uint64_t, XalUserGetId_Original, void* user);

static uint64_t XalUserGetId_Hook(void* user)
{
    static uint64_t xuid_list[SplitScreenMaxUsers] = {};
    uint64_t ret_xuid = XalUserGetId_Original.ptr(user);
    if (!ret_xuid)
    {
        for (uint64_t i = 1; i < iUsers; i++)
        {
            if (!xuid_list[i])
            {
                union xuid
                {
                    uint64_t guid[2];
                    GUID _guid;
                } local_xuid = {};
                CoCreateGuid(&local_xuid._guid);
                xuid_list[i] = local_xuid.guid[0] ^ local_xuid.guid[1];
                LOG("added xuid 0x%p for player %d!\n", xuid_list[i], i + 1);
            }
            // if return id is 0, give it something!
            if (xuid_list[i] && g_LoggedInUsers[0]->users[i].ptr1 == user)
            {
                ret_xuid = xuid_list[i];
                g_LoggedInUsers[0]->users[i].xuid = ret_xuid;
                break;
            }
        }
    }
    return ret_xuid;
}

uiTYPEDEF_FUNCTION_PTR(CUserProfile*, GetUserProfile_Original, const void* this_, const uint64_t xuid);

static CUserProfile* GetUserProfileHook(const void* this_, const uint64_t xuid)
{
    CUserProfile* ret = GetUserProfile_Original.ptr(this_, g_LoggedInUsers[0]->users[0].xuid);
    for (uint64_t i = 1; i < iUsers; i++)
    {
        if (xuid == g_LoggedInUsers[0]->users[i].xuid && ConfigSettings.SplitScreenPlayerConfig[i].PlayerInvertY)
        {
            ret->LookControlsInverted = true;
            LOG("Set player %d invert\n", i + 1);
        }
    }
    return ret;
}

// https://github.com/icemesh/StringId/blob/main/StringId64/main.c
static constexpr StringId64 ToStringId64(const wchar_t* str)
{
    uint64_t base = 0xCBF29CE484222325;
    if (*str)
    {
        do
        {
            base = 0x100000001B3 * (base ^ *str++);
        }
        while (*str);
    }
    return base;
}

struct moduleInfo
{
    uint32_t title;
    uint32_t errorCode;
    HMODULE hModule;
};

static void PatchModules(const HMODULE hModule, const wchar_t* moduleName)
{
    if (!hModule)
    {
        LOG(L"moduleName: %s not loaded!\n", moduleName);
        return;
    }
    //LOG(L"moduleName: %s\n", moduleName);
    const uint64_t key = SID(moduleName);
    switch (key)
    {
        case SID(L"halo1\\halo1.dll"):
        {
            PatchHalo1(hModule);
            break;
        }
        case SID(L"halo3\\halo3.dll"):
        case SID(L"halo3odst\\halo3odst.dll"):
        {
            constexpr uint64_t sid_temp = SID(L"halo3odst\\halo3odst.dll");
            PatchHalo3(hModule, key == sid_temp);
            break;
        }
        default:
        {
            break;
        }
    }
}

uiTYPEDEF_FUNCTION_PTR(void, ModuleLoad_Original, const void* param_1, const void* param_2, const wchar_t* moduleName);

static void ModuleLoadHook(const moduleInfo* param_1, const void* param_2, const wchar_t* moduleName)
{
    ModuleLoad_Original.ptr(param_1, param_2, moduleName);
    PatchModules(param_1->hModule,moduleName);
}

static void DoPatches()
{
    const uintptr_t DeviceMgrPtr = FindAndPrintPatternW(L"48 89 35 ? ? ? ? 48 8b 3d ? ? ? ? 48 85 ff", L"DeviceMgrPtr");
    if (DeviceMgrPtr)
    {
        g_DeviceManager = (gamepad_device_manager::this_**)ReadLEA32(DeviceMgrPtr, L"g_DeviceManager", 0, 3, 7);
    }
    const uintptr_t LoggedInUserPtr = FindAndPrintPatternW(L"48 8b 0d ? ? ? ? 48 85 c9 74 17 45 33 c0 48 8d 55 ?", L"LoggedInUserPtr");
    if (LoggedInUserPtr)
    {
        g_LoggedInUsers = (LoggedInUsers**)ReadLEA32(LoggedInUserPtr, L"g_DeviceManager", 0, 3, 7);
    }
    const uintptr_t AccountLinkClickAddr = FindAndPrintPatternW(L"48 8b c4 55 48 8b ec 48 81 ec 80 00 00 00 48 c7 45 b0 fe ff ff ff 48 89 58 08 48 89 78 10 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 f0 33 c0 48 89 45 e0 48 89 45 e8 4c 8b c2 48 8d 55 e0",L"AccountLinkClick");
    const uintptr_t CreateUserAddr = FindAndPrintPatternW(L"48 8b c4 55 57 41 54 41 56 41 57 48 8d 68 a1 48 81 ec f0 00 00 00 48 c7 45 8f fe ff ff ff 48 89 58 10 48 89 70 18 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 27 48 8b f1 48 89 4c 24 20 41 bc 48 00 00 00 41 8b cc", L"CreateUser");
    if (AccountLinkClickAddr && CreateUserAddr)
    {
        CreateUser.addr = CreateUserAddr;
        const uintptr_t pAccountLinkClickAddr = (uintptr_t)Memory::u64_Scan(baseModule, AccountLinkClickAddr);
        if (pAccountLinkClickAddr)
        {
            UploadNewVftable(pAccountLinkClickAddr, &onAccountLinkClick, nullptr);
        }
    }
    const uintptr_t XalGetUserIdAddr = FindAndPrintPatternW(L"40 53 48 83 ec 30 48 8b 05 ? ? ? ? 48 33 c4 48 89 44 24 28 48 83 64 24 20 00 48 8d 54 24 20 48 8b d9 48 8b 49 18 e8 ? ? ? ?", L"XalGetUserId");
    if (XalGetUserIdAddr)
    {
        const uintptr_t int3 = FindInt3Jmp();
        if (int3)
        {
            const uint64_t pCave = Memory::CreatePrologueHook(XalGetUserIdAddr, 6);
            if (pCave)
            {
                XalUserGetId_Original.addr = pCave;
                if (int3)
                {
                    MAKE32HOOK(XalGetUserIdAddr, int3, XalUserGetId_Hook, 6);
                }
            }
        }
    }
    const uintptr_t GamepadPlayersFixAddr = FindAndPrintPatternW(L"40 53 48 83 ec 20 33 db 38 1d ? ? ? ? 74 ? b1 1f", L"GamepadPlayersFix");
    if (GamepadPlayersFixAddr)
    {
        static const uint8_t mov_al_1[] = { 0xb0,0x01,0xc3 };
        Memory::PatchBytes(GamepadPlayersFixAddr, mov_al_1, sizeof(mov_al_1));
    }
    const uintptr_t getGamepadDeviceFix = FindAndPrintPatternW(L"4c 8b dc 41 56 48 83 ec 50 49 c7 43 c8 fe ff ff ff 49 89 5b 08 49 89 6b 18 49 89 7b 20 48 8b 05 ? ? ? ? 48 33 c4 48 89 44 24 48 8b da 4c 8b f1", L"getGamepadDeviceFix");
    if (getGamepadDeviceFix)
    {
        Memory::DetourFunction64(getGamepadDeviceFix, (uintptr_t)getPlayerGamepadDevice, 14);
    }
    const uintptr_t getUserProfileAddr = FindAndPrintPatternW(L"48 83 ec 38 4c 8b c2 48 8d 54 24 20 e8 ? ? ? ? 48 8b d0 e8 ? ? ? ? 48 83 c4 38 c3", L"getUserProfileAddr");
    if (getUserProfileAddr)
    {
        const uintptr_t pGetUserProfileAddr = (uintptr_t)Memory::u64_Scan(baseModule, getUserProfileAddr);
        if (pGetUserProfileAddr)
        {
            UploadNewVftable(pGetUserProfileAddr, GetUserProfileHook, &GetUserProfile_Original.addr);
        }
    }
    const uintptr_t ModuleLoadCallerAddr = FindAndPrintPatternW(L"48 8b cf e8 ? ? ? ? 8b 53 10 48 8b 4b 08", L"ModuleLoadCaller", 3);
    if (ModuleLoadCallerAddr)
    {
        const uintptr_t int3 = FindInt3Jmp();
        if (int3)
        {
            ModuleLoad_Original.addr = ReadLEA32(ModuleLoadCallerAddr, L"ModuleLoad_Original", 0, 1, 5);
            MAKE32CALL(ModuleLoadCallerAddr, int3, ModuleLoadHook, 5);
        }
    }
}

static DWORD Main(void*)
{
    baseModule = GetModuleHandle(L"MCC-Win64-Shipping.exe") ? : GetModuleHandle(L"MCCWinStore-Win64-Shipping.exe");

    if (!baseModule)
    {
        return false;
    }

    if (DEBUG_IN_RELEASE)
    {
        ShowConsole1();
    }
    DoPatches();
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            return Main(0);
        }
        default:
        {
            break;
        }
    }
    return TRUE;
}
