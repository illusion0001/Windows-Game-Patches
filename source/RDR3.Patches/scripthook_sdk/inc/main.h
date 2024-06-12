/*
    THIS FILE IS A PART OF RDR 2 SCRIPT HOOK SDK
                http://dev-c.com
            (C) Alexander Blade 2019
*/

#pragma once

#include <windows.h>

#define IMPORT __declspec(dllimport)

/* keyboard */

// DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow
typedef void(*KeyboardHandler)(DWORD, WORD, BYTE, BOOL, BOOL, BOOL, BOOL);

// Register keyboard handler
// must be called on dll attach
IMPORT void keyboardHandlerRegister(KeyboardHandler handler);

// Unregister keyboard handler
// must be called on dll detach
IMPORT void keyboardHandlerUnregister(KeyboardHandler handler);

/* scripts */

IMPORT void scriptWait(DWORD time);
IMPORT void scriptRegister(HMODULE module, void(*LP_SCRIPT_MAIN)());
IMPORT void scriptRegisterAdditionalThread(HMODULE module, void(*LP_SCRIPT_MAIN)());
IMPORT void scriptUnregister(HMODULE module);
IMPORT void scriptUnregister(void(*LP_SCRIPT_MAIN)()); // deprecated

IMPORT void nativeInit(UINT64 hash);
IMPORT void nativePush64(UINT64 val);
IMPORT PUINT64 nativeCall();

static void WAIT(DWORD time) { scriptWait(time); }
static void TERMINATE() { WAIT(MAXDWORD); }

// Returns pointer to global variable
// make sure that you check game version before accessing globals because
// ids may differ between patches
IMPORT UINT64 *getGlobalPtr(int globalId);

/* world */

// Get entities from internal pools
// return value represents filled array elements count
// can be called only in the same thread as natives
IMPORT int worldGetAllVehicles(int *arr, int arrSize);
IMPORT int worldGetAllPeds(int *arr, int arrSize);
IMPORT int worldGetAllObjects(int *arr, int arrSize);
IMPORT int worldGetAllPickups(int *arr, int arrSize);

/* misc */

// Returns base object pointer using it's script handle
// make sure that you check game version before accessing object fields because
// offsets may differ between patches
IMPORT BYTE *getScriptHandleBaseAddress(int handle);

enum eGameVersion : int
{
    VER_AUTO,
    VER_1_0_1207_60_RGS,
    VER_1_0_1207_69_RGS,

    VER_SIZE,
    VER_UNK = -1
};

IMPORT eGameVersion getGameVersion();
