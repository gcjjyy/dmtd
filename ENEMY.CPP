#include "enemy.h"
#include <stdio.h>
#include <string.h>
#include "STR_KSSM.H"

BasicEnemy::BasicEnemy(float x, float y) : Enemy(x, y, 0.5f, 100 * ENEMY_HEALTH_MULTIPLIER, 10 * ENEMY_POINTS_MULTIPLIER) {
    color = 14;
}

void BasicEnemy::draw() {
    draw(1);  // 기본 모드는 정보창 모드 (박스 중앙)
}

void BasicEnemy::draw(int mode) {
    int ix = (int)x;
    int iy = (int)y;

    if (mode == 0) {
        // 맵 모드: 타일 하단 - 8픽셀에 스프라이트 하단이 오도록 조정
        int sprite_bottom_y = iy + TILE_SIZE - 8 - 4;  // 타일 하단 - 8픽셀 - 스프라이트 하단에서 중심까지

        CBox_Fill(ix - 4, sprite_bottom_y - 4, ix + 4, sprite_bottom_y + 4, color);
        drawHealthBar(ix, sprite_bottom_y - 4, 8);
    } else {
        // 정보창 모드: 박스 중앙에 출력 (기존 방식)
        CBox_Fill(ix - 4, iy - 4, ix + 4, iy + 4, color);
        drawHealthBar(ix, iy - 4, 8);
    }
}

FastEnemy::FastEnemy(float x, float y) : Enemy(x, y, 1.0f, 50 * ENEMY_HEALTH_MULTIPLIER, 20 * ENEMY_POINTS_MULTIPLIER) {
    color = 11;
}

void FastEnemy::draw() {
    draw(1);  // 기본 모드는 정보창 모드 (박스 중앙)
}

void FastEnemy::draw(int mode) {
    int ix = (int)x;
    int iy = (int)y;

    if (mode == 0) {
        // 맵 모드: 타일 하단 - 8픽셀에 스프라이트 하단이 오도록 조정
        int sprite_bottom_y = iy + TILE_SIZE - 8 - 3;  // 타일 하단 - 8픽셀 - 스프라이트 하단에서 중심까지

        CBox_Fill(ix - 3, sprite_bottom_y - 3, ix + 3, sprite_bottom_y + 3, color);
        drawHealthBar(ix, sprite_bottom_y - 3, 6);
    } else {
        // 정보창 모드: 박스 중앙에 출력 (기존 방식)
        CBox_Fill(ix - 3, iy - 3, ix + 3, iy + 3, color);
        drawHealthBar(ix, iy - 3, 6);
    }
}

StrongEnemy::StrongEnemy(float x, float y) : Enemy(x, y, 0.3f, 200 * ENEMY_HEALTH_MULTIPLIER, 30 * ENEMY_POINTS_MULTIPLIER) {
    color = 4;
}

void StrongEnemy::draw() {
    draw(1);  // 기본 모드는 정보창 모드 (박스 중앙)
}

void StrongEnemy::draw(int mode) {
    int ix = (int)x;
    int iy = (int)y;

    if (mode == 0) {
        // 맵 모드: 타일 하단 - 8픽셀에 스프라이트 하단이 오도록 조정
        int sprite_bottom_y = iy + TILE_SIZE - 8 - 5;  // 타일 하단 - 8픽셀 - 스프라이트 하단에서 중심까지

        CBox_Fill(ix - 5, sprite_bottom_y - 5, ix + 5, sprite_bottom_y + 5, color);
        CBox(ix - 6, sprite_bottom_y - 6, ix + 6, sprite_bottom_y + 6, color + 8);
        drawHealthBar(ix, sprite_bottom_y - 6, 10);
    } else {
        // 정보창 모드: 박스 중앙에 출력 (기존 방식)
        CBox_Fill(ix - 5, iy - 5, ix + 5, iy + 5, color);
        CBox(ix - 6, iy - 6, ix + 6, iy + 6, color + 8);
        drawHealthBar(ix, iy - 6, 10);
    }
}

StageEnemy::StageEnemy(float x, float y, int stage) : Enemy(x, y, 0, 0, 0) {
    stage_num = stage;

    // 스테이지별 적 이름 설정
    switch(stage) {
        case 1: name = (char*)ENEMY_STAGE_1; break;
        case 2: name = (char*)ENEMY_STAGE_2; break;
        case 3: name = (char*)ENEMY_STAGE_3; break;
        case 4: name = (char*)ENEMY_STAGE_4; break;
        case 5: name = (char*)ENEMY_STAGE_5; break;
        case 6: name = (char*)ENEMY_STAGE_6; break;
        case 7: name = (char*)ENEMY_STAGE_7; break;
        case 8: name = (char*)ENEMY_STAGE_8; break;
        case 9: name = (char*)ENEMY_STAGE_9; break;
        case 10: name = (char*)ENEMY_STAGE_10; break;
        case 11: name = (char*)ENEMY_STAGE_11; break;
        case 12: name = (char*)ENEMY_STAGE_12; break;
        case 13: name = (char*)ENEMY_STAGE_13; break;
        case 14: name = (char*)ENEMY_STAGE_14; break;
        case 15: name = (char*)ENEMY_STAGE_15; break;
        case 16: name = (char*)ENEMY_STAGE_16; break;
        case 17: name = (char*)ENEMY_STAGE_17; break;
        case 18: name = (char*)ENEMY_STAGE_18; break;
        case 19: name = (char*)ENEMY_STAGE_19; break;
        case 20: name = (char*)ENEMY_STAGE_20; break;
        default: name = (char*)"UNKNOWN"; break;
    }

    // 전역 이미지 매니저에서 이미지 가져오기
    sprite_image = GetEnemyImage(stage - 1);  // 0-based index

    // 스테이지별 속도, 체력, 포인트 설정
    // 속도: 0.3 ~ 1.5 (스테이지 1-20)
    speed = 0.3f + (stage - 1) * 0.06f;

    // 체력: 1스테이지는 원래대로, 후반은 급격히 증가
    health = (int)((50 + (stage - 1) * 30 + (stage - 1) * (stage - 1) * 1.5f) * ENEMY_HEALTH_MULTIPLIER);
    max_health = health;

    // 포인트: 1스테이지는 원래대로, 후반은 적게 주기
    points = (int)((8 + stage + (stage - 1) * (stage - 1) * 0.2f) * ENEMY_POINTS_MULTIPLIER);

    // 색상: 스테이지별로 다양하게
    BYTE colors[] = {14, 10, 11, 12, 13, 9, 15, 6, 2, 3, 4, 5, 1, 7, 8, 14, 10, 11, 12, 4};
    color = colors[stage - 1];
}

// StageEnemy destructor removed - no need to free global images

char* StageEnemy::getName() const {
    return name;
}

void StageEnemy::draw() {
    draw(1);  // 기본 모드는 정보창 모드 (박스 중앙)
}

void StageEnemy::draw(int mode) {
    int ix = (int)x;
    int iy = (int)y;

    // 전역 이미지가 있으면 이미지를 그리고, 없으면 기존 박스 방식
    if (sprite_image) {
        int img_x = ix - sprite_image->x / 2;  // 가로 중앙 정렬
        int img_y;

        if (mode == 0) {
            // 맵 모드: 타일 하단 - 8픽셀에 스프라이트 하단이 오도록 위치 계산
            img_y = iy + TILE_SIZE - 8 - sprite_image->y;  // 타일 하단 - 8픽셀에서 스프라이트 높이만큼 위로
        } else {
            // 정보창 모드: 박스 중앙에 스프라이트 중앙이 오도록
            img_y = iy - sprite_image->y / 2;  // 중앙 정렬
        }

        RPut_Spr(img_x, img_y, sprite_image, 0);

        // 체력바 (이미지 위에)
        drawHealthBar(img_x + sprite_image->x / 2, img_y - 1, sprite_image->x);
    } else {
        // 기존 박스 방식
        int size = 4 + (stage_num - 1) / 5;
        int sprite_center_y;
        
        if (mode == 0) {
            // 맵 모드: 타일 하단 - 8픽셀에 스프라이트 하단이 오도록 조정
            sprite_center_y = iy + TILE_SIZE - 8 - size;  // 타일 하단 - 8픽셀 - 스프라이트 하단에서 중심까지
        } else {
            // 정보창 모드: 박스 중앙에 출력
            sprite_center_y = iy;
        }
        
        CBox_Fill(ix - size, sprite_center_y - size, ix + size, sprite_center_y + size, color);

        // 체력바
        drawHealthBar(ix, sprite_center_y - size, size * 2);
    }
}