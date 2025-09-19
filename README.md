# Dos Museum Turret Defense (DMTD)

도스박물관 레트로 타워 디펜스 게임

<img width="640" height="400" alt="Screenshot 2025-09-18 at 1<img width="640" height="400" alt="dmtd_1" src="https://github.com/user-attachments/assets/73f3f3e7-8013-48be-9a5a-9c3dc4842a8c" />
6 51 44" src="https://github.com/user-attachments/assets/803c733c-d0b4-4866-8b1b-4083deef3217" /><img width="640" height="400" alt="dmtd_2" src="https://github.com/user-attachments/assets/8e9c54f4-5de9-4d36-bc3f-57ffde346154" />
<img width="640" height="400" alt="dmtd_3" src="https://github.com/user-attachments/assets/2dc7be37-02bb-4dc4-8053-ed1d0ef02db8" />
<img width="640" height="400" alt="Screenshot 2025-09-18 at 16 41 39" src="https://github.com/user-attachments/assets/f882f464-1619-4ea8-a0f8-8a4ff9aa5e9a" />

<img width="640" height="400" alt="Screenshot 2025-09-18 at 16 53 29" src="https://github.com/user-attachments/assets/11212063-a009-4ccf-b2f1-eeed6217ea44" />



## 게임 소개

DMTD는 DOS 환경에서 실행되는 클래식 타워 디펜스 게임입니다. 골드를 모아 터렛을 설치하고, 밀려오는 적들로부터 기지를 방어하세요!

### 주요 특징

- 📺 VGA Mode 13h (320x200) 그래픽
- 🎮 총 20개의 스테이지
- 🖥️ 레트로 컴퓨터 테마 터렛 (5.25" 디스크, XT, AT, 486, CD-ROM, 펜티엄)
- 🎯 다양한 적 유형과 전략적 터렛 배치
- 🇰🇷 한글 지원

## 시스템 요구사항

- DOS 또는 DOSBox
- VGA 그래픽 카드
- 최소 640KB RAM

## 빌드 방법

Borland C++ 컴파일러가 필요합니다:

```bash
make clean
make
```

## 실행 방법

```bash
cd BUILD
DMTD.EXE
```

또는 DOSBox에서:

```bash
dosbox DMTD.EXE
```

## 게임 조작법

- **방향키**: 커서 이동
- **Enter**: 터렛 설치/선택
- **ESC**: 메뉴로 돌아가기

## 터렛 종류

1. **5.25" 디스크**: 기본 터렛
2. **XT**: 저렴한 속사 터렛
3. **AT**: 범위 공격 터렛
4. **386**: 밸런스형 터렛
5. **486**: 고급 터렛
6. **CD-ROM**: 레이저 터렛
7. **펜티엄**: 최강 터렛

## 개발 정보

- **제작자**: QuickBASIC (도스박물관)
- **언어**: C++ (Borland C++)
- **그래픽 라이브러리**: SED 3.05

## 기여

버그 리포트나 기능 제안은 [GitHub Issues](https://github.com/gcjjyy/dmtd/issues)를 이용해주세요.

## 라이센스

이 프로젝트는 MIT 라이센스 하에 배포됩니다. 자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

## 감사의 말

게임 개발에 도움을 주신 모든 분들께 감사드립니다:

하늘소, 피시키드, dosnavi, moonlord, 키노피오, 접공, 다윈의불독, Greatpsycho, 복룡, 서티브잡스, 실직양파, 컨커러, 화산폭발, Ospaggi, oldpc, 셔카, pgone, hachya, 최강맥둥이, 초귀요미아빠

---

**도스박물관** - 도스 시대의 추억을 간직하는 곳
