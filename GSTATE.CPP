#include <mem.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gstate.h"
#include "game.h"
#include "enemy.h"
#include "turret.h"
#include "bullet.h"
#include "map.h"
#include "drawing.h"
#include "STR_KSSM.H"
#include "IMAGES.H"

GameManager* gameManager = NULL;

GameManager::GameManager() {
    init();
}

GameManager::~GameManager() {
    endGame();
}

void GameManager::init() {
    // 전역 이미지 시스템 초기화
    Load_Pal("dmtd.pal");
    InitGameImages();
    
    

    currentState = STATE_MENU;
    inputState = INPUT_IDLE;
    selectedTurret = TURRET_525_DISK;
    selectedTurretIndex = 0;

    enemy_count = 0;
    turret_count = 0;
    bullet_count = 0;
    explosion_count = 0;

    life = 20;
    points = 100;
    total_gold_earned = 100;  // 초기 골드도 누적골드에 포함
    stage = 0;
    stage_intro_timer = 0;
    spawn_timer = 0;
    enemies_spawned = 0;
    enemies_per_stage = 5;
    stage_in_progress = 0;
    
    cursor_x = GRID_START_X + 9 * TILE_SIZE;
    cursor_y = GRID_START_Y + 5 * TILE_SIZE;
    placement_x = 0;
    placement_y = 0;
    
    // 키 상태 초기화
    last_key_left = last_key_right = last_key_up = last_key_down = 0;
    last_key_enter = last_key_esc = 0;
    last_key_1 = last_key_2 = last_key_3 = last_key_4 = 0;
    last_turret_left = last_turret_right = last_turret_up = last_turret_down = 0;
    
    memset(enemies, 0, sizeof(enemies));
    memset(turrets, 0, sizeof(turrets));
    memset(bullets, 0, sizeof(bullets));
    memset(turret_grid, 0, sizeof(turret_grid));

    // Initialize difficulty selection
    difficulty_selection = 1; // NORMAL by default

    // Initialize exit confirmation selection
    exit_confirmation_selection = 0; // NO by default

    // Initialize turret sell variables
    turret_sell_selection = 0; // NO by default
    selling_turret_index = -1;
    selling_turret_x = selling_turret_y = 0;

    // Initialize game statistics
    total_enemies_killed = 0;
    total_turrets_placed = 0;
    total_turrets_sold = 0;
    total_gold_earned = 0;

    Multi_Key();
}

void GameManager::startGame() {
    writeLog("startGame() called");
    endGame();
    init();
    life = 20;
    points = 100;
    total_gold_earned = 100;  // 초기 골드도 누적골드에 포함
    stage = 1;

    // Reset game statistics
    total_enemies_killed = 0;
    total_turrets_placed = 0;
    total_turrets_sold = 0;
    total_gold_earned = 100;  // 초기 골드도 누적골드에 포함

    // 게임 시작 시 타이머 리셋
    getGameTime();  // 시간 초기화
    getDeltaTime(); // delta time 초기화

    writeLog("Calling startStageIntro()");
    startStageIntro();
    writeLog("startGame() finished");
}

void GameManager::endGame() {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i]) {
            delete enemies[i];
            enemies[i] = NULL;
        }
    }
    
    for (i = 0; i < MAX_TURRETS; i++) {
        if (turrets[i]) {
            delete turrets[i];
            turrets[i] = NULL;
        }
    }
    
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i]) {
            delete bullets[i];
            bullets[i] = NULL;
        }
    }

    for (i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i]) {
            delete explosions[i];
            explosions[i] = NULL;
        }
    }

    enemy_count = 0;
    turret_count = 0;
    bullet_count = 0;
    explosion_count = 0;

    // 전역 이미지 시스템 해제
    FreeGameImages();

    Restore_Key();
}

void GameManager::update() {
    switch (currentState) {
        case STATE_GAME_PLAY:
            updateGame();
            break;
        case STATE_STAGE_INTRO:
            updateStageIntro(getDeltaTime());
            break;
    }
}

void GameManager::updateGame() {
    float deltaTime = getDeltaTime();
    
    spawn_timer += deltaTime;
    
    if (stage_in_progress && enemies_spawned < enemies_per_stage && spawn_timer > 0.467f) {
        spawnEnemy();
        spawn_timer = 0;
        enemies_spawned++;
    }
    
    if (stage_in_progress && enemies_spawned >= enemies_per_stage && enemy_count == 0) {
        // 스테이지 클리어
        stage_in_progress = 0;
        addPoints(stage * 15 + stage * stage * 2);  // 스테이지 클리어 보너스 (조정됨)

        Multi_Clear();  // 키 버퍼 초기화
        Restore_Key();  // 단일 키 입력 모드로 전환

        if (stage >= 20) {
            currentState = STATE_GAME_CLEAR;
        } else {
            stage++;
            startStageIntro();
        }
    }
    
    updateEnemies(deltaTime);
    updateTurrets();
    updateBullets(deltaTime);
    updateExplosions(deltaTime);
    checkCollisions();
    
    if (life <= 0) {
        currentState = STATE_GAME_OVER;
    }
}

void GameManager::updateEnemies(float deltaTime) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i] && !enemies[i]->isDead()) {
            enemies[i]->update(deltaTime);
            
            if (enemies[i]->reachedEnd()) {
                loseLife();
                delete enemies[i];
                enemies[i] = NULL;
                enemy_count--;
            }
        } else if (enemies[i] && enemies[i]->isDead()) {
            addPoints(enemies[i]->getPoints());
            total_enemies_killed++;  // Track enemy kill
            delete enemies[i];
            enemies[i] = NULL;
            enemy_count--;
        }
    }
}

void GameManager::updateTurrets() {
    int i, j, k;
    for (i = 0; i < MAX_TURRETS; i++) {
        if (turrets[i]) {
            Bullet* newBullet = turrets[i]->fire(enemies, MAX_ENEMIES);
            if (newBullet) {
                // MultiBullet인지 확인하고 다방향 총알 처리
                if (newBullet->getType() == BULLET_MULTI && bullet_count < MAX_BULLETS - 2) {
                    // CD 터렛: 3발의 MultiBullet 생성
                    Enemy* target = NULL;
                    for (k = 0; k < MAX_ENEMIES; k++) {
                        if (enemies[k] && !enemies[k]->isDead()) {
                            float dx = enemies[k]->getX() - (turrets[i]->getX() + 8);
                            float dy = enemies[k]->getY() - (turrets[i]->getY() + 8);
                            float range = turrets[i]->getRange();
                            if (dx*dx + dy*dy <= range*range) {
                                target = enemies[k];
                                break;
                            }
                        }
                    }
                    
                    if (target) {
                        for (k = 0; k < 3; k++) {
                            Bullet* multiBullet = new MultiBullet(turrets[i]->getX() + 8, turrets[i]->getY() + 8, target, newBullet->getDamage(), k);
                            for (j = 0; j < MAX_BULLETS; j++) {
                                if (!bullets[j]) {
                                    bullets[j] = multiBullet;
                                    bullet_count++;
                                    break;
                                }
                            }
                        }
                    }
                    delete newBullet; // 원본 총알 삭제
                } else if (bullet_count < MAX_BULLETS) {
                    // 일반 터렛: 단일 총알 추가
                    for (j = 0; j < MAX_BULLETS; j++) {
                        if (!bullets[j]) {
                            bullets[j] = newBullet;
                            bullet_count++;
                            break;
                        }
                    }
                } else {
                    delete newBullet;
                }
            }
        }
    }
}

void GameManager::updateBullets(float deltaTime) {
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i]) {
            bullets[i]->update(deltaTime);

            if (bullets[i]->isOffScreen() || bullets[i]->shouldBeRemoved()) {
                delete bullets[i];
                bullets[i] = NULL;
                bullet_count--;
            }
        }
    }
}

void GameManager::checkCollisions() {
    int i, j;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i] && !bullets[i]->isDying()) {
            // 관통 총알인지 확인
            if (bullets[i]->getType() == BULLET_PIERCING) {
                // 관통 총알 처리
                PiercingBullet* piercingBullet = (PiercingBullet*)bullets[i];
                if (piercingBullet->checkPiercingCollision(enemies, MAX_ENEMIES)) {
                    // 관통 총알도 충돌 시 폭발 효과 생성
                    addExplosion(bullets[i]->getX(), bullets[i]->getY());
                    // 관통 한계에 도달했으면 dying 상태로 설정
                    if (piercingBullet->getPierceCount() >= piercingBullet->getMaxPierce()) {
                        piercingBullet->setDying();
                    }
                }
            } else {
                // 일반 총알 처리
                for (j = 0; j < MAX_ENEMIES; j++) {
                    if (enemies[j] && !enemies[j]->isDead()) {
                        if (bullets[i]->checkCollision(enemies[j])) {
                            // 폭발 효과 생성
                            addExplosion(bullets[i]->getX(), bullets[i]->getY());

                            // Apply difficulty damage multiplier
                            int damage = bullets[i]->getDamage();
                            damage = (int)(damage * getDifficultyDamageMultiplier());
                            enemies[j]->takeDamage(damage);
                            bullets[i]->setDying();  // 즉시 삭제 대신 dying 상태로 설정
                            break;
                        }
                    }
                }
            }
        }
    }
}

void GameManager::spawnEnemy() {
    if (enemy_count >= MAX_ENEMIES) return;
    
    float start_x = GRID_START_X + waypoints[0].x * TILE_SIZE + TILE_SIZE/2;
    float start_y = GRID_START_Y + waypoints[0].y * TILE_SIZE + TILE_SIZE/2;
    
    // 스테이지별로 한 종류의 적만 생성
    Enemy* newEnemy = new StageEnemy(start_x, start_y, stage);
    
    if (newEnemy) {
        int i;
        for (i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i]) {
                enemies[i] = newEnemy;
                enemy_count++;
                break;
            }
        }
    }
}

void GameManager::draw() {
    switch (currentState) {
        case STATE_GAME_PLAY:
            drawGame();
            break;
        case STATE_STAGE_INTRO:
            drawStageIntro();
            break;
    }
}

void GameManager::drawGame() {
    drawGameBackground();
    drawGameSorted();  // Y축 정렬된 스프라이트 그리기로 변경
    drawUI();

    if (inputState == INPUT_IDLE) {
        drawCursor();
    } else if (inputState == INPUT_TURRET_SELECT) {
        drawTurretSelect();
    } else if (inputState == INPUT_TURRET_SELL) {
        drawTurretSell();
    }
}

void GameManager::drawGameSorted() {
    SpriteInfo sprites[MAX_TURRETS + MAX_ENEMIES + MAX_BULLETS + MAX_EXPLOSIONS];
    int sprite_count = collectSprites(sprites);
    
    if (sprite_count > 0) {
        sortSprites(sprites, sprite_count);
        drawSortedSprites(sprites, sprite_count);
    }
}

int GameManager::collectSprites(SpriteInfo sprites[]) {
    int count = 0;
    int i;
    
    // 터렛 수집
    for (i = 0; i < MAX_TURRETS; i++) {
        if (turrets[i]) {
            sprites[count].object = turrets[i];
            sprites[count].x = turrets[i]->getX();
            sprites[count].y = turrets[i]->getY();
            sprites[count].z = ZORDER_TURRET;
            sprites[count].type = ZORDER_TURRET;
            count++;
        }
    }
    
    // 적 수집
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i] && !enemies[i]->isDead()) {
            sprites[count].object = enemies[i];
            sprites[count].x = (int)enemies[i]->getX();
            sprites[count].y = (int)enemies[i]->getY();
            sprites[count].z = ZORDER_ENEMY;
            sprites[count].type = ZORDER_ENEMY;
            count++;
        }
    }
    
    // 총알 수집
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i]) {
            sprites[count].object = bullets[i];
            sprites[count].x = (int)bullets[i]->getX();
            sprites[count].y = (int)bullets[i]->getY();
            sprites[count].z = ZORDER_BULLET;
            sprites[count].type = ZORDER_BULLET;
            count++;
        }
    }

    // 폭발 수집
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i] && explosions[i]->isActive()) {
            sprites[count].object = explosions[i];
            sprites[count].x = (int)explosions[i]->getDrawX();
            sprites[count].y = (int)explosions[i]->getDrawY();
            sprites[count].z = ZORDER_EXPLOSION;
            sprites[count].type = ZORDER_EXPLOSION;
            count++;
        }
    }

    return count;
}

void GameManager::sortSprites(SpriteInfo sprites[], int count) {
    // 버블 정렬 사용 (Z-order 우선, Y축 차순, X축 마지막)
    int i, j;
    SpriteInfo temp;

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            int should_swap = 0;

            // 1차 정렬: Z-order (z값이 작을수록 먼저)
            if (sprites[j].z > sprites[j + 1].z) {
                should_swap = 1;
            } else if (sprites[j].z == sprites[j + 1].z) {
                // 2차 정렬: Y축 (Y가 작을수록 먼저)
                if (sprites[j].y > sprites[j + 1].y) {
                    should_swap = 1;
                } else if (sprites[j].y == sprites[j + 1].y) {
                    // 3차 정렬: X축 (X가 작을수록 먼저)
                    if (sprites[j].x > sprites[j + 1].x) {
                        should_swap = 1;
                    }
                }
            }

            if (should_swap) {
                temp = sprites[j];
                sprites[j] = sprites[j + 1];
                sprites[j + 1] = temp;
            }
        }
    }
}

void GameManager::drawSortedSprites(SpriteInfo sprites[], int count) {
    int i;
    
    for (i = 0; i < count; i++) {
        switch (sprites[i].type) {
            case ZORDER_TURRET: // 터렛
                ((Turret*)sprites[i].object)->draw();
                break;
            case ZORDER_ENEMY: // 적
                ((Enemy*)sprites[i].object)->draw(0);  // 맵 모드로 그리기
                break;
            case ZORDER_BULLET: // 총알
                ((Bullet*)sprites[i].object)->draw();
                break;
            case ZORDER_EXPLOSION: // 폭발
                ((Explosion*)sprites[i].object)->draw();
                break;
        }
    }
}

void GameManager::drawUI() {
    // Header background is drawn in drawGameBackground()

    // Draw points (number only) at x=41, y=3 (위로 2픽셀), 흰색(45번)
    char buf[32];
    sprintf(buf, "%d", points);
    Printf_Han(41, 3, 45, 0, NORMAL_FONT, buf);

    // Draw lives using LIGHT/DARK images at x=196 (왼쪽으로 26픽셀), y=7 (아래로 2픽셀)
    sprites* light_img = GetLightImage();
    sprites* dark_img = GetDarkImage();

    if (light_img && dark_img) {
        int life_x = 196;  // 207 - 11 = 196
        int life_y = 7;    // 5 + 2 = 7
        int life_spacing = 5; // Space between life indicators

        for (int i = 0; i < 20; i++) {
            if (i < life) {
                // Active life - use LIGHT
                if (light_img->image) {
                    RPut_Spr(life_x + i * life_spacing, life_y, light_img, 0);
                }
            } else {
                // Lost life - use DARK
                if (dark_img->image) {
                    RPut_Spr(life_x + i * life_spacing, life_y, dark_img, 0);
                }
            }
        }
    }

    // Stage number is no longer displayed
}

void GameManager::drawTurretSelect() {
    // TURSEL.PCX 이미지 가져오기
    sprites* turret_select_img = GetTurretSelectImage();
    if (!turret_select_img || !turret_select_img->image) {
        return;  // 이미지가 없으면 리턴
    }

    // 이미지를 화면 중앙에 배치 (5픽셀 아래로)
    int menu_x = (320 - turret_select_img->x) / 2;
    int menu_y = (200 - turret_select_img->y) / 2 + 5;  // 5픽셀 아래로

    // TURSEL.PCX 이미지 출력
    RPut_Spr(menu_x, menu_y, turret_select_img, 0);

    // 터렛 선택 타이틀 출력 (이미지 위에)
    int title_width = strlen(TURRET_SELECT_TITLE) * 8;  // 한글은 문자당 약 8픽셀
    Printf_Han(menu_x + (turret_select_img->x - title_width) / 2, menu_y + 24, 45, 0, NORMAL_FONT, TURRET_SELECT_TITLE);

    // 터렛 셀 크기 및 위치 설정 (이미지에 맞게 조정)
    int turret_cell_width = 50;
    int turret_cell_height = 50;

    // 터렛 배치 시작 위치 (TURSEL.PCX 이미지 내부에 맞게 조정)
    int content_start_x = menu_x + 15;  // 이미지 내부 여백 고려
    int content_start_y = menu_y + 40;  // 타이틀 공간 고려
    int row_spacing = 10;

    for (int i = 0; i < TURRET_TYPE_COUNT; i++) {
        int cell_x, cell_y;
        if (i < 4) {  // 첫번째 줄 (4개)
            cell_x = content_start_x + i * turret_cell_width + i;  // x좌표 1만큼씩 증가
            cell_y = content_start_y + 8;  // y좌표 8만큼 증가
        } else {  // 두번째 줄 (3개, 왼쪽 정렬)
            cell_x = content_start_x + (i - 4) * turret_cell_width + (i - 4);  // x좌표 1만큼씩 증가
            cell_y = content_start_y + turret_cell_height + row_spacing + 8;  // y좌표 8만큼 증가
        }

        // 선택된 포탑 커서 이미지
        if (selectedTurretIndex == i) {
            sprites* cursor_img = GetTurretCursorImage();
            if (cursor_img && cursor_img->image) {
                // TCURSOR.PCX는 56x56 크기이므로 터렛 셀 중앙에 배치 (Y좌표 3픽셀 위로)
                int cursor_x = cell_x + (turret_cell_width - 56) / 2;
                int cursor_y = cell_y + (turret_cell_height - 56) / 2 - 3;
                RPut_Spr(cursor_x, cursor_y, cursor_img, 0);
            }
        }

        // 터렛 이미지 그리기 (하단 정렬)
        int turret_max_height = 24;  // 터렛 최대 높이
        int turret_bottom_y = cell_y + turret_max_height;  // 터렛 하단 위치

        // 터렛 중앙 위치 (터렛은 16x16 타일 기준)
        int turret_center_x = cell_x + turret_cell_width / 2 - TILE_SIZE / 2;
        int turret_center_y = turret_bottom_y - TILE_SIZE - 7;  // Y좌표를 7픽셀 위로

        // 터렛 이미지 직접 그리기 (인스턴스 생성 없이)
        sprites* turret_img = GetTurretImage(i);
        if (turret_img) {
            RPut_Spr(turret_center_x, turret_center_y, turret_img, 0);
        }
        
        // 가격 표시 준비
        int turret_cost = getTurretCost((TurretType)i);
        char cost_str[10];
        sprintf(cost_str, "%d", turret_cost);

        // 코인 이미지와 가격 텍스트 너비 계산
        sprites* coin_img = GetCoinImage();
        int text_width = strlen(cost_str) * 8;  // 숫자 한 글자당 8픽셀
        int coin_width = coin_img ? coin_img->x : 0;
        int total_price_width = coin_width + 2 + text_width;  // 코인 + 간격 2 + 텍스트

        // 가격 표시 위치 (중앙 정렬)
        int price_y = cell_y + turret_max_height + 5;  // 터렛 아래 5픽셀
        int price_start_x = cell_x + (turret_cell_width - total_price_width) / 2;

        // 코인 이미지 출력
        if (coin_img) {
            RPut_Spr(price_start_x, price_y, coin_img, 0);
        }

        // 가격 텍스트 출력
        int text_x = price_start_x + coin_width + 2;
        Printf_Han(text_x, price_y, 45, 0, NORMAL_FONT, cost_str);
    }
}

void GameManager::drawPlacementCursor() {
    if (canPlaceTurret(placement_x, placement_y)) {
        CBox(GRID_START_X + placement_x * TILE_SIZE, GRID_START_Y + placement_y * TILE_SIZE,
             GRID_START_X + (placement_x + 1) * TILE_SIZE - 1,
             GRID_START_Y + (placement_y + 1) * TILE_SIZE - 1, 10);
    } else {
        CBox(GRID_START_X + placement_x * TILE_SIZE, GRID_START_Y + placement_y * TILE_SIZE,
             GRID_START_X + (placement_x + 1) * TILE_SIZE - 1,
             GRID_START_Y + (placement_y + 1) * TILE_SIZE - 1, 12);
    }
}

void GameManager::handleInput() {
    if (inputState == INPUT_IDLE) {
        // 왼쪽 화살표
        if (_key[_LEFT] && !last_key_left && cursor_x > GRID_START_X) {
            cursor_x -= TILE_SIZE;
        }
        last_key_left = _key[_LEFT];
        
        // 오른쪽 화살표
        if (_key[_RIGHT] && !last_key_right && cursor_x < GRID_START_X + (19 * TILE_SIZE)) {
            cursor_x += TILE_SIZE;
        }
        last_key_right = _key[_RIGHT];
        
        // 위쪽 화살표
        if (_key[_UP] && !last_key_up && cursor_y > GRID_START_Y) {
            cursor_y -= TILE_SIZE;
        }
        last_key_up = _key[_UP];
        
        // 아래쪽 화살표
        if (_key[_DOWN] && !last_key_down && cursor_y < GRID_START_Y + (10 * TILE_SIZE)) {
            cursor_y += TILE_SIZE;
        }
        last_key_down = _key[_DOWN];
        
        // 엔터키
        if (_key[_ENTER] && !last_key_enter) {
            placement_x = (cursor_x - GRID_START_X) / TILE_SIZE;
            placement_y = (cursor_y - GRID_START_Y) / TILE_SIZE;

            // 해당 위치에 터렛이 있는지 확인
            int turret_index = findTurretAt(placement_x, placement_y);
            if (turret_index >= 0) {
                // 기존 터렛이 있으면 판매 팝업 표시
                selling_turret_index = turret_index;
                selling_turret_x = turrets[turret_index]->getX();
                selling_turret_y = turrets[turret_index]->getY();
                turret_sell_selection = 1;  // "아니오"가 기본값
                inputState = INPUT_TURRET_SELL;
            } else if (canPlaceTurret(placement_x, placement_y)) {
                // 설치 가능한 땅이면 터렛 선택창 표시
                inputState = INPUT_TURRET_SELECT;
            }
            // 설치 불가능한 땅에서는 아무것도 하지 않음
        }
        last_key_enter = _key[_ENTER];
        
        // ESC키
        if (_key[_ESC] && !last_key_esc) {
            if (currentState == STATE_GAME_PLAY) {
                exit_confirmation_selection = 1;  // "아니오"가 기본값
                currentState = STATE_EXIT_CONFIRM;
            }
        }
        last_key_esc = _key[_ESC];
        
    } else if (inputState == INPUT_TURRET_SELECT) {
        // 2D 격자 네비게이션 (4/3 배치)
        
        // 좌측 키
        if (_key[_LEFT] && !last_turret_left) {
            if (selectedTurretIndex < 4) {  // 첫번째 줄
                selectedTurretIndex--;
                if (selectedTurretIndex < 0) selectedTurretIndex = 3;
            } else {  // 두번째 줄
                selectedTurretIndex--;
                if (selectedTurretIndex < 4) selectedTurretIndex = 6;
            }
            selectedTurret = (TurretType)selectedTurretIndex;
        }
        last_turret_left = _key[_LEFT];
        
        // 우측 키
        if (_key[_RIGHT] && !last_turret_right) {
            if (selectedTurretIndex < 4) {  // 첫번째 줄
                selectedTurretIndex++;
                if (selectedTurretIndex > 3) selectedTurretIndex = 0;
            } else {  // 두번째 줄
                selectedTurretIndex++;
                if (selectedTurretIndex > 6) selectedTurretIndex = 4;
            }
            selectedTurret = (TurretType)selectedTurretIndex;
        }
        last_turret_right = _key[_RIGHT];
        
        // 위 키
        if (_key[_UP] && !last_turret_up) {
            if (selectedTurretIndex >= 4) {  // 두번째 줄에서 첫번째 줄로
                selectedTurretIndex -= 4;
                if (selectedTurretIndex > 3) selectedTurretIndex = 3;  // 두번째줄 끝에서 첫번째줄 끝으로
            }
            selectedTurret = (TurretType)selectedTurretIndex;
        }
        last_turret_up = _key[_UP];
        
        // 아래 키
        if (_key[_DOWN] && !last_turret_down) {
            if (selectedTurretIndex < 4) {  // 첫번째 줄에서 두번째 줄로
                selectedTurretIndex += 4;
                if (selectedTurretIndex > 6) selectedTurretIndex = 6;  // 첫번째줄 끝에서 두번째줄 끝으로
            }
            selectedTurret = (TurretType)selectedTurretIndex;
        }
        last_turret_down = _key[_DOWN];
        
        // 엔터키 - 포탑 배치
        if (_key[_ENTER] && !last_key_enter) {
            int cost = getTurretCost(selectedTurret);
            if (points >= cost) {
                placeTurret();
                inputState = INPUT_IDLE;
            }
        }
        last_key_enter = _key[_ENTER];
        
        // ESC키 - 취소
        if (_key[_ESC] && !last_key_esc) {
            inputState = INPUT_IDLE;
        }
        last_key_esc = _key[_ESC];
    } else if (inputState == INPUT_TURRET_SELL) {
        // 터렛 판매 팝업 입력 처리

        // 좌측 키
        if (_key[_LEFT] && !last_key_left) {
            turret_sell_selection--;
            if (turret_sell_selection < 0) turret_sell_selection = 1;
        }
        last_key_left = _key[_LEFT];

        // 우측 키
        if (_key[_RIGHT] && !last_key_right) {
            turret_sell_selection++;
            if (turret_sell_selection > 1) turret_sell_selection = 0;
        }
        last_key_right = _key[_RIGHT];

        // 엔터키
        if (_key[_ENTER] && !last_key_enter) {
            if (turret_sell_selection == 0) {
                // 예 선택 - 터렛 판매
                sellTurret(selling_turret_index);
            }
            // 팝업 닫기
            inputState = INPUT_IDLE;
        }
        last_key_enter = _key[_ENTER];

        // ESC키 - 취소
        if (_key[_ESC] && !last_key_esc) {
            inputState = INPUT_IDLE;
        }
        last_key_esc = _key[_ESC];
    }
}

void GameManager::placeTurret() {
    if (!canPlaceTurret(placement_x, placement_y)) return;
    
    int cost = getTurretCost(selectedTurret);
    if (points < cost) return;
    
    if (turret_count >= MAX_TURRETS) return;
    
    Turret* newTurret = NULL;
    int px = GRID_START_X + placement_x * TILE_SIZE;
    int py = GRID_START_Y + placement_y * TILE_SIZE;
    
    switch (selectedTurret) {
        case TURRET_525_DISK:
            newTurret = new Disk525Turret(px, py);
            break;
        case TURRET_XT:
            newTurret = new XTTurret(px, py);
            break;
        case TURRET_AT:
            newTurret = new ATTurret(px, py);
            break;
        case TURRET_35_DISK:
            newTurret = new Disk35Turret(px, py);
            break;
        case TURRET_486:
            newTurret = new Turret486(px, py);
            break;
        case TURRET_CD:
            newTurret = new CDTurret(px, py);
            break;
        case TURRET_PENTIUM:
            newTurret = new PentiumTurret(px, py);
            break;
    }
    
    if (newTurret) {
        int i;
        for (i = 0; i < MAX_TURRETS; i++) {
            if (!turrets[i]) {
                turrets[i] = newTurret;
                turret_count++;
                turret_grid[placement_x][placement_y] = 1;
                points -= cost;
                total_turrets_placed++;  // Track turret placement
                break;
            }
        }
    }
}

int GameManager::canPlaceTurret(int x, int y) {
    if (x < 0 || x >= 20 || y < 0 || y >= 11) return 0;
    
    if (turret_grid[x][y]) return 0;
    
    // 타워가 차지할 격자 위치 확인 (1x1 타일)
    int k;
    // 각 웨이포인트 경로 확인
    for (k = 0; k < waypoint_count - 1; k++) {
        int gx1 = waypoints[k].x;
        int gy1 = waypoints[k].y;
        int gx2 = waypoints[k+1].x;
        int gy2 = waypoints[k+1].y;
        
        if (gx1 == gx2) {
            // 수직 경로
            int gymin = gy1 < gy2 ? gy1 : gy2;
            int gymax = gy1 > gy2 ? gy1 : gy2;
            if (x == gx1 && y >= gymin && y <= gymax) {
                return 0;
            }
        } else {
            // 수평 경로
            int gxmin = gx1 < gx2 ? gx1 : gx2;
            int gxmax = gx1 > gx2 ? gx1 : gx2;
            if (y == gy1 && x >= gxmin && x <= gxmax) {
                return 0;
            }
        }
    }
    
    return 1;
}

int GameManager::getTurretCost(TurretType type) {
    switch (type) {
        case TURRET_525_DISK: return 8;    // 기본 터렛 - 저렴하고 기본적
        case TURRET_XT: return 15;         // 8 * 1.875 = 15 (87.5% 성능 향상)
        case TURRET_AT: return 28;         // 15 * 1.87 = 28 (저격형, 긴 사거리)
        case TURRET_35_DISK: return 52;    // 28 * 1.86 = 52 (범용형)
        case TURRET_486: return 98;        // 52 * 1.88 = 98 (고속 연사)
        case TURRET_CD: return 184;        // 98 * 1.88 = 184 (다방향 특수)
        case TURRET_PENTIUM: return 345;   // 184 * 1.88 = 345 (관통 최고급)
        default: return 8;
    }
}

int GameManager::getTurretDamage(TurretType type) {
    switch (type) {
        case TURRET_525_DISK: return 8;
        case TURRET_XT: return 12;
        case TURRET_AT: return 20;
        case TURRET_35_DISK: return 16;
        case TURRET_486: return 20;
        case TURRET_CD: return 18;
        case TURRET_PENTIUM: return 24;
        default: return 8;
    }
}

int GameManager::getTurretRange(TurretType type) {
    switch (type) {
        case TURRET_525_DISK: return 35;
        case TURRET_XT: return 45;
        case TURRET_AT: return 75;
        case TURRET_35_DISK: return 55;
        case TURRET_486: return 60;
        case TURRET_CD: return 50;
        case TURRET_PENTIUM: return 65;
        default: return 35;
    }
}

float GameManager::getTurretFireSpeed(TurretType type) {
    switch (type) {
        case TURRET_525_DISK: return 1.2f;
        case TURRET_XT: return 1.0f;
        case TURRET_AT: return 1.5f;
        case TURRET_35_DISK: return 0.9f;
        case TURRET_486: return 0.7f;
        case TURRET_CD: return 1.1f;
        case TURRET_PENTIUM: return 0.8f;
        default: return 1.0f;
    }
}


void GameManager::addPoints(int amount) {
    points += amount;
    total_gold_earned += amount;  // 누적 골드 기록
}

void GameManager::loseLife() {
    life--;
}

int GameManager::getTextWidth(char* text) {
    int width = 0;
    unsigned char* p = (unsigned char*)text;
    
    while (*p) {
        if (*p & 0x80) {  // 한글 (최상위 비트가 1)
            width += 16;
            p += 2;  // 한글은 2바이트
        } else {  // 영문
            width += 8;
            p++;
        }
    }
    
    return width;
}

void GameManager::printCentered(int x1, int x2, int y, BYTE color, char* text) {
    int textWidth = getTextWidth(text);
    int centerX = (x1 + x2) / 2;
    int textX = centerX - textWidth / 2;
    
    Printf_Han(textX, y, color, 0, NORMAL_FONT, text);
}

void GameManager::drawCursor() {
    int gx = (cursor_x - GRID_START_X) / TILE_SIZE;
    int gy = (cursor_y - GRID_START_Y) / TILE_SIZE;
    
    if (gx >= 0 && gx < 20 && gy >= 0 && gy < 11) {
        // 현재 커서 위치에 터렛이 있는지 확인
        int turretFound = 0;
        Turret* hoveredTurret = NULL;
        
        for (int i = 0; i < MAX_TURRETS; i++) {
            if (turrets[i]) {
                int tx = turrets[i]->getX();
                int ty = turrets[i]->getY();
                
                // 터렛 영역 체크 (16x16 픽셀)
                if (cursor_x >= tx && cursor_x < tx + TILE_SIZE && 
                    cursor_y >= ty && cursor_y < ty + TILE_SIZE) {
                    hoveredTurret = turrets[i];
                    turretFound = 1;
                    break;
                }
            }
        }
        
        // 맵 커서 이미지 표시 (32x32 크기, 타일 중앙에 배치)
        int cursor_img_x = cursor_x - 8;  // 타일에서 8픽셀 빼서 중앙 정렬
        int cursor_img_y = cursor_y - 8;  // 타일에서 8픽셀 빼서 중앙 정렬

        if (turretFound && hoveredTurret) {
            // 터렛 위에 커서가 있으면 사거리 표시 + 배치 가능한 맵 커서
            drawTurretRange(hoveredTurret->getX(), hoveredTurret->getY(),
                          hoveredTurret->getRange());
            sprites* cursor_img = GetMapCursorImage();
            if (cursor_img && cursor_img->image) {
                RPut_Spr(cursor_img_x, cursor_img_y, cursor_img, 0);
            }
        } else {
            // 빈 타일에서는 배치 가능/불가능에 따라 다른 커서 표시
            if (canPlaceTurret(gx, gy)) {
                sprites* cursor_img = GetMapCursorImage();
                if (cursor_img && cursor_img->image) {
                    RPut_Spr(cursor_img_x, cursor_img_y, cursor_img, 0);
                }
            } else {
                sprites* cursor_img = GetMapCursor2Image();
                if (cursor_img && cursor_img->image) {
                    RPut_Spr(cursor_img_x, cursor_img_y, cursor_img, 0);
                }
            }
        }
    }
}

void GameManager::startStageIntro() {
    writeLog("startStageIntro() called - setting state to STATE_STAGE_INTRO");
    currentState = STATE_STAGE_INTRO;
    stage_intro_timer = 3.0f;
    setupStage(stage);

    // 4스테이지마다 음악 변경
    if ((stage - 1) % 4 == 0) {  // 1, 5, 9, 13, 17 스테이지에서 음악 변경
        int availableMusic[] = {0, 1, 2, 4, 9, 10, 11, 12, 13};  // 7번 제외
        int randomIndex = rand() % 9;  // 0~8 랜덤 선택
        int newMusic = availableMusic[randomIndex];

        char logBuffer[100];
        sprintf(logBuffer, "Stage %d: Random music changed to %d", stage, newMusic);
        writeLog(logBuffer);

        Mus_End();  // 기존 음악 정지
        Mus_Play("PUYO.MUS", newMusic);  // 새 음악 재생
    }

    writeLog("startStageIntro() finished");
}

void GameManager::updateStageIntro(float deltaTime) {
    static int logCounter = 0;

    // 10번에 한 번만 로그 (로그 스팸 방지)
    if (logCounter++ % 10 == 0) {
        char logBuffer[100];
        sprintf(logBuffer, "updateStageIntro: deltaTime=%.3f, timer=%.3f", deltaTime, stage_intro_timer);
        writeLog(logBuffer);
    }

    stage_intro_timer -= deltaTime;

    if (stage_intro_timer <= 0) {
        writeLog("Stage intro timer expired, switching to STATE_GAME_PLAY");
        currentState = STATE_GAME_PLAY;
        stage_in_progress = 1;
        enemies_spawned = 0;
        spawn_timer = 0;
    }
}

void GameManager::drawDifficultySelect() {
    Work_Clear(0);

    // 난이도 선택 박스 (위아래로 4픽셀씩 확장)
    DrawStandardWindow(70, 46, 260, 164);

    // 제목 (한국어로)
    printCentered(70, 260, 68, 45, DIFFICULTY_SELECT_TITLE);

    // 난이도 옵션들 (한국어로, 제목과 4픽셀 더 간격)
    char* difficulties[] = {DIFFICULTY_EASY, DIFFICULTY_NORMAL, DIFFICULTY_HARD};

    for (int i = 0; i < 3; i++) {
        int y = 94 + i * 15;  // 90에서 94로 4픽셀 아래로
        BYTE color = (i == difficulty_selection) ? 8 : 19; // 선택된 항목은 노란색, 나머지는 흰색

        printCentered(70, 260, y, color, difficulties[i]);
    }
}

void GameManager::runDifficultySelect() {
    writeLog("runDifficultySelect() started");
    char page = 0;

    View_Page(0);
    int key = 0;

    Restore_Key();
    Multi_Clear();
    writeLog("About to enter key input loop");

    while ((key = Get_Key()) != 0) {
        writeLog("Key pressed in loop");
        // 키 입력 체크
        if (key == UP) {
            difficulty_selection--;
            if (difficulty_selection < 0) difficulty_selection = 2;

            // 다음 페이지에 그리기
            page = !page;
            Active_Page(page);
            drawDifficultySelect();
            View_Page(page);
        }

        if (key == DOWN) {
            difficulty_selection++;
            if (difficulty_selection > 2) difficulty_selection = 0;

            // 다음 페이지에 그리기
            page = !page;
            Active_Page(page);
            drawDifficultySelect();
            View_Page(page);
        }

        if (key == ENTER) {
            writeLog("ENTER key pressed - starting game");
            Fade_Out(0, 255, 10);
            Page_Clear(0, 0);
            Page_Clear(1, 0);

            // Fade Out 후 화면 지우기
            Active_Page(0);
            Work_Clear(0);
            Active_Page(1);
            Work_Clear(0);

            // 난이도 설정
            switch (difficulty_selection) {
                case 0: setDifficulty(DIFF_EASY); writeLog("Set difficulty to EASY"); break;
                case 1: setDifficulty(DIFF_NORMAL); writeLog("Set difficulty to NORMAL"); break;
                case 2: setDifficulty(DIFF_HARD); writeLog("Set difficulty to HARD"); break;
            }

            // 게임 시작
            writeLog("Setting state to STATE_GAME_PLAY");
            setState(STATE_GAME_PLAY);
            writeLog("Calling startGame()");
            Multi_Key();  // 멀티키 다시 활성화
            Multi_Clear();
            startGame();
            writeLog("Returning from runDifficultySelect()");
            return;
        }

        if (key == ESC) {
            // ESC로 메뉴로 돌아가기
            writeLog("ESC key pressed - returning to menu");
            Fade_Out(0, 255, 10);
            Page_Clear(0, 0);
            Page_Clear(1, 0);

            // Fade Out 후 화면 지우기
            Active_Page(0);
            Work_Clear(0);
            Active_Page(1);
            Work_Clear(0);

            Multi_Key();  // 멀티키 다시 활성화 (메뉴에서도 필요할 수 있음)
            Multi_Clear();
            setState(STATE_MENU);
            return;
        }
    }
    writeLog("Exited key input loop - no key pressed, function ending");
}

void GameManager::drawStageIntro() {
    drawGameBackground();
    drawGameSorted();  // 터렛, 미사일, 폭발 표시
    drawUI();  // UI 표시 (생명력, 포인트)

    // 스테이지 정보 박스 (새로운 레이아웃을 위해 높이 조정)
    DrawStandardWindow(92, 30, 228, 190);
    
    // 스테이지 번호 (중앙 정렬)
    char buf[50];
    sprintf(buf, "%s %d", STAGE_PREFIX, stage);
    printCentered(92, 228, 50, 45, buf);
    
    // 1. 적 이름을 최상단에 배치
    StageEnemy* tempEnemy = new StageEnemy(0, 0, stage);  // 위치는 임시
    char* enemyName = tempEnemy->getName();
    printCentered(92, 228, 69, 8, enemyName);  // 상단에 적 이름
    
    // 캐릭터 이미지를 화면 중앙에 배치
    delete tempEnemy;
    int center_x = 160;  // 화면 가로 중앙 (320/2)
    int center_y = 118;   // 2픽셀 위로 (기존 120에서 118로)
    tempEnemy = new StageEnemy(center_x, center_y, stage);
    tempEnemy->draw();  // 기본 모드(정보창 모드)로 그리기
    delete tempEnemy;
    
    // 바 그래프와 P, C 정보는 제거됨
    
    // 5. 하단 중앙에 4x8 입체 박스 3개로 대기시간 표시
    int box_count = 3;
    int box_width = 4;
    int box_spacing = 2;  // 박스 간 간격
    int total_width = box_count * box_width + (box_count - 1) * box_spacing;
    int boxes_x = 92 + (136 - total_width) / 2;  // 중앙 정렬 (새로운 창 너비에 맞춤)
    int boxes_y = 155;
    
    // 3초를 3개 박스로 나누어 표시 (각 박스당 1초)
    float time_per_box = 1.0f;
    int lit_boxes = (int)(stage_intro_timer / time_per_box) + 1;  // 켜진 박스 수
    if (lit_boxes > box_count) lit_boxes = box_count;
    if (lit_boxes < 0) lit_boxes = 0;
    
    for (int i = 0; i < box_count; i++) {
        int box_x = boxes_x + i * (box_width + box_spacing);
        
        if (i < lit_boxes) {
            // 켜진 박스 (LIGHT.PCX 스프라이트)
            sprites* light_spr = GetLightImage();
            if (light_spr && light_spr->image) {
                RPut_Spr(box_x, boxes_y, light_spr, 0);
            }
        } else {
            // 꺼진 박스 (DARK.PCX 스프라이트)
            sprites* dark_spr = GetDarkImage();
            if (dark_spr && dark_spr->image) {
                RPut_Spr(box_x, boxes_y, dark_spr, 0);
            }
        }
    }
}

void GameManager::setupStage(int stageNum) {
    // 스테이지별 적 수 설정 (10개부터 시작해서 20개까지 증가)
    // 1단계: 10명, 20단계: 20명
    enemies_per_stage = 10 + ((stageNum - 1) * 10) / 19;
    if (enemies_per_stage > 20) enemies_per_stage = 20;  // 최대 20마리
}

void GameManager::getStageEnemyStats(int stage, float& speed, int& health, int& points) {
    // StageEnemy 클래스와 동일한 계산식 사용 (밸런스 배율 적용)
    speed = 0.3f + (stage - 1) * 0.06f;
    health = (50 + (stage - 1) * 24) * ENEMY_HEALTH_MULTIPLIER;
    // 포인트: 로그 스케일 적용으로 후반 인플레이션 방지
    points = (int)((8 + stage * 2 + (stage - 1) * (stage - 1) * 0.3f) * ENEMY_POINTS_MULTIPLIER);
}

void GameManager::drawTurretRange(int tx, int ty, int range) {
    // 터렛 중심에서 사거리 원을 그림 (반투명 효과를 위해 점선으로)
    int cx = tx + 8;  // 터렛 중심 X
    int cy = ty + 8;  // 터렛 중심 Y
    
    // 사거리 원을 점선으로 그리기 (반투명 효과)
    CCircle(cx, cy, range, 9);  // 노란색으로 사거리 원 표시
}

void GameManager::drawEnemyInfoBar(int x, int y, int value, int maxValue, int color) {
    int bar_width = 60;
    int bar_height = 7;  // 3에서 7로 변경
    
    // 배경 (검정)
    CBar(x, y, x + bar_width, y + bar_height, 0);
    // 테두리 (회색)
    CBox(x, y, x + bar_width, y + bar_height, 15);
    
    // 프로그레스 바 채우기
    if (value > 0 && maxValue > 0) {
        int filled_width = (bar_width * value) / maxValue;
        if (filled_width > bar_width) filled_width = bar_width;
        if (filled_width > 0) {
            CBar(x + 1, y + 1, x + filled_width - 1, y + bar_height - 1, color);
        }
    }
}

void GameManager::getRelativeEnemyStats(int stage, int& health, float& speed, int& points, int& count) {
    // 현재 스테이지 적 스탯 계산
    speed = 0.3f + (stage - 1) * 0.06f;
    health = (int)((50 + (stage - 1) * 30 + (stage - 1) * (stage - 1) * 1.5f) * ENEMY_HEALTH_MULTIPLIER);
    // 포인트: 1스테이지는 원래대로, 후반은 적게 주기
    points = (int)((8 + stage + (stage - 1) * (stage - 1) * 0.2f) * ENEMY_POINTS_MULTIPLIER);
    count = enemies_per_stage;
}

void GameManager::drawHealthIcon(int x, int y) {
    // 9x9 하트 아이콘 더미 (빨간색 사각형)
    CBar(x, y, x + 8, y + 8, 12);  // 빨간색 배경 (8에서 8로 변경)
    CBox(x, y, x + 8, y + 8, 15);  // 흰색 테두리
    // 간단한 하트 모양 패턴
    CBar(x + 2, y + 2, x + 6, y + 6, 4);  // 중앙 어두운 빨간색 (크기 증가)
}

void GameManager::drawSpeedIcon(int x, int y) {
    // 9x9 번개 아이콘 더미 (파란색 사각형)
    CBar(x, y, x + 8, y + 8, 11);   // 파란색 배경 (8에서 8로 변경)
    CBox(x, y, x + 8, y + 8, 15);  // 흰색 테두리
    // 간단한 번개 모양 패턴
    CBar(x + 3, y + 1, x + 5, y + 7, 11);  // 중앙 세로선 (연한 파랑, 크기 증가)
    CBar(x + 1, y + 3, x + 7, y + 5, 11);  // 중앙 가로선 (연한 파랑, 크기 증가)
}

void GameManager::updateExplosions(float deltaTime) {
    int i;
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i] && explosions[i]->isActive()) {
            explosions[i]->update(deltaTime);

            // 폭발 애니메이션이 끝났으면 제거
            if (!explosions[i]->isActive()) {
                delete explosions[i];
                explosions[i] = NULL;
                explosion_count--;
            }
        }
    }
}

void GameManager::addExplosion(float x, float y) {
    int i;
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i] == NULL) {
            explosions[i] = new Explosion(x, y);
            explosion_count++;
            break;
        }
    }
}

float GameManager::getDifficultyDamageMultiplier() {
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

void GameManager::drawExitConfirm() {
    // 게임 화면을 배경으로 그리기
    drawGameBackground();
    drawGameSorted();
    drawUI();

    // 게임 종료 확인 박스 (하단 16픽셀 줄임)
    DrawStandardWindow(46, 60, 274, 154);

    // 제목
    printCentered(46, 274, 85, 45, EXIT_CONFIRM);

    // 선택 옵션들 (가로 배치) - 예/아니오 순서
    // 각 옵션의 너비 계산
    int yes_width = getTextWidth(SELECT_YES);
    int no_width = getTextWidth(SELECT_NO);
    int gap = 20;  // 옵션 사이 간격
    int total_width = yes_width + gap + no_width;

    // 중앙 정렬을 위한 시작 위치
    int start_x = 46 + (228 - total_width) / 2;  // 창 너비 228픽셀 기준
    int options_y = 110;

    // 예 출력
    BYTE yes_color = (exit_confirmation_selection == 0) ? 8 : 19;
    Printf_Han(start_x, options_y, yes_color, 0, NORMAL_FONT, SELECT_YES);

    // 아니오 출력
    BYTE no_color = (exit_confirmation_selection == 1) ? 8 : 19;
    Printf_Han(start_x + yes_width + gap, options_y, no_color, 0, NORMAL_FONT, SELECT_NO);
}

void GameManager::runExitConfirm() {
    char page = 0;

    // 기본값을 "아니오"로 설정 (index 1)
    exit_confirmation_selection = 1;

    View_Page(0);
    int key = 0;

    Restore_Key();
    Multi_Clear();

    // 키 입력이 있을 때까지 대기
    while (1) {
        key = Get_Key();
        if (key == 0) continue;  // 키가 없으면 계속 대기
        // 키 입력 체크 (좌우 키로 변경)
        if (key == LEFT) {
            exit_confirmation_selection--;
            if (exit_confirmation_selection < 0) exit_confirmation_selection = 1;

            // 다음 페이지에 그리기
            page = !page;
            Active_Page(page);
            drawExitConfirm();
            View_Page(page);
        }

        if (key == RIGHT) {
            exit_confirmation_selection++;
            if (exit_confirmation_selection > 1) exit_confirmation_selection = 0;

            // 다음 페이지에 그리기
            page = !page;
            Active_Page(page);
            drawExitConfirm();
            View_Page(page);
        }

        if (key == ENTER) {
            if (exit_confirmation_selection == 0) {
                // 예 선택 - 메인메뉴로 돌아가기
                Multi_Key();  // 멀티키 다시 활성화
                Multi_Clear();
                setState(STATE_MENU);
            } else {
                // 아니오 선택 - 게임으로 돌아가기
                Multi_Key();  // 멀티키 다시 활성화
                Multi_Clear();
                setState(STATE_GAME_PLAY);
            }
            return;
        }

        if (key == ESC) {
            // ESC로 게임으로 돌아가기
            Multi_Key();  // 멀티키 다시 활성화
            Multi_Clear();
            setState(STATE_GAME_PLAY);
            return;
        }
    }
}


// Utility function for consistent window drawing using nine-patch dialog images
void DrawStandardWindow(int x1, int y1, int x2, int y2) {
    int width = x2 - x1 + 1;
    int height = y2 - y1 + 1;

    // Get dialog patch images
    sprites* dialog0 = GetDialogImage(0);  // Top-left (36x40)
    sprites* dialog1 = GetDialogImage(1);  // Top-center (8x40)
    sprites* dialog2 = GetDialogImage(2);  // Top-right (34x40)
    sprites* dialog3 = GetDialogImage(3);  // Middle-left (36x8)
    sprites* dialog4 = GetDialogImage(4);  // Middle-center (8x8)
    sprites* dialog5 = GetDialogImage(5);  // Middle-right (34x8)
    sprites* dialog6 = GetDialogImage(6);  // Bottom-left (36x50)
    sprites* dialog7 = GetDialogImage(7);  // Bottom-center (8x50)
    sprites* dialog8 = GetDialogImage(8);  // Bottom-right (34x40)

    if (!dialog0 || !dialog1 || !dialog2 || !dialog3 || !dialog4 ||
        !dialog5 || !dialog6 || !dialog7 || !dialog8) {
        // Fallback to old method if images not loaded
        CWin_3dB(x1, y1, x2, y2, 0, 15, 7, 3, 1);
        return;
    }

    // Nine-patch dimensions
    int corner_left_w = 36;
    int corner_right_w = 34;
    int corner_top_h = 40;
    int corner_bottom_left_h = 50;  // dialog6 height
    int corner_bottom_right_h = 40; // dialog8 height

    // Calculate stretch areas
    int stretch_w = width - corner_left_w - corner_right_w;
    int stretch_h = height - corner_top_h - corner_bottom_left_h;  // Left side stretch height
    int right_stretch_h = height - corner_top_h - corner_bottom_right_h;  // Right side stretch height

    // Draw corners (no stretching)
    Put_Spr(x1, y1, dialog0, 0);  // Top-left
    Put_Spr(x2 - corner_right_w + 1, y1, dialog2, 0);  // Top-right
    Put_Spr(x1, y2 - corner_bottom_left_h + 1, dialog6, 0);  // Bottom-left
    Put_Spr(x2 - corner_right_w + 1, y2 - corner_bottom_right_h + 1, dialog8, 0);  // Bottom-right

    // Draw horizontal edges (stretch horizontally)
    if (stretch_w > 0) {
        // Top edge
        ZPut_Spr(x1 + corner_left_w, y1, dialog1, stretch_w, corner_top_h, 0);
        // Bottom edge (dialog7 is 50px high)
        ZPut_Spr(x1 + corner_left_w, y2 - corner_bottom_left_h + 1, dialog7, stretch_w, corner_bottom_left_h, 0);
    }

    // Draw vertical edges (stretch vertically)
    if (stretch_h > 0) {
        // Left edge
        ZPut_Spr(x1, y1 + corner_top_h, dialog3, corner_left_w, stretch_h, 0);
    }

    if (right_stretch_h > 0) {
        // Right edge
        ZPut_Spr(x2 - corner_right_w + 1, y1 + corner_top_h, dialog5, corner_right_w, right_stretch_h, 0);
    }

    // Draw center (stretch both directions)
    // Use the minimum stretch height to ensure proper fitting
    int center_stretch_h = (stretch_h < right_stretch_h) ? stretch_h : right_stretch_h;
    if (stretch_w > 0 && center_stretch_h > 0) {
        ZPut_Spr(x1 + corner_left_w, y1 + corner_top_h, dialog4, stretch_w, center_stretch_h, 0);
    }
}

int GameManager::findTurretAt(int grid_x, int grid_y) {
    // 그리드 좌표를 픽셀 좌표로 변환
    int pixel_x = GRID_START_X + grid_x * TILE_SIZE;
    int pixel_y = GRID_START_Y + grid_y * TILE_SIZE;

    // 모든 터렛을 검사하여 해당 위치에 있는 터렛 찾기
    for (int i = 0; i < MAX_TURRETS; i++) {
        if (turrets[i]) {
            int tx = turrets[i]->getX();
            int ty = turrets[i]->getY();

            // 터렛 위치와 정확히 일치하는지 확인
            if (tx == pixel_x && ty == pixel_y) {
                return i;
            }
        }
    }

    return -1; // 터렛이 없음
}

void GameManager::sellTurret(int turret_index) {
    if (turret_index < 0 || turret_index >= MAX_TURRETS || !turrets[turret_index]) {
        return;
    }

    // 터렛 타입을 터렛 속성으로 추정 (범위 기반)
    TurretType turret_type = (TurretType)TURRET_525_DISK; // 기본값
    int range = turrets[turret_index]->getRange();

    // 각 터렛의 고유한 사거리로 타입 식별
    if (range == 35) {
        turret_type = (TurretType)TURRET_525_DISK;
    } else if (range == 45) {
        turret_type = (TurretType)TURRET_XT;
    } else if (range == 75) {
        turret_type = (TurretType)TURRET_AT;
    } else if (range == 55) {
        turret_type = (TurretType)TURRET_35_DISK;
    } else if (range == 60) {
        turret_type = (TurretType)TURRET_486;
    } else if (range == 50) {
        turret_type = (TurretType)TURRET_CD;
    } else if (range == 65) {
        turret_type = (TurretType)TURRET_PENTIUM;
    }

    // 터렛 비용의 30% 환급
    int refund = (getTurretCost(turret_type) * 30) / 100;
    addPoints(refund);

    // 터렛 위치에 폭발 애니메이션 추가
    addExplosion(turrets[turret_index]->getX() + 8, turrets[turret_index]->getY() + 8);

    // 그리드에서 터렛 제거
    int grid_x = (turrets[turret_index]->getX() - GRID_START_X) / TILE_SIZE;
    int grid_y = (turrets[turret_index]->getY() - GRID_START_Y) / TILE_SIZE;
    turret_grid[grid_x][grid_y] = 0;

    // 터렛 삭제
    delete turrets[turret_index];
    turrets[turret_index] = NULL;
    turret_count--;
    total_turrets_sold++;  // Track turret sale
}

void GameManager::drawTurretSell() {
    // 게임 화면은 이미 그려져 있으므로 팝업만 그림

    // 터렛 판매 확인 박스 (세로 8픽셀 줄임)
    DrawStandardWindow(46, 60, 274, 172);

    // 확인 메시지
    printCentered(46, 274, 85, 45, TURRET_SELL_CONFIRM);

    // 판매 가격 표시
    if (selling_turret_index >= 0 && turrets[selling_turret_index]) {
        // 터렛 타입을 터렛 속성으로 추정 (범위 기반)
        TurretType turret_type = (TurretType)TURRET_525_DISK; // 기본값
        int range = turrets[selling_turret_index]->getRange();

        // 각 터렛의 고유한 사거리로 타입 식별
        if (range == 35) {
            turret_type = (TurretType)TURRET_525_DISK;
        } else if (range == 45) {
            turret_type = (TurretType)TURRET_XT;
        } else if (range == 75) {
            turret_type = (TurretType)TURRET_AT;
        } else if (range == 55) {
            turret_type = (TurretType)TURRET_35_DISK;
        } else if (range == 60) {
            turret_type = (TurretType)TURRET_486;
        } else if (range == 50) {
            turret_type = (TurretType)TURRET_CD;
        } else if (range == 65) {
            turret_type = (TurretType)TURRET_PENTIUM;
        }

        int refund = (getTurretCost(turret_type) * 30) / 100;

        // 코인 아이콘과 숫자 표시 (상태바 스타일)
        sprites* coin_img = GetCoinImage();
        char refund_str[10];
        sprintf(refund_str, "%d", refund);

        int text_width = strlen(refund_str) * 8;  // 숫자 한 글자당 8픽셀
        int coin_width = coin_img ? coin_img->x : 0;
        int total_width = coin_width + 4 + text_width;  // 코인 + 간격 4 + 텍스트

        // 중앙 정렬
        int start_x = 46 + (228 - total_width) / 2;  // 창 너비 228픽셀 기준
        int price_y = 110;  // 5픽셀 아래로

        // 코인 이미지 출력
        if (coin_img) {
            RPut_Spr(start_x, price_y, coin_img, 0);
        }

        // 숫자 출력
        int text_x = start_x + coin_width + 4;
        Printf_Han(text_x, price_y, 45, 0, NORMAL_FONT, refund_str);
    }

    // 선택 옵션들 (가로 배치) - 예가 먼저
    int yes_width = getTextWidth(SELECT_YES);
    int no_width = getTextWidth(SELECT_NO);
    int gap = 20;  // 옵션 사이 간격
    int total_width = yes_width + gap + no_width;

    // 중앙 정렬을 위한 시작 위치
    int start_x = 46 + (228 - total_width) / 2;  // 창 너비 228픽셀 기준
    int options_y = 135;  // 코인 정보에서 간격 유지

    // 예 출력 (왼쪽)
    BYTE yes_color = (turret_sell_selection == 0) ? 8 : 19;
    Printf_Han(start_x, options_y, yes_color, 0, NORMAL_FONT, SELECT_YES);

    // 아니오 출력 (오른쪽)
    BYTE no_color = (turret_sell_selection == 1) ? 8 : 19;
    Printf_Han(start_x + yes_width + gap, options_y, no_color, 0, NORMAL_FONT, SELECT_NO);
}

// runTurretSell 함수는 더이상 사용되지 않음 (INPUT_TURRET_SELL로 대체)
// void GameManager::runTurretSell() { ... }

void GameManager::drawGameClear() {
    // Game background
    drawGameBackground();
    drawGameSorted();
    drawUI();

    // Game clear window (위로 4픽셀, 아래로 12픽셀 늘림)
    DrawStandardWindow(40, 26, 280, 182);

    // Title with difficulty
    int title_y = 46;
    char title_text[100];
    char* difficulty_str;

    switch (CURRENT_DIFFICULTY) {
        case 0: difficulty_str = DIFFICULTY_EASY; break;
        case 1: difficulty_str = DIFFICULTY_NORMAL; break;
        case 2: difficulty_str = DIFFICULTY_HARD; break;
        default: difficulty_str = DIFFICULTY_NORMAL; break;
    }

    sprintf(title_text, "[%s] %s", difficulty_str, GAME_CLEAR_TEXT);
    printCentered(40, 280, title_y, 45, title_text);

    // Statistics section (6 pixel gap after title, 2픽셀 더 내림)
    int stats_start_y = title_y + 16 + 6;  // 16 for font height + 6 pixel gap
    int line_height = 18;  // 16 + 2픽셀 줄간격
    int stats_x = 55;      // 오른쪽으로 5픽셀 이동

    char label_buf[50], number_buf[20];

    // 누적 골드 (라벨: 8번 컬러, 숫자: 9번 컬러)
    Printf_Han(stats_x, stats_start_y, 8, 0, NORMAL_FONT, STAT_TOTAL_GOLD);
    sprintf(number_buf, " %d", total_gold_earned);
    Printf_Han(stats_x + strlen(STAT_TOTAL_GOLD) * 8, stats_start_y, 9, 0, NORMAL_FONT, number_buf);

    // 처치한 적
    Printf_Han(stats_x, stats_start_y + line_height, 8, 0, NORMAL_FONT, STAT_ENEMIES_KILLED);
    sprintf(number_buf, " %d", total_enemies_killed);
    Printf_Han(stats_x + strlen(STAT_ENEMIES_KILLED) * 8, stats_start_y + line_height, 9, 0, NORMAL_FONT, number_buf);

    // 남은 생명
    Printf_Han(stats_x, stats_start_y + line_height * 2, 8, 0, NORMAL_FONT, STAT_LIFE_REMAINING);
    sprintf(number_buf, " %d / 20", life);
    Printf_Han(stats_x + strlen(STAT_LIFE_REMAINING) * 8, stats_start_y + line_height * 2, 9, 0, NORMAL_FONT, number_buf);

    // 설치한 터렛
    Printf_Han(stats_x, stats_start_y + line_height * 3, 8, 0, NORMAL_FONT, STAT_TURRETS_PLACED);
    sprintf(number_buf, " %d", total_turrets_placed);
    Printf_Han(stats_x + strlen(STAT_TURRETS_PLACED) * 8, stats_start_y + line_height * 3, 9, 0, NORMAL_FONT, number_buf);

    // 판매한 터렛
    Printf_Han(stats_x, stats_start_y + line_height * 4, 8, 0, NORMAL_FONT, STAT_TURRETS_SOLD);
    sprintf(number_buf, " %d", total_turrets_sold);
    Printf_Han(stats_x + strlen(STAT_TURRETS_SOLD) * 8, stats_start_y + line_height * 4, 9, 0, NORMAL_FONT, number_buf);

    // Press ESC key message at bottom (위로 4픽셀, 45번 색상)
    printCentered(0, 320, 176, 45, PRESS_ESC_KEY);
}