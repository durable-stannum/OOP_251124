#include "main.h"
#include <iostream>
#include <fstream>
#include "Initializer.h"
#include "Interface.h"

using namespace std;

// 전역 변수 정의
string language = "Unselected";
Initializer* globalInitializer = nullptr; // [추가]

int main() {
	Interface ui;

	// 2. 파일 열기
	ui.displayMessage("InitializationStart");
	string filePath;
	getline(cin, filePath);

	// 스냅샷 기능 테스트를 위해 여기서 / 입력 체크를 할 수도 있지만, 
	// 파일 경로는 보통 그냥 받으므로 넘어갑니다.

	ifstream fin(filePath);
	if (!fin.is_open()) { 
		ui.displayMessage("FileOpenFail");
		return 1;
	}
	ui.displayMessage("FileOpenSuccess"); 

	// 4. Initializer 생성 및 전역 포인터 연결
	Initializer* init = new Initializer(fin, ui); 
	globalInitializer = init; // [추가] 전역 변수에 주소 저장

	// 5. 실행
	init->run();

	delete init;
	return 0;
}