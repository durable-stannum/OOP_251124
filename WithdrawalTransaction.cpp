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

    // 0 입력 시 취소 (ID 생성 전이므로 기록 없이 종료하거나, 필요 시 기록)
    // 여기서는 ID 증가 전이므로 기록하지 않고 false 반환
    if (amount == 0) {
        ui.displayMessage("TransactionCancelled");
        return false;
    }

    // [중요] 거래 시도가 확정되었으므로 ID 발급
    transactionID = nextID++;

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
