namespace Patterns
{
	const wchar_t* DevMenu_MenuSize = L"c7 41 ?? 9a 99 19 3f 49 8b d1 c6 41 ?? 00 44 89 51 ??";
	const wchar_t* ConsoleOutput = L"0f 45 ce 88 0d ?? ?? ?? ?? 48 8d ?? ?? ?? ?? ?? e8 ?? ?? ?? ??";
	const wchar_t* m_onDisc_DevMenu = L"8a 8f ?? ?? ?? ?? 84 c9 0f 94 c2 84 c9 0f 95 c1";
	const wchar_t* GameSnprintf = L"4c 89 44 24 18 4c 89 4c 24 20 53 55 56 57 48 83 ec 38 49 8b f0 48 8d 6c 24 78 48 8b fa 48 8b d9 e8 ?? ?? ?? ?? 48 89 6c 24 28 4c 8b ce 48 83 64 24 20 00 4c 8b c7 48 8b d3 48 8b 08 48 83 c9 01 e8 ?? ?? ?? ?? 83 c9 ff 85 c0 0f 48 c1 48 83 c4 38 5f 5e 5d 5b c3";
	const wchar_t* GameWarnScriptPrint = L"4c 89 44 24 18 4c 89 4c 24 20 c3 cc 48 89 5c 24 08 57 48 83 ec 30";
	const wchar_t* GamePrintf = L"48 8b c4 48 89 48 08 48 89 50 10 4c 89 40 18 4c 89 48 20 53 48 83 ec 20 b9 01 00 00 00 48 8d 58 10 e8 ?? ?? ?? ?? 48 8b 54 24 30 4c 8b cb 45 33 c0 48 8b c8 e8 ?? ?? ?? ?? 48 83 c4 20 5b c3";
	const wchar_t* GameWarnScriptPrint2 = L"48 89 54 24 10 4c 89 44 24 18 4c 89 4c 24 20 c3 48 89 54 24 10 4c 89 44 24 18 4c 89 4c 24 20 c3 48 89 5c 24 18 48 89 74 24 20 55 57 41 55 41 56 41 57 48 8d ac 24 60 fc ff ff";
	const wchar_t* CrashScriptTest = L"48 83 ec 28 83 fa 05 75 ?? 48 b9 f5 b8 48 3d 53 2e 17 90";
	const wchar_t* ScriptManager_LookupClass = L"40 53 48 83 ec ?? 48 8b da e8 ?? ?? ?? ?? 48 85 c0 75 ?? 48 8b c3 eb ?? 48 8b 00 48 83 c4 ?? 5b c3";
	// Dev Menu
	// TODO: Mask these!
	const wchar_t* AllocMemoryforStructure = L"48 83 ec ?? 45 33 c9 4c 8d 05 ?? ?? ?? ?? 4c 89 44 24 ?? 41 8d 51 ?? e8 ?? ?? ?? ?? 48 83 c4 38 c3";
	const wchar_t* CreateDevMenuStructure = L"40 53 48 83 ec ?? 48 8b 81 b0 00 00 00 48 8b da 48 85 c0 74 22 66 66 66 0f 1f 84 00 00 00 00 00 48 8b d0 48 8b 40 40 48 85 c0 75 f4 48 85 d2 74 06 48 89 5a 40 eb 07 48 89 99 b0 00 00 00 48 8b d3 48 c7 43 40 00 00 00 00 48 89 4b 38 e8 ?? ?? ?? ?? 48 8b c3 48 83 c4 20 5b c3";
	const wchar_t* AllocDevMenuMemoryforStructure = L"48 89 5c 24 10 48 89 6c 24 18 48 89 74 24 20 57 48 83 ec 30 48 85 c9 bf 01 00 00 00 41 8b f1 49 8b e8 48 0f 45 f9 48 8b da";
	const wchar_t* AllocDevMenu1 = L"4c 8b d9 0f b6 d2 49 b9 01 01 01 01 01 01 01 01 4c 0f af ca 49 83 f8 10 0f 86 ?? ?? ?? ??";
	const wchar_t* DevMenuCreateHeader = L"48 89 5c 24 08 57 48 83 ec 20 41 8b d8 48 8b f9 e8 ?? ?? ?? ?? 48 8d 05 ?? ?? ?? ?? 89 9f b8 00 00 00 48 8b 5c 24 30 48 89 07 33 c0 48 89 87 a0 00 00 00 89 87 a8 00 00 00 48 89 87 b0 00 00 00 88 87 bc 00 00 00 48 89 87 c0 00 00 00 48 89 87 c8 00 00 00 48 89 87 d0 00 00 00 89 87 d8 00 00 00 48 8b c7 c6 87 ac 00 00 00 01 48 83 c4 20 5f c3";
	const wchar_t* DevMenuCreateEntry = L"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 49 8b d9 49 8b f0 48 8b f9 e8 ?? ?? ?? ?? 48 8b 44 24 50 33 c9 48 89 47 60 48 8b 44 24 58";
	const wchar_t* DevMenuAddBool = L"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 49 8b d9 49 8b f8 48 8b f1 e8 ?? ?? ?? ?? 33 c9 48 89 9e 80 00 00 00 48 8b 5c 24 30 48 8d 05 ?? ?? ?? ?? 48 89 06 48 8d 05 ?? ?? ?? ?? 48 89 86 b0 00 00 00 48 89 8e a0 00 00 00 48 89 4e 60 c7 86 a8 00 00 00 03 00 00 00 0f b6 07 88 86 b8 00 00 00 48 8b c6 48 89 7e 60 48 89 4e 58 c7 86 a0 00 00 00 01 00 00 00 48 8b 74 24 38 48 83 c4 20 5f c3";
	const wchar_t* MeleeMenuHook = L"48 89 5c 24 10 56 48 83 ec 30 48 8b f1 e8 ?? ?? ?? ?? 48 8b d8 48 85 c0 74 ?? b9 c8 00 00 00";
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
	const wchar_t* Memory_ValidateContext = L"cc 48 85 db 0f 84 ?? ?? ?? ?? 33 c9 ff d3 90 e9 ?? ?? ?? ?? 48 8b 43 08 49 39 47 08 75 ?? 48 8b 43 10 49 39 47 10 0f 84 ?? ?? ?? ??";
	const wchar_t* MenuHeap_UsableMemorySize = L"80 00 00 00 05 00 00 00 00 00 ?? 00 00 00 00 00 00 40 00 00"; // ALLOCATION_MENU_HEAP
	const wchar_t* ScriptHeap_UsableMemorySize = L"5e 00 00 00 05 00 00 00 00 00 ?? 00 00 00 00 00 00 40 00 00"; // ALLOCATION_SCRIPT_HEAP
	const wchar_t* CPUHeap_UsableMemorySize = L"03 00 00 00 02 00 00 00 00 00 ?? ?? 00 00 00 00 00 00 10 00"; // ALLOCATION_CPU_MEMORY
	// inline asserts
	// based on source lines
	const wchar_t* Assert_UpdateSelectRegionByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 8e 00 00 00";
	const wchar_t* Assert_UpdateSelectIgcByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? bb 10 00 00 00 41 b9 84 03 00 00";
	const wchar_t* Assert_UpdateSelectSpawnerByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 a3 00 00 00";
}
