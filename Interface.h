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
	// [추가] 세션 강제 종료를 위한 예외 클래스 정의
	class SessionAbortException {};

	Interface();

	void addMessages();
	void addMainMenu(const vector<ATM*>& allATMs);
	void addATMWelcome(const ATM* atm);
	void addLanguageModeNotification(string languageMode);

	void displayMessage(string msgKey);
	void displayErrorMessage(string msgKey);

	int inputInt(string msgKey);
	long inputCheckAmount(string msgKey);
	string inputString(string msgKey);

	void totalCheckInfo(int amount, int count);
	void wait();
};

#endif
