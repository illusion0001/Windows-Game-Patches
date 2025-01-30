#include "helper.hpp"

#ifndef MAX_CAVE_SIZE
#define MAX_CAVE_SIZE 1024
#endif

// TODO: Find out where this came from
namespace Memory
{
    void MakeNops(const uintptr_t address, const int numOfNops);
    void FillInt(const uintptr_t address, const int intNum, const int numOfNops);
    void PatchBytes(uintptr_t address, uintptr_t pattern, unsigned int numBytes);
    void PatchBytes(uintptr_t address, const void* pattern, unsigned int numBytes);
    void PatchBytes(void* address, const void* pattern, unsigned int numBytes);
    void nPatchBytes(void* address, const void* pattern, unsigned int numBytes, int toPtr);
    void nReadBytes(const uintptr_t address, void* const buffer, const SIZE_T size);
    void ReadBytes(const uintptr_t address, void* const buffer, const SIZE_T size);
    uintptr_t ReadMultiLevelPointer(uintptr_t base, const std::vector<uint32_t>& offsets);
    void DetourFunction32(void* src, void* dst, int len);
    void DetourFunction32(uintptr_t src, uintptr_t dst, int len);
    void DetourFunction64(const void* pSource, const void* pDestination, const DWORD dwLen, const BOOL callMode = false);
    void DetourFunction64(uintptr_t pSource, uintptr_t pDestination, DWORD dwLen);
    void CallFunction32(void* src, void* dst, int len);
    void CallFunction64(void* pSource, void* pDestination, DWORD dwLen);
    static uint8_t cavePad[MAX_CAVE_SIZE]{};
    uintptr_t CreatePrologueHook(const uintptr_t address, const int min_instruction_size);
    uintptr_t CreateCodeCaveBlockX64(const void* data, const uintptr_t retaddr, const size_t data_size);
    HMODULE GetThisDllHandle();
    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    std::uint8_t* PatternScan(void* module, const char* signature);
    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    // Unicode Version
    std::uint8_t* PatternScanW(void* module, const wchar_t* signature);
    uint8_t* char_Scan(void* module, const char* value);
    uint8_t* char_Scan(void* module, const char* value, size_t value_len);
    uint8_t* wchar_Scan(void* module, const wchar_t* value, size_t value_len);
    uint8_t* wchar_Scan(void* module, const wchar_t* value);
    uint8_t* u32_Scan(void* module, uint32_t value);
    uint8_t* u64_Scan(void* module, uint64_t value);
#if !defined(WINXP)
    std::string GetVersionString();
    std::vector<int> string_to_ints(const std::string& s, char delimiter);
    std::wstring GetVersionProductName();
#endif
}

// https://github.com/emoose/NieRAutomata-LodMod/blob/fc7316bd3309ab38eb2a4016a96f9282d285fb90/Automata-LodMod/pch.h#L54
template <typename T>
inline T GameAddress(uintptr_t addr)
{
    return reinterpret_cast<T>(addr);
}
