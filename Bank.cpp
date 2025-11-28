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

Bank* Bank::getBankByCardNumber(const string& cardNumberInput) {
    for (auto const& pair : accounts) {
        Account* pAccount = pair.second;
        if (cardNumberInput == pAccount->getCardNumber()) {
            return this;
        }
    }
    return nullptr;
}

Account* Bank::getAccountPtrByCardNumber(const string& cardNumberInput) {
    for (auto const& pair : accounts) {
        Account* pAccount = pair.second;
        if (cardNumberInput == pAccount->getCardNumber()) {
            return pAccount;
        }
    }
    return nullptr;
}

bool Bank::isCorrectCardPW(const Account* pAccount, const string& cardPWInput) const {
    return pAccount->getCardPassword() == cardPWInput;
}

Account* Bank::findAccount(const string& accountNumber) {
    auto it = accounts.find(accountNumber);
    if (it != accounts.end()) {
        return it->second;
    }
    return nullptr;
}
