#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include "Bank.h"
#include "ATM.h"
#include "Interface.h"

using namespace std;

class Initializer {
private:
    map<string, Bank*> allBanks; // 모든 은행 데이터 관리
    vector<ATM*> allATMs;        // 모든 ATM 데이터 관리
    Interface& ui;

public:
    Initializer(ifstream& fin, Interface& uiInput);
    ~Initializer();

    void run();
    map<string, Bank*>& getAllBanks() { return allBanks; }
    
    //
    ATM* findATMBySerialNumber(const string& serialNumberInput);
    Bank* findBankByCardNumber(const string& cardNumberInput); 
    Account* findAccountPtrByCardNumber(const string& cardNumberInput);

    // 스냅샷 출력 함수
    void printSnapshot();
};

#endif
