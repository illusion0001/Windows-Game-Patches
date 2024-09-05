#include "helper.hpp"

// TODO: Find out where this came from
namespace Memory
{
    void PatchBytes(uintptr_t address, const void* pattern, unsigned int numBytes);
    void ReadBytes(const uintptr_t address, void* const buffer, const SIZE_T size);
    uintptr_t ReadMultiLevelPointer(uintptr_t base, const std::vector<uint32_t>& offsets);
    bool DetourFunction32(void* src, void* dst, int len);
    void* DetourFunction64(void* pSource, void* pDestination, DWORD dwLen);
    bool CallFunction32(void* src, void* dst, int len);
    void* CallFunction64(void* pSource, void* pDestination, DWORD dwLen);
    HMODULE GetThisDllHandle();
    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    std::uint8_t* PatternScan(void* module, const char* signature);
    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    // Unicode Version
    std::uint8_t* PatternScanW(void* module, const wchar_t* signature);
    uint8_t* char_Scan(void* module, const char* value, size_t value_len);
    uint8_t* u32_Scan(void* module, uint32_t value);
    uint8_t* u64_Scan(void* module, uint64_t value);
    std::string GetVersionString();
    std::vector<int> string_to_ints(const std::string& s, char delimiter);
    std::wstring GetVersionProductName();
}
