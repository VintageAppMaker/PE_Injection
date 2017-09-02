# PE_Injection

### 1. 목적
개발한 DLL을 특정 인증된 Application에서만 실행가능하도록 한다. 
- DLL을 아무나 사용하지 못하도록 해야할 경우
- DLL 사용자를 관리하고자 하는 경우(외부 개발자가 최종 릴리즈 요청하면 인증키를 EXE에 삽입해준다)
      
### 2. 구조
DLL에서 자신을 호출한 EXE의 정보를 분석한다. EXE의 해더(PE)에서 인증 정보가 있으면 DLL 내의 메시지 함수를 제거한다.
메시지 함수는 디버깅을 어렵게 하기위해서 Assembly 형태의 배열로 만들어 사용한다.
![](http://postfiles7.naver.net/20110901_230/adsloader_1314864120480cdBBz_PNG/1.PNG?type=w2)

### 3. VC++에서 어셈블리 보기
디버깅을 하는 중에 ALT+8을 입력하여 어셈블리 모드로 진입가능하다.

![](http://postfiles11.naver.net/20110901_154/adsloader_1314843877413nLAWJ_PNG/1.PNG?type=w2)

함수의 시작점과 끝점의 주소를 본 후, ALT + 6을 누르면 브레이크포인트가 있는 시점의 메모리가 덤프된다.

![](http://postfiles7.naver.net/20110901_86/adsloader_1314864747899IXM24_PNG/2.PNG?type=w2)

