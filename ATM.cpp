#include "ATM.h"
#include <iostream>
#include <algorithm>
#include <fstream> // ofstream 사용을 위해 필요
#include <sstream> // stringstream 사용
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

    setLanguage();

    string cardNumberInput;
    try {
        while (true) {
            ui.displayMessage("EnterCardNumber");
            cardNumberInput = ui.inputString("");
            if (cardNumberInput.empty()) {
                continue;
            }
            break;
        }
    }
    catch (const Interface::SessionAbortException&) {
        language = "Unselected";
        return;
    }

    if (cardNumberInput == "Back" || cardNumberInput == "이전") {
        ui.displayMessage("DeactivateATM");
        language = "Unselected";
        return;
    }

    // [수정] Admin 여부와 상관없이 카드 입력 시 즉시 세션 카운트 증가
    totalSessionCount++;

    if (isAdmin(cardNumberInput)) {
        try {
            handleAdminSession();
        }
        catch (const Interface::SessionAbortException&) {
            ui.displayMessage("SessionEnd");
        }

        // [추가] Admin 세션이 끝난 후 이력을 저장 (현재 조회 시에는 안 뜨고, 다음 조회부터 뜸)
        // AccNum 자리에는 "Administrator", 로그에는 "Admin access" 기록
        saveSessionHistory(cardNumberInput, "Administrator", "Admin access");

        language = "Unselected";
        return;
    }

    Bank* cardHoldingBank = pInit->findBankByCardNumber(cardNumberInput);
    Account* pAccount = pInit->findAccountPtrByCardNumber(cardNumberInput);

    if (pAccount == nullptr) {
        ui.displayMessage("CheckValidity");
        ui.displayMessage("IsNotValid");
        ui.displayMessage("SessionEnd");
        language = "Unselected";
        return;
    }

    if (!handleUserSession(cardNumberInput, cardHoldingBank)) {
        language = "Unselected";
        return;
    }

    ui.displayMessage("SessionStart");

    Session* session = new Session(
        cardHoldingBank,
        pAccount,
        ui,
        this,
        pInit->getAllBanks()
    );

    session->run();

    if (session->isSessionAborted()) {
        delete session;
        language = "Unselected";
        ui.displayMessage("SessionEnd");
        return;
    }

    delete session;
    language = "Unselected";
}

void ATM::setLanguage() {
    if (this->getLanguageMode() == "Bilingual") {
        ui.displayMessage("ChooseLanguage");
        string langChoice = ui.inputString("");

        if (langChoice == "2" || langChoice == "Korean" || langChoice == "한국어") {
            language = "Korean";
        }
        else {
            language = "English";
        }
    }
    else {
        language = "English";
    }
    ui.addLanguageModeNotification(this->getLanguageMode());
    ui.displayMessage("LanguageModeNotification");
}

bool ATM::isAdmin(const string& cardNumberInput) {
    string targetAdminNum = "admin" + pPrimaryBank->getPrimaryBank();
    return cardNumberInput == targetAdminNum;
}

// [수정] 단순 문자열 추가가 아니라, 세션 정보를 포맷팅해서 저장
void ATM::saveSessionHistory(const string& cardNum, const string& accNum, const string& sessionLogs) {
    stringstream ss;

    // [REQ Format Implementation]
    ss << "Session : " << totalSessionCount << endl;
    ss << "Card number: " << cardNum << endl;
    ss << accNum << "'s transaction history" << endl;
    ss << sessionLogs; // transaction logs (이미 개행 포함됨)

    if (!atmTransactionHistory.empty()) {
        atmTransactionHistory += "\n";
    }
    atmTransactionHistory += ss.str();
}

void ATM::handleAdminSession() {
    ui.displayMessage("TransactionHistoryMenu");

    int choice = ui.inputInt("");

    switch (choice) {
    case 1:
        cout << "========== Transaction History ==========" << endl;
        // cout << "[Summary]" << endl;
        cout << "Total called session : " << totalSessionCount << endl;
        cout << "Transaction history of ATM :" << endl;
        cout << "=========================================" << endl;

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
        break;
    default:
        cout << "Invalid selection." << endl;
        break;
    }
}

bool ATM::writeHistoryToFile(const string& historyContent) const {
    string filename = "ATM_" + serialNumber + "_History_" + ".txt";
    ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << "Transaction history output successed." << endl;
        outFile << "======================================" << endl;
        outFile << "======================================" << endl;
        outFile << "Total called session : " << totalSessionCount << endl;
        outFile << "Transaction history of ATM :" << endl;
        outFile << "======================================" << endl;
        outFile << historyContent << endl;
        outFile.close();
        return true;
    }
    return false;
}

bool ATM::handleUserSession(const string& cardNumberInput, Bank* cardHoldingBank) {
    ui.displayMessage("CheckValidity");
    if (isSingle()) {
        if (!isValid(cardNumberInput, cardHoldingBank)) {
            ui.displayMessage("IsNotValid");
            ui.displayMessage("SessionEnd");
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
