namespace Patterns
{
	const wchar_t* DevMenu_MenuSize = L"c7 41 ?? 9a 99 19 3f 49 8b d1 c6 41 ?? 00 44 89 51 ??";
	const wchar_t* m_onDisc_DevMenu = L"8a 8f ?? ?? ?? ?? 84 c9 0f 94 c2 84 c9 0f 95 c1";
	const wchar_t* GameWarnScriptPrint = L"4c 89 44 24 18 4c 89 4c 24 20 c3 cc 48 89 5c 24 08 57 48 83 ec 30";
	const wchar_t* GameWarnScriptPrint2 = L"48 89 54 24 10 4c 89 44 24 18 4c 89 4c 24 20 c3 48 89 54 24 10 4c 89 44 24 18 4c 89 4c 24 20 c3 48 89 5c 24 18 48 89 74 24 20 55 57 41 55 41 56 41 57 48 8d ac 24 60 fc ff ff";
	const wchar_t* CrashScriptTest = L"48 83 ec 28 83 fa 05 75 ?? 48 b9 f5 b8 48 3d 53 2e 17 90";
	const wchar_t* ScriptManager_LookupClass = L"40 53 48 83 ec ?? 48 8b da e8 ?? ?? ?? ?? 48 85 c0 75 ?? 48 8b c3 eb ?? 48 8b 00 48 83 c4 ?? 5b c3";
	const wchar_t* Int3_14bytes = L"cc cc cc cc cc cc cc cc cc cc cc cc cc cc";
	const wchar_t* PlayerPtr = L"48 8b 54 24 30 48 8d 4c 24 50 8a 44 24 38 45 33 c9 44 8b c6 48 89 53 20 88 83 98 00 00 00";
	const wchar_t* DoutMemPrint = L"48 89 5c 24 08 48 89 6c 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 ec 30 48 8b 01 4c 8b e2 48 8b d9 ff 50 48 84 c0 0f 84 d5 01 00 00 48 83 c8 ff";
	const wchar_t* DoutPrintf = L"48 8b c4 48 89 48 08 48 89 50 10 4c 89 40 18 4c 89 48 20 53 57 48 83 ec 28";
	const wchar_t* EntitySpawner = L"45 33 e4 45 8a f9 49 8b f0 4c 8b ea 48 8b d9 48 85 ff";
	const wchar_t* LoadLevelByName = L"40 53 48 83 ec 50 48 8b da 4c 8b c1 ba 01 00 00 00 48 8d 4c 24 20";
	const wchar_t* LoadActorByName = L"48 89 5c 24 08 48 89 6c 24 10 48 89 74 24 18 57 41 56 41 57 48 83 ec 50 41 bf 02 00 00 00";
	const wchar_t* ReadCurrentLookID = L"48 8b 81 e0 1c 00 00 48 89 02 48 8b c2 c3";
	const wchar_t* DebugOutputString1 = L"48 8b c4 48 89 48 08 48 89 50 10 4c 89 40 18 4c 89 48 20 48 83 ec 28 48 8d 50 10";
	const wchar_t* ActiveTaskDisplay = L"48 8d 4d a0 48 8d 85 c0 03 00 00 4d 8b c6 48 89 44 24 28 48 89 74 24 20";
	const wchar_t* CharPrintText = L"48 8b 8b f8 2f 00 00 48 8d 55 d7 48 8b 01 ff 90 e8 01 00 00 48 8d 4d d7";
	const wchar_t* TextPrintV = L"4c 8b dc 48 83 ec 58 c5 fa 10 84 24 80 00 00 00 49 8d 43 40 49 89 43 f0 49 8b 43 38 49 89 43 e8 83 64 24 38 00 83 64 24 30 00 8b 84 24 88 00 00 00 89 44 24 28 c5 fa 11 44 24 20";
	const wchar_t* RestartCheckpointInternal = L"48 8b c4 48 89 58 08 48 89 70 10 48 89 78 18 4c 89 60 20 55 41 56 41 57 48 8d 68 a1 48 81 ec b0 00 00 00 48 8b 05 ?? ?? ?? ?? 48 33 c4 48 89 45 37 44 8a f1 40 8a fa 48 8d 0d ?? ?? ?? ?? e8 ?? ?? ?? ??";
	// Dev Menu
	const wchar_t* AllocMemoryforStructure = L"48 83 ec ?? 45 33 c9 4c 8d 05 ?? ?? ?? ?? 4c 89 44 24 ?? 41 8d 51 ?? e8 ?? ?? ?? ?? 48 83 c4 38 c3";
	const wchar_t* CreateDevMenuStructure = L"40 53 48 83 ec ?? 48 8b 81 b0 00 00 00 48 8b da 48 85 c0 74 22 66 66 66 0f 1f 84 00 00 00 00 00 48 8b d0 48 8b 40 40 48 85 c0 75 f4 48 85 d2 74 06 48 89 5a 40 eb 07 48 89 99 b0 00 00 00 48 8b d3 48 c7 43 40 00 00 00 00 48 89 4b 38 e8 ?? ?? ?? ?? 48 8b c3 48 83 c4 20 5b c3";
	const wchar_t* AllocDevMenuMemoryforStructure = L"48 89 5c 24 10 48 89 6c 24 18 48 89 74 24 20 57 48 83 ec 30 48 85 c9 bf 01 00 00 00 41 8b f1 49 8b e8 48 0f 45 f9 48 8b da";
	const wchar_t* DevMenuCreateHeader = L"48 89 5c 24 08 57 48 83 ec 20 41 8b d8 48 8b f9 e8 ?? ?? ?? ?? 48 8d 05 ?? ?? ?? ?? 89 9f b8 00 00 00 48 8b 5c 24 30 48 89 07 33 c0 48 89 87 a0 00 00 00 89 87 a8 00 00 00 48 89 87 b0 00 00 00 88 87 bc 00 00 00 48 89 87 c0 00 00 00 48 89 87 c8 00 00 00 48 89 87 d0 00 00 00 89 87 d8 00 00 00 48 8b c7 c6 87 ac 00 00 00 01 48 83 c4 20 5f c3";
	const wchar_t* DevMenuCreateEntry = L"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 49 8b d9 49 8b f0 48 8b f9 e8 ?? ?? ?? ?? 48 8b 44 24 50 33 c9 48 89 47 60 48 8b 44 24 58";
	const wchar_t* DevMenuAddBool = L"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 49 8b d9 49 8b f8 48 8b f1 e8 ?? ?? ?? ?? 33 c9 48 89 9e 80 00 00 00 48 8b 5c 24 30 48 8d 05 ?? ?? ?? ?? 48 89 06 48 8d 05 ?? ?? ?? ?? 48 89 86 b0 00 00 00 48 89 8e a0 00 00 00 48 89 4e 60 c7 86 a8 00 00 00 03 00 00 00 0f b6 07 88 86 b8 00 00 00 48 8b c6 48 89 7e 60 48 89 4e 58 c7 86 a0 00 00 00 01 00 00 00 48 8b 74 24 38 48 83 c4 20 5f c3";
	const wchar_t* DevMenuAddFuncButton = L"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 49 8b d9 49 8b f8 48 8b f1 e8 ?? ?? ?? ?? 33 c9 48 89 5e 60 48 8b 5c 24 30 48 8d 05 ?? ?? ?? ?? 48 89 06 0f b6 44 24 50 88 86 c1 00 00 00 48 8b c6 48 89 8e a0 00 00 00 c7 86 a8 00 00 00 05 00 00 00";
	const wchar_t* DevMenuAddIntSlider = L"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 49 8b f9 49 8b d8 48 8b f1 e8 ?? ?? ?? ?? 48 8b 44 24 58 33 c9 66 c7 86 b0 00 00 00 00 07";
	const wchar_t* DevMenuCreateSeparationLine = L"33 c0 c7 41 68 c8 c8 c8 ff 48 89 41 30 89 41 4e 48 89 41 38 48 89 41 40 89 41 48";
	const wchar_t* DevMenuCreateCyanSubText = L"48 89 5c 24 08 57 48 83 ec 20 48 8b da 48 8b f9 e8 ?? ?? ?? ?? 33 c0 48 89 9f b0 00 00 00 48 8b 5c 24 30 48 89 87 a0 00 00 00 48 89 47 60 48 89 87 80 00 00 00";
	const wchar_t* MeleeMenuHook = L"48 89 5c 24 10 56 48 83 ec 30 48 8b f1 e8 ?? ?? ?? ?? 48 8b d8 48 85 c0 74 ?? b9 c8 00 00 00";
	const wchar_t* InitProfileMenu = L"40 55 57 48 8d 6c 24 b1 48 81 ec a8 00 00 00 48 8b f9 33 c9 e8 ?? ?? ?? ?? 84 c0 0f 84 ?? ?? ?? ?? 48 89 9c 24 c0 00 00 00 4c 8d 0d ?? ?? ?? ?? 48 89 b4 24 d8 00 00 00 48 8d 15 ?? ?? ?? ??";
	const wchar_t* DevMenuCreateSelection = L"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 49 8b d9 49 8b f8 48 8b f1 e8 ?? ?? ?? ?? 8b 44 24 68 33 c9 89 86 a0 00 00 00 48 8b 44 24 70 48 89 86 80 00 00 00";
	const wchar_t* DevMenuSelectionCallback = L"48 89 5c 24 08 57 48 83 ec 20 48 8b 59 60 83 ea 05 74 ?? 83 ea 02 74 ?? 83 fa 01 75 ?? 48 8b 0d ?? ?? ?? ?? 8b 3b 48 85 c9";
	// Give Player Weapons
	const wchar_t* GivePlayerWeapon_Main = L"49 8b 4f 08 48 8d 54 24 30 41 b0 01 c7 44 24 30 ?? ?? ?? ?? 48 8b 0c 0e e8 ?? ?? ?? ??";
	const wchar_t* GivePlayerWeapon_SubSection = L"49 8b 4d 08 48 8d 54 24 40 41 b0 01 49 8b 0c 0c e8 ?? ?? ?? ??";
	const wchar_t* GivePlayerWeapon_Entry = L"49 8b 4e 08 48 8d 54 24 30 41 b0 01 49 8b 0c 0f e8 ?? ?? ?? ??";
	// New for 1.0.5.0
	// They ifdef the code to format SID to char*
	// Now its compiled with inlined `return "#????????????????";`
	const wchar_t* GivePlayerWeapon_Main1050 = L"48 8d 15 ?? ?? ?? ?? 40 88 7c 24 20 4c 8b 0c 0e 49 8b cd e8 ?? ?? ??";
	const wchar_t* GivePlayerWeapon_SubSection1050 = L"48 8d 15 ?? ?? ?? ?? 40 88 7c 24 20 4e 8b 0c 29 48 8b 4c 24 58";
	const wchar_t* GivePlayerWeapon_Entry1050 = L"4c 8d 05 ?? ?? ?? ?? 48 8d 15 ?? ?? ?? ?? 48 8d 4d 90 e8 ?? ?? ?? ??";
	const wchar_t* GivePlayerWeapon_EntryHeader1050 = L"48 8d 15 ?? ?? ?? ?? 49 8b cf e8 ?? ?? ?? ?? 4c 8b e8";
	//
	const wchar_t* Assert_LevelDef_LevelManifst = L"c7 44 24 20 f3 04 00 00 4c 8d 05";
	// thanks to infogram for patches
	const wchar_t* Memory_PushAllocator = L"48 8d 4c 24 30 89 44 24 30 4d 8b f9 45 8b f0 48 8b ea e8 ?? ?? ?? ??";
	const wchar_t* Memory_NewHandler = L"8b 02 48 8b e9 49 8d 4b 20 89 44 24 58 49 8b d8 e8 ?? ?? ?? ??";
	const wchar_t* DebugDrawStaticContext = L"e8 ?? ?? ?? ?? 33 d2 33 c9 e8 ?? ?? ?? ?? 80 b8 3d 3f 00 00 00 75 ??";
	const wchar_t* ParticlesMenu = L"40 56 48 83 ec 30 48 8b f1 33 c9 e8 ?? ?? ?? ?? 84 c0 0f 84 ?? ?? ?? ?? 48 89 5c 24 40 4c 8d 0d ?? ?? ?? ??";
	const wchar_t* Memory_ValidateContext = L"c7 44 24 20 27 00 00 00 48 8d 15";
	const wchar_t* MenuHeap_UsableMemorySize = L"5e 00 00 00 05 00 00 00 00 00 ?? 00 00 00 00 00 00 40 00 00"; // ALLOCATION_MENU_HEAP
	const wchar_t* ScriptHeap_UsableMemorySize = L"80 00 00 00 05 00 00 00 00 00 ?? 00 00 00 00 00 00 40 00 00"; // ALLOCATION_SCRIPT_HEAP
	const wchar_t* CPUHeap_UsableMemorySize = L"03 00 00 00 02 00 00 00 00 00 ?? ?? 00 00 00 00 00 00 10 00"; // ALLOCATION_CPU_MEMORY
	// inline asserts
	// based on source lines
	const wchar_t* Assert_UpdateSelectRegionByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 8e 00 00 00";
	const wchar_t* Assert_UpdateSelectIgcByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? bb 10 00 00 00 41 b9 84 03 00 00";
	const wchar_t* Assert_UpdateSelectSpawnerByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 a3 00 00 00";
}
