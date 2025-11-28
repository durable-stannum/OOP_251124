#ifndef MAIN_H
#define MAIN_H
#include <string>

using namespace std;

class Initializer; // 전방 선언

// 전역 변수 선언
extern string language;
extern Initializer* globalInitializer; //스냅샷 출력을 위한 전역 포인터

#endif
