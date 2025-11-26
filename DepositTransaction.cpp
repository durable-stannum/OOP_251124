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
        return;
    }
    if (totalBills == 0) {
        ui.displayErrorMessage("NoCashInserted");
        return;
    }

    long depositAmount = (long)cnt50k * 50000 + (long)cnt10k * 10000 +
        (long)cnt5k * 5000 + (long)cnt1k * 1000;

    CashDenominations feeCash = { 0, 0, 0, 0 };
    if (!collectFee(fee, feeCash)) { return; }

    CashDenominations depositCash = { cnt50k, cnt10k, cnt5k, cnt1k };
    CashDenominations totalToATM;
    totalToATM.c50k = depositCash.c50k + feeCash.c50k;
    totalToATM.c10k = depositCash.c10k + feeCash.c10k;
    totalToATM.c5k = depositCash.c5k + feeCash.c5k;
    totalToATM.c1k = depositCash.c1k + feeCash.c1k;

    atm->addCashToATM(totalToATM);

    if (account->addFunds(depositAmount)) {
        // [수정] 로그 포맷 변경: Transaction ID[N]: [Amount]Won added to [AccNum]
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
            continue;
        }

        int count = ui.inputInt("PromptCheckCount");
        if (count <= 0) {
            ui.displayErrorMessage("InvalidInput_Negative");
            continue;
        }

        totalCheckAmount += (amount * count);
        totalCheckCount += count;

        ui.totalCheckInfo((int)totalCheckAmount, totalCheckCount);

        if (totalCheckCount > 30) {
            ui.displayErrorMessage("ExceedCheckLimit");
            return;
        }
    }

    if (totalCheckCount == 0) {
        ui.displayErrorMessage("NoCheckInputExit");
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
    if (!collectFee(fee, feeCash)) { return; }

    atm->addCashToATM(feeCash);

    if (account->addFunds(totalCheckAmount)) {
        // [수정] 로그 포맷 변경
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

    transactionID = nextID++;

    long fee = calculateFee(TransactionType::DEPOSIT);

    if (depositType == 1) {
        processCashDeposit(fee);
    }
    else {
        processCheckDeposit(fee);
    }
}
