namespace Patterns
{
	const wchar_t* DevMenu_MenuSize = L"c7 41 ?? 9a 99 19 3f 49 8b d1 c6 41 ?? 00 44 89 51 ??";
	const wchar_t* ConsoleOutput = L"0f 45 ce 88 0d ?? ?? ?? ?? 48 8d ?? ?? ?? ?? ?? e8 ?? ?? ?? ??";
	const wchar_t* m_onDisc_DevMenu = L"8a 8f ?? ?? ?? ?? 84 c9 0f 94 c2 84 c9 0f 95 c1";
	const wchar_t* GameSnprintf = L"4c 89 44 24 18 4c 89 4c 24 20 53 55 56 57 48 83 ec 38 49 8b f0 48 8d 6c 24 78 48 8b fa 48 8b d9 e8 ?? ?? ?? ?? 48 89 6c 24 28 4c 8b ce 48 83 64 24 20 00 4c 8b c7 48 8b d3 48 8b 08 48 83 c9 01 e8 ?? ?? ?? ?? 83 c9 ff 85 c0 0f 48 c1 48 83 c4 38 5f 5e 5d 5b c3";
	// Give Player Weapons
	const wchar_t* GivePlayerWeapon_Main = L"49 8b 4f 08 48 8d 54 24 30 41 b0 01 c7 44 24 30 ?? ?? ?? ?? 48 8b 0c 0e e8 ?? ?? ?? ??";
	const wchar_t* GivePlayerWeapon_SubSection = L"49 8b 4d 08 48 8d 54 24 40 41 b0 01 49 8b 0c 0c e8 ?? ?? ?? ??";
	const wchar_t* GivePlayerWeapon_Entry = L"49 8b 4e 08 48 8d 54 24 30 41 b0 01 49 8b 0c 0f e8 ?? ?? ?? ??";
	const wchar_t* Assert_LevelDef_LevelManifst = L"c7 44 24 20 f3 04 00 00 4c 8d 05";
	// thanks to infogram for offsets
	const wchar_t* Memory_isDebugMemoryAval = L"32 c0 c3 cc 48 8b 41 08 c3";
	const wchar_t* Memory_PushAllocatorJMPAddr = L"48 8d 4c 24 30 89 44 24 30 4d 8b f9 45 8b f0 48 8b ea e8 ?? ?? ?? ??";
	const wchar_t* DebugDrawStaticContext = L"e8 ?? ?? ?? ?? 33 d2 33 c9 e8 ?? ?? ?? ?? 80 b8 3d 3f 00 00 00 75 ??";
	const wchar_t* MaterialDebug_Heap = L"c7 44 24 ?? 10 00 00 00 b9 18 00 00 00 48 8d 54 24 ?? e8 ?? ?? ?? ??";
	const wchar_t* ParticlesMenu = L"40 56 48 83 ec 30 48 8b f1 33 c9 e8 ?? ?? ?? ?? 84 c0 0f 84 ?? ?? ?? ?? 48 89 5c 24 40 4c 8d 0d ?? ?? ?? ??";
	const wchar_t* UpdateSelectRegionByNameMenu_Heap = L"b9 08 40 02 00 c7 44 24 ?? 0a 00 00 00 48 8d 54 24 ??";
	const wchar_t* UpdateSelectSpawner = L"c7 44 24 ?? 0a 00 00 00 48 8d 54 24 ?? 88 5c 24 ?? b9 08 20 04 00 e8 ?? ?? ?? ??";
	const wchar_t* Memory_ValidateContext = L"cc 48 85 db 74 04 ?? ?? ?? ?? 41 8b ec 0f 1f 80 ?? ?? ?? ?? 49 8b fc 4d 85 ff 0f 84 3b ?? ?? ?? 49 8d 5f 10 8b 4b f0 85 ?? ?? ?? ??";
	const wchar_t* MenuHeap_UsableMemorySize = L"80 00 00 00 05 00 00 00 00 00 ?? 00 00 00 00 00 00 40 00 00"; // ALLOCATION_MENU_HEAP
	const wchar_t* ScriptHeap_UsableMemorySize = L"5e 00 00 00 05 00 00 00 00 00 ?? 00 00 00 00 00 00 40 00 00"; // ALLOCATION_SCRIPT_HEAP
	const wchar_t* CPUHeap_UsableMemorySize = L"03 00 00 00 02 00 00 00 00 00 ?? ?? 00 00 00 00 00 00 10 00"; // ALLOCATION_CPU_MEMORY
	// inline asserts
	// based on source lines
	const wchar_t* Assert_UpdateSelectRegionByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 8e 00 00 00";
	const wchar_t* Assert_UpdateSelectIgcByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? bb 10 00 00 00 41 b9 84 03 00 00";
	const wchar_t* Assert_UpdateSelectSpawnerByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 a3 00 00 00";
}
