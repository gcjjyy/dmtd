#include "sed.h"
#include <conio.h>
#include <string.h>
#include "help.h"
#include "drawing.h"
#include "gstate.h"
#include "STR_KSSM.H"

// 도움말 내용 배열과 컬러 정보
struct HelpLine {
    const char* text;
    int color;
};

// 전역 배열을 제거하고 함수 내부로 다시 이동

void drawHelp() {
    HelpLine help_lines[] = {
        {HELP_HEADER, 8},
        {HELP_SCROLL, 45},
        {HELP_GO_BACK, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_GAME_HEADER, 8},
        {HELP_GAME_DESC1, 45},
        {HELP_GAME_DESC2, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_CURSOR_MOVE, 45},
        {HELP_TURRET_ACTION, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_STAGE_INFO, 45},
        {HELP_SELL_INFO, 45},
        {HELP_SELL_INFO2, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_CREDITS_HEADER, 8},
        {HELP_AUTHOR, 45},
        {HELP_EMAIL, 45},
        {HELP_GITHUB, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_THANKS_HEADER, 8},
        {HELP_THANKS1, 45},
        {HELP_THANKS2, 45},
        {HELP_THANKS3, 45},
        {HELP_THANKS4, 45},
        {HELP_THANKS5, 45},
        {HELP_THANKS6, 45},
        {HELP_THANKS7, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_LICENSE_HEADER, 8},
        {HELP_LICENSE1, 45},
        {HELP_BUG_REPORT, 45},
        {HELP_BUG_REPORT2, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_DATE, 19}
    };

    int total_lines = sizeof(help_lines) / sizeof(HelpLine);
    int visible_lines = 10;  // 화면에 보이는 줄 수

    Page_Clear(1, 0);
    DrawStandardWindow(0, 0, 319, 199);

    // 도움말 내용 출력 (스크롤 없이 첫 화면만)
    for(int i = 0; i < visible_lines && i < total_lines; i++) {
        // "HELP_EMPTY_LINE" 문자열이면 출력하지 않음 (빈 줄 처리)
        if(strcmp(help_lines[i].text, "HELP_EMPTY_LINE") != 0) {
            Printf_Han(18, 20 + i * 16, help_lines[i].color, 0, 3,
                      (char*)help_lines[i].text);
        }
    }
}

void runHelp() {
    HelpLine help_lines[] = {
        {HELP_HEADER, 8},
        {HELP_SCROLL, 45},
        {HELP_GO_BACK, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_GAME_HEADER, 8},
        {HELP_GAME_DESC1, 45},
        {HELP_GAME_DESC2, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_CURSOR_MOVE, 45},
        {HELP_TURRET_ACTION, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_STAGE_INFO, 45},
        {HELP_SELL_INFO, 45},
        {HELP_SELL_INFO2, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_CREDITS_HEADER, 8},
        {HELP_AUTHOR, 45},
        {HELP_EMAIL, 45},
        {HELP_GITHUB, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_THANKS_HEADER, 8},
        {HELP_THANKS1, 45},
        {HELP_THANKS2, 45},
        {HELP_THANKS3, 45},
        {HELP_THANKS4, 45},
        {HELP_THANKS5, 45},
        {HELP_THANKS6, 45},
        {HELP_THANKS7, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_LICENSE_HEADER, 8},
        {HELP_LICENSE1, 45},
        {HELP_BUG_REPORT, 45},
        {HELP_BUG_REPORT2, 45},
        {"HELP_EMPTY_LINE", 15},
        {HELP_DATE, 19}
    };

    int scroll_pos = 0;
    int max_scroll = 0;
    int key;

    int total_lines = sizeof(help_lines) / sizeof(HelpLine);
    int visible_lines = 10;  // 화면에 보이는 줄 수
    max_scroll = (total_lines > visible_lines) ? total_lines - visible_lines : 0;
    int page = 0;

    Restore_Key();

    while(1) {
        Active_Page(page);
        Page_Clear(page, 0);
        DrawStandardWindow(0, 0, 319, 199);

        // 도움말 내용 출력
        for(int i = 0; i < visible_lines && (scroll_pos + i) < total_lines; i++) {
            // "HELP_EMPTY_LINE" 문자열이면 출력하지 않음 (빈 줄 처리)
            if(strcmp(help_lines[scroll_pos + i].text, "HELP_EMPTY_LINE") != 0) {
                Printf_Han(18, 20 + i * 16, help_lines[scroll_pos + i].color, 0, 3,
                          (char*)help_lines[scroll_pos + i].text);
            }
        }

        View_Page(page);

        key = Get_Key();

        switch(key) {
            case UP:  // UP 화살표
                if(scroll_pos > 0) scroll_pos--;
                break;
            case DOWN:  // DOWN 화살표
                if(scroll_pos < max_scroll) scroll_pos++;
                break;
            case ESC:  // ESC
                Fade_Out(0, 255, 10);
                Page_Clear(0, 0);
                Page_Clear(1, 0);
                return;
        }

        page = !page;
    }
}