# Windows-Game-Patches

[![MSBuild](https://github.com/illusion0001/Windows-Game-Patches/actions/workflows/msbuild.yml/badge.svg)](https://github.com/illusion0001/Windows-Game-Patches/actions/workflows/msbuild.yml)

[Latest Release](https://github.com/illusion0001/Windows-Game-Patches/releases/latest)

# Patches

### **Battlefield V**
  - Disable TAA
  - Custom Internal AA Options.
    - PostProcessAAMode_None: `0`
    - PostProcessAAMode_FxaaLow: `1`
    - PostProcessAAMode_FxaaMedium: `2`
    - PostProcessAAMode_FxaaHigh: `3`
    - PostProcessAAMode_FxaaCompute: `4`
    - PostProcessAAMode_FxaaComputeExtreme: `5`
    - PostProcessAAMode_Smaa1x: `6`
    - PostProcessAAMode_SmaaT2x: `7`
    - PostProcessAAMode_TemporalAA: `8`

#### Installation
- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
- ***This has not been tested online, use at your own risk!***.
  - Extract the following contents of the release zip in to the the game root folder.
  - `winmm.dll`
  - `BFV.NoTAA.asi`

### **Bright Memory: Infinite**
  - Disable TAA
  - Disable Sharpness

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the Win64 folder. (`BrightMemoryInfinite\Binaries\Win64\`).
  - `winmm.dll`
  - `BrightMemoryInfinite.NoTAA.asi`

### **Death Stranding (original and Director's Cut)**
  - Force AA mode (None, FXAA or TAA)
  - Skip savegame checks (for savegame transfer between different versions/profiles)

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the game root folder.
  - `version.dll`
  - `version.ini`
  - `DeathStranding.Fix.asi`

### **Elden Ring**
  - Force TAA off
  - Disable Chromatic Aberration (thanks to [techiew](https://github.com/techiew/EldenRingMods) for the original fix)

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the Game folder. (`ELDEN RING\Game\`).
  - `dinput8.dll`
  - `EldenRing.NoTAA.asi`

### **Lies of P Demo**
  - Disable TAA
  - Disable Depth of Field

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the Win64 folder. (`Lies of P Demo\LiesofP\Binaries\Win64\`).
  - `winmm.dll`
  - `LiesOfP.NoTAA.asi`

### **Planet of Lana**
  - Disable TAA

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the game root folder.
  - `d3d11.dll`
  - `PlanetOfLana.NoTAA.asi`
  - `d3d11.ini`

### **Remnant II**
  - Disable TAA
  - Disable Sharpness

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the Win64 folder. (`Remnant 2\Remnant2\Binaries\Win64\`).
  - `winmm.dll`
  - `Remnant2.NoTAA.asi`

### **Resident Evil 4 Remake Demo**
  - Remove Forced Sharpening

#### Installation
- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the game root folder.
  - `dinput8.dll`
  - `RE4.Sharpness.asi`

 ### **The Last of Us: Part 1**
  - Debug Menu
  - Custom Debug Menu (Work in Progress)
  - Restored Active Task Display
  - Extended Debug Menu by [infogram](https://web.archive.org/web/20230413143249/https://cs.rin.ru/forum/viewtopic.php?p=2806625)
    - `1.0.85-44a951ca` now supports game version `1.0.5.0` and newer.

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the game root folder.
  - `winmm.dll`
  - `T1X.DebugFeatures.asi`

### **Wo Long: Fallen Dynasty**
  - Remove 30FPS Limit for Cutscenes
#### Installation
- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the game root folder.
  - `winmm.dll`
  - `WoLong.Fix.asi`

### **NieR Replicant ver.1.22474487139**
  - Remove FPS Limit
#### Installation
- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip in to the the game root folder.
  - `winmm.dll`
  - `NierReplicant.Fix.asi`

# Credits
- [Lyall](https://github.com/Lyall) for Project Template.
- [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading.
- [inipp](https://github.com/mcmtroffaes/inipp) for ini reading.
- [length-disassembler](https://github.com/Nomade040/length-disassembler) for length disassembly.
