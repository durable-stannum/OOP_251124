#include "Initializer.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "main.h"
#include "Interface.h"
#include "ATM.h"
#include "Account.h"

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
        } else {
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

        int c50k, c10k, c5k, c1k;
        {
            stringstream ss(initialFund);
            ss >> c50k >> c10k >> c5k >> c1k;
        }

        if (allBanks.find(primaryBankStr) != allBanks.end()) {
            ATM* pATM = new ATM(
                allBanks[primaryBankStr],
                serial,
                type,
                languageMode,
                c50k, c10k, c5k, c1k,
                this,
                ui
            );
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
    // 일반적으로 Bank가 Account의 소유권을 가진다면 Bank만 지워도 됨.
    // 현재 구조상 Bank -> addAccount -> map에 저장. 
    // Bank 소멸자가 없다면 메모리 누수 발생 가능. Bank 소멸자 추가 권장.
    for (auto& pair : allBanks) {
        delete pair.second;
    }
}

void Initializer::run() {
    ui.addMainMenu(allATMs);
    while (true) {
        ui.displayMessage("MainMenu");
        ui.displayMessage("EnterATMSerialNumber");
        
        // [수정] cin -> ui.inputString (스냅샷 대응)
        string serialNumberInput = ui.inputString("");

        ATM* selectedATM = findATMBySerialNumber(serialNumberInput);
        if (selectedATM != nullptr) {
            selectedATM->run();
        } else {
            // 잘못된 시리얼 번호 처리 (메세지 출력 등)
            cout << "Invalid Serial Number." << endl; 
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
        cout << "Serial: " << atm->getSerialNumber() 
             << " (" << atm->getType() << ", " << atm->getLanguageMode() << ")" << endl;
        
        CashDenominations cash = atm->getCash();
        cout << "   Remianing Cash: " 
             << "50k(" << cash.c50k << ") "
             << "10k(" << cash.c10k << ") "
             << "5k(" << cash.c5k << ") "
             << "1k(" << cash.c1k << ")" << endl;
    }
    cout << "--------------------------------------------------------" << endl;

    // 2. Account 정보 출력
    cout << "[ Account List ]" << endl;
    for (auto const& [bankName, bankPtr] : allBanks) {
        // Bank::getAccounts()를 사용 (Bank.h 수정 전제)
        const map<string, Account*>& accounts = bankPtr->getAccounts();
        
        for (auto const& [accNum, accPtr] : accounts) {
            cout << "Bank: " << bankName << " | Name: " << accPtr->getUserName() << endl;
            cout << "   Acct No: " << accPtr->getAccountNumber() 
                 << " | Balance: " << accPtr->getBalance() << endl;
        }
    }
    cout << "========================================================" << endl;
}