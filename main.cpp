#include "main.h"
#include <iostream>
#include <fstream>
#include "Initializer.h"
#include "Interface.h"

using namespace std;

// 전역 변수 정의
string language = "Unselected";
Initializer* globalInitializer = nullptr;

int main() {
	Interface ui;

	//파일 열기
	ui.displayMessage("InitializationStart");
	string filePath;
	getline(cin, filePath);


	ifstream fin(filePath);
	if (!fin.is_open()) { 
		ui.displayMessage("FileOpenFail");
		return 1;
	}
	ui.displayMessage("FileOpenSuccess"); 

	//Initializer 생성 및 전역 포인터 연결
	Initializer* init = new Initializer(fin, ui); 
	globalInitializer = init; //전역 변수에 주소 저장

	//실행
	init->run();

	delete init;
	return 0;
}
