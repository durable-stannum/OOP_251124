#include "Session.h"
#include <iostream>
#include <string>
#include "DepositTransaction.h"
#include "WithdrawalTransaction.h"
#include "TransferTransaction.h"
#include "ATM.h" // ATM::addHistory 호출을 위해 필요

using namespace std;

int Session::sessionCount = 0;

// 1. 생성자
Session::Session(Bank* pBank, Account* pAccount, Interface& ui, ATM* atm, map<string, Bank*>& banks)
    : pBank(pBank), pAccount(pAccount), ui(ui), pATM(atm), allBanks(banks), withdrawalCount(0)
{
    // [수정] ui가 아니라 this(Session 포인터)를 넘겨야 함
    deposit = new DepositTransaction(this);
    withdrawal = new WithdrawalTransaction(this);
    transfer = new TransferTransaction(this);
    sessionCount = 0;
    sessionSummary = "\n========== [Session Summary] ==========\n";
}

// 2. 소멸자
Session::~Session() {
    delete deposit;
    delete withdrawal;
    delete transfer;
}

// 3. 거래 기록
void Session::recordTransaction(const string& log) {
    historyLog.push_back(log);
    // [추가] ATM에도 거래 내역을 남겨야 Admin 모드에서 볼 수 있음
    if (pATM != nullptr) {
        pATM->addHistory(log);
    }
    // [추가] 계좌에도 내역 남기기 (Account.h에 addHistory가 있다면)
    if (pAccount != nullptr) {
        pAccount->addHistory(log);
    }
}

// 4. 세션 요약 기록
void Session::recordSessionSummary(string accountNumberInput, string cardNumberInput, string transactionTypeInput, int amountInput) {
    sessionSummary += "#";
    string sessoinCounstStr = to_string(++sessionCount);
    sessionSummary += sessoinCounstStr;
    sessionSummary += "\n";
    sessionSummary += "Account Number: ";
    sessionSummary += accountNumberInput;
    sessionSummary += "\n";
    sessionSummary += "Card Number: ";
    sessionSummary += cardNumberInput;
    sessionSummary += "\n";
    sessionSummary += "Transaction Type: ";
    sessionSummary += transactionTypeInput;
    sessionSummary += "\n";
    sessionSummary += "Amount: ";
    string amount = to_string(amountInput);
    sessionSummary += amount;
    sessionSummary += "\n";
}

void Session::run() {
    // [수정] 전체 로직을 try-catch로 감싸서 -1 입력 시 세션 종료 처리
    try {
        // 1. 카드 비번 입력 받기
        ui.displayMessage("EnterCardPW");

        string cardPWInput;
        for (int i = 0; i < 3; i++) {
            cardPWInput = ui.inputString(""); // 여기서 -1 입력 시 throw 됨

            if (pBank->isCorrectCardPW(pAccount, cardPWInput)) {
                ui.displayMessage("CorrectCardPW");
                break;
            }
            else {
                ui.displayMessage("WrongCardPW");
            }

            if (i == 2) {
                ui.displayMessage("PwIncorrect3Times");
                ui.displayMessage("SessionEnd");
                return;
            }
        }

        // 2. 거래 반복
        while (true) {
            if (isSessionAborted()) {
                ui.displayMessage("SessionEnd");
                break;
            }

            // 메뉴 입력에서 -1 입력 시 catch 블록으로 이동
            int transactionNumberInput = ui.inputInt("SessionMenu");

            if (transactionNumberInput == 0 || transactionNumberInput == 4) {
                ui.displayMessage("SessionEnd");
                break;
            }

            switch (transactionNumberInput) {
            case 1:
                deposit->run(); // 내부 입력에서 -1 입력 시 throw -> 여기 탈출
                break;
            case 2:
                withdrawal->run();
                break;
            case 3:
                transfer->run();
                break;
            default:
                ui.displayErrorMessage("InvalidSelection");
                break;
            }
        }

        // 세션 요약 출력
        if (sessionCount == 0) {
            cout << "\n========== [Session Summary] ==========\n" << "No sessions were conducted." << endl;
        }
        else cout << sessionSummary;

    }
    catch (const Interface::SessionAbortException&) {
        // [추가] 예외 발생 시 세션 강제 종료 처리
        ui.displayMessage("SessionEnd"); // "Session is ended."
        return; // Session::run() 종료 -> ATM::run()으로 복귀
    }
}
