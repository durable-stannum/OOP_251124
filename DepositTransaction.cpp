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
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Deposit Failed (Exceed Cash Limit)");
        pSession->setSessionAborted(true); // 세션 종료
        return;
    }
    if (totalBills == 0) {
        ui.displayErrorMessage("NoCashInserted");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Deposit Failed (No Cash Inserted)");
        pSession->setSessionAborted(true); // 세션 종료
        return;
    }

    long depositAmount = (long)cnt50k * 50000 + (long)cnt10k * 10000 +
        (long)cnt5k * 5000 + (long)cnt1k * 1000;

    CashDenominations feeCash = { 0, 0, 0, 0 };
    if (!collectFee(fee, feeCash)) {
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Deposit Failed (Fee Not Paid)");
        pSession->setSessionAborted(true); // 세션 종료
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

        ui.displayMessage("DepositAmountLabel");
        cout << depositAmount;
        ui.displayMessage("WonUnit");
        cout << endl;

        ui.displayMessage("FeeLabel");
        cout << fee;
        ui.displayMessage("WonUnit");
        cout << endl;
    }
    else {
        ui.displayErrorMessage("DepositFailed");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Deposit Failed (System Error)");
        pSession->setSessionAborted(true); // 세션 종료
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

        if (amount < 100000) {
            ui.displayErrorMessage("InvalidCheckAmount");
            pSession->setSessionAborted(true); // 유효하지 않은 입력 범위 -> 세션 종료
            return;
        }

        int count = ui.inputInt("PromptCheckCount");
        if (count <= 0) {
            ui.displayErrorMessage("InvalidInput_Negative");
            pSession->setSessionAborted(true); // 유효하지 않은 입력 범위 -> 세션 종료
            return;
        }

        totalCheckAmount += (amount * count);
        totalCheckCount += count;

        ui.totalCheckInfo((int)totalCheckAmount, totalCheckCount);

        if (totalCheckCount > 30) {
            ui.displayErrorMessage("ExceedCheckLimit");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Check Deposit Failed (Exceed Check Limit)");
            pSession->setSessionAborted(true); // 세션 종료
            return;
        }
    }

    if (totalCheckCount == 0) {
        ui.displayErrorMessage("NoCheckInputExit");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Check Deposit Cancelled");
        // 수표 미입력은 단순 취소로 볼 수도 있으나, 에러 상황으로 간주하고 종료
        pSession->setSessionAborted(true);
        return;
    }

    cout << "---------------------------------" << endl;
    ui.displayMessage("FinalCheckTotalAmount");
    cout << totalCheckAmount;
    ui.displayMessage("WonUnit");
    cout << endl;

    ui.displayMessage("FinalCheckTotalCount");
    cout << totalCheckCount;
    ui.displayMessage("SheetUnit");
    cout << endl;

    ui.displayMessage("FinalCheckDepositConfirm");
    cout << "---------------------------------" << endl;

    CashDenominations feeCash = { 0, 0, 0, 0 };
    if (!collectFee(fee, feeCash)) {
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Check Deposit Failed (Fee Not Paid)");
        pSession->setSessionAborted(true); // 세션 종료
        return;
    }

    atm->addCashToATM(feeCash);

    if (account->addFunds(totalCheckAmount)) {
        string summaryLog = "Transaction ID" + to_string(transactionID) + ": " +
            to_string(totalCheckAmount) + "Won (Check) added to " + account->getAccountNumber();

        pSession->recordTransaction(summaryLog);

        ui.displayMessage("CheckDepositSuccess");

        ui.displayMessage("TotalDepositAmountLabel");
        cout << totalCheckAmount;
        ui.displayMessage("WonUnit");
        cout << endl;
    }
    else {
        ui.displayErrorMessage("DepositFailed");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Check Deposit Failed (System Error)");
        pSession->setSessionAborted(true); // 세션 종료
    }
    ui.wait();
}


void DepositTransaction::run() {
    Interface& ui = pSession->getUI();

    transactionID = nextID++;

    try {
        int depositType = ui.inputInt("AskDepositType");

        if (depositType == 0) {
            ui.displayMessage("TransactionCancelled");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Deposit Failed (Canceled Transaction By User)");
            return;
        }

        if (depositType != 1 && depositType != 2) {
            ui.displayErrorMessage("InvalidSelection");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Deposit Failed (Invalid Selection)");
            pSession->setSessionAborted(true); // 잘못된 선택 -> 세션 종료
            return;
        }

        long fee = calculateFee(TransactionType::DEPOSIT);

        if (depositType == 1) {
            processCashDeposit(fee);
        }
        else if (depositType == 2) {
            processCheckDeposit(fee);
        }
    }
    // -1 입력 등으로 인한 SessionAbortException 발생 시 로그 기록
    catch (const Interface::SessionAbortException&) {
        string cancelLog = "Transaction ID" + to_string(transactionID) + ": Deposit Failed (Canceled Transaction By User)";
        pSession->recordTransaction(cancelLog);
        ui.displayMessage("TransactionCancelled");
        throw; // Session::run()에서 세션 종료를 위해 예외를 다시 던짐
    }
}
