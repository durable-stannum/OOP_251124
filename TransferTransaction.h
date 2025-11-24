#ifndef TRANSFER_TRANSACTION_H
#define TRANSFER_TRANSACTION_H

#include "Transaction.h"

class Account; // 전방 선언

class TransferTransaction : public Transaction {
private:
    // 목적지 계좌 찾기 (전체 은행 검색)
    Account* findDestinationAccount(const string& destAccNum);
    
    // 1. 현금 송금 (Cash Transfer)
    void processCashTransfer(long fee, Account* destAccount);
    
    // 2. 계좌 이체 (Account Transfer)
    void processAccountTransfer(long fee, Account* destAccount);

public:
    TransferTransaction(Session* session);
    void run() override;
};    

#endif