#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include "ATM.h" // CashDenominations 정의를 위해 필요

using namespace std;

class Session; // 전방 선언

// [추가] 거래 타입을 명확하게 구분하기 위한 Enum Class
enum class TransactionType {
    DEPOSIT,        // 입금
    WITHDRAWAL,     // 출금
    TRANSFER,       // 계좌 이체
    CASH_TRANSFER   // 현금 송금
};

class Transaction {
protected:
    Session* pSession;      
    int transactionID;      
    static int nextID;      

public:
    Transaction(Session* session);
    virtual ~Transaction() {}

    virtual void run() = 0;

    int getTransactionID() const;

    // [수정] string 대신 Enum 사용
    long calculateFee(TransactionType type, const string& destBankName = "") const;
    
    bool collectFee(long fee, CashDenominations& outFeeCash);
};

#endif