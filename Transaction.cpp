#include "Transaction.h"
#include "Session.h"
#include "Account.h"
#include "ATM.h"
#include "Interface.h"
#include "Bank.h" // Bank 클래스 함수 호출을 위해 필요
#include <iostream>

using namespace std;

int Transaction::nextID = 1;

Transaction::Transaction(Session* session) : pSession(session) {
    transactionID = nextID++;
}

int Transaction::getTransactionID() const {
    return transactionID;
}

// [수정] 수수료 계산 로직 개선
long Transaction::calculateFee(TransactionType type, const string& destBankName) const {
    Bank* userBankPtr = pSession->getBank();
    ATM* atm = pSession->getATM();

    // nullptr 체크 (안전장치)
    if (!userBankPtr || !atm) return 0;

    const string userBank  = userBankPtr->getPrimaryBank(); 
    const string atmBank   = atm->getPrimaryBankName(); // ATM 수정 시 getPrimaryBankName()으로 통일했다고 가정
    const string destBank  = destBankName; 

    bool isUserPrimary = (userBank == atmBank);
    
    switch (type) {
    case TransactionType::DEPOSIT:
        // [REQ 1.8] Primary: 0, Non-Primary: 1000
        return isUserPrimary ? 0 : 1000;

    case TransactionType::WITHDRAWAL:
        // [REQ 1.8] Primary: 1000, Non-Primary: 2000
        return isUserPrimary ? 1000 : 2000;

    case TransactionType::CASH_TRANSFER:
        // [REQ 1.8] Any bank type: 2000
        return 2000;

    case TransactionType::TRANSFER:
        if (destBank.empty()) return -1; // 목적지 은행 이름 필수

        bool isDestPrimary = (destBank == atmBank);

        // 1. 자행 -> 자행 (Primary -> Primary) : 1000원
        if (isUserPrimary && isDestPrimary) {
            return 1000;
        }
        // 2. 자행 <-> 타행 (Primary <-> Non-Primary) : 2000원
        // (하나만 Primary인 경우)
        else if (isUserPrimary != isDestPrimary) {
            return 2000;
        }
        // 3. 타행 -> 타행 (Non-Primary -> Non-Primary) : 4000원
        // (둘 다 ATM의 주거래 은행이 아님)
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

    // [확인] ui.inputInt 내부에서 "/" 입력 시 스냅샷 출력되도록 Interface 수정되었음.
    int feeBillsInserted = ui.inputInt("Input1kCount");

    if (feeBillsInserted < feeBillsRequired) {
        ui.displayMessage("InsufficientFee"); 
        return false;
    }

    // 거스름돈 기능은 없으므로, 딱 맞춰 냈거나 더 냈어도 수수료만큼만 가져감(혹은 더 낸 거 반환 로직 필요하지만 여기선 생략)
    outFeeCash.c1k = feeBillsInserted; 
    
    // 만약 사용자가 2000원(2장) 내야 하는데 3장 넣었다면?
    // ATM 시재에는 받은 만큼 추가되어야 하므로, 그대로 outFeeCash에 넣습니다.
    
    return true;
}