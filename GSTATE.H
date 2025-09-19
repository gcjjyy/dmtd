#ifndef GSTATE_H
#define GSTATE_H

#include "game.h"

// Forward declarations
class Enemy;
class Turret;
class Bullet;

// Z-order 정의 (렌더링 우선순위: 낮을수록 먼저, 높을수록 나중에)
enum SpriteZOrder {
    ZORDER_TURRET = 0,    // 터렛 - 가장 뒤에
    ZORDER_ENEMY = 1,     // 적
    ZORDER_BULLET = 2,    // 총알
    ZORDER_EXPLOSION = 3  // 폭발 - 가장 앞에
};

// 스프라이트 정렬을 위한 구조체
struct SpriteInfo {
    void* object;
    int x, y, z;
    int type; // SpriteZOrder 값 사용
};

enum GameState {
    STATE_MENU,
    STATE_HELP,
    STATE_DIFFICULTY_SELECT,
    STATE_GAME_PLAY,
    STATE_STAGE_INTRO,
    STATE_EXIT_CONFIRM,
    STATE_GAME_OVER,
    STATE_GAME_CLEAR,
    STATE_EXIT
};

enum InputState {
    INPUT_IDLE,
    INPUT_TURRET_SELECT,
    INPUT_TURRET_SELL
};

enum TurretType {
    TURRET_525_DISK,
    TURRET_XT,
    TURRET_AT,
    TURRET_35_DISK,
    TURRET_486,
    TURRET_CD,
    TURRET_PENTIUM,
    TURRET_TYPE_COUNT
};

class GameManager {
private:
    GameState currentState;
    InputState inputState;
    TurretType selectedTurret;
    int selectedTurretIndex;
    
    Enemy* enemies[MAX_ENEMIES];
    Turret* turrets[MAX_TURRETS];
    Bullet* bullets[MAX_BULLETS];
    Explosion* explosions[MAX_EXPLOSIONS];

    int enemy_count;
    int turret_count;
    int bullet_count;
    int explosion_count;
    
    int life;
    int points;
    int stage;
    float stage_intro_timer;
    float spawn_timer;
    int enemies_spawned;
    int enemies_per_stage;
    int stage_in_progress;
    
    int cursor_x, cursor_y;
    int placement_x, placement_y;
    
    // 키 반복 방지용 변수들
    int last_key_left, last_key_right, last_key_up, last_key_down;
    int last_key_enter, last_key_esc;
    int last_key_1, last_key_2, last_key_3, last_key_4;
    int last_turret_left, last_turret_right, last_turret_up, last_turret_down;
    
    int turret_grid[20][11];

    // Game statistics for end screen
    int total_enemies_killed;
    int total_turrets_placed;
    int total_turrets_sold;
    int total_gold_earned;  // 누적 골드 (벌어들인 총 골드)

    // Menu variables
    int menu_selection;

    // Difficulty selection variables
    int difficulty_selection;

    // Exit confirmation variables
    int exit_confirmation_selection;

    // Turret sell variables
    int turret_sell_selection;
    int selling_turret_index;
    int selling_turret_x, selling_turret_y;
    
public:
    GameManager();
    ~GameManager();
    
    void init();
    void update();
    void draw();
    void handleInput();
    
    GameState getState() const { return currentState; }
    void setState(GameState state) { currentState = state; }
    
    void startGame();
    void endGame();

    void setDifficulty(int difficulty) {
        CURRENT_DIFFICULTY = difficulty;
    }

    float getDifficultyDamageMultiplier();
    void drawDifficultySelect();
    void runDifficultySelect();

    void drawExitConfirm();
    void runExitConfirm();

    void drawTurretSell();
    // void runTurretSell();  // 더이상 사용되지 않음 (INPUT_TURRET_SELL로 대체)

    void drawGameClear();
    void printCentered(int x1, int x2, int y, BYTE color, char* text);

private:
    void updateGame();
    void updateEnemies(float deltaTime);
    void updateTurrets();
    void updateBullets(float deltaTime);
    void updateExplosions(float deltaTime);
    void checkCollisions();
    void spawnEnemy();
    void placeTurret();
    void addExplosion(float x, float y);
    
    void drawGame();
    void drawGameSorted();  // Y축 정렬된 스프라이트 그리기
    void drawUI();
    void drawTurretSelect();
    void drawPlacementCursor();
    void drawCursor();
    void drawTurretRange(int tx, int ty, int range);
    
    // 스프라이트 정렬을 위한 유틸리티 함수들
    int collectSprites(SpriteInfo sprites[]);
    void sortSprites(SpriteInfo sprites[], int count);
    void drawSortedSprites(SpriteInfo sprites[], int count);
    
    void startStageIntro();
    void updateStageIntro(float deltaTime);
    void drawStageIntro();
    void setupStage(int stageNum);
    void getStageEnemyStats(int stage, float& speed, int& health, int& points);
    
    int canPlaceTurret(int x, int y);
    int getTurretCost(TurretType type);
    int getTurretDamage(TurretType type);
    int getTurretRange(TurretType type);
    float getTurretFireSpeed(TurretType type);
    
    void addPoints(int amount);
    void loseLife();

    int findTurretAt(int x, int y);
    void sellTurret(int turret_index);
    
    int getTextWidth(char* text);
    
    void drawEnemyInfoBar(int x, int y, int value, int maxValue, int color);
    void getRelativeEnemyStats(int stage, int& health, float& speed, int& points, int& count);
    void drawHealthIcon(int x, int y);
    void drawSpeedIcon(int x, int y);
};

extern GameManager* gameManager;

// Utility function for consistent window drawing
void DrawStandardWindow(int x1, int y1, int x2, int y2);

#endif