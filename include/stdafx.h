#pragma once
#pragma comment(lib,"Version.lib")
#pragma comment(lib, "shlwapi.lib")
#define WIN32_LEAN_AND_MEAN

#include <cassert>
#include <cstdint>
#pragma GCC diagnostic ignored "-Wdelete-non-abstract-non-virtual-dtor"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <inttypes.h>
#include <sstream>
#include "inipp/inipp/inipp.h"
#pragma GCC diagnostic warning "-Wdelete-non-abstract-non-virtual-dtor"
#include <SDKDDKVer.h>
#include <Windows.h>
#include <shlwapi.h>
#include <stdio.h>
