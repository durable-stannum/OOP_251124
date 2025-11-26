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

    // [수정] while(true) 반복문 제거
    // 세션이 끝나면 함수가 종료되어 Initializer의 메인 메뉴로 돌아가야 함

    // [수정] cin -> ui.inputString (스냅샷 REQ 10.1 대응)
    ui.displayMessage("EnterCardNumber");
    string cardNumberInput = ui.inputString("");

    // 세션 카운트 증가 (입력 시도 기준)
    totalSessionCount++;

    // 'Back' 또는 '이전' 입력 시 종료
    if (cardNumberInput == "Back" || cardNumberInput == "이전") {
        ui.displayMessage("DeactivateATM");
        language = "Unselected"; // [중요] 메인 메뉴 복귀 전 언어 초기화
        return;
    }

    // 관리자 확인
    if (isAdmin(cardNumberInput)) {
        handleAdminSession();
        // [수정] 관리자 세션 종료 후 메인 메뉴로 복귀
        language = "Unselected";
        return;
    }

    // 카드 존재 유무 확인 및 유효성 검사
    Bank* cardHoldingBank = pInit->findBankByCardNumber(cardNumberInput);
    Account* pAccount = pInit->findAccountPtrByCardNumber(cardNumberInput);

    if (pAccount == nullptr) {
        ui.displayMessage("CheckValidity");
        ui.displayMessage("IsNotValid");
        ui.displayMessage("SessionEnd");
        language = "Unselected"; // [중요] 메인 메뉴 복귀 전 언어 초기화
        return;
    }

    // 3. ATM 유형에 따른 사용 가능 여부 검사 (Single/Multi)
    if (!handleUserSession(cardNumberInput, cardHoldingBank)) {
        language = "Unselected"; // [중요] 메인 메뉴 복귀 전 언어 초기화
        return;
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


    if (session->isSessionAborted()) {
            // 출금 3회 초과 등 강제 종료 신호가 발생했을 경우 (Session::run()이 break로 복귀함)
            
            delete session;
            
            language = "Unselected"; // 언어 복구 (MainMenu 출력 가능하도록)
            ui.displayMessage("SessionEnd");
            
            // ATM을 비활성화하고 Initializer로 돌아가 ATM 고유번호 입력창(MainMenu)으로 이동
            return; 
        }
    // 6. 세션 종료 및 정리

    
    delete session;

    // [수정] 카드 삽입 단계로 돌아가는 것이 아니라 메인 메뉴로 돌아가므로 메세지 제거 혹은 변경
    // ui.displayMessage("GoBackToEnteringCardNumber"); (제거됨)

    // [중요] 메인 메뉴(Initializer)로 돌아가기 위해 언어 상태 초기화
    language = "Unselected";
}

void ATM::setLanguage() {
    if (this->getLanguageMode() == "Bilingual") {
        // [수정] 숫자 입력으로 안전하게 처리
        ui.displayMessage("ChooseLanguage");

        // 임시로 cin 대신 inputString 사용
        string langChoice = ui.inputString("");

        if (langChoice == "2" || langChoice == "Korean" || langChoice == "한국어") {
            language = "Korean";
        }
        else {
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
        }
        else {
            cout << atmTransactionHistory << endl;
        }
        cout << "=========================================" << endl;

        ui.displayMessage("WritingHistoryToFile");
        if (writeHistoryToFile(atmTransactionHistory)) {
            ui.displayMessage("FileWritingSuccess");
        }
        else {
            ui.displayMessage("FileWritingFailure");
        }
        ui.displayMessage("SessionEnd");
        break;
    case 2:
        // 나가기 선택 시 그냥 종료 (상위 run()에서 language 초기화 후 리턴됨)
        break;
    default:
        // ui.displayErrorMessage("InvalidSelection");
        cout << "Invalid selection." << endl;
        break;
    }
}

bool ATM::writeHistoryToFile(const string& historyContent) const {
    // 파일명 포맷: ATM_serial_History.txt
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
            // ui.displayMessage("GoBackToEnteringCardNumber"); // 제거
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
    availableCash.c5k += deposit.c5k;
    availableCash.c1k += deposit.c1k;
}

bool ATM::dispenseCash(long amount) {
    CashDenominations tempCash = availableCash;
    long remainingAmount = amount;

    // Greedy Algorithm (50k -> 10k -> 5k -> 1k)
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
    }
    else {
        return false;
    }
}
