#include <stdio.h>
#include "IMAGES.H"

// Global image arrays
static sprites bullet_images[MAX_BULLET_TYPES];
static sprites turret_images[MAX_TURRET_TYPES];
static sprites enemy_images[MAX_ENEMY_STAGES];
static sprites explosion_images[MAX_EXPLOSION_FRAMES];
static sprites coin_image;
static sprites dialog_images[MAX_DIALOG_PATCHES];
static sprites light_image;
static sprites dark_image;
static sprites dosmuseum_image;
static sprites turret_cursor_image;
static sprites map_cursor_image;
static sprites map_cursor2_image;
static sprites turret_select_image;

// Initialization flags
static int images_initialized = 0;

void InitGameImages() {
    int i;

    if (images_initialized) return;

    // Initialize bullet images
    for (i = 0; i < MAX_BULLET_TYPES; i++) {
        bullet_images[i].image = NULL;
    }

    // Load bullet images BULLET1.PCX ~ BULLET7.PCX
    Load_Pcx("BULLET1.PCX", &bullet_images[0]);
    Load_Pcx("BULLET2.PCX", &bullet_images[1]);
    Load_Pcx("BULLET3.PCX", &bullet_images[2]);
    Load_Pcx("BULLET4.PCX", &bullet_images[3]);
    Load_Pcx("BULLET5.PCX", &bullet_images[4]);
    Load_Pcx("BULLET6.PCX", &bullet_images[5]);
    Load_Pcx("BULLET7.PCX", &bullet_images[6]);

    // Decode bullet images for faster access
    for (i = 0; i < MAX_BULLET_TYPES; i++) {
        if (bullet_images[i].image) {
            Decode_Data(&bullet_images[i]);
        }
    }

    // Initialize turret images
    for (i = 0; i < MAX_TURRET_TYPES; i++) {
        turret_images[i].image = NULL;
    }

    // Load turret images
    Load_Pcx("525.PCX", &turret_images[TURRET_525]);
    Load_Pcx("XT.PCX", &turret_images[TURRET_XT]);
    Load_Pcx("AT.PCX", &turret_images[TURRET_AT]);
    Load_Pcx("35.PCX", &turret_images[TURRET_35]);
    Load_Pcx("486.PCX", &turret_images[TURRET_486]);
    Load_Pcx("CD.PCX", &turret_images[TURRET_CD]);
    Load_Pcx("PENTIUM.PCX", &turret_images[TURRET_PENTIUM]);

    // Decode turret images
    for (i = 0; i < MAX_TURRET_TYPES; i++) {
        if (turret_images[i].image) {
            Decode_Data(&turret_images[i]);
        }
    }

    // Initialize enemy images
    for (i = 0; i < MAX_ENEMY_STAGES; i++) {
        enemy_images[i].image = NULL;
    }

    // Load enemy images for different stages
    Load_Pcx("HANULSO.PCX", &enemy_images[0]);    // Stage 1 - 하늘소
    Load_Pcx("PCKID.PCX", &enemy_images[1]);      // Stage 2 - 피시키드
    Load_Pcx("DOSNAVI.PCX", &enemy_images[2]);    // Stage 3 - dosnavi
    Load_Pcx("MOONLORD.PCX", &enemy_images[3]);   // Stage 4 - moonlord
    Load_Pcx("KINOPIO.PCX", &enemy_images[4]);    // Stage 5 - 키노피오
    Load_Pcx("JEOPGONG.PCX", &enemy_images[5]);   // Stage 6 - 접공
    Load_Pcx("DARWIN.PCX", &enemy_images[6]);     // Stage 7 - 다윈의불독
    Load_Pcx("WINKLE.PCX", &enemy_images[7]);     // Stage 8 - Greatpsycho
    Load_Pcx("BOKRYONG.PCX", &enemy_images[8]);   // Stage 9 - 복룡
    Load_Pcx("JOBS.PCX", &enemy_images[9]);       // Stage 10 - 서티브잡스
    Load_Pcx("GRANDPA.PCX", &enemy_images[10]);   // Stage 11 - 실직양파
    Load_Pcx("CONQUOR.PCX", &enemy_images[11]);   // Stage 12 - 컨커러
    Load_Pcx("VOLCANO.PCX", &enemy_images[12]);   // Stage 13 - 화산폭발
    Load_Pcx("OSPAGGI.PCX", &enemy_images[13]);    // Stage 14 - Ospaggi
    Load_Pcx("OLDPC.PCX", &enemy_images[14]);     // Stage 15 - oldpc
    Load_Pcx("SHUKA.PCX", &enemy_images[15]);     // Stage 16 - 셔카
    Load_Pcx("PGONE.PCX", &enemy_images[16]);     // Stage 17 - pgone
    Load_Pcx("HACHYA.PCX", &enemy_images[17]);    // Stage 18 - hachya
    Load_Pcx("MACDUNG.PCX", &enemy_images[18]);   // Stage 19 - 최강맥둥이
    Load_Pcx("CHO.PCX", &enemy_images[19]);       // Stage 20 - 초귀요미아빠

    // Decode enemy images for faster access
    for (i = 0; i < 20; i++) { // Now all 20 stages have images
        if (enemy_images[i].image) {
            Decode_Data(&enemy_images[i]);
        }
    }

    // Initialize explosion images
    for (i = 0; i < MAX_EXPLOSION_FRAMES; i++) {
        explosion_images[i].image = NULL;
    }

    // Load explosion images EXPLO1.PCX ~ EXPLO5.PCX
    Load_Pcx("EXPLO1.PCX", &explosion_images[0]);
    Load_Pcx("EXPLO2.PCX", &explosion_images[1]);
    Load_Pcx("EXPLO3.PCX", &explosion_images[2]);
    Load_Pcx("EXPLO4.PCX", &explosion_images[3]);
    Load_Pcx("EXPLO5.PCX", &explosion_images[4]);

    // Decode explosion images for faster access
    for (i = 0; i < MAX_EXPLOSION_FRAMES; i++) {
        if (explosion_images[i].image) {
            Decode_Data(&explosion_images[i]);
        }
    }

    // Load coin image
    coin_image.image = NULL;
    Load_Pcx("COIN.PCX", &coin_image);
    if (coin_image.image) {
        Decode_Data(&coin_image);
    }

    // Initialize dialog images
    for (i = 0; i < MAX_DIALOG_PATCHES; i++) {
        dialog_images[i].image = NULL;
    }

    // Load dialog images DIALOG0.PCX ~ DIALOG8.PCX
    Load_Pcx("DIALOG0.PCX", &dialog_images[0]);
    Load_Pcx("DIALOG1.PCX", &dialog_images[1]);
    Load_Pcx("DIALOG2.PCX", &dialog_images[2]);
    Load_Pcx("DIALOG3.PCX", &dialog_images[3]);
    Load_Pcx("DIALOG4.PCX", &dialog_images[4]);
    Load_Pcx("DIALOG5.PCX", &dialog_images[5]);
    Load_Pcx("DIALOG6.PCX", &dialog_images[6]);
    Load_Pcx("DIALOG7.PCX", &dialog_images[7]);
    Load_Pcx("DIALOG8.PCX", &dialog_images[8]);

    // Decode dialog images for faster access
    for (i = 0; i < MAX_DIALOG_PATCHES; i++) {
        if (dialog_images[i].image) {
            Decode_Data(&dialog_images[i]);
        }
    }

    // Load light and dark images
    light_image.image = NULL;
    dark_image.image = NULL;
    Load_Pcx("LIGHT.PCX", &light_image);
    Load_Pcx("DARK.PCX", &dark_image);

    // Decode light and dark images
    if (light_image.image) {
        Decode_Data(&light_image);
    }
    if (dark_image.image) {
        Decode_Data(&dark_image);
    }

    // Load DOS Museum background image
    dosmuseum_image.image = NULL;
    Load_Pcx("DOSMUS.PCX", &dosmuseum_image);
    if (dosmuseum_image.image) {
        Decode_Data(&dosmuseum_image);
    }

    // Load cursor images
    turret_cursor_image.image = NULL;
    map_cursor_image.image = NULL;
    map_cursor2_image.image = NULL;
    Load_Pcx("TCURSOR.PCX", &turret_cursor_image);
    Load_Pcx("MCURSOR.PCX", &map_cursor_image);
    Load_Pcx("MCURSOR2.PCX", &map_cursor2_image);

    // Decode cursor images
    if (turret_cursor_image.image) {
        Decode_Data(&turret_cursor_image);
    }
    if (map_cursor_image.image) {
        Decode_Data(&map_cursor_image);
    }
    if (map_cursor2_image.image) {
        Decode_Data(&map_cursor2_image);
    }

    // Load turret selection window image
    turret_select_image.image = NULL;
    Load_Pcx("TURSEL.PCX", &turret_select_image);
    if (turret_select_image.image) {
        Decode_Data(&turret_select_image);
    }

    images_initialized = 1;
}

void FreeGameImages() {
    int i;

    if (!images_initialized) return;

    // Free bullet images
    for (i = 0; i < MAX_BULLET_TYPES; i++) {
        if (bullet_images[i].image) {
            Free(&bullet_images[i]);
        }
    }

    // Free turret images
    for (i = 0; i < MAX_TURRET_TYPES; i++) {
        if (turret_images[i].image) {
            Free(&turret_images[i]);
        }
    }

    // Free enemy images
    for (i = 0; i < MAX_ENEMY_STAGES; i++) {
        if (enemy_images[i].image) {
            Free(&enemy_images[i]);
        }
    }

    // Free explosion images
    for (i = 0; i < MAX_EXPLOSION_FRAMES; i++) {
        if (explosion_images[i].image) {
            Free(&explosion_images[i]);
        }
    }

    // Free coin image
    if (coin_image.image) {
        Free(&coin_image);
    }

    // Free dialog images
    for (i = 0; i < MAX_DIALOG_PATCHES; i++) {
        if (dialog_images[i].image) {
            Free(&dialog_images[i]);
        }
    }

    // Free light and dark images
    if (light_image.image) {
        Free(&light_image);
    }
    if (dark_image.image) {
        Free(&dark_image);
    }
    if (dosmuseum_image.image) {
        Free(&dosmuseum_image);
    }

    // Free cursor images
    if (turret_cursor_image.image) {
        Free(&turret_cursor_image);
    }
    if (map_cursor_image.image) {
        Free(&map_cursor_image);
    }
    if (map_cursor2_image.image) {
        Free(&map_cursor2_image);
    }

    // Free turret selection window image
    if (turret_select_image.image) {
        Free(&turret_select_image);
    }

    images_initialized = 0;
}

sprites* GetBulletImage(int type) {
    if (!images_initialized) return NULL;
    if (type < 1 || type > MAX_BULLET_TYPES) return NULL;

    return &bullet_images[type - 1];  // Convert 1-based to 0-based index
}

sprites* GetTurretImage(int type) {
    if (!images_initialized) return NULL;
    if (type < 0 || type >= MAX_TURRET_TYPES) return NULL;

    return &turret_images[type];
}

sprites* GetEnemyImage(int stage) {
    if (!images_initialized) return NULL;
    if (stage < 0 || stage >= MAX_ENEMY_STAGES) return NULL;

    return &enemy_images[stage];
}

sprites* GetExplosionImage(int frame) {
    if (!images_initialized) return NULL;
    if (frame < 0 || frame >= MAX_EXPLOSION_FRAMES) return NULL;

    return &explosion_images[frame];
}

sprites* GetCoinImage() {
    if (!images_initialized) return NULL;
    return &coin_image;
}

sprites* GetDialogImage(int patch) {
    if (!images_initialized) return NULL;
    if (patch < 0 || patch >= MAX_DIALOG_PATCHES) return NULL;

    return &dialog_images[patch];
}

sprites* GetLightImage() {
    if (!images_initialized) return NULL;
    return &light_image;
}

sprites* GetDarkImage() {
    if (!images_initialized) return NULL;
    return &dark_image;
}

sprites* GetDosMuseumImage() {
    if (!images_initialized) return NULL;
    return &dosmuseum_image;
}

sprites* GetTurretCursorImage() {
    if (!images_initialized) return NULL;
    return &turret_cursor_image;
}

sprites* GetMapCursorImage() {
    if (!images_initialized) return NULL;
    return &map_cursor_image;
}

sprites* GetMapCursor2Image() {
    if (!images_initialized) return NULL;
    return &map_cursor2_image;
}

sprites* GetTurretSelectImage() {
    if (!images_initialized) return NULL;
    return &turret_select_image;
}