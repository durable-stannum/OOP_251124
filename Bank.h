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
    
    // Getter
    string getPrimaryBank() const { return bankName; }
    
    // [추가] 스냅샷(REQ 10.1)을 위해 계좌 목록 전체를 반환하는 함수
    // map의 복사본이 아니라 원본의 참조(reference)를 반환하여 효율성 확보
    const map<string, Account*>& getAccounts() const { return accounts; }

    // 검색 및 검증 기능 (const string& 사용 및 const 함수화)
    Bank* getBankByCardNumber(const string& cardNumberInput);
    Account* getAccountPtrByCardNumber(const string& cardNumberInput);
    bool isCorrectCardPW(const Account* pAccount, const string& cardPWInput) const;

    Account* findAccount(const string& accountNumber);
};

#endif