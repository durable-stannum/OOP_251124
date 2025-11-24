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

// 전체 은행을 뒤져서 계좌번호로 계좌 찾기
Account* TransferTransaction::findDestinationAccount(const string& destAccNum) {
    // Session을 통해 전체 은행 맵 접근
    map<string, Bank*>& allBanks = pSession->getAllBanks();
    
    for (auto const& [bankName, bankPtr] : allBanks) {
        // Bank::findAccount는 해당 은행에 계좌가 없으면 nullptr 반환
        Account* found = bankPtr->findAccount(destAccNum);
        if (found != nullptr) {
            return found;
        }
    }
    return nullptr;
}

// [기능 1] 현금 송금 (ATM에 현금 투입 -> 타인 계좌로 입금)
void TransferTransaction::processCashTransfer(long fee, Account* destAccount) {
    Interface& ui = pSession->getUI();
    ATM* atm = pSession->getATM();

    // 현금 투입 안내
    ui.displayMessage("CashTransferInputGuide");

    // 현금 장수 입력 (스냅샷 지원 inputInt 사용)
    int cnt50k = ui.inputInt("Input50kCount");
    int cnt10k = ui.inputInt("Input10kCount");
    int cnt5k  = ui.inputInt("Input5kCount");
    int cnt1k  = ui.inputInt("Input1kCount");

    int totalBills = cnt50k + cnt10k + cnt5k + cnt1k;

    // [REQ] 최대 50장 제한
    if (totalBills > 50) { 
        ui.displayErrorMessage("ExceedCashLimit"); 
        return; 
    }
    if (totalBills == 0) { 
        ui.displayMessage("TransactionCancelled"); 
        return; 
    }

    long transferAmount = (long)cnt50k * 50000 + (long)cnt10k * 10000 + 
                          (long)cnt5k * 5000 + (long)cnt1k * 1000;

    // 수수료 징수 (현금으로 따로 받음)
    CashDenominations feeCash = {0, 0, 0, 0};
    if (!collectFee(fee, feeCash)) { return; }

    // ATM 내부 시재 증가 (송금액 + 수수료)
    CashDenominations transferCash = {cnt50k, cnt10k, cnt5k, cnt1k};
    CashDenominations totalToATM;
    totalToATM.c50k = transferCash.c50k + feeCash.c50k;
    totalToATM.c10k = transferCash.c10k + feeCash.c10k;
    totalToATM.c5k  = transferCash.c5k  + feeCash.c5k;
    totalToATM.c1k  = transferCash.c1k  + feeCash.c1k;

    atm->addCashToATM(totalToATM);

    // [REQ 6.7] 상대방 계좌 잔액 증가
    if (destAccount->addFunds(transferAmount)) {
        // 로그 기록
        string summary = "[TxID: " + to_string(transactionID) + "] Cash Transfer " +
                         to_string(transferAmount) + "KRW to " + destAccount->getAccountNumber() + 
                         " (Fee: " + to_string(fee) + ")";
        pSession->recordTransaction(summary);
        
        ui.displayMessage("TransferSuccess");
        
        // 결과 출력
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
    } else {
        ui.displayErrorMessage("TransactionFailed");
    }
    ui.wait();
}

// [기능 2] 계좌 이체 (내 계좌 잔액 차감 -> 타인 계좌 입금)
void TransferTransaction::processAccountTransfer(long fee, Account* destAccount) {
    Interface& ui = pSession->getUI();
    Account* sourceAccount = pSession->getAccount();

    // 이체 금액 입력
    long amount = ui.inputInt("TransferAmountPrompt");

    if (amount == 0) { 
        ui.displayMessage("TransactionCancelled"); 
        return; 
    }

    // [REQ] 내 계좌에서 금액 + 수수료 차감
    long totalDeduction = amount + fee;
    
    if (sourceAccount->getBalance() < totalDeduction) {
        ui.displayErrorMessage("InsufficientBalance"); 
        return;
    }

    // 내 계좌 차감 시도
    if (sourceAccount->deductFunds(totalDeduction)) {
        // 성공 시 상대 계좌 입금
        destAccount->addFunds(amount);

        // 로그 기록
        string summary = "[TxID: " + to_string(transactionID) + "] Account Transfer " +
                         to_string(amount) + "KRW to " + destAccount->getAccountNumber() + 
                         " (Fee: " + to_string(fee) + ")";
        pSession->recordTransaction(summary);

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
        cout << sourceAccount->getBalance(); // 잔액 확인
        ui.displayMessage("WonUnit");
        cout << endl;
    } else {
        ui.displayErrorMessage("TransactionFailed");
    }
    ui.wait();
}

void TransferTransaction::run() {
    Interface& ui = pSession->getUI(); 
    Account* myAccount = pSession->getAccount(); // 내 계좌 (현금 송금 시에는 사용 안 할 수도 있음)

    while (true) {
        // 1. 송금 방식 선택 (1: 현금, 2: 계좌)
        int choice = ui.inputInt("TransferOptionMenu"); 
        
        if (choice == 0) { 
            ui.displayMessage("TransactionCancelled");
            return; 
        }
        
        if (choice != 1 && choice != 2) { 
            // ui.displayErrorMessage("InvalidSelection"); 
            cout << "Invalid selection." << endl;
            continue; 
        }

        // 2. 목적지 계좌번호 입력
        ui.displayMessage("AccountNumPrompt");
        
        // [수정] cin -> ui.inputString 사용 (스냅샷 REQ 10.1 대응)
        // 빈 문자열("")을 넘겨서 메세지 중복 출력 방지
        string destAccNum = ui.inputString("");

        if (destAccNum == "0" || destAccNum == "Back") { 
            ui.displayMessage("TransactionCancelled");
            return; 
        }

        // 자가 이체 방지 (계좌 이체의 경우)
        if (choice == 2 && destAccNum == myAccount->getAccountNumber()) {
             ui.displayMessage("TransferToSelfError"); 
             continue; 
        }

        // 목적지 계좌 존재 확인
        Account* destAccount = findDestinationAccount(destAccNum);
        if (destAccount == nullptr) {
            ui.displayMessage("InvalidAccount"); 
            continue; 
        }

        // 3. 실제 이체 프로세스 시작
        if (choice == 1) {
            // 현금 송금: 타행 여부 상관없이 수수료 고정 (REQ 1.8: Cash transfer fee to any bank type: 2000)
            // destAccount 정보는 필요하지만, 수수료 계산에는 '내 은행' 정보가 필요 없음
            long fee = calculateFee(TransactionType::CASH_TRANSFER);
            processCashTransfer(fee, destAccount);
        } else {
            // 계좌 이체: 상대 은행 이름 필요 -> 수수료 계산에 사용
            long fee = calculateFee(TransactionType::TRANSFER, destAccount->getBankName());
            processAccountTransfer(fee, destAccount);
        }
        
        // 거래 완료 후 루프 탈출
        break; 
    }
}