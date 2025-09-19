#include "bullet.h"
#include <math.h>

BasicBullet::BasicBullet(float x, float y, Enemy* target, int damage, int index) : Bullet(x, y, target, damage, BULLET_BASIC) {
    color = 15;
    bullet_index = index;
    bullet_image = GetBulletImage(bullet_index);
}

void BasicBullet::draw() {
    int ix = (int)x;
    int iy = (int)y;

    if (bullet_image) {
        // Center the bullet image
        CPut_Spr(ix - bullet_image->x/2, iy - bullet_image->y/2, bullet_image, 0);
    } else {
        // Fallback to simple drawing
        CPut_Pixel(ix, iy, color);
        CPut_Pixel(ix + 1, iy, color);
        CPut_Pixel(ix, iy + 1, color);
        CPut_Pixel(ix + 1, iy + 1, color);
    }
}

SniperBullet::SniperBullet(float x, float y, Enemy* target, int damage) : Bullet(x, y, target, damage, BULLET_SNIPER) {
    color = 14;
    vx *= 2.0f;
    vy *= 2.0f;
    bullet_image = GetBulletImage(3);  // BULLET3.PCX
}

void SniperBullet::draw() {
    int ix = (int)x;
    int iy = (int)y;

    if (bullet_image) {
        CPut_Spr(ix - bullet_image->x/2, iy - bullet_image->y/2, bullet_image, 0);
    } else {
        CH_Line(ix - 2, iy, ix + 2, color);
        CV_Line(ix, iy - 2, iy + 2, color);
    }
}

RapidBullet::RapidBullet(float x, float y, Enemy* target, int damage, int index) : Bullet(x, y, target, damage, BULLET_RAPID) {
    color = 11;
    bullet_index = index;
    bullet_image = GetBulletImage(bullet_index);
}

void RapidBullet::draw() {
    int ix = (int)x;
    int iy = (int)y;

    if (bullet_image) {
        CPut_Spr(ix - bullet_image->x/2, iy - bullet_image->y/2, bullet_image, 0);
    } else {
        CPut_Pixel(ix, iy, color);
    }
}

// 다방향 총알 - CD 터렛용
MultiBullet::MultiBullet(float x, float y, Enemy* target, int damage, int index) : Bullet(x, y, target, damage, BULLET_MULTI) {
    color = 13;
    bullet_index = index;
    bullet_image = GetBulletImage(6);  // BULLET6.PCX
    
    // 기본 방향 계산
    if (target) {
        float dx = target->getX() - x;
        float dy = target->getY() - y;
        float dist = sqrt(dx*dx + dy*dy);
        if (dist > 0) {
            base_vx = (dx / dist) * 2.0f;
            base_vy = (dy / dist) * 2.0f;
        } else {
            base_vx = base_vy = 0;
        }
    } else {
        base_vx = base_vy = 0;
    }
    
    // 각 총알마다 다른 각도 오프셋 (3발 발사: -30도, 0도, +30도)
    float angles[] = {-0.5236f, 0.0f, 0.5236f}; // -30, 0, +30 degrees in radians
    angle_offset = angles[index % 3];
    
    // 회전된 방향 계산
    float cos_angle = cos(angle_offset);
    float sin_angle = sin(angle_offset);
    vx = base_vx * cos_angle - base_vy * sin_angle;
    vy = base_vx * sin_angle + base_vy * cos_angle;
}

void MultiBullet::update(float deltaTime) {
    x += vx * deltaTime * 60.0f;
    y += vy * deltaTime * 60.0f;
}

void MultiBullet::draw() {
    int ix = (int)x;
    int iy = (int)y;

    if (bullet_image) {
        CPut_Spr(ix - bullet_image->x/2, iy - bullet_image->y/2, bullet_image, 0);
    } else {
        CPut_Pixel(ix, iy, color);
        CPut_Pixel(ix + 1, iy, color);
    }
}

// 관통 총알 - 펜티엄 터렛용
PiercingBullet::PiercingBullet(float x, float y, Enemy* target, int damage) : Bullet(x, y, target, damage, BULLET_PIERCING) {
    color = 14;
    pierce_count = 0;
    max_pierce = 3; // 최대 3마리까지 관통
    bullet_image = GetBulletImage(7);  // BULLET7.PCX
    
    // 더 빠른 속도
    vx *= 1.5f;
    vy *= 1.5f;
}

void PiercingBullet::update(float deltaTime) {
    x += vx * deltaTime * 60.0f;
    y += vy * deltaTime * 60.0f;
}

void PiercingBullet::draw() {
    int ix = (int)x;
    int iy = (int)y;

    if (bullet_image) {
        CPut_Spr(ix - bullet_image->x/2, iy - bullet_image->y/2, bullet_image, 0);
    } else {
        // 관통 총알은 더 큰 그래픽으로 표시
        CPut_Pixel(ix - 1, iy, color);
        CPut_Pixel(ix, iy - 1, color);
        CPut_Pixel(ix, iy, 15); // 중앙은 흰색
        CPut_Pixel(ix, iy + 1, color);
        CPut_Pixel(ix + 1, iy, color);
    }
}

int PiercingBullet::checkPiercingCollision(Enemy* enemies[], int enemy_count) {
    if (pierce_count >= max_pierce) return 0;

    int hit_any = 0;  // 이번 프레임에 충돌한 적이 있는지 추적

    // 총알 경계 박스 계산 (스프라이트 크기의 80%)
    float bullet_half_width = (getSpriteWidth() * 0.8f) / 2.0f;
    float bullet_half_height = (getSpriteHeight() * 0.8f) / 2.0f;

    float bullet_left = x - bullet_half_width;
    float bullet_right = x + bullet_half_width;
    float bullet_top = y - bullet_half_height;
    float bullet_bottom = y + bullet_half_height;

    // 모든 적과 충돌 체크 (한 프레임에 여러 적과 동시 충돌 가능)
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] && !enemies[i]->isDead() && pierce_count < max_pierce) {
            // 적 경계 박스 (스프라이트 크기의 80%)
            float enemy_half_width = (enemies[i]->getSpriteWidth() * 0.8f) / 2.0f;
            float enemy_half_height = (enemies[i]->getSpriteHeight() * 0.8f) / 2.0f;
            float enemy_left = enemies[i]->getX() - enemy_half_width;
            float enemy_right = enemies[i]->getX() + enemy_half_width;
            float enemy_top = enemies[i]->getY() - enemy_half_height;
            float enemy_bottom = enemies[i]->getY() + enemy_half_height;

            // AABB 충돌 체크
            if (bullet_left < enemy_right && bullet_right > enemy_left &&
                bullet_top < enemy_bottom && bullet_bottom > enemy_top) {
                // Apply difficulty damage multiplier
                int actual_damage = (int)(damage * getCurrentDifficultyDamageMultiplier());
                enemies[i]->takeDamage(actual_damage);
                pierce_count++;
                hit_any = 1;
            }
        }
    }
    return hit_any;
}