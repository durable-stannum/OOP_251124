#include "WithdrawalTransaction.h"
#include "Session.h"
#include "ATM.h"
#include "Account.h"
#include "Interface.h"
#include "CashDenominations.h"
#include <iostream>

using namespace std;

WithdrawalTransaction::WithdrawalTransaction(Session* session) : Transaction(session) {}

bool WithdrawalTransaction::processSingleWithdrawal() {
    Interface& ui = pSession->getUI();
    Account* account = pSession->getAccount();
    ATM* atm = pSession->getATM();

    ui.displayMessage("CurrentBalance");
    cout << account->getBalance();
    ui.displayMessage("WonUnit");
    cout << endl;

    long amount = ui.inputInt("WithdrawalAmountPrompt");

    if (amount == 0) {
        ui.displayMessage("TransactionCancelled");
        return false;
    }

    // [중요] ID가 여기서 증가하므로, 이후 실패 시 반드시 기록을 남겨야 함
    transactionID = nextID++;

    if (amount > 500000) {
        ui.displayErrorMessage("ExceedWithdrawalLimit");
        // [수정] 실패 로그 기록
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Exceeded Limit)");
        return false;
    }
    if (amount % 1000 != 0) {
        ui.displayErrorMessage("InvalidAmountUnit");
        // [수정] 실패 로그 기록
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Invalid Unit)");
        return false;
    }

    long fee = calculateFee(TransactionType::WITHDRAWAL);
    long long totalDeduction = amount + fee;

    if (account->getBalance() < totalDeduction) {
        ui.displayErrorMessage("InsufficientBalance");
        // [수정] 실패 로그 기록
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Insufficient Balance)");
        return false;
    }
    CashDenominations dispensedCash;

    if (!atm->dispenseCash(amount, dispensedCash)) {
        ui.displayErrorMessage("InsufficientATMCash");
        // [수정] 실패 로그 기록 (질문하신 상황: ATM 현금 부족)
        string failLog = "Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Insufficient ATM Cash)";
        pSession->recordTransaction(failLog);
        return false;
    }

    if (account->deductFunds(totalDeduction)) {
        // 성공 로그
        string summaryLog = "Transaction ID" + to_string(transactionID) + ": " +
            to_string(amount) + "Won withdrawed from " + account->getAccountNumber();

        pSession->recordTransaction(summaryLog);
        pSession->recordSessionSummary(pSession->getAccount()->getAccountNumber(), pSession->getAccount()->getCardNumber(), "Withdrawal", amount);

        ui.displayMessage("WithdrawalSuccess");
        ui.displayDispensedCash(dispensedCash);

        ui.displayMessage("WithdrawalAmountLabel");
        cout << amount;
        ui.displayMessage("WonUnit");
        cout << endl;

        ui.displayMessage("FeeLabel");
        cout << fee;
        ui.displayMessage("WonUnit");
        cout << endl;

        ui.displayMessage("BalanceAfterTransaction");
        cout << account->getBalance();
        ui.displayMessage("WonUnit");
        cout << endl;

        return true;
    }
    else {
        ui.displayErrorMessage("TransactionFailed");
        // [수정] 실패 로그 기록
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (System Error)");
        return false;
    }
}

void WithdrawalTransaction::run() {
    Interface& ui = pSession->getUI();

    if (pSession->getWithdrawalCount() >= 3) {
        ui.displayErrorMessage("MaxWithdrawalLimitReached");

        pSession->setSessionAborted(true);

        ui.displayMessage("ReturningToMenu");
        ui.wait();
        return;
    }

    while (pSession->getWithdrawalCount() < 3) {
        bool success = processSingleWithdrawal();

        if (success) {
            pSession->increaseWithdrawalCount();
        }
        else {
            // 실패하더라도 루프를 돌 것인지, 나갈 것인지 결정 필요.
            // 보통 실패하면 해당 거래 시도는 끝나고 재시도 여부를 묻거나 메뉴로 돌아감.
            // 여기서는 실패 시 루프 탈출 (메뉴로 복귀) 하도록 유지
            break;
        }

        if (pSession->getWithdrawalCount() == 3) {
            ui.displayErrorMessage("MaxWithdrawalLimitReached");
            ui.displayMessage("ReturningToMenu");
            ui.wait();
            return;
        }
        int choice = ui.inputInt("AskAdditionalWithdrawal");
        if (choice != 1) {
            ui.displayMessage("ReturningToMenu");
            ui.wait();
            return;
        }
    }
}
