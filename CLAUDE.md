# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

DMTD (Dos Museum Turret Defense) is a retro tower defense game written in C++ for DOS systems using Borland C++ 3.1. The game runs in VGA Mode 13h (320x200, 256 colors) and uses the SED 3.05 graphics library.

## Build Commands

### Standard Build
```bash
make clean
make
```

### Testing the Game
```bash
cd BUILD
DMTD.EXE
```

### For DOSBox
```bash
dosbox DMTD.EXE
```

## Code Architecture

### Memory Management
The codebase implements memory optimization for DOS systems with limited RAM (640KB):

- **Lazy Loading**: Images are loaded on-demand per stage using `LoadStageImages(stage)`
- **Object Pools**: Static arrays with reduced limits (MAX_ENEMIES=30, MAX_TURRETS=100, MAX_BULLETS=50, MAX_EXPLOSIONS=15)
- **Memory Monitoring**: Functions in MAIN.CPP track memory usage using `coreleft()` and `farcoreleft()`

### Core Components

**Game Engine (GSTATE.H/CPP)**
- `GameManager` class handles all game states and logic
- Y-sorted sprite rendering via `Drawable` interface
- Static object pools for enemies, turrets, bullets, explosions

**Graphics System**
- SED 3.05 library for VGA graphics (`SED.H`)
- PCX image loading with `Load_Pcx()` and `Decode_Data()`
- Lazy image loading in `IMAGES.CPP` with `InitCoreImages()` and `LoadStageImages()`

**Game Objects**
- Inheritance hierarchy: `Enemy`, `Turret`, `Bullet` inherit from `Drawable`
- Stage-specific enemy types loaded dynamically
- 7 turret types representing retro computers (5.25" disk, XT, AT, 486, CD-ROM, Pentium)

### String Localization
- Korean text support via `STR_UTF8.H` → `STR_KSSM.H` conversion
- `UTF2KSSM` utility converts UTF-8 strings to KSSM format

### Balancing System
Global multipliers in `GAME.H`:
- `TURRET_FIRE_SPEED_MULTIPLIER`
- `BULLET_DAMAGE_MULTIPLIER`
- `ENEMY_HEALTH_MULTIPLIER`
- `ENEMY_POINTS_MULTIPLIER`

### Difficulty System
Three difficulty levels (DIFF_EASY, DIFF_NORMAL, DIFF_HARD) affecting damage multipliers.

## Important Notes

- Uses Large memory model (`-ml` flag) required by SED library
- PCX files in BUILD/ directory (60 files total, ~1KB each)
- Music system via `Mus_Play()`, `Mus_End()` functions
- Logging system with `writeLog()` for debugging

## Memory Optimization Features

When working on memory-related issues:
1. Use `logMemoryStatus()` to check current memory usage
2. Call `LoadStageImages(stage)` when entering new stages
3. Use `UnloadStageImages()` to free unused images
4. Monitor with `isMemoryCritical()` for low memory conditions
5. Call `freeUnusedResources()` when memory is critical

The game is optimized to run on real DOS machines and 86Box emulators with limited memory.