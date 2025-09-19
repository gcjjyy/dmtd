#include "sed.h"
#include <conio.h>
#include "menu.h"
#include "drawing.h"
#include "gstate.h"
#include "STR_KSSM.H"

extern GameManager* gameManager;

void runMenu() {
    char page = 0;
    int selection = 0;
    
    View_Page(0);
    int key = 0;

    Restore_Key();
    
    while ((key = Get_Key()) != 0) {
        // 키 입력 체크
        if (key == UP) {
            selection--;
            if (selection < 0) selection = 2;

            // 다음 페이지에 그리기
            page = !page;
            Active_Page(page);
            drawMenu(selection);
            View_Page(page);
        }

        if (key == DOWN) {
            selection++;
            if (selection > 2) selection = 0;
            
            // 다음 페이지에 그리기
            page = !page;
            Active_Page(page);
            drawMenu(selection);
            View_Page(page);
        }
        
        if (key == ENTER) {
            Fade_Out(0, 255, 10);
            Page_Clear(0, 0);
            Page_Clear(1, 0);
            switch (selection) {
                case 0: // 게임 시작
                    gameManager->setState(STATE_DIFFICULTY_SELECT);
                    return;
                case 1: // 도움말
                    gameManager->setState(STATE_HELP);
                    return;
                case 2: // 게임 종료
                    gameManager->setState(STATE_EXIT);
                    return;
            }
        }
        
        if (key == ESC) {
            // ESC로도 게임 종료 가능
            Fade_Out(0, 255, 10);
            Page_Clear(0, 0);
            Page_Clear(1, 0);
            gameManager->setState(STATE_EXIT);
            return;
        }
    }
}