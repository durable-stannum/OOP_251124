#include "Bank.h"
#include <string>
#include <map>

using namespace std;

Bank::Bank(const string& name)
    : bankName(name) {
}

void Bank::addAccount(Account* pAccount) {
    accounts[pAccount->getAccountNumber()] = pAccount; // key: 계좌번호
}

// [수정] const string& 사용
Bank* Bank::getBankByCardNumber(const string& cardNumberInput) {
    for (auto const& pair : accounts) {
        Account* pAccount = pair.second;
        if (cardNumberInput == pAccount->getCardNumber()) {
            return this;
        }
    }
    return nullptr;
}

// [수정] const string& 사용
Account* Bank::getAccountPtrByCardNumber(const string& cardNumberInput) {
    for (auto const& pair : accounts) {
        Account* pAccount = pair.second;
        if (cardNumberInput == pAccount->getCardNumber()) {
            return pAccount;
        }
    }
    return nullptr;
}

// [수정] Account 포인터와 비밀번호 모두 const 처리
bool Bank::isCorrectCardPW(const Account* pAccount, const string& cardPWInput) const {
    // Account의 getCardPassword()도 const여야 함 (Account.h 수정완료 전제)
    return pAccount->getCardPassword() == cardPWInput;
}

// [수정] const string& 사용
Account* Bank::findAccount(const string& accountNumber) {
    auto it = accounts.find(accountNumber);
    if (it != accounts.end()) {
        return it->second;
    }
    return nullptr;
}