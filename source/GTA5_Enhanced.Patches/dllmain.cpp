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

#define DEBUG_IN_RELEASE 0

HMODULE baseModule = 0;

struct BenchmarkConfig
{
    bool bUnlockFPS;
    size_t iBenchmarkSize, iBenchmarkIndex;

    std::filesystem::path config_path;
    std::filesystem::path index_path;

    std::string currentConfigPath;
    std::string currentConfigPathFileName;
    std::filesystem::path fs_currentConfigPath;
    std::filesystem::path currentDir;
    std::filesystem::path cfgPath;

    DWORD ReadConfig()
    {
        memset(this, 0, sizeof(*this));
        config_path = _PROJECT_NAME ".yaml";
        index_path = _PROJECT_NAME ".index.yaml";

        {
            currentDir = std::filesystem::current_path();
            cfgPath = currentDir / config_path;
            index_path = currentDir / index_path;
            std::ifstream yamlFile(cfgPath);
            if (!yamlFile)
            {
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << "Settings" << YAML::Value << YAML::BeginMap;
                ADD_YAML_KEY(out, bUnlockFPS, true);
                out << YAML::Comment(
                    "Patches internal audio time out + limiters.\n"
                    "Currently does not patch frame limiter (~300FPS Max)\n"
                    //"For CPU Benching, this can become an issue. there might be stutters during streaming\n"
                    //"Recommended to cap FPS (via RTSS or other means) to 240 for stable frametime!!\n"
                    //"For GPU Benching, this won't be an issue.\n"
                    //"due to partial limiters trying to sync threads(?).\n\n"
                    "Patch is based on `GTAV.AudioFixes`\n"
                    "(Patch itself is based on FiveM fix)\n\n\n"
                    "#### Adapted from the CitizenFX project, retrieved 2025-03-16. ####\n"
                    "#### You can view the original project and license terms at: ####\n"
                    "    https://runtime.fivem.net/fivem-service-agreement-4.pdf\n"
                    "    https://github.com/citizenfx/fivem/blob/master/code/LICENSE\n"
                    "    https://github.com/citizenfx/fivem/blob/4e9aa42c700f88735a2b9c2f51738568daf597e4/code/components/gta-core-five/src/GameAudioState.cpp#L434\n"
                    "    https://github.com/Gogsi/GTAV.AudioFixes/blob/e0588ec4667898ceec5a5bbcf628fcb8c46ba09d/script.cpp#L42\n"
                );
                out << YAML::Newline;
                out << YAML::EndMap;
                std::ofstream yamlFileOut(config_path);
                yamlFileOut << out.c_str();
                yamlFileOut << "\n";
                printf_s("%s\n", out.c_str());
                {
                    wchar_t msg[512]{};
                    _snwprintf_s(msg, _countof(msg), L""
                                 "Created patch config file at \"%s\".\n\n"
                                 "Click OK to write file contents and close application for user to adjust file.", cfgPath.u16string().c_str());
                    iMSGW(msg, _PROJECT_NAME);
                }
                return 1;
            }
        }
        YAML::Node config = YAML::LoadFile(config_path.u8string());
        YAML::Node indexNode;
        if (config["Settings"])
        {
            GET_KEY_BOOL(bUnlockFPS, "Settings", bool, true);
        }
        else
        {
            MessageBox(0, L"Settings not found in YAML file", L"" PROJECT_NAME " Startup Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        return 0;
    }
} g_BenchmarkCfg;

static void DoFPSPatches()
{
    if (g_BenchmarkCfg.bUnlockFPS)
    {
        /*
        *	Adapted from the CitizenFX project, retrieved 2025-03-16.
        *   You can view the original project and license terms at:
        *	https://runtime.fivem.net/fivem-service-agreement-4.pdf
        *	https://github.com/citizenfx/fivem/blob/master/code/LICENSE
        *   https://github.com/citizenfx/fivem/blob/4e9aa42c700f88735a2b9c2f51738568daf597e4/code/components/gta-core-five/src/GameAudioState.cpp#L434
        */
        const uintptr_t asynchronousAudioByte = FindAndPrintPatternW(L"48 89 d9 89 fa e8 ? ? ? ? 80 3d ? ? ? ? 00 75 ? e8 ? ? ? ?", L"asynchronousAudioByte", 10);
        const uintptr_t audioTimeoutLoad = FindAndPrintPatternW(L"81 c3 e8 03 00 00 39 d7 0f 94 c1 39 d8", L"audioTimeoutLoad", 2);
        // https://github.com/Gogsi/GTAV.AudioFixes/blob/e0588ec4667898ceec5a5bbcf628fcb8c46ba09d/script.cpp#L42
        // Not sure if this does anything in gen9
        const uintptr_t audioLimiter2Address = FindAndPrintPatternW(L"48 8b 0d ? ? ? ? ba ff ff ff ff e8 ? ? ? ? 0f b6", L"audioLimiter2Pattern", 12);
        if (asynchronousAudioByte && audioTimeoutLoad && audioLimiter2Address)
        {
            bool* AsyncByte = (bool*)ReadLEA32(asynchronousAudioByte, L"AsyncByte", 0, 2, 7);
            if (AsyncByte)
            {
                *AsyncByte = 0;
            }
            int TimeoutMs = 0;
            Memory::PatchBytes(audioTimeoutLoad, &TimeoutMs, sizeof(TimeoutMs));
            Memory::MakeNops(audioLimiter2Address, 5);
        }
    }
}

static void DoAlwaysEnabledPatches()
{
    if (DEBUG_IN_RELEASE)
    {
        const uintptr_t CloseConsoleIfOpenAddr = FindAndPrintPatternW(L"48 83 ec ? ff 15 ? ? ? ? 48 85 c0 74 ? 48 89 c1 31 d2", L"CloseConsoleIfOpen", 13);
        if (CloseConsoleIfOpenAddr)
        {
            static const uint8_t jmp[] = { 0xeb };
            Memory::PatchBytes(CloseConsoleIfOpenAddr, jmp, sizeof(jmp));
        }
    }
    const uintptr_t HasLegalVideosCompleted = FindAndPrintPatternW(L"80 3d ? ? ? ? 00 0f 85 ? ? ? ? 48 8d 54 24 ? 48 89 c1", L"HasLegalVideosCompleted");
    if (HasLegalVideosCompleted)
    {
        uint8_t* pHasLegalVideosCompleted = (uint8_t*)ReadLEA32(HasLegalVideosCompleted, L"pHasLegalVideosCompleted", 0, 2, 7);
        *pHasLegalVideosCompleted = true;
    }
    const uintptr_t GameStartedFromLauncher = FindAndPrintPatternW(L"e8 ? ? ? ? 89 c1 31 c0 84 c9 75 ? b9 2f a9 c2 f4", L"GameStartedFromLauncher");
    if (GameStartedFromLauncher)
    {
        static const uint8_t mov_eax_1[] = { 0xB8, 0x01, 0x00, 0x00, 0x00 };
        Memory::PatchBytes(GameStartedFromLauncher, mov_eax_1, sizeof(mov_eax_1));
    }
}

uiTYPEDEF_FUNCTION_PTR(HRESULT, WerSetFlags_Followed_Original, DWORD flag);

static HRESULT WerSetFlags_Hook(DWORD flag)
{
    if (DEBUG_IN_RELEASE)
    {
        if (!IsDebuggerPresent())
        {
            iMSGW(L"Wait for debugger", L"" PROJECT_NAME);
        }
    }
    DoAlwaysEnabledPatches();
    DoFPSPatches();
    // While we're here, lets reenable the error ui too
    return WerSetFlags_Followed_Original.ptr(flag | WER_FAULT_REPORTING_ALWAYS_SHOW_UI);
}

static void HookStartupFunc()
{
    // Go from our thunk
    const uint8_t* pWerSetFlags = (uint8_t*)WerSetFlags;
    printf_s("WerSetFlags: 0x%p\n", pWerSetFlags);
    // Wine has this export
    const HMODULE kbase = GetModuleHandle(L"kernelbase.dll");
    const uintptr_t kbase_WerSetFlags = (uintptr_t)GetProcAddress(kbase, "WerSetFlags");
    if (pWerSetFlags[0] == 0xff && pWerSetFlags[1] == 0x25)
    {
        // To kernel32 thunk
        // For safety, I use int3 jump trick to prevent any extra memory being allocated!
        uint8_t** pWerSetFlags2 = (uint8_t**)ReadLEA32((uintptr_t)pWerSetFlags, L"pWerSetFlags2", 0, 2, 6);
        if (pWerSetFlags2[0][0] == 0xe9)
        {
            WerSetFlags_Followed_Original.addr = ReadLEA32((uintptr_t)pWerSetFlags2[0], L"WerSetFlags_Followed_Original", 0, 1, 5);
            const HMODULE k32 = GetModuleHandle(L"kernel32.dll");
            if (k32)
            {
                const uintptr_t int3 = FindInt3Jmp(k32);
                if (int3)
                {
                    MAKE32HOOK(pWerSetFlags2[0], int3, WerSetFlags_Hook, 5);
                }
            }
        }
        // Wine module
        else if (kbase_WerSetFlags)
        {
            WerSetFlags_Followed_Original.addr = kbase_WerSetFlags;
            Memory::DetourFunction64((uintptr_t)pWerSetFlags2[0], (uintptr_t)&WerSetFlags_Hook, 14);
        }
        else
        {
            wchar_t msg[256]{};
            _snwprintf_s(msg, _countof(msg), _TRUNCATE, L""
                         "First instruction of WerSetFlags is not a long branch!\n"
                         "Patching game will not be possible (Code is still encrypted by DRM)\n"
                         "Address 0x%p\n"
                         "Byte 0x%x 0x%x 0x%x",
                         (uintptr_t)pWerSetFlags2[0],
                         pWerSetFlags2[0][0], pWerSetFlags2[0][1], pWerSetFlags2[0][2]);
            iMSGW(msg, _PROJECT_NAME " Startup Error");
        }
    }
}

static DWORD Main(void*)
{
    baseModule = GetModuleHandle(L"GTA5_Enhanced.exe");
    if (!baseModule)
    {
        return false;
    }

    if (DEBUG_IN_RELEASE)
    {
        ShowConsole1();
    }
    HookStartupFunc();

    if (g_BenchmarkCfg.ReadConfig())
    {
        ExitProcess(0);
        return false;
    }
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
