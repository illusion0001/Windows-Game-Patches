# Windows-Game-Patches

[![MSBuild](https://github.com/illusion0001/Windows-Game-Patches/actions/workflows/msbuild.yml/badge.svg)](https://github.com/illusion0001/Windows-Game-Patches/actions/workflows/msbuild.yml)

[Latest Release](https://github.com/illusion0001/Windows-Game-Patches/releases/latest)

# Patches

<details><summary>
  
  ### Assassin's Creed Mirage
</summary>
  
  - Disable TAA
  - Disable CA

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the Win64 folder.
  - `winmm.dll`
  - `ACMirage.NoTAA.asi`
</details>
<details><summary>
  
  ### Battlefield V
</summary>

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
  - Extract the following contents of the release zip into the game root folder.
  - `winmm.dll`
  - `BFV.NoTAA.asi`
</details>
<details><summary>
  
  ### Bright Memory: Infinite
</summary>

  - Disable TAA
  - Disable Sharpness

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the Win64 folder. (`BrightMemoryInfinite\Binaries\Win64\`).
  - `winmm.dll`
  - `BrightMemoryInfinite.NoTAA.asi`
</details>
<details><summary>
  
### Death Stranding (original and Director's Cut)
</summary>

  - Force AA mode (None, FXAA or TAA)
  - Skip savegame checks (for savegame transfer between different versions/profiles)

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the game root folder.
  - `version.dll`
  - `version.ini`
  - `DeathStranding.Fix.asi`
</details>
<details><summary>
  
### Elden Ring
</summary>

  - Force TAA off
  - Disable Chromatic Aberration (thanks to [techiew](https://github.com/techiew/EldenRingMods) for the original fix)

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the game folder. (`ELDEN RING\Game\`).
  - `dinput8.dll`
  - `EldenRing.NoTAA.asi`
</details>
<details><summary>
  
### HITMAN 3
</summary>

  - Disable TAA
  - Disable Sharpness

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the game folder. (`HITMAN3\Retail\`).
  - `dinput8.dll`
  - `Hitman3.NoTAA.asi`
</details>
<details><summary>

### Lies of P Demo
</summary>

  - Disable TAA
  - Disable Depth of Field

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the Win64 folder. (`Lies of P Demo\LiesofP\Binaries\Win64\`).
  - `winmm.dll`
  - `LiesOfP.NoTAA.asi`
</details>
<details><summary>
  
### NieR Replicant ver.1.22474487139
</summary>

  - Remove FPS Limit
#### Installation
- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the game root folder.
  - `winmm.dll`
  - `NierReplicant.Fix.asi`
</details>
<details><summary>
  
### Planet of Lana
</summary>

  - Disable TAA

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the game root folder.
  - `d3d11.dll`
  - `PlanetOfLana.NoTAA.asi`
  - `d3d11.ini`
</details>
<details><summary>
  
### Remnant II
</summary>

  - Disable TAA
  - Disable Sharpness

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the Win64 folder. (`Remnant 2\Remnant2\Binaries\Win64\`).
  - `winmm.dll`
  - `Remnant2.NoTAA.asi`
</details>
<details><summary>
  
### Resident Evil 4 Remake
</summary>

  - Remove Forced Sharpening

#### Installation
- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Download [REFramework](https://github.com/praydog/REFramework) and extract `dinput8.dll` into the game root folder.
  - Extract `RE4.Sharpness.asi` from the release zip in the folder `RESIDENT EVIL 4  BIOHAZARD RE4\reframework\plugins` and rename it into `RE4.Sharpness.dll`.
</details>
<details><summary>
  
### Uncharted 4 & The Lost Legacy
</summary>

  - Disable TAA
  - Disable Sharpness
  - Disable Barrel Distortion
  - Disable Screen Zoom
  - Disable Chromatic Aberration
  - Disable Depth of Field
  - Disable Vignette

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the game root folder.
  - `winmm.dll`
  - `Uncharted4TLL.NoTAA.asi`
</details>
<details><summary>
  
### The Last of Us: Part 1
</summary>

  - Debug Menu
  - Custom Debug Menu (Work in Progress)
  - Restored Active Task Display
  - Extended Debug Menu by [infogram](https://web.archive.org/web/20230413143249/https://cs.rin.ru/forum/viewtopic.php?p=2806625)
    - `1.0.85-44a951ca` now supports game version `1.0.5.0` and newer.

#### Installation

- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the game root folder.
  - `winmm.dll`
  - `T1X.DebugFeatures.asi`
</details>
<details><summary>
  
### Wo Long: Fallen Dynasty
</summary>

  - Remove 30FPS Limit for Cutscenes
#### Installation
- **Note:** ***Please make sure any executable hex edits are removed/reverted first***.
  - Extract the following contents of the release zip into the game root folder.
  - `winmm.dll`
  - `WoLong.Fix.asi`
</details>
  
# Credits
- [Lyall](https://github.com/Lyall) for Project Template.
- [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading.
- [inipp](https://github.com/mcmtroffaes/inipp) for ini reading.
- [length-disassembler](https://github.com/Nomade040/length-disassembler) for length disassembly.
- [tl431](https://github.com/TL431/) for game fixes.
