#ifndef IMAGES_H
#define IMAGES_H

#include "sed.h"

#define MAX_BULLET_TYPES 7
#define MAX_TURRET_TYPES 7
#define MAX_ENEMY_STAGES 20
#define MAX_EXPLOSION_FRAMES 5
#define MAX_DIALOG_PATCHES 9

// Global image management functions
void InitGameImages();
void FreeGameImages();

// Image getter functions
sprites* GetBulletImage(int type);  // type: 1-7 for BULLET1.PCX~BULLET7.PCX
sprites* GetTurretImage(int type);  // type: TURRET_525, TURRET_XT, etc.
sprites* GetEnemyImage(int stage);  // stage: 0-19 for enemy stages
sprites* GetExplosionImage(int frame);  // frame: 0-4 for EXPLO1.PCX~EXPLO5.PCX
sprites* GetCoinImage();  // COIN.PCX for price display
sprites* GetDialogImage(int patch);  // patch: 0-8 for DIALOG0.PCX~DIALOG8.PCX
sprites* GetLightImage();  // LIGHT.PCX for lit timer boxes
sprites* GetDarkImage();   // DARK.PCX for unlit timer boxes
sprites* GetDosMuseumImage(); // DOSMUS.PCX for menu background
sprites* GetTurretCursorImage(); // TCURSOR.PCX for turret selection cursor
sprites* GetMapCursorImage(); // MCURSOR.PCX for placeable map cursor
sprites* GetMapCursor2Image(); // MCURSOR2.PCX for non-placeable map cursor
sprites* GetTurretSelectImage(); // TURSEL.PCX for turret selection window

// Turret image type definitions
#define TURRET_525    0
#define TURRET_XT     1
#define TURRET_AT     2
#define TURRET_35     3
#define TURRET_486    4
#define TURRET_CD     5
#define TURRET_PENTIUM 6

#endif