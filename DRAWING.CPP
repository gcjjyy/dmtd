#include "sed.h"
#include <string.h>
#include "drawing.h"
#include "map.h"
#include "gstate.h"
#include "STR_KSSM.H"
#include "images.h"

void drawMenu(int selection)
{
    // 배경 이미지 그리기
    sprites* bgImage = GetDosMuseumImage();
    if (bgImage && bgImage->image) {
        // RPut_Spr로 배경 이미지 그리기
        RPut_Spr(0, 0, bgImage, 0);
    } else {
        // 배경 이미지가 없으면 기본 배경색으로 채우기
        Page_Clear(0, 0);
    }

    // 타이틀 텍스트 "DOS MUSEUM TURRET DEFENSE" (그림자 효과)
    char title[] = "DOS MUSEUM TURRET DEFENSE";
    int title_x = 160 - (strlen(title) * 4); // 중앙 정렬 (각 문자 폭 8픽셀의 절반)

    // 그림자 (1픽셀 오프셋, 어두운 색)
    Printf_Han(title_x + 1, 31, 0, 0, NORMAL_FONT, (char*)title);
    // 메인 텍스트 (밝은 색)
    Printf_Han(title_x, 30, 8, 0, NORMAL_FONT, (char*)title);

    // 버전 정보
    char version[] = "v1.3";
    int version_x = 160 - (strlen(version) * 4); // 중앙 정렬
    Printf_Han(version_x, 50, 45, 0, NORMAL_FONT, (char*)version);

    // 메뉴 윈도우 (y좌표 8픽셀 위로, 크기 10픽셀 확장)
    DrawStandardWindow(60, 92, 270, 190);

    // 메뉴 항목들
    int menu_y = 115;  // y좌표 13픽셀 내림 (112 + 3)
    int menu_spacing = 18;

    // 게임 시작 (x좌표 13픽셀 추가)
    BYTE color = (selection == 0) ? 8 : 19;
    Printf_Han(123, menu_y, color, 0, NORMAL_FONT, MENU_START);

    // 도움말 (x좌표 13픽셀 추가)
    color = (selection == 1) ? 8 : 19;
    Printf_Han(123, menu_y + menu_spacing, color, 0, NORMAL_FONT, MENU_HELP);

    // 게임 종료 (x좌표 13픽셀 추가)
    color = (selection == 2) ? 8 : 19;
    Printf_Han(123, menu_y + menu_spacing * 2, color, 0, NORMAL_FONT, MENU_EXIT);
}
