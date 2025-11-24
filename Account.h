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
    long long balance; // [수정] int -> long long (10^15원 지원을 위해 필수)
    string cardNumber;
    string cardPassword;
    
    // [수정] 관리를 편하게 하기 위해 vector로 변경하거나 string에 append 방식 사용
    // 여기서는 string에 계속 이어 붙이는 방식을 사용하겠습니다.
    string transactionHistories; 

public:
    Account(const string& bank, const string& user, const string& accNum, long long bal, const string& cardNum, const string& cardPw);
    
    // [수정] const 추가 (멤버 변수를 바꾸지 않는 조회 함수들)
    string getAccountNumber() const { return accountNumber; }
    string getCardNumber() const { return cardNumber; }
    string getCardPassword() const { return cardPassword; }

    string getBankName() const { return bankName; }
    string getUserName() const { return userName; }
    long long getBalance() const { return balance; } // [수정] 리턴 타입 long long
    string getTransactionHistory() const { return transactionHistories; }

    bool addFunds(long long amount);      // [수정] 매개변수 long long
    bool deductFunds(long long amount);   // [수정] 매개변수 long long
    
    // [추가] 거래 내역 추가 함수
    void addHistory(const string& log);
};

#endif