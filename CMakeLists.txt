cmake_minimum_required(VERSION 3.20)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
ADD_DEFINITIONS(-D_USRDLL)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17) # CXX17 for `filesystem`

project(Windows-Game-Patches CXX)

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/external/inipp"
)

add_subdirectory(source/ACMirage.NoTAA)
add_subdirectory(source/ACUnity.Patches)
add_subdirectory(source/APT2.Patches)
# TODO: 32 bit project
# add_subdirectory(source/BF3.Patches)
add_subdirectory(source/BFV.NoTAA)
add_subdirectory(source/BrightMemoryInfinite.NoTAA)
add_subdirectory(source/Control.Patches)
add_subdirectory(source/DeadSpace.NoTAA)
add_subdirectory(source/DeathStranding.Fix)
add_subdirectory(source/EldenRing.NoTAA)
add_subdirectory(source/GotG.NoTAA)
add_subdirectory(source/GoW.NoTAA)
add_subdirectory(source/Hitman3.NoTAA)
add_subdirectory(source/LiesOfP.NoTAA)
add_subdirectory(source/NierReplicant.Fix)
add_subdirectory(source/PlanetOfLana.NoTAA)
# TODO: Figure out linking or ditch scriptHook
# add_subdirectory(source/RDR3.Patches)
add_subdirectory(source/RE4.Sharpness)
add_subdirectory(source/RE8.NoTAA)
add_subdirectory(source/Remnant2.NoTAA)
add_subdirectory(source/SpiderMan.MilesMorales.Patches)
add_subdirectory(source/T1X.DebugFeatures)
add_subdirectory(source/Uncharted4TLL.NoTAA)
add_subdirectory(source/WoLong.Fix)
