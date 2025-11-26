#include "WithdrawalTransaction.h"
#include "Session.h"
#include "ATM.h"
#include "Account.h"
#include "Interface.h"
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

    transactionID = nextID++;

    if (amount > 500000) {
        ui.displayErrorMessage("ExceedWithdrawalLimit");
        return false;
    }
    if (amount % 1000 != 0) {
        ui.displayErrorMessage("InvalidAmountUnit");
        return false;
    }

    long fee = calculateFee(TransactionType::WITHDRAWAL);
    long long totalDeduction = amount + fee;

    if (account->getBalance() < totalDeduction) {
        ui.displayErrorMessage("InsufficientBalance");
        return false;
    }

    if (!atm->dispenseCash(amount)) {
        ui.displayErrorMessage("InsufficientATMCash");
        return false;
    }

    if (account->deductFunds(totalDeduction)) {
        // [수정] 로그 포맷 변경: Transaction ID[N]: [Amount]Won withdrawed from [AccNum]
        // (요구사항에 맞춰 withdrawed라는 표현 유지)
        string summaryLog = "Transaction ID" + to_string(transactionID) + ": " +
            to_string(amount) + "Won withdrawed from " + account->getAccountNumber();

        pSession->recordTransaction(summaryLog);
        pSession->recordSessionSummary(pSession->getAccount()->getAccountNumber(), pSession->getAccount()->getCardNumber(), "Withdrawal", amount);

        ui.displayMessage("WithdrawalSuccess");

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
