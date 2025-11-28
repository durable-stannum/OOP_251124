#ifndef BANK_H
#define BANK_H

#include <string>
#include <map>
#include "Account.h"

using namespace std;

class Bank {
private:
    string bankName;
    map<string, Account*> accounts; // key: 계좌번호

public:    
    Bank(const string& name);
    
    // 계좌 추가
    void addAccount(Account* account);
    
    // Get
    string getPrimaryBank() const { return bankName; }
    const map<string, Account*>& getAccounts() const { return accounts; }

    // 검색 및 검증 기능
    Bank* getBankByCardNumber(const string& cardNumberInput);
    Account* getAccountPtrByCardNumber(const string& cardNumberInput);
    bool isCorrectCardPW(const Account* pAccount, const string& cardPWInput) const;

    Account* findAccount(const string& accountNumber);
};

#endif
