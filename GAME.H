#ifndef GAME_H
#define GAME_H

#include "sed.h"

#ifndef NULL
#define NULL 0
#endif

// Forward declarations
class Enemy;
class Turret;

// Base class for drawable objects that need Y-sorting
class Drawable {
public:
    virtual ~Drawable() {}
    virtual void draw() = 0;
    virtual float getDrawY() const = 0;  // Y coordinate for sorting
    virtual float getDrawX() const = 0;  // X coordinate for secondary sorting
    virtual int isEnemy() const = 0;     // Type identifier
};

#define MAX_ENEMIES 30      // Reduced from 50 for memory optimization
#define MAX_TURRETS 100     // Reduced from 200 for memory optimization
#define MAX_BULLETS 50      // Reduced from 100 for memory optimization
#define MAX_EXPLOSIONS 15   // Reduced from 30 for memory optimization
#define TILE_SIZE 16
#define GRID_START_X 0
#define GRID_START_Y 24

#define EXPLOSION_FRAMES 5
#define EXPLOSION_FRAME_TIME 0.1f

enum BulletType {
    BULLET_BASIC,
    BULLET_SNIPER,
    BULLET_RAPID,
    BULLET_MULTI,
    BULLET_PIERCING
};

class Enemy : public Drawable {
protected:
    float x, y;
    float speed;
    int health;
    int max_health;
    int points;
    int current_waypoint;
    BYTE color;
    float total_distance_traveled;

    void drawHealthBar(int center_x, int center_y, int bar_width);
    
public:
    Enemy(float x, float y, float speed, int health, int points);
    virtual ~Enemy() {}
    
    virtual void update(float deltaTime);
    virtual void draw() = 0;
    virtual void draw(int mode) { draw(); }  // 호환성을 위한 기본 구현 (0=맵모드, 1=정보창모드)
    
    void takeDamage(int damage);
    int isDead() const { return health <= 0; }
    int reachedEnd() const;
    
    float getX() const { return x; }
    float getY() const { return y; }
    int getPoints() const { return points; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return max_health; }
    float getTotalDistance() const { return total_distance_traveled; }

    // 스프라이트 크기 (충돌 체크용)
    virtual int getSpriteWidth() const { return 8; }   // 기본 8픽셀
    virtual int getSpriteHeight() const { return 8; }  // 기본 8픽셀
    
    // Drawable interface
    virtual float getDrawY() const { return y; }
    virtual float getDrawX() const { return x; }
    virtual int isEnemy() const { return 1; }
};

class Bullet {
protected:
    float x, y;
    float vx, vy;
    int damage;
    Enemy* target;
    BYTE color;
    BulletType bullet_type;

    // dying 상태 (충돌 후 몇 프레임 더 보이기 위함)
    int dying;
    float dying_timer;
    
public:
    Bullet(float x, float y, Enemy* target, int damage, BulletType type);
    virtual ~Bullet() {}
    
    virtual void update(float deltaTime);
    virtual void draw() = 0;

    // 스프라이트 크기 (하위 클래스에서 구현)
    virtual int getSpriteWidth() const { return 2; }   // 기본 2픽셀
    virtual int getSpriteHeight() const { return 2; }  // 기본 2픽셀

    int isOffScreen() const;
    int checkCollision(Enemy* enemy);

    float getX() const { return x; }
    float getY() const { return y; }
    int getDamage() const { return damage; }
    BulletType getType() const { return bullet_type; }

    // dying 상태 관련
    int isDying() const { return dying; }
    void setDying() { dying = 1; dying_timer = 0.2f; }  // 0.2초 동안 보이기
    int shouldBeRemoved() const { return dying && dying_timer <= 0.0f; }
};

class Turret : public Drawable {
protected:
    int x, y;
    float attack_speed;
    float last_fire_time;
    int range;
    int damage;
    BYTE color;
    
public:
    Turret(int x, int y, float attack_speed, int range, int damage);
    virtual ~Turret() {}
    
    virtual Bullet* fire(Enemy* enemies[], int enemy_count);
    virtual void draw() = 0;
    
    int canFire() const;
    Enemy* findTarget(Enemy* enemies[], int enemy_count);
    float distanceTo(Enemy* enemy);
    
    int getX() const { return x; }
    int getY() const { return y; }
    int getRange() const { return range; }

    // Drawable interface
    virtual float getDrawY() const { return (float)y; }
    virtual float getDrawX() const { return (float)x; }
    virtual int isEnemy() const { return 0; }
};

class Explosion : public Drawable {
private:
    float x, y;
    int current_frame;
    float timer;
    int active;

public:
    Explosion(float x, float y);
    virtual ~Explosion() {}

    void update(float deltaTime);
    virtual void draw();

    int isActive() const { return active; }
    void setInactive() { active = 0; }

    // Drawable interface
    virtual float getDrawY() const { return y; }
    virtual float getDrawX() const { return x; }
    virtual int isEnemy() const { return 0; }
};

struct Waypoint {
    int x, y;
};

extern Waypoint waypoints[];
extern int waypoint_count;

// 밸런스 조절용 전역 변수들
extern float TURRET_FIRE_SPEED_MULTIPLIER;  // 터렛 발사 속도 배율 (값이 낮을수록 빠름)
extern float BULLET_DAMAGE_MULTIPLIER;      // 미사일 대미지 배율
extern float ENEMY_HEALTH_MULTIPLIER;       // 적 체력 배율
extern float ENEMY_POINTS_MULTIPLIER;       // 적 포인트 획득 배율

// 난이도 시스템
// 난이도 인덱스
#define DIFF_EASY 0
#define DIFF_NORMAL 1
#define DIFF_HARD 2

extern int CURRENT_DIFFICULTY;

// 로그 시스템
extern int gIsLogEnabled;
void writeLog(const char* message);

// 메모리 모니터링 시스템
typedef struct {
    unsigned long available_memory;
    unsigned long used_memory;
    unsigned long largest_free_block;
    int critical_threshold_reached;
} MemoryInfo;

void checkMemoryStatus(MemoryInfo* info);
void logMemoryStatus();
int isMemoryCritical();
void freeUnusedResources();

float getGameTime();
float getDeltaTime();
float getCurrentDifficultyDamageMultiplier();

#endif