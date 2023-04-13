#include "helper.hpp"

// TODO: Find out where this came from
namespace Memory
{
    void PatchBytes(uintptr_t address, const unsigned char* pattern, unsigned int numBytes);
    void ReadBytes(const uintptr_t address, void* const buffer, const SIZE_T size);
    uintptr_t ReadMultiLevelPointer(uintptr_t base, const std::vector<uint32_t>& offsets);
    size_t GetHookLength(char* src, size_t minLen);
    bool DetourFunction32(void* src, void* dst, int len);
    void* DetourFunction64(void* pSource, void* pDestination, DWORD dwLen);
    HMODULE GetThisDllHandle();
    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    std::uint8_t* PatternScan(void* module, const char* signature);
    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    // Unicode Version
    std::uint8_t* PatternScanW(void* module, const wchar_t* signature);
    std::string GetVersionString();
    std::vector<int> string_to_ints(const std::string& s, char delimiter);
    std::wstring GetVersionProductName();
}
