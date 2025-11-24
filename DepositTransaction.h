#ifndef DEPOSIT_TRANSACTION_H
#define DEPOSIT_TRANSACTION_H

#include "Transaction.h"

class DepositTransaction : public Transaction {
private:
    void processCashDeposit(long fee);
    void processCheckDeposit(long fee);

public:
    DepositTransaction(Session* session);
    void run() override;
    
};

#endif
