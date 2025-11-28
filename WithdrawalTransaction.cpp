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

    transactionID = nextID++;

    try {
        ui.displayMessage("CurrentBalance");
        cout << account->getBalance();
        ui.displayMessage("WonUnit");
        cout << endl;
    
        long amount = ui.inputInt("WithdrawalAmountPrompt");
    
        if (amount == 0) {
            ui.displayMessage("TransactionCancelled");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Canceled Transaction By User)");
            return false;
        }
    
        // 1. 1회 출금 한도 체크
        if (amount > 500000) {
            ui.displayErrorMessage("ExceedWithdrawalLimit");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Exceeded Limit)");
            return false;
        }
        // 2. 금액 단위 체크
        if (amount % 1000 != 0) {
            ui.displayErrorMessage("InvalidAmountUnit");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Invalid Unit)");
            return false;
        }
    
        long fee = calculateFee(TransactionType::WITHDRAWAL);
        long long totalDeduction = amount + fee;
    
        // 3. 계좌 잔액 체크
        if (account->getBalance() < totalDeduction) {
            ui.displayErrorMessage("InsufficientBalance");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Insufficient Balance)");
            return false;
        }
    
        CashDenominations dispensedCash;
    
        // 4. ATM 시재(현금) 부족 체크
        if (!atm->dispenseCash(amount, dispensedCash)) {
            ui.displayErrorMessage("InsufficientATMCash");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Insufficient ATM Cash)");
            return false;
        }

        // 5. 최종 출금 처리 (잔액 차감)
        if (account->deductFunds(totalDeduction)) {
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
            // 이론상 위에서 잔액 체크를 했지만, 알 수 없는 오류로 실패 시
            ui.displayErrorMessage("TransactionFailed");
            pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (System Error)");
            return false;
        }
    }
    catch (const Interface::SessionAbortException&) {
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Withdrawal Failed (Canceled Transaction By User)");
        ui.displayMessage("TransactionCancelled");
        throw; // Session::run()에서 세션 종료를 위해 예외를 다시 던짐
    }
    return true;
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
            // 실패 시 루프를 탈출하여 메뉴로 돌아감 (재시도 로직이 필요하다면 여기서 continue 등 처리)
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
