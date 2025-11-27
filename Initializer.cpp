#include "Initializer.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "main.h"
#include "Interface.h"
#include "ATM.h"
#include "Account.h"
#include "CashDenominations.h"
#include <limits>

using namespace std;

Initializer::Initializer(ifstream& fin, Interface& uiInput) : ui(uiInput) {
    if (!fin.is_open()) {
        // main에서 체크하지만 안전을 위해 한번 더
        return;
    }

    int B, A, N;
    fin >> B >> A >> N;
    fin.ignore(); // 줄바꿈 문자 제거

    // 1. Bank 생성
    for (int i = 0; i < B; i++) {
        string bankName;
        getline(fin, bankName);
        Bank* pBank = new Bank(bankName);
        allBanks[bankName] = pBank;
    }

    // 2. Account 생성
    for (int i = 0; i < A; i++) {
        string bankName, userName, accountNumber, availableFundsStr;
        string cardNumber, cardPassword;

        getline(fin, bankName);
        getline(fin, userName);
        getline(fin, accountNumber);
        getline(fin, availableFundsStr);
        getline(fin, cardNumber);
        getline(fin, cardPassword);

        // [수정] int -> long long (stoll 사용)
        long long availableFunds = stoll(availableFundsStr);

        // [수정] Account 생성자 인자 맞춤 (transactionHistories 제거)
        Account* pAccount = new Account(
            bankName,
            userName,
            accountNumber,
            availableFunds,
            cardNumber,
            cardPassword
        );

        // 은행이 존재하는지 확인 후 추가
        if (allBanks.find(bankName) != allBanks.end()) {
            allBanks[bankName]->addAccount(pAccount);
        }
        else {
            // 에러 처리 혹은 무시 (메모리 해제 필요)
            delete pAccount;
        }
    }

    // 3. ATM 생성
    for (int i = 0; i < N; i++) {
        string primaryBankStr, serial, type, languageMode, initialFund;
        getline(fin, primaryBankStr);
        getline(fin, serial);
        getline(fin, type);
        getline(fin, languageMode);
        getline(fin, initialFund);

        // initialFund를 권종별 장수로 분리
        CashDenominations cash;
        stringstream ss(initialFund);
        ss >> cash.c50k >> cash.c10k >> cash.c5k >> cash.c1k;
        if (allBanks.find(primaryBankStr) != allBanks.end()) {
            ATM* pATM = new ATM(allBanks[primaryBankStr], serial, type, languageMode, cash, this, ui);
            allATMs.push_back(pATM);
        }
    }
    cout << "Initialization Completed (초기화 완료)" << endl;
}

// [추가] 소멸자: 할당된 메모리 해제
Initializer::~Initializer() {
    // ATM 해제
    for (ATM* atm : allATMs) {
        delete atm;
    }
    // Bank 해제 (Bank 소멸자에서 Account도 해제해준다고 가정하거나, 여기서 Account도 처리 필요)
    for (auto& pair : allBanks) {
        delete pair.second;
    }
}

void Initializer::run() {
    ui.addMainMenu(allATMs);
    while (true) {
        try {
            ui.displayMessage("MainMenu");
            ui.displayMessage("EnterATMSerialNumber");

            string serialNumberInput = ui.inputString(""); // 여기서 -1 입력 시 예외
            if (serialNumberInput.empty()) {
            continue; // ATM 선택 로직을 건너뛰고 MainMenu를 다시 출력
            }

            ATM* selectedATM = findATMBySerialNumber(serialNumberInput);
            if (selectedATM != nullptr) {
                selectedATM->run();
            }
            else {
                cout << "Invalid Serial Number." << endl;
            }
        }
        catch (const Interface::SessionAbortException&) {
            // 메인 메뉴에서 -1 입력 시 그냥 입력을 취소하고 메뉴 다시 출력
            cout << "\nInput cancelled.\n";
            continue;
        }
    }
}

ATM* Initializer::findATMBySerialNumber(const string& serialNumberInput) {
    for (ATM* atm : allATMs) {
        if (atm->getSerialNumber() == serialNumberInput) return atm;
    }
    return nullptr; // [수정] 못 찾으면 nullptr 반환
}

Bank* Initializer::findBankByCardNumber(const string& cardNumberInput) {
    for (auto const& pair : allBanks) {
        Bank* pBank = pair.second;
        if (pBank->getBankByCardNumber(cardNumberInput) != nullptr) {
            return pBank;
        }
    }
    return nullptr;
}

Account* Initializer::findAccountPtrByCardNumber(const string& cardNumberInput) {
    for (auto const& pair : allBanks) {
        Bank* pBank = pair.second;
        Account* result = pBank->getAccountPtrByCardNumber(cardNumberInput);
        if (result != nullptr) {
            return result;
        }
    }
    return nullptr;
}

// [REQ 10.1] 스냅샷 출력 구현
void Initializer::printSnapshot() {
    cout << "\n==================== DEBUG SNAPSHOT ====================" << endl;

    // 1. ATM 정보 출력
    cout << "[ ATM List ]" << endl;
    for (ATM* atm : allATMs) {
        // [수정] Primary Bank 정보 추가
        cout << "Serial: " << atm->getSerialNumber()
            << " | Primary Bank: " << atm->getPrimaryBankName()
            << " (" << atm->getType() << ", " << atm->getLanguageMode() << ")" << endl;

        CashDenominations cash = atm->getCash();

        // [추가] 총액 계산
        long long totalCashAmount = (long long)cash.c50k * 50000
            + (long long)cash.c10k * 10000
            + (long long)cash.c5k * 5000
            + (long long)cash.c1k * 1000;

        // [수정] 권종별 장수와 총액 함께 출력
        cout << "   Remaining Cash: "
            << "50k(" << cash.c50k << ") "
            << "10k(" << cash.c10k << ") "
            << "5k(" << cash.c5k << ") "
            << "1k(" << cash.c1k << ")"
            << " | Total: " << totalCashAmount << " KRW" << endl;
    }
    cout << "--------------------------------------------------------" << endl;

    // 2. Account 정보 출력
    cout << "[ Account List ]" << endl;
    for (auto const& [bankName, bankPtr] : allBanks) {
        // Bank::getAccounts()를 사용
        const map<string, Account*>& accounts = bankPtr->getAccounts();

        for (auto const& [accNum, accPtr] : accounts) {
            cout << "Bank: " << bankName << " | Name: " << accPtr->getUserName() << endl;
            cout << "   Acct No: " << accPtr->getAccountNumber()
                << " | Balance: " << accPtr->getBalance() << endl;
        }
    }
    cout << "========================================================" << endl;
}


