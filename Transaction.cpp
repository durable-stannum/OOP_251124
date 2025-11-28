#include "Transaction.h"
#include "Session.h"
#include "Account.h"
#include "ATM.h"
#include "Interface.h"
#include "Bank.h" // Bank 클래스의 함수 사용
#include <iostream>

using namespace std;

int Transaction::nextID = 1;

Transaction::Transaction(Session* session) : pSession(session) {
    transactionID = 0;
}

int Transaction::getTransactionID() const {
    return transactionID;
}

long Transaction::calculateFee(TransactionType type, const string& destBankName) const {
    Bank* userBankPtr = pSession->getBank();
    ATM* atm = pSession->getATM();

    // nullptr 체크
    if (!userBankPtr || !atm) return 0;

    const string userBank = userBankPtr->getPrimaryBank();
    const string atmBank = atm->getPrimaryBankName();
    const string destBank = destBankName;

    bool isUserPrimary = (userBank == atmBank);

    switch (type) {
    case TransactionType::DEPOSIT:
        return isUserPrimary ? 0 : 1000;

    case TransactionType::WITHDRAWAL:
        return isUserPrimary ? 1000 : 2000;

    case TransactionType::CASH_TRANSFER:
        return 2000;

    case TransactionType::TRANSFER:
        if (destBank.empty()) return -1; // 목적지 은행 이름 필수

        bool isDestPrimary = (destBank == atmBank);

        // 1. 자행 -> 자행 1000원
        if (isUserPrimary && isDestPrimary) {
            return 1000;
        }
        // 2. 자행 <-> 타행: 2000원
        else if (isUserPrimary != isDestPrimary) {
            return 2000;
        }
        // 3. 타행 -> 타행: 4000원
        else {
            return 4000;
        }
        break;
    }
    return 0;
}

bool Transaction::collectFee(long fee, CashDenominations& outFeeCash) {
    if (fee <= 0) return true; // 수수료 없으면 바로 성공

    Interface& ui = pSession->getUI();

    ui.displayMessage("FeeNoticePart1");
    cout << fee;
    ui.displayMessage("FeeNoticePart2");
    cout << endl;

    int feeBillsRequired = fee / 1000;

    ui.displayMessage("FeePromptPart1");
    cout << feeBillsRequired;
    ui.displayMessage("FeePromptPart2");

    int feeBillsInserted = ui.inputInt("Input1kCount");

    if (feeBillsInserted < feeBillsRequired) {
        ui.displayMessage("InsufficientFee");
        return false;
    }

    // 거스름돈 기능은 없으므로, 딱 맞춰 냈거나 더 냈어도 수수료만큼만 가져감
    outFeeCash.c1k = feeBillsInserted;

    return true;
}
