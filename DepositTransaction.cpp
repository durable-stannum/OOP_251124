#include "DepositTransaction.h"
#include "Session.h"
#include "ATM.h"    
#include "Account.h" 
#include "Interface.h" 
#include <iostream>

using namespace std;

DepositTransaction::DepositTransaction(Session* session) : Transaction(session) {}

// [기능 1] 현금 입금
void DepositTransaction::processCashDeposit(long fee) {
    Interface& ui = pSession->getUI();
    Account* account = pSession->getAccount();
    ATM* atm = pSession->getATM();

    int cnt50k = ui.inputInt("Input50kCount");
    int cnt10k = ui.inputInt("Input10kCount");
    int cnt5k = ui.inputInt("Input5kCount");
    int cnt1k = ui.inputInt("Input1kCount");

    int totalBills = cnt50k + cnt10k + cnt5k + cnt1k;

    if (totalBills > 50) {
        ui.displayErrorMessage("ExceedCashLimit");
        // [수정] 실패 로그
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Deposit Failed (Exceed Cash Limit)");
        return;
    }
    if (totalBills == 0) {
        ui.displayErrorMessage("NoCashInserted");
        // [수정] 실패 로그
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Deposit Failed (No Cash Inserted)");
        return;
    }

    long depositAmount = (long)cnt50k * 50000 + (long)cnt10k * 10000 +
        (long)cnt5k * 5000 + (long)cnt1k * 1000;

    CashDenominations feeCash = { 0, 0, 0, 0 };
    if (!collectFee(fee, feeCash)) {
        // [수정] 실패 로그
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Deposit Failed (Fee Not Paid)");
        return;
    }

    CashDenominations depositCash = { cnt50k, cnt10k, cnt5k, cnt1k };
    CashDenominations totalToATM;
    totalToATM.c50k = depositCash.c50k + feeCash.c50k;
    totalToATM.c10k = depositCash.c10k + feeCash.c10k;
    totalToATM.c5k = depositCash.c5k + feeCash.c5k;
    totalToATM.c1k = depositCash.c1k + feeCash.c1k;

    atm->addCashToATM(totalToATM);

    if (account->addFunds(depositAmount)) {
        string summaryLog = "Transaction ID" + to_string(transactionID) + ": " +
            to_string(depositAmount) + "Won added to " + account->getAccountNumber();

        pSession->recordTransaction(summaryLog);
        pSession->recordSessionSummary(pSession->getAccount()->getAccountNumber(), pSession->getAccount()->getCardNumber(), "Cash Deposit", depositAmount);

        ui.displayMessage("DepositSuccess");
        // (UI 출력...)
    }
    else {
        ui.displayErrorMessage("DepositFailed");
        // [수정] 실패 로그
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Deposit Failed (System Error)");
    }
    ui.wait();
}


// [기능 2] 수표 입금
void DepositTransaction::processCheckDeposit(long fee) {
    Interface& ui = pSession->getUI();
    Account* account = pSession->getAccount();
    ATM* atm = pSession->getATM();

    long long totalCheckAmount = 0;
    int totalCheckCount = 0;

    ui.displayMessage("StartCheckDepositInfo");

    while (true) {
        long amount = ui.inputCheckAmount("PromptCheckAmountLoop");
        if (amount == 0) break;

        // ... (중간 생략: 수표 유효성 검사 등) ...

        // 만약 중간에 취소되거나 에러로 리턴된다면 로그 추가 필요할 수 있음
        // 현재 로직상 루프 내에서 continue만 하므로 최종 집계로 이동
        // 단, ExceedCheckLimit일 경우 return하므로 처리 필요
        if (totalCheckCount > 30) {
            ui.displayErrorMessage("ExceedCheckLimit");
            // [수정] 실패 로그 (ID는 run에서 이미 발급됨)
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Check Deposit Failed (Exceed Check Limit)");
            return;
        }
        // ...
    }

    if (totalCheckCount == 0) {
        ui.displayErrorMessage("NoCheckInputExit");
        // [수정] 실패 로그
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Check Deposit Cancelled");
        return;
    }

    // ... (UI 출력) ...

    CashDenominations feeCash = { 0, 0, 0, 0 };
    if (!collectFee(fee, feeCash)) {
        // [수정] 실패 로그
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Check Deposit Failed (Fee Not Paid)");
        return;
    }

    atm->addCashToATM(feeCash);

    if (account->addFunds(totalCheckAmount)) {
        string summaryLog = "Transaction ID" + to_string(transactionID) + ": " +
            to_string(totalCheckAmount) + "Won (Check) added to " + account->getAccountNumber();

        pSession->recordTransaction(summaryLog);
        ui.displayMessage("CheckDepositSuccess");
        // ...
    }
    else {
        ui.displayErrorMessage("DepositFailed");
        // [수정] 실패 로그
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Check Deposit Failed (System Error)");
    }
    ui.wait();
}

void DepositTransaction::run() {
    Interface& ui = pSession->getUI();

    int depositType = ui.inputInt("AskDepositType");

    if (depositType == 0) {
        ui.displayMessage("TransactionCancelled");
        return;
    }

    if (depositType != 1 && depositType != 2) {
        ui.displayErrorMessage("InvalidSelection");
        return;
    }

    // [중요] ID 할당
    transactionID = nextID++;

    long fee = calculateFee(TransactionType::DEPOSIT);

    if (depositType == 1) {
        processCashDeposit(fee);
    }
    else {
        processCheckDeposit(fee);
    }
}
