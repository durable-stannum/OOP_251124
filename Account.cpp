#include "Account.h"
#include <iostream>

using namespace std;

Account::Account(const string& bank, const string& user, const string& accNum, long long bal, const string& cardNum, const string& cardPw)
    : bankName(bank), userName(user), accountNumber(accNum), balance(bal), cardNumber(cardNum), cardPassword(cardPw), transactionHistories("") {} 

bool Account::addFunds(long long amount) {
    if (amount <= 0) {
        return false; 
    }

    balance += amount;
    return true;
}

bool Account::deductFunds(long long amount) {
    if (amount <= 0) {
        return false;
    }
    
    if (balance >= amount) {
        balance -= amount;
        return true;
    }
    return false; // 잔액 부족
}

void Account::addHistory(const string& log) {
    if (!transactionHistories.empty()) {
        transactionHistories += "\n";
    }
    transactionHistories += log;
}
