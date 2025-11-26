#include "WithdrawalTransaction.h"
#include "Session.h"
#include "ATM.h"
#include "Account.h"
#include "Interface.h"
#include <iostream>

using namespace std;

// 생성자: 부모 생성자에 this(Session) 전달
WithdrawalTransaction::WithdrawalTransaction(Session* session) : Transaction(session) {}

bool WithdrawalTransaction::processSingleWithdrawal() {
    Interface& ui = pSession->getUI();
    Account* account = pSession->getAccount();
    ATM* atm = pSession->getATM();

    // 현재 잔액 표시
    ui.displayMessage("CurrentBalance");
    cout << account->getBalance();
    ui.displayMessage("WonUnit");
    cout << endl;

    // [REQ 5.1] 출금액 입력 (스냅샷 지원)
    long amount = ui.inputInt("WithdrawalAmountPrompt");

    if (amount == 0) {
        ui.displayMessage("TransactionCancelled");
        return false;
    }

    // [수정] 출금 시도 시마다 새로운 ID 발급 (반복 출금 시에도 ID 증가)
    transactionID = nextID++;

    // [REQ 5.7] 1회 최대 50만원 제한
    if (amount > 500000) {
        ui.displayErrorMessage("ExceedWithdrawalLimit");
        return false;
    }
    // 1000원 단위 체크
    if (amount % 1000 != 0) {
        ui.displayErrorMessage("InvalidAmountUnit");
        return false;
    }

    // 수수료 계산 (자행/타행 여부에 따라 1000 or 2000)
    long fee = calculateFee(TransactionType::WITHDRAWAL);

    // [REQ 1.8] 출금 수수료는 출금 계좌에서 차감
    long long totalDeduction = amount + fee; // long long으로 계산 안전하게

    // 1. 계좌 잔액 확인
    if (account->getBalance() < totalDeduction) {
        ui.displayErrorMessage("InsufficientBalance");
        return false;
    }

    // 2. ATM 시재(현금) 확인 및 차감 시도
    // [REQ 5.1, 5.2] ATM이 해당 금액을 내줄 수 있는지 확인
    if (!atm->dispenseCash(amount)) {
        ui.displayErrorMessage("InsufficientATMCash");
        return false;
    }

    // 3. 계좌 잔액 실제 차감
    // (앞서 검사했지만, 동시성 이슈 등이 없으므로 안전)
    if (account->deductFunds(totalDeduction)) {
        // 로그 기록
        string summaryLog = "[TransactionID: " + to_string(transactionID) + "] " +
            to_string(amount) + "KRW Withdrawn (Fee: " + to_string(fee) + ")";
        pSession->recordTransaction(summaryLog);
        // 세션 요약 기록
        pSession->recordSessionSummary(pSession->getAccount()->getAccountNumber(), pSession->getAccount()->getCardNumber(), "Withdrawal", amount);

        ui.displayMessage("WithdrawalSuccess");

        // 결과 출력
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

        return true; // 성공 반환
    }
    else {
        // 이론상 여기 도달하면 안 되지만 안전장치
        ui.displayErrorMessage("TransactionFailed");
        return false;
    }
}

void WithdrawalTransaction::run() {
    Interface& ui = pSession->getUI();

    // [REQ 5.6] 세션 당 최대 3회 출금 제한 (진입 전 체크)
    if (pSession->getWithdrawalCount() >= 3) {
        ui.displayErrorMessage("MaxWithdrawalLimitReached");

        pSession->setSessionAborted(true);

        ui.displayMessage("ReturningToMenu");
        ui.wait();
        return;
    }

    // 3회 미만일 때만 루프 진입
    while (pSession->getWithdrawalCount() < 3) {
        bool success = processSingleWithdrawal();

        if (success) {
            // 성공 시 횟수 증가
            pSession->increaseWithdrawalCount();
        }
        else {
            // 실패/취소 시 루프 탈출 (메뉴로 복귀)
            break;
        }

        // 3회 도달 체크 (3회째 출금 성공 직후)
        if (pSession->getWithdrawalCount() == 3) {

            ui.displayErrorMessage("MaxWithdrawalLimitReached"); // 횟수 제한 도달 안내
            ui.displayMessage("ReturningToMenu");
            ui.wait();
            return; // Session::run()으로 복귀
        }

        // 추가 출금 의사 묻기
        int choice = ui.inputInt("AskAdditionalWithdrawal");
        if (choice != 1) {
            ui.displayMessage("ReturningToMenu");
            ui.wait();
            return;
        }
        // 1번 선택 시 루프 계속 (추가 출금)
    }
}
