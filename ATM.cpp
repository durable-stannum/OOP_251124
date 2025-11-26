#include "ATM.h"
#include <iostream>
#include <algorithm>
#include <fstream> // ofstream 사용을 위해 필요
#include "Interface.h"
#include "main.h" // 전역 language 변수 사용
#include "Initializer.h"
#include "Session.h"
#include "CashDenominations.h"

using namespace std;

ATM::ATM(Bank* primaryBank, const string& serial, const string& t, const string& lang, CashDenominations& initialCash, Initializer* initializer, Interface& uiInput)
	: pPrimaryBank(primaryBank), serialNumber(serial), type(t), languageMode(lang), availableCash(initialCash), atmTransactionHistory(""), pInit(initializer), ui(uiInput) {
	atmTransactionHistory = "";
}

void ATM::run() {
	ui.addATMWelcome(this);
	ui.displayMessage("ATMWelcome");

	// 언어 설정
	setLanguage();

	while (true) {
        // [수정] cin -> ui.inputString (스냅샷 REQ 10.1 대응)
		// "카드를 입력하세요" 메세지는 inputString 내부 혹은 displayMessage로 처리
        ui.displayMessage("EnterCardNumber");
		string cardNumberInput = ui.inputString("");
		totalSessionCount++; 
		if (cardNumberInput == "Back" || cardNumberInput == "이전") {
		ui.displayMessage("DeactivateATM");
		language = "Unselected";
		return;
		}

        // Back 입력 시 종료 로직 등이 필요하면 여기에 추가

		// 관리자 확인
		if (isAdmin(cardNumberInput)) { 
			handleAdminSession();
			continue;
		}
		
		//카드 존재 유무 확인 및 유효성 검사
		Bank* cardHoldingBank = pInit->findBankByCardNumber(cardNumberInput);
        Account* pAccount = pInit->findAccountPtrByCardNumber(cardNumberInput);
		
		if (pAccount == nullptr) {
			ui.displayMessage("CheckValidity");
			ui.displayMessage("IsNotValid");
			ui.displayMessage("SessionEnd");
			continue;
		}


// 3. ATM 유형에 따른 사용 가능 여부 검사 (Single/Multi)
        if (!handleUserSession(cardNumberInput, cardHoldingBank)) {
            continue;
        }
        

        // 4. ⭐️ 검증 완료 후 Session 생성
        ui.displayMessage("SessionStart");

        Session* session = new Session(
            cardHoldingBank, 
            pAccount, 
            ui, 
            this, 
            pInit->getAllBanks()
        );

        // 5. 세션 실행 (비밀번호 입력 파트로 진입)
        session->run();

        // 6. 세션 종료 및 정리
        delete session;
        ui.displayMessage("GoBackToEnteringCardNumber");
	}
}

void ATM::setLanguage() {
	if (this->getLanguageMode() == "Bilingual") {
        // [수정] 숫자 입력으로 안전하게 처리
		ui.displayMessage("ChooseLanguage"); // "1. English, 2. Korean" 출력한다고 가정
        // Interface에 해당 키가 없다면 "1. English\n2. 한국어\n> " 내용을 추가해야 함.
        // 임시로 cin 대신 inputString 사용
		string langChoice = ui.inputString(""); 
        
        if (langChoice == "2" || langChoice == "Korean" || langChoice == "한국어") {
            language = "Korean";
        } else {
            language = "English"; // 기본값
        }
	}
	else { // Unilingual
		language = "English";
	}
    // 안내 메세지 출력
	ui.addLanguageModeNotification(this->getLanguageMode());
	ui.displayMessage("LanguageModeNotification");
}

bool ATM::isAdmin(const string& cardNumberInput) {
    // "admin" + [주거래은행이름]
    // 혹은 Initializer에서 은행별 관리자 번호를 따로 관리해도 되지만, 
    // 간단하게 규칙성 있는 번호로 구현하는 것을 추천합니다.
    string targetAdminNum = "admin" + pPrimaryBank->getPrimaryBank(); 
    
    return cardNumberInput == targetAdminNum;
}

// [추가] 거래 내역 기록
void ATM::addHistory(const string& log) {
    if (!atmTransactionHistory.empty()) {
        atmTransactionHistory += "\n";
    }
    atmTransactionHistory += log;
}

void ATM::handleAdminSession() {
	ui.displayMessage("TransactionHistoryMenu");

    // [수정] cin -> ui.inputInt
	int choice = ui.inputInt(""); 

	switch (choice) {
	case 1: // 내역 출력 및 저장
		cout << "========== Transaction History ==========" << endl;

        cout << "[Summary]" << endl;
        cout << "Total Sessions: " << totalSessionCount << endl; // 총 세션 수 출력
        cout << "-----------------------------------------" << endl;
		
        if (atmTransactionHistory.empty()) {
            cout << "No transactions yet." << endl;
        } else {
            cout << atmTransactionHistory << endl;
        }
        cout << "=========================================" << endl;

		ui.displayMessage("WritingHistoryToFile"); // 키 이름 대소문자 확인 필요 (writing... -> Writing...)
		if (writeHistoryToFile(atmTransactionHistory)) {
			ui.displayMessage("FileWritingSuccess");
		}
		else {
			ui.displayMessage("FileWritingFailure");
		}
		ui.displayMessage("SessionEnd");
		break;
	case 2:
		ui.displayMessage("GoBackToEnteringCardNumber");
		break;
	default:
		// ui.displayErrorMessage("InvalidSelection");
        cout << "Invalid selection." << endl;
		break;
	}
}

bool ATM::writeHistoryToFile(const string& historyContent) const {
    // 파일명 포맷: ATM_serial_History_Language.txt
	string filename = "ATM_" + serialNumber + "_History_" + ".txt";
	ofstream outFile(filename);
	if (outFile.is_open()) {
		outFile << "[ ATM Information ]" << endl;
		outFile << "Primary Bank: " << pPrimaryBank->getPrimaryBank() << endl;
		outFile << "Serial: " << serialNumber << endl;
		outFile << "Type: " << type << endl;
		outFile << "Language: " << languageMode << endl;
		outFile << "Total Sessions: " << totalSessionCount << endl;
		outFile << "========================================" << endl;
		outFile << "[ Transaction History ]" << endl;
		outFile << historyContent << endl;
		outFile.close();
		return true;
	}
	return false;
}

bool ATM::handleUserSession(const string& cardNumberInput, Bank* cardHoldingBank) {
	ui.displayMessage("CheckValidity");
	if (isSingle()) { 
		if (!isValid(cardNumberInput, cardHoldingBank)) { // 타행 카드 거절
			ui.displayMessage("IsNotValid");
			ui.displayMessage("SessionEnd");
			ui.displayMessage("GoBackToEnteringCardNumber");
			return false;
		}
	}
	ui.displayMessage("IsValid");
	return true;
}

bool ATM::isSingle() const {
	return this->getType() == "Single";
}

bool ATM::isValid(const string& cardNumberInput, Bank* cardBank) const {
    // Initializer를 통해 카드 번호로 은행을 찾아서 주거래 은행과 비교
	if (isSingle()) {
		return cardBank == pPrimaryBank;
	}
 
	return true;
}

void ATM::addCashToATM(const CashDenominations& deposit) {
    availableCash.c50k += deposit.c50k;
    availableCash.c10k += deposit.c10k;
    availableCash.c5k  += deposit.c5k;
    availableCash.c1k  += deposit.c1k;
}

bool ATM::dispenseCash(long amount) {
    CashDenominations tempCash = availableCash;
    long remainingAmount = amount;

    // Greedy Algorithm (50k -> 10k -> 5k -> 1k)
    // [REQ 5.1] fewest number of possible bills
    int count50k = min((long)tempCash.c50k, remainingAmount / 50000);
    remainingAmount -= (long)count50k * 50000;

    int count10k = min((long)tempCash.c10k, remainingAmount / 10000);
    remainingAmount -= (long)count10k * 10000;

    int count5k = min((long)tempCash.c5k, remainingAmount / 5000);
    remainingAmount -= (long)count5k * 5000;

    int count1k = min((long)tempCash.c1k, remainingAmount / 1000);
    remainingAmount -= (long)count1k * 1000;

    if (remainingAmount == 0) {
        availableCash.c50k -= count50k;
        availableCash.c10k -= count10k;
        availableCash.c5k -= count5k;
        availableCash.c1k -= count1k;
        return true;
    } else {
        return false;
    }
}
