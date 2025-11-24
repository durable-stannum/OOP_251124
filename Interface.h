#ifndef INTERFACE_H
#define INTERFACE_H

#include <map>
#include <string>
#include <vector>

using namespace std;

class ATM; // 전방 선언

class Interface {
private:
	map<string, map<string, string>> msgMap; 

public:
	Interface(); 

	void addMessages();
	void addMainMenu(const vector<ATM*>& allATMs);
	void addATMWelcome(const ATM* atm);
	void addLanguageModeNotification(string languageMode);

	void displayMessage(string msgKey);
	void displayErrorMessage(string msgKey);

	// [수정] 스냅샷 기능을 위해 내부 로직 변경됨
	int inputInt(string msgKey);
	long inputCheckAmount(string msgKey);
	
	// [추가] 문자열 입력 함수 (카드번호, 시리얼번호 등)
	string inputString(string msgKey);

	void totalCheckInfo(int amount, int count);
	void wait();
};

#endif