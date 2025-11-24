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
    ~Initializer(); // [추가] 메모리 해제를 위한 소멸자

    void run();
    map<string, Bank*>& getAllBanks() { return allBanks; }
    
    // [수정] const string& 적용 및 안전성 강화
    ATM* findATMBySerialNumber(const string& serialNumberInput);
    Bank* findBankByCardNumber(const string& cardNumberInput); 
    Account* findAccountPtrByCardNumber(const string& cardNumberInput);

    // [추가] REQ 10.1 스냅샷 출력 함수
    void printSnapshot();
};

#endif