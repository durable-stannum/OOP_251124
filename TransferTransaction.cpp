#include "TransferTransaction.h"
#include "Session.h"
#include "ATM.h"
#include "Account.h"
#include "Interface.h"
#include "Bank.h"
#include <iostream>
#include <map>

using namespace std;

TransferTransaction::TransferTransaction(Session* session) : Transaction(session) {}

Account* TransferTransaction::findDestinationAccount(const string& destAccNum) {
    map<string, Bank*>& allBanks = pSession->getAllBanks();

    for (auto const& [bankName, bankPtr] : allBanks) {
        Account* found = bankPtr->findAccount(destAccNum);
        if (found != nullptr) {
            return found;
        }
    }
    return nullptr;
}

// [기능 1] 현금 송금
void TransferTransaction::processCashTransfer(long fee, Account* destAccount) {
    Interface& ui = pSession->getUI();
    ATM* atm = pSession->getATM();

    ui.displayMessage("CashTransferInputGuide");

    int cnt50k = ui.inputInt("Input50kCount");
    int cnt10k = ui.inputInt("Input10kCount");
    int cnt5k = ui.inputInt("Input5kCount");
    int cnt1k = ui.inputInt("Input1kCount");

    int totalBills = cnt50k + cnt10k + cnt5k + cnt1k;

    if (totalBills > 50) {
        ui.displayErrorMessage("ExceedCashLimit");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Transfer Failed (Exceed Cash Limit)");
        return;
    }
    if (totalBills == 0) {
        ui.displayMessage("TransactionCancelled");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Transfer Cancelled");
        return;
    }

    long transferAmount = (long)cnt50k * 50000 + (long)cnt10k * 10000 +
        (long)cnt5k * 5000 + (long)cnt1k * 1000;

    CashDenominations feeCash = { 0, 0, 0, 0 };
    if (!collectFee(fee, feeCash)) {
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Transfer Failed (Fee Not Paid)");
        return;
    }

    CashDenominations transferCash = { cnt50k, cnt10k, cnt5k, cnt1k };
    CashDenominations totalToATM;
    totalToATM.c50k = transferCash.c50k + feeCash.c50k;
    totalToATM.c10k = transferCash.c10k + feeCash.c10k;
    totalToATM.c5k = transferCash.c5k + feeCash.c5k;
    totalToATM.c1k = transferCash.c1k + feeCash.c1k;

    atm->addCashToATM(totalToATM);

    if (destAccount->addFunds(transferAmount)) {
        string summary = "Transaction ID" + to_string(transactionID) + ": " +
            to_string(transferAmount) + "Won transfered from " +
            pSession->getAccount()->getAccountNumber() +
            " to " + destAccount->getAccountNumber();

        pSession->recordTransaction(summary);
        pSession->recordSessionSummary(pSession->getAccount()->getAccountNumber(), pSession->getAccount()->getCardNumber(), "Cash Transfer", transferAmount);

        ui.displayMessage("TransferSuccess");

        ui.displayMessage("TransferAmountLabel");
        cout << transferAmount;
        ui.displayMessage("WonUnit");
        cout << endl;

        ui.displayMessage("FeeLabel");
        cout << fee;
        ui.displayMessage("WonUnit");
        cout << endl;

        ui.displayMessage("ReceiverLabel");
        cout << destAccount->getUserName() << endl;
    }
    else {
        ui.displayErrorMessage("TransactionFailed");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Cash Transfer Failed (System Error)");
    }
    ui.wait();
}

// [기능 2] 계좌 이체
void TransferTransaction::processAccountTransfer(long fee, Account* destAccount) {
    Interface& ui = pSession->getUI();
    Account* sourceAccount = pSession->getAccount();

    ui.displayMessage("CurrentBalance");
    cout << sourceAccount->getBalance();
    ui.displayMessage("WonUnit");
    cout << endl;

    long amount = ui.inputInt("TransferAmountPrompt");

    if (amount == 0) {
        ui.displayMessage("TransactionCancelled");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Account Transfer Cancelled");
        return;
    }

    long totalDeduction = amount + fee;

    if (sourceAccount->getBalance() < totalDeduction) {
        ui.displayErrorMessage("InsufficientBalance");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Account Transfer Failed (Insufficient Balance)");
        return;
    }

    if (sourceAccount->deductFunds(totalDeduction)) {
        destAccount->addFunds(amount);

        string summary = "Transaction ID" + to_string(transactionID) + ": " +
            to_string(amount) + "Won transfered from " +
            sourceAccount->getAccountNumber() +
            " to " + destAccount->getAccountNumber();

        pSession->recordTransaction(summary);
        pSession->recordSessionSummary(pSession->getAccount()->getAccountNumber(), pSession->getAccount()->getCardNumber(), "Account Transfer", amount);

        ui.displayMessage("TransferSuccess");

        ui.displayMessage("TransferAmountLabel");
        cout << amount;
        ui.displayMessage("WonUnit");
        cout << endl;

        ui.displayMessage("FeeLabel");
        cout << fee;
        ui.displayMessage("WonUnit");
        cout << endl;

        ui.displayMessage("BalanceAfterTransaction");
        cout << sourceAccount->getBalance();
        ui.displayMessage("WonUnit");
        cout << endl;
    }
    else {
        ui.displayErrorMessage("TransactionFailed");
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Account Transfer Failed (System Error)");
    }
    ui.wait();
}

void TransferTransaction::run() {
    Interface& ui = pSession->getUI();
    Account* myAccount = pSession->getAccount();

    transactionID = nextID++;
    try {
        while (true) {
            int choice = ui.inputInt("TransferOptionMenu");
    
            if (choice == 0) {
                ui.displayMessage("TransactionCancelled");
                pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Transfer Failed (Canceled Transaction By User)");
                return;
            }
    
            if (choice != 1 && choice != 2) {
                ui.displayErrorMessage("InvalidSelection");
                continue;
            }
    
            string destAccNum;
            while (true) {
                ui.displayMessage("AccountNumPrompt");
                destAccNum = ui.inputString("");
                if (destAccNum.empty()) {
                    continue;
                }
                break;
            }
    
            if (destAccNum == "0" || destAccNum == "Back") {
                ui.displayMessage("TransactionCancelled");
                pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Transfer Failed (Canceled Transaction By User)");
                return;
            }
    
            if (choice == 2 && destAccNum == myAccount->getAccountNumber()) {
                ui.displayMessage("TransferToSelfError");
                continue;
            }
    
            Account* destAccount = findDestinationAccount(destAccNum);
            if (destAccount == nullptr) {
                ui.displayMessage("InvalidAccount");
                continue;
            }
    
    
            if (choice == 1) {
                long fee = calculateFee(TransactionType::CASH_TRANSFER);
                processCashTransfer(fee, destAccount);
            }
            else {
                long fee = calculateFee(TransactionType::TRANSFER, destAccount->getBankName());
                processAccountTransfer(fee, destAccount);
            }
    
            break;
        }
    }
    catch (const Interface::SessionAbortException&) {
        pSession->recordTransaction("Transaction ID" + to_string(transactionID) + ": Transfer Failed (Canceled Transaction By User)");
        ui.displayMessage("TransactionCancelled");
        throw; // Session::run()에서 세션 종료를 위해 예외를 다시 던짐
    }
}
