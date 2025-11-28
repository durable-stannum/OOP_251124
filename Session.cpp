#include "Session.h"
#include <iostream>
#include <string>
#include "DepositTransaction.h"
#include "WithdrawalTransaction.h"
#include "TransferTransaction.h"
#include "ATM.h"

using namespace std;

int Session::sessionCount = 0;

// 1. 생성자
Session::Session(Bank* pBank, Account* pAccount, Interface& ui, ATM* atm, map<string, Bank*>& banks)
    : pBank(pBank), pAccount(pAccount), ui(ui), pATM(atm), allBanks(banks), withdrawalCount(0), sessionHistoryBuffer("")
{
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
    if (!sessionHistoryBuffer.empty()) {
        sessionHistoryBuffer += "\n";
    }
    sessionHistoryBuffer += log;

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
    try {
        string cardPWInput;
        for (int i = 0; i < 3; i++) {
            while (true) {
                ui.displayMessage("EnterCardPW");
                cardPWInput = ui.inputString("");
                if (cardPWInput.empty()) {
                    continue;
                }
                break;
            }

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

            int transactionNumberInput = ui.inputInt("SessionMenu");

            if (transactionNumberInput == 0 || transactionNumberInput == 4) {
                ui.displayMessage("SessionEnd");
                break;
            }

            switch (transactionNumberInput) {
            case 1:
                deposit->run();
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

        // [정상 종료 시 저장 로직]
        if (pATM != nullptr && !sessionHistoryBuffer.empty()) {
            pATM->saveSessionHistory(
                pAccount->getCardNumber(),
                pAccount->getAccountNumber(),
                sessionHistoryBuffer
            );
        }

        if (sessionCount != 0) cout << sessionSummary;
    }
    catch (const Interface::SessionAbortException&) {
        ui.displayMessage("SessionEnd");

        // [수정] 강제 종료(-1) 시에도 거래 내역이 있다면 저장하도록 추가
        if (pATM != nullptr && !sessionHistoryBuffer.empty()) {
            pATM->saveSessionHistory(
                pAccount->getCardNumber(),
                pAccount->getAccountNumber(),
                sessionHistoryBuffer
            );
        }

        if (sessionCount != 0) cout << sessionSummary;
        return;
    }
}
