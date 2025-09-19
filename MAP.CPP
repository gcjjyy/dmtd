#include "map.h"
#include "sed.h"

Waypoint waypoints[] = {
    {1, 0},     // Start - 왼쪽 상단 시작
    {1, 2},     // 아래로
    {8, 2},     // 오른쪽으로 긴 직선
    {8, 8},     // 아래로 긴 직선
    {2, 8},     // 왼쪽으로 긴 직선 
    {2, 5},     // 위로
    {12, 5},    // 오른쪽으로 긴 직선
    {12, 10},   // 아래로
    {16, 10},   // 오른쪽으로
    {16, 3},    // 위로 긴 직선
    {10, 3},    // 왼쪽으로
    {10, 1},    // 위로
    {19, 1}     // 오른쪽 끝점으로
};

int waypoint_count = 13;

void initMap() {
    sprites MapSprite;

    // Load and draw MAP.PCX
    Load_Pcx("MAP.PCX", &MapSprite);

    // 페이지 2에 맵을 미리 그려놓기
    Active_Page(2);
    Work_Clear(0);
    RPut_Spr(0, 0, &MapSprite, 0);

    Active_Page(0);
}

void drawGameBackground() {
    // 페이지 2에서 현재 페이지로 백그라운드 복사
    // 매번 RPut_Spr보다 훨씬 빠름
    char page = GActive_Page();
    Page_Copy(2, page);
}

void drawPath() {
    int i, x, y;
    
    // 모든 경로를 그립니다
    for (i = 0; i < waypoint_count - 1; i++) {
        int gx1 = waypoints[i].x;
        int gy1 = waypoints[i].y;
        int gx2 = waypoints[i+1].x;
        int gy2 = waypoints[i+1].y;
        
        if (gx1 == gx2) {
            // 수직 경로
            int gymin = gy1 < gy2 ? gy1 : gy2;
            int gymax = gy1 > gy2 ? gy1 : gy2;
            for (y = gymin; y <= gymax; y++) {
                int px = GRID_START_X + gx1 * TILE_SIZE;
                int py = GRID_START_Y + y * TILE_SIZE;
                CBar(px, py, px + TILE_SIZE - 1, py + TILE_SIZE - 1, 6);
            }
        } else {
            // 수평 경로
            int gxmin = gx1 < gx2 ? gx1 : gx2;
            int gxmax = gx1 > gx2 ? gx1 : gx2;
            for (x = gxmin; x <= gxmax; x++) {
                int px = GRID_START_X + x * TILE_SIZE;
                int py = GRID_START_Y + gy1 * TILE_SIZE;
                CBar(px, py, px + TILE_SIZE - 1, py + TILE_SIZE - 1, 6);
            }
        }
    }
    
    // 시작점과 끝점을 다른 색으로 표시
    int start_x = GRID_START_X + waypoints[0].x * TILE_SIZE;
    int start_y = GRID_START_Y + waypoints[0].y * TILE_SIZE;
    CBar(start_x + 2, start_y + 2, start_x + 13, start_y + 13, 10);  // 초록색 시작점
    
    int end_x = GRID_START_X + waypoints[waypoint_count-1].x * TILE_SIZE;
    int end_y = GRID_START_Y + waypoints[waypoint_count-1].y * TILE_SIZE;
    CBar(end_x + 2, end_y + 2, end_x + 13, end_y + 13, 12);  // 빨간색 끝점
}