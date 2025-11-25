#ifndef ATM_H
#define ATM_H

#include <string>
#include <algorithm>
#include "Bank.h"
#include "Interface.h"
#include "CashDenominations.h"

using namespace std;

class Initializer; // 전방 선언

class ATM {
private:    
    Bank* pPrimaryBank;
    string serialNumber;
    string type;
    string languageMode;
    CashDenominations availableCash;
    const string adminCardNumber = "0000-0000-0000"; // [수정] 상수화 권장
    string atmTransactionHistory;
    Initializer* pInit;
    Interface& ui;

public:
    ATM(Bank* primaryBank, const string& serial, const string& t, const string& lang, CashDenominations& initialCash, Initializer* initializer, Interface& uiInput);

    void run();

    string getPrimaryBankName() const { return pPrimaryBank->getPrimaryBank(); }
    string getSerialNumber() const { return serialNumber; }
    string getType() const { return type; }
    string getLanguageMode() const { return languageMode; }
    
    // [수정] const 추가
    string getATMTransactionHistory() const { return atmTransactionHistory; }
    
    void setLanguage();
    bool isAdmin(const string& cardNumberInput); // [수정] const 참조
    void handleAdminSession();
    bool writeHistoryToFile(const string& historyContent) const;
    bool handleUserSession(const string& cardNumberInput, Bank* cardHoldingBank);
    bool isSingle() const; // [수정] const 추가
    bool isValid(const string& cardNumberInput, Bank* cardBank) const;

    // [추가] 거래 내역 추가 함수 (Session에서 호출)
    void addHistory(const string& log);

    // 입출금
    void addCashToATM(const CashDenominations& deposit);
    bool dispenseCash(long amount);

    CashDenominations getCash() const { return availableCash; }
};

#endif
