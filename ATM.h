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
    int totalSessionCount = 0;
    Bank* pPrimaryBank;
    string serialNumber;
    string type;
    string languageMode;
    CashDenominations availableCash;
    string atmTransactionHistory; // 전체 히스토리 저장
    Initializer* pInit;
    Interface& ui;

public:
    ATM(Bank* primaryBank, const string& serial, const string& t, const string& lang, CashDenominations& initialCash, Initializer* initializer, Interface& uiInput);

    void run();

    string getPrimaryBankName() const { return pPrimaryBank->getPrimaryBank(); }
    string getSerialNumber() const { return serialNumber; }
    string getType() const { return type; }
    string getLanguageMode() const { return languageMode; }

    string getATMTransactionHistory() const { return atmTransactionHistory; }

    void setLanguage();
    bool isAdmin(const string& cardNumberInput);
    void handleAdminSession();
    bool writeHistoryToFile(const string& historyContent) const;
    bool handleUserSession(const string& cardNumberInput, Bank* cardHoldingBank);
    bool isSingle() const;
    bool isValid(const string& cardNumberInput, Bank* cardBank) const;

    //Session 클래스 종료 시 호출
    void saveSessionHistory(const string& cardNum, const string& accNum, const string& sessionLogs);

    // 입출금
    void addCashToATM(const CashDenominations& deposit);
    bool dispenseCash(long amount, CashDenominations& outDispensedCash);

    CashDenominations getCash() const { return availableCash; }
};

#endif
