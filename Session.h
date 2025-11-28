#ifndef SESSION_H
#define SESSION_H

#include <vector>
#include <map>
#include <string>
#include "Interface.h"
#include "Bank.h"
#include "Account.h"

// 전방 선언
class ATM;
class Transaction;

class Session {
private:
    Bank* pBank;
    Account* pAccount;
    Interface& ui;
    ATM* pATM;

    //Bank* 맵 참조 저장 (TransferTransaction에서 사용)
    map<string, Bank*>& allBanks;

    int withdrawalCount;
    string sessionHistoryBuffer;  //현재 세션의 거래 내역을 모아두는 버퍼

    Transaction* deposit;
    Transaction* withdrawal;
    Transaction* transfer;
    static int sessionCount;
    string sessionSummary;

    bool sessionAborted = false;

public:
    Session(Bank* pBank, Account* pAccount, Interface& ui, ATM* atm, map<string, Bank*>& banks);

    ~Session();

    void run();

    //Transaction 객체들이 사용할 Get
    Interface& getUI() { return ui; }
    Bank* getBank() { return pBank; }
    Account* getAccount() { return pAccount; }
    ATM* getATM() { return pATM; }
    map<string, Bank*>& getAllBanks() { return allBanks; }

    int getWithdrawalCount() const { return withdrawalCount; }
    void increaseWithdrawalCount() { withdrawalCount++; }

    void setSessionAborted(bool status) { sessionAborted = status; }
    bool isSessionAborted() const { return sessionAborted; }

    //거래 내역 기록 (버퍼에 저장)
    void recordTransaction(const string& log);

    //세션 요약 기록
    void recordSessionSummary(string accountNumberInput, string cardNumberInput, string transactionTypeInput, int amountInput);
};

#endif
