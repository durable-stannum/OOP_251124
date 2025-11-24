#ifndef WITHDRAWAL_TRANSACTION_H
#define WITHDRAWAL_TRANSACTION_H

#include "Transaction.h"

class WithdrawalTransaction : public Transaction {
private:
    bool processSingleWithdrawal();

public:
    WithdrawalTransaction(Session* session);
    void run() override;

};

#endif
