#include "sed.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include "menu.h"
#include "help.h"
#include "gstate.h"
#include "map.h"
#include "drawing.h"
#include "STR_KSSM.H"

// 로그 시스템 전역 변수
int gIsLogEnabled = 0;

void init();
void cleanup();
void gameLoop();

void writeLog(const char* message) {
    if (!gIsLogEnabled) {
        return;
    }

    FILE* logFile = fopen("dmtd.log", "a");
    if (logFile != NULL) {
        time_t now = time(0);
        struct tm* timeinfo = localtime(&now);

        fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
                timeinfo->tm_year + 1900,
                timeinfo->tm_mon + 1,
                timeinfo->tm_mday,
                timeinfo->tm_hour,
                timeinfo->tm_min,
                timeinfo->tm_sec,
                message);

        fclose(logFile);
    }
}

int main() {
  init();

  writeLog("Game started");

  gameManager = new GameManager();

  gameLoop();
  
  delete gameManager;
  cleanup();

  writeLog("Game ended");

  return 0;
}

void init() {
  randomize();
  Vga_Plane();
  Load_Font("hangul.fnt","english.fnt");
  Decode_Init(64);
  initMap();

  // 음악 재생 설정
  Mus_Continue = 1;  // 계속 반복 재생
}

void cleanup() {
  Mus_End();  // 음악 종료
  Decode_End();
  Text_Mode();
}

void gameLoop() {
  char page = 0;
  char logBuffer[100];
  static int lastState = -1;
  static int currentMusic = -1;  // 현재 재생 중인 음악 번호

  while (gameManager->getState() != STATE_EXIT) {
    int currentState = gameManager->getState();

    // 상태가 변경될 때만 로그 및 음악 변경
    if (currentState != lastState) {
      Multi_Clear();
      
      sprintf(logBuffer, "gameLoop: State changed from %d to %d", lastState, currentState);
      writeLog(logBuffer);

      // 상태에 따라 음악 변경
      int newMusic = -1;
      switch (currentState) {
        case STATE_MENU:
        case STATE_DIFFICULTY_SELECT:
          newMusic = 8;  // 메인메뉴 음악
          break;
        case STATE_HELP:
          newMusic = 3;  // 도움말 음악
          break;
        case STATE_GAME_PLAY:
          Multi_Key();
        case STATE_STAGE_INTRO:
        case STATE_EXIT_CONFIRM:
          // 게임 플레이 중에는 랜덤 음악 (0~13 중 3,5,6,8 제외)
          if (lastState != STATE_GAME_PLAY && lastState != STATE_STAGE_INTRO &&
              lastState != STATE_EXIT_CONFIRM) {
            // 게임 플레이로 처음 진입할 때만 랜덤 선택
            // int availableMusic[] = {0, 1, 2, 4, 7, 9, 10, 11, 12, 13};
            int availableMusic[] = {0, 1, 2, 4, 9, 10, 11, 12, 13};  // 7번 제외
            int randomIndex = rand() % 10;
            newMusic = availableMusic[randomIndex];

            sprintf(logBuffer, "Selected random game music: %d", newMusic);
            writeLog(logBuffer);
          } else {
            // 게임 플레이 중 상태 전환은 음악 유지
            newMusic = currentMusic;
          }
          break;
        case STATE_GAME_OVER:
          newMusic = 5;  // 게임 오버 음악
          break;
        case STATE_GAME_CLEAR:
          newMusic = 6;  // 게임 클리어 음악
          break;
      }

      // 음악이 변경되어야 하면 재생
      if (newMusic != -1 && newMusic != currentMusic) {
        sprintf(logBuffer, "Changing music from %d to %d", currentMusic, newMusic);
        writeLog(logBuffer);
        Mus_End();  // 기존 음악 정지
        Mus_Play("PUYO.MUS", newMusic);  // 새 음악 재생
        currentMusic = newMusic;
      }

      // 이제 lastState 업데이트
      lastState = currentState;
    }

    switch (gameManager->getState()) {
      case STATE_MENU:
        Active_Page(0);
        drawMenu(0);
        Fade(0, 255, 255);
        runMenu();
        break;
        
      case STATE_HELP:
        Active_Page(1);
        drawHelp();
        Fade(0, 255, 255);
        runHelp();
        gameManager->setState(STATE_MENU);
        break;


      case STATE_DIFFICULTY_SELECT:
        writeLog("Entering STATE_DIFFICULTY_SELECT");
        Active_Page(0);
        gameManager->drawDifficultySelect();
        Fade(0, 255, 255);
        writeLog("About to call runDifficultySelect()");
        gameManager->runDifficultySelect();
        writeLog("Returned from runDifficultySelect()");
        // 상태가 변경되었으면 바로 다음 상태로 진행
        continue;
        
      case STATE_GAME_PLAY:
        Active_Page(page);

        gameManager->update();
        gameManager->draw();
        gameManager->handleInput();

        View_Page(page);
        page = !page;

        break;
        
      case STATE_STAGE_INTRO:
        Active_Page(page);

        gameManager->update();
        gameManager->draw();

        View_Page(page);
        page = !page;

        break;
        
      case STATE_EXIT_CONFIRM:
        Active_Page(0);
        gameManager->drawExitConfirm();
        View_Page(0);
        gameManager->runExitConfirm();
        break;


      case STATE_GAME_OVER:
        Active_Page(0);
        Work_Clear(0);

        gameManager->printCentered(0, 319, 80, 8, GAME_OVER_TEXT);
        gameManager->printCentered(0, 319, 100, 19, PRESS_ESC_KEY);

        View_Page(0);

        Multi_Clear();
        Restore_Key();
        // ESC 키만 허용
        while (1) {
            int key = Get_Key();
            if (key == ESC) {
                break;
            }
        }

        gameManager->setState(STATE_MENU);
        break;
        
      case STATE_GAME_CLEAR:
        Active_Page(0);
        gameManager->drawGameClear();
        View_Page(0);

        // ESC 키만 허용
        Restore_Key();
        Multi_Clear();
        while (1) {
            int key = Get_Key();
            if (key == ESC) {
                break;
            }
        }

        Multi_Key();  // 멀티키 다시 활성화
        gameManager->setState(STATE_MENU);
        break;
    }
  }
}
