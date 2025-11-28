#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>
using namespace std;

class Account {
private:
    string bankName;
    string userName;
    string accountNumber;
    long long balance;
    string cardNumber;
    string cardPassword;
    
    // string에 계속 이어 붙이기
    string transactionHistories; 

public:
    Account(const string& bank, const string& user, const string& accNum, long long bal, const string& cardNum, const string& cardPw);
    
    //
    string getAccountNumber() const { return accountNumber; }
    string getCardNumber() const { return cardNumber; }
    string getCardPassword() const { return cardPassword; }
    string getBankName() const { return bankName; }
    string getUserName() const { return userName; }
    long long getBalance() const { return balance; }
    string getTransactionHistory() const { return transactionHistories; }

    bool addFunds(long long amount);     
    bool deductFunds(long long amount);  
    
    //거래 내역 추가 함수
    void addHistory(const string& log);
};

#endif
