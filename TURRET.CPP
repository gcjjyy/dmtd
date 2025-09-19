#include "turret.h"
#include "bullet.h"
#include "STR_KSSM.H"

// 5.25" 디스크 터렛 - 가장 기본적인 터렛 (8포인트)
Disk525Turret::Disk525Turret(int x, int y) : Turret(x, y, 1.2f * TURRET_FIRE_SPEED_MULTIPLIER, 35, 8 * BULLET_DAMAGE_MULTIPLIER) {
    color = 15;
    turret_image = GetTurretImage(TURRET_525);
}

void Disk525Turret::draw() {
    if (turret_image) {
        // 16x16 타일 기준으로 하단 앵커 적용
        int img_x = x + (16 - turret_image->x) / 2;  // 가로 중앙 정렬
        int img_y = y + 16 - turret_image->y;  // 타일 하단을 기준으로 이미지 하단 맞춤
        RPut_Spr(img_x, img_y, turret_image, 0);
    } else {
        // 이미지가 없으면 기존 방식으로 fallback
        CBox_Fill(x, y, x + 16 - 1, y + 16 - 1, color);
        Printf_Han(x + 1, y + 4, 15, 0, NORMAL_FONT, "525");
    }
}

// XT 터렛 - 약간 향상된 기본 터렛 (15포인트)
XTTurret::XTTurret(int x, int y) : Turret(x, y, 1.0f * TURRET_FIRE_SPEED_MULTIPLIER, 45, 12 * BULLET_DAMAGE_MULTIPLIER) {
    color = 10;
    turret_image = GetTurretImage(TURRET_XT);
}

void XTTurret::draw() {
    if (turret_image) {
        // 16x16 타일 기준으로 하단 앵커 적용
        int img_x = x + (16 - turret_image->x) / 2;  // 가로 중앙 정렬
        int img_y = y + 16 - turret_image->y;  // 타일 하단을 기준으로 이미지 하단 맞춤
        RPut_Spr(img_x, img_y, turret_image, 0);
    }
}

// AT 터렛 - 저격 특화 터렛 (28포인트)
ATTurret::ATTurret(int x, int y) : Turret(x, y, 1.5f * TURRET_FIRE_SPEED_MULTIPLIER, 75, 20 * BULLET_DAMAGE_MULTIPLIER) {
    color = 11;
    turret_image = GetTurretImage(TURRET_AT);
}

void ATTurret::draw() {
    if (turret_image) {
        // 16x16 타일 기준으로 하단 앵커 적용
        int img_x = x + (16 - turret_image->x) / 2;  // 가로 중앙 정렬
        int img_y = y + 16 - turret_image->y;  // 타일 하단을 기준으로 이미지 하단 맞춤
        RPut_Spr(img_x, img_y, turret_image, 0);
    }
}

// 3.5" 디스크 터렛 - 향상된 범용 터렛 (52포인트)
Disk35Turret::Disk35Turret(int x, int y) : Turret(x, y, 0.9f * TURRET_FIRE_SPEED_MULTIPLIER, 55, 16 * BULLET_DAMAGE_MULTIPLIER) {
    color = 11;
    turret_image = GetTurretImage(TURRET_35);
}

void Disk35Turret::draw() {
    if (turret_image) {
        // 16x16 타일 기준으로 하단 앵커 적용
        int img_x = x + (16 - turret_image->x) / 2;  // 가로 중앙 정렬
        int img_y = y + 16 - turret_image->y;  // 타일 하단을 기준으로 이미지 하단 맞춤
        RPut_Spr(img_x, img_y, turret_image, 0);
    } else {
        // 이미지가 없으면 기존 방식으로 fallback
        CBox_Fill(x, y, x + 16 - 1, y + 16 - 1, color);
        Printf_Han(x + 2, y + 4, 15, 0, NORMAL_FONT, "35");
    }
}

// 486 터렛 - 고속 연사 터렛 (98포인트)
Turret486::Turret486(int x, int y) : Turret(x, y, 0.7f * TURRET_FIRE_SPEED_MULTIPLIER, 60, 20 * BULLET_DAMAGE_MULTIPLIER) {
    color = 12;
    turret_image = GetTurretImage(TURRET_486);
}

void Turret486::draw() {
    if (turret_image) {
        // 16x16 타일 기준으로 하단 앵커 적용
        int img_x = x + (16 - turret_image->x) / 2;  // 가로 중앙 정렬
        int img_y = y + 16 - turret_image->y;  // 타일 하단을 기준으로 이미지 하단 맞춤
        RPut_Spr(img_x, img_y, turret_image, 0);
    } else {
        // 이미지가 없으면 기존 방식으로 fallback
        CBox_Fill(x, y, x + 16 - 1, y + 16 - 1, color);
        Printf_Han(x + 1, y + 4, 15, 0, NORMAL_FONT, "486");
    }
}

// CD 터렛 - 다방향 공격 터렛 (184포인트)
CDTurret::CDTurret(int x, int y) : Turret(x, y, 1.1f * TURRET_FIRE_SPEED_MULTIPLIER, 50, 18 * BULLET_DAMAGE_MULTIPLIER) {
    color = 13;
    turret_image = GetTurretImage(TURRET_CD);
}

void CDTurret::draw() {
    if (turret_image) {
        // 16x16 타일 기준으로 하단 앵커 적용
        int img_x = x + (16 - turret_image->x) / 2;  // 가로 중앙 정렬
        int img_y = y + 16 - turret_image->y;  // 타일 하단을 기준으로 이미지 하단 맞춤
        RPut_Spr(img_x, img_y, turret_image, 0);
    } else {
        // 이미지가 없으면 기존 방식으로 fallback
        CBox_Fill(x, y, x + 16 - 1, y + 16 - 1, color);
        Printf_Han(x + 3, y + 4, 15, 0, NORMAL_FONT, "CD");
    }
}

// 펜티엄 터렛 - 관통 공격 터렛 (345포인트)
PentiumTurret::PentiumTurret(int x, int y) : Turret(x, y, 0.8f * TURRET_FIRE_SPEED_MULTIPLIER, 65, 22 * BULLET_DAMAGE_MULTIPLIER) {
    color = 14;
    turret_image = GetTurretImage(TURRET_PENTIUM);
}

void PentiumTurret::draw() {
    if (turret_image) {
        // 16x16 타일 기준으로 하단 앵커 적용
        int img_x = x + (16 - turret_image->x) / 2;  // 가로 중앙 정렬
        int img_y = y + 16 - turret_image->y;  // 타일 하단을 기준으로 이미지 하단 맞춤
        RPut_Spr(img_x, img_y, turret_image, 0);
    } else {
        // 이미지가 없으면 기존 방식으로 fallback
        CBox_Fill(x, y, x + 16 - 1, y + 16 - 1, color);
        Printf_Han(x, y + 4, 15, 0, NORMAL_FONT, "P");
    }
}

Bullet* Disk525Turret::fire(Enemy* enemies[], int enemy_count) {
    if (!canFire()) return NULL;

    Enemy* target = findTarget(enemies, enemy_count);
    if (!target) return NULL;

    last_fire_time = getGameTime();
    return new BasicBullet(x + 8, y + 8, target, damage, 1); // BULLET1.PCX
}

Bullet* XTTurret::fire(Enemy* enemies[], int enemy_count) {
    if (!canFire()) return NULL;

    Enemy* target = findTarget(enemies, enemy_count);
    if (!target) return NULL;

    last_fire_time = getGameTime();
    return new BasicBullet(x + 8, y + 8, target, damage, 2); // BULLET2.PCX
}

Bullet* ATTurret::fire(Enemy* enemies[], int enemy_count) {
    if (!canFire()) return NULL;
    
    Enemy* target = findTarget(enemies, enemy_count);
    if (!target) return NULL;
    
    last_fire_time = getGameTime();
    return new SniperBullet(x + 8, y + 8, target, damage);
}

Bullet* Disk35Turret::fire(Enemy* enemies[], int enemy_count) {
    if (!canFire()) return NULL;

    Enemy* target = findTarget(enemies, enemy_count);
    if (!target) return NULL;

    last_fire_time = getGameTime();
    return new RapidBullet(x + 8, y + 8, target, damage, 4); // BULLET4.PCX
}

Bullet* Turret486::fire(Enemy* enemies[], int enemy_count) {
    if (!canFire()) return NULL;

    Enemy* target = findTarget(enemies, enemy_count);
    if (!target) return NULL;

    last_fire_time = getGameTime();
    return new RapidBullet(x + 8, y + 8, target, damage, 5); // BULLET5.PCX
}

Bullet* CDTurret::fire(Enemy* enemies[], int enemy_count) {
    if (!canFire()) return NULL;
    
    Enemy* target = findTarget(enemies, enemy_count);
    if (!target) return NULL;
    
    last_fire_time = getGameTime();
    // 다방향 총알 - 첫 번째 총알만 반환 (나머지는 GameManager에서 처리)
    return new MultiBullet(x + 8, y + 8, target, damage, 0);
}

Bullet* PentiumTurret::fire(Enemy* enemies[], int enemy_count) {
    if (!canFire()) return NULL;
    
    Enemy* target = findTarget(enemies, enemy_count);
    if (!target) return NULL;
    
    last_fire_time = getGameTime();
    // 관통 총알 구현
    return new PiercingBullet(x + 8, y + 8, target, damage);
}