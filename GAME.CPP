#include <math.h>
#include <dos.h>
#include "game.h"
#include "map.h"
#include "IMAGES.H"
#include "STR_KSSM.H"

static unsigned long game_start_time;
static float last_frame_time;

// 밸런스 조절용 전역 변수들 (기본값 1.0)
float TURRET_FIRE_SPEED_MULTIPLIER = 1.0f;  // 터렛 발사 속도 배율
float BULLET_DAMAGE_MULTIPLIER = 1.0f;      // 미사일 대미지 배율
float ENEMY_HEALTH_MULTIPLIER = 2.0f;       // 적 체력 배율 (밸런스 조정)
float ENEMY_POINTS_MULTIPLIER = 1.0f;       // 적 포인트 획득 배율

// 난이도 변수
int CURRENT_DIFFICULTY = DIFF_NORMAL;

Enemy::Enemy(float x, float y, float speed, int health, int points) {
    this->x = x;
    this->y = y;
    this->speed = speed;
    this->health = health;
    this->max_health = health;
    this->points = points;
    this->current_waypoint = 0;
    this->color = 14;
    this->total_distance_traveled = 0.0f;
}

void Enemy::update(float deltaTime) {
    if (current_waypoint >= waypoint_count - 1) {
        return;
    }
    
    Waypoint& target = waypoints[current_waypoint + 1];
    float target_x = GRID_START_X + target.x * TILE_SIZE + TILE_SIZE/2;
    float target_y = GRID_START_Y + target.y * TILE_SIZE + TILE_SIZE/2;
    float dx = target_x - x;
    float dy = target_y - y;
    float dist = sqrt(dx*dx + dy*dy);
    
    if (dist < 2.0f) {
        current_waypoint++;
        if (current_waypoint >= waypoint_count - 1) {
            return;
        }
    } else {
        float moveDistance = speed * 100.0f * deltaTime;
        // 웨이포인트를 넘어서지 않도록 이동거리 제한
        if (moveDistance > dist) {
            moveDistance = dist;
        }
        x += (dx / dist) * moveDistance;
        y += (dy / dist) * moveDistance;
        
        // 이동거리 누적
        total_distance_traveled += moveDistance;
    }
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

int Enemy::reachedEnd() const {
    return current_waypoint >= waypoint_count - 1;
}

void Enemy::drawHealthBar(int center_x, int center_y, int bar_width) {
    if (health < max_health) {
        // 체력바 배경 (전체 너비)
        CBox_Fill(center_x - bar_width/2, center_y - 2, center_x + bar_width/2 - 1, center_y - 1, 5);
        // 남은 체력
        int health_bar_width = bar_width * health / max_health;
        if (health_bar_width > 0) {
            CBox_Fill(center_x - bar_width/2, center_y - 2, center_x - bar_width/2 + health_bar_width - 1, center_y - 1, 8);
        }
    }
}

Bullet::Bullet(float x, float y, Enemy* target, int damage, BulletType type) {
    this->x = x;
    this->y = y;
    this->target = target;
    this->damage = damage;
    this->bullet_type = type;
    this->color = 15;
    this->dying = 0;
    this->dying_timer = 0.0f;
    if (target) {
        float dx = target->getX() - x;
        float dy = target->getY() - y;
        float dist = sqrt(dx*dx + dy*dy);
        if (dist > 0) {
            vx = (dx / dist) * 2.5f;
            vy = (dy / dist) * 2.5f;
        } else {
            vx = vy = 0;
        }
    } else {
        vx = vy = 0;
    }
}

void Bullet::update(float deltaTime) {
    if (dying) {
        dying_timer -= deltaTime;
        if (dying_timer <= 0.0f) {
            dying_timer = 0.0f;
        }
        return;  // dying 상태에서는 이동하지 않음
    }

    x += vx * deltaTime * 60.0f;
    y += vy * deltaTime * 60.0f;
}

int Bullet::isOffScreen() const {
    return x < 0 || x >= 320 || y < 0 || y >= 200;
}

int Bullet::checkCollision(Enemy* enemy) {
    if (!enemy || enemy->isDead()) return 0;

    // 총알 경계 박스 계산 (스프라이트 크기의 80%)
    float bullet_half_width = (getSpriteWidth() * 0.8f) / 2.0f;
    float bullet_half_height = (getSpriteHeight() * 0.8f) / 2.0f;

    float bullet_left = x - bullet_half_width;
    float bullet_right = x + bullet_half_width;
    float bullet_top = y - bullet_half_height;
    float bullet_bottom = y + bullet_half_height;

    // 적 경계 박스 (스프라이트 크기의 80%)
    float enemy_half_width = (enemy->getSpriteWidth() * 0.8f) / 2.0f;
    float enemy_half_height = (enemy->getSpriteHeight() * 0.8f) / 2.0f;
    float enemy_left = enemy->getX() - enemy_half_width;
    float enemy_right = enemy->getX() + enemy_half_width;
    float enemy_top = enemy->getY() - enemy_half_height;
    float enemy_bottom = enemy->getY() + enemy_half_height;

    // AABB 충돌 체크
    return (bullet_left < enemy_right && bullet_right > enemy_left &&
            bullet_top < enemy_bottom && bullet_bottom > enemy_top);
}

Turret::Turret(int x, int y, float attack_speed, int range, int damage) {
    this->x = x;
    this->y = y;
    this->attack_speed = attack_speed;
    this->range = range;
    this->damage = damage;
    this->last_fire_time = 0;
    this->color = 10;
}

int Turret::canFire() const {
    return (getGameTime() - last_fire_time) >= attack_speed;
}

Enemy* Turret::findTarget(Enemy* enemies[], int enemy_count) {
    Enemy* best_target = NULL;
    float max_distance = -1.0f;
    
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] && !enemies[i]->isDead()) {
            float dist = distanceTo(enemies[i]);
            
            // 사거리 내에 있는지 확인
            if (dist <= range * range) {
                float traveled = enemies[i]->getTotalDistance();
                
                // 가장 많이 이동한 적을 선택
                if (traveled > max_distance) {
                    max_distance = traveled;
                    best_target = enemies[i];
                }
            }
        }
    }
    
    return best_target;
}

float Turret::distanceTo(Enemy* enemy) {
    float dx = enemy->getX() - x;
    float dy = enemy->getY() - y;
    return dx*dx + dy*dy;  // 거리의 제곱 반환 (성능 개선 및 올바른 사거리 계산)
}

Bullet* Turret::fire(Enemy* /* enemies */[], int /* enemy_count */) {
    return NULL;
}

float getGameTime() {
    struct time t;
    gettime(&t);
    unsigned long current = t.ti_hour * 3600000L + t.ti_min * 60000L + 
                           t.ti_sec * 1000L + t.ti_hund * 10L;
    
    if (game_start_time == 0) {
        game_start_time = current;
    }
    
    return (current - game_start_time) / 1000.0f;
}

float getDeltaTime() {
    float current_time = getGameTime();

    // 첫 프레임인 경우
    if (last_frame_time == 0) {
        last_frame_time = current_time;
        return 0.016f;  // 기본값 약 60fps
    }

    float delta = current_time - last_frame_time;
    last_frame_time = current_time;

    if (delta > 0.1f) delta = 0.1f;
    if (delta < 0) delta = 0.016f;  // 음수 방지

    return delta;
}

float getCurrentDifficultyDamageMultiplier() {
    switch (CURRENT_DIFFICULTY) {
        case DIFF_EASY:
            return 2.0f;
        case DIFF_NORMAL:
            return 1.0f;
        case DIFF_HARD:
            return 0.75f;
        default:
            return 1.0f;
    }
}

// Explosion class implementation
Explosion::Explosion(float x, float y) {
    this->x = x;
    this->y = y;
    this->current_frame = 0;
    this->timer = 0.0f;
    this->active = 1;
}

void Explosion::update(float deltaTime) {
    if (!active) return;

    timer += deltaTime;

    // 프레임 변경 (0.1초마다)
    if (timer >= EXPLOSION_FRAME_TIME) {
        timer = 0.0f;
        current_frame++;

        // 모든 프레임을 재생했으면 비활성화
        if (current_frame >= EXPLOSION_FRAMES) {
            active = 0;
        }
    }
}

void Explosion::draw() {
    if (!active || current_frame >= EXPLOSION_FRAMES) return;

    sprites* explosion_image = GetExplosionImage(current_frame);
    if (explosion_image && explosion_image->image) {
        int draw_x = (int)(x - explosion_image->x / 2);
        int draw_y = (int)(y - explosion_image->y / 2);
        RPut_Spr(draw_x, draw_y, explosion_image, 0);
    } else {
        // 이미지가 없으면 간단한 원으로 표시 (fallback)
        int size = 8 + current_frame * 2;
        BYTE colors[] = {14, 12, 4, 6, 8}; // 노랑 -> 빨강 -> 어두운 빨강 -> 갈색 -> 회색
        BYTE color = (current_frame < 5) ? colors[current_frame] : 8;
        CCircle((int)x, (int)y, size, color);
    }
}