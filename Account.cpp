#include "Account.h"
#include <iostream>

using namespace std;

// [수정] 생성자: balance를 long long으로 받음, transactionHistory 인자 제거 (초기값은 비어있음)
Account::Account(const string& bank, const string& user, const string& accNum, long long bal, const string& cardNum, const string& cardPw)
    : bankName(bank), userName(user), accountNumber(accNum), balance(bal), cardNumber(cardNum), cardPassword(cardPw), transactionHistories("") {} 

bool Account::addFunds(long long amount) {
    if (amount <= 0) {
        return false; 
    }
    // [추가] 오버플로우 방지 로직 (선택사항이지만 안전을 위해 추천)
    // if (balance > LLONG_MAX - amount) return false; 

    balance += amount;
    return true;
}

bool Account::deductFunds(long long amount) {
    // [수정] 출금 단위 체크 (1000원 단위)는 Account가 아니라 WithdrawalTransaction에서 하는 게 맞지만,
    // 이중 검증 차원에서 남겨두어도 무방합니다.
    if (amount <= 0) {
        return false;
    }
    
    if (balance >= amount) {
        balance -= amount;
        return true;
    }
    return false; // 잔액 부족
}

// [추가] 거래 내역 기록 구현
void Account::addHistory(const string& log) {
    if (!transactionHistories.empty()) {
        transactionHistories += "\n";
    }
    transactionHistories += log;
}