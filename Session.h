#ifndef SESSION_H
#define SESSION_H

#include <vector>
#include <map>
#include <string>
#include "Interface.h"
#include "Bank.h"
#include "Account.h"

// 전방 선언 (Circular Dependency 방지)
class ATM;
class Transaction;

class Session {
private:
    Bank* pBank;
    Account* pAccount;
    Interface& ui;
    ATM* pATM; 
    
    // [수정] Bank* 맵 참조 저장 (TransferTransaction에서 사용)
    map<string, Bank*>& allBanks; 
    
    int withdrawalCount;
    vector<string> historyLog;

    Transaction* deposit;
    Transaction* withdrawal;
    Transaction* transfer;
    static int sessionCount;
    string sessionSummary;

    bool sessionAborted = false;

public:
    // 생성자
    Session(Bank* pBank, Account* pAccount, Interface& ui, ATM* atm, map<string, Bank*>& banks);
    
    // [수정] 소멸자 (구현 필수)
    ~Session();

    void run();

    // [추가] Transaction 객체들이 사용할 Getter
    Interface& getUI() { return ui; }
    Bank* getBank() { return pBank; }
    Account* getAccount() { return pAccount; }
    ATM* getATM() { return pATM; }
    map<string, Bank*>& getAllBanks() { return allBanks; }
    
    int getWithdrawalCount() const { return withdrawalCount; }
    void increaseWithdrawalCount() { withdrawalCount++; }

    void setSessionAborted(bool status) { sessionAborted = status; }
    bool isSessionAborted() const { return sessionAborted; }

    // [추가] 거래 내역 기록 (ATM에도 전달)
    void recordTransaction(const string& log);
    // 세션 요약 기록
    void recordSessionSummary(string accountNumberInput, string cardNumberInput, string transactionTypeInput, int amountInput);
};

#endif
