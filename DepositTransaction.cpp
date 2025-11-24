#include "DepositTransaction.h"
#include "Session.h"
#include "ATM.h"     // CashDenominations 구조체 및 ATM 함수 사용
#include "Account.h" // Account 함수 사용
#include "Interface.h" // Interface 함수 사용
#include <iostream>

using namespace std;

// 생성자
DepositTransaction::DepositTransaction(Session* session) : Transaction(session) {}

// [기능 1] 현금 입금
void DepositTransaction::processCashDeposit(long fee) {
    Interface& ui = pSession->getUI();
    Account* account = pSession->getAccount();
    ATM* atm = pSession->getATM();

    // 현금 장수 입력 (스냅샷 지원)
    int cnt50k = ui.inputInt("Input50kCount");
    int cnt10k = ui.inputInt("Input10kCount");
    int cnt5k  = ui.inputInt("Input5kCount");
    int cnt1k  = ui.inputInt("Input1kCount");

    int totalBills = cnt50k + cnt10k + cnt5k + cnt1k;

    // [REQ 4.2] 최대 매수 제한 (50장)
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

    // 수수료 징수 (현금)
    CashDenominations feeCash = {0, 0, 0, 0};
    if (!collectFee(fee, feeCash)) { return; }

    // ATM에 추가할 현금 계산 (입금액 + 수수료)
    CashDenominations depositCash = {cnt50k, cnt10k, cnt5k, cnt1k};
    CashDenominations totalToATM;
    totalToATM.c50k = depositCash.c50k + feeCash.c50k;
    totalToATM.c10k = depositCash.c10k + feeCash.c10k;
    totalToATM.c5k  = depositCash.c5k  + feeCash.c5k;
    totalToATM.c1k  = depositCash.c1k  + feeCash.c1k;

    // [REQ 4.5] ATM 시재 증가 (입금된 현금은 다시 출금 가능)
    atm->addCashToATM(totalToATM);

    // [REQ 4.3] 계좌 잔액 증가
    if (account->addFunds(depositAmount)) {
        // 로그 기록
        string summaryLog = "[TxID: " + to_string(transactionID) + "] " + 
                            to_string(depositAmount) + "KRW Deposited (Fee: " + to_string(fee) + ")";
        pSession->recordTransaction(summaryLog); 
        
        ui.displayMessage("DepositSuccess");
        
        // 결과 출력
        ui.displayMessage("DepositAmountLabel"); 
        cout << depositAmount;
        ui.displayMessage("WonUnit"); 
        cout << endl;

        ui.displayMessage("FeeLabel");
        cout << fee;
        ui.displayMessage("WonUnit");
        cout << endl;
    } else {
        ui.displayErrorMessage("DepositFailed");
    }
    ui.wait();
}


// [기능 2] 수표 입금
void DepositTransaction::processCheckDeposit(long fee) {
    Interface& ui = pSession->getUI();
    Account* account = pSession->getAccount();
    ATM* atm = pSession->getATM();
    
    long long totalCheckAmount = 0; // 금액이 크므로 long long 권장
    int totalCheckCount = 0;

    ui.displayMessage("StartCheckDepositInfo");

    while (true) {
        // 수표 금액 입력 (10만원 이상)
        long amount = ui.inputCheckAmount("PromptCheckAmountLoop"); 
        if (amount == 0) break; // 0 입력 시 종료
        
        // [REQ] 수표는 10만원 이상이어야 함
        if (amount < 100000) { 
            ui.displayErrorMessage("InvalidCheckAmount"); // "10만원 이상만 가능" 메세지 필요
            continue; 
        }
        
        int count = ui.inputInt("PromptCheckCount");
        if (count <= 0) { 
            // ui.displayErrorMessage("InvalidInput_Negative"); 
            cout << "Invalid count." << endl;
            continue; 
        }
        
        totalCheckAmount += (amount * count);
        totalCheckCount += count;
        
        // 중간 집계 출력 
        ui.totalCheckInfo((int)totalCheckAmount, totalCheckCount);
        
        // [REQ 4.2] 수표 최대 장수 제한 (30장) - 루프 중간에도 체크 가능
        if (totalCheckCount > 30) {
            ui.displayErrorMessage("ExceedCheckLimit");
            // 초과 시 전액 취소하고 리턴할지, 다시 입력받을지는 정책 나름. 여기선 리턴.
            return;
        }
    }

    if (totalCheckCount == 0) { 
        ui.displayErrorMessage("NoCheckInputExit"); 
        return; 
    }

    // 최종 확인 출력
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

    // 수수료 징수 (현금)
    CashDenominations feeCash = {0, 0, 0, 0};
    if (!collectFee(fee, feeCash)) { return; }
    
    // [REQ 4.6] 수표는 ATM 시재를 증가시키지 않음. 
    // 하지만 수수료로 받은 '현금'은 ATM 시재에 추가해야 함.
    atm->addCashToATM(feeCash);

    // 계좌 잔액 증가
    if (account->addFunds(totalCheckAmount)) {
        string summaryLog = "[TxID: " + to_string(transactionID) + "] " +
                            to_string(totalCheckAmount) + "KRW (Check) Deposited (Fee: " + to_string(fee) + ")";
        pSession->recordTransaction(summaryLog);
  
        ui.displayMessage("CheckDepositSuccess");
        
        ui.displayMessage("TotalDepositAmountLabel");
        cout << totalCheckAmount;
        ui.displayMessage("WonUnit");
        cout << endl;
    } else {
        ui.displayErrorMessage("DepositFailed");
    }
    ui.wait();
}


void DepositTransaction::run() {
    Interface& ui = pSession->getUI();
    
    // 입금 유형 선택 (1: 현금, 2: 수표)
    int depositType = ui.inputInt("AskDepositType"); 
    
    if (depositType != 1 && depositType != 2) {
        // ui.displayErrorMessage("InvalidSelection");
        cout << "Invalid selection." << endl;
        return;
    }

    // 입금 수수료는 자행/타행 여부만 따짐 (현금/수표 동일 로직 적용)
    long fee = calculateFee(TransactionType::DEPOSIT);

    if (depositType == 1) {
        processCashDeposit(fee);
    } else {
        processCheckDeposit(fee);
    }
}