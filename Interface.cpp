#include "Interface.h"
#include <string>
#include <iostream>
#include <vector>
#include "main.h"
#include "ATM.h"
#include "Initializer.h"
#include "CashDenominations.h"
#include <limits>
using namespace std;

Interface::Interface() { addMessages(); } // 생성자



//===================================================================
// 메세지 맵에 문구 추가 (parameter 불필요)
//===================================================================
void Interface::addMessages() {
	// ATM의 언어가 정해지기 전
	msgMap["InitializationStart"]["Unselected"] =
		"======================================================================================================\n"
		"[ System Initialization | 시스템 초기화 ]\n"
		"------------------------------------------------------------------------------------------------------\n"
		"Enter the initial configuration file path. (초기 설정 파일 경로를 입력하세요.)\n"
		"> ";
	msgMap["FileOpenSuccess"]["Unselected"] = "\nFile opened successfully (파일 열기 성공)\n";
	msgMap["FileOpenFail"]["Unselected"] = "\nFailed to open file (파일 열기 실패)\n";
	msgMap["EnterATMSerialNumber"]["Unselected"] =
		"Enter ATM's serial number. (ATM의 고유번호를 입력하세요.)\n"
		"> ";
	msgMap["ChooseLanguage"]["Unselected"] =
		"Press 1 or 2 to choose one language. [1. English/2. Korean] (언어를 선택하기 위해 1 또는 2를 누르세요. (1. 영어/2. 한국어)\n"
		"> ";

	// ATM의 언어가 정해진 후
	msgMap["EnterCardNumber"]["English"] =
		"\nInsert your card into the slot and enter the card number(Include '-'). (Enter 'Back' to quit.)\n"
		"> ";
	msgMap["EnterCardNumber"]["Korean"] =
		"\n카드를 슬롯에 삽입하고 카드 번호('-' 포함)를 입력하세요. (나가려면 '이전'을 입력하세요.)\n"
		"> ";

	msgMap["DeactivateATM"]["English"] = "Deactivate this ATM.\n";
	msgMap["DeactivateATM"]["Korean"] = "이 ATM을 비활성화합니다.\n";

	msgMap["CheckValidity"]["English"] = "Checking card validity... ";
	msgMap["CheckValidity"]["Korean"] = "카드 유효성 확인 중... ";

	msgMap["IsNotValid"]["English"] = "Card is not accepted.\n";
	msgMap["IsNotValid"]["Korean"] = "카드 사용 불가\n";

	msgMap["IsValid"]["English"] = "Card accepted\n";
	msgMap["IsValid"]["Korean"] = "카드 사용 가능\n";

	msgMap["SessionStart"]["English"] = "Session is started.\n";
	msgMap["SessionStart"]["Korean"] = "세션이 시작되었습니다.\n";

	msgMap["TransactionHistoryMenu"]["English"] =
		"======================================================================================================\n"
		"[ Transaction History Menu ]\n"
		"------------------------------------------------------------------------------------------------------\n"
		"1. Viewing and exporting this ATM's transaction history\n"
		"2. Quit\n"
		"> ";
	msgMap["TransactionHistoryMenu"]["Korean"] =
		"======================================================================================================\n"
		"[ 거래 내역 메뉴 ]\n"
		"------------------------------------------------------------------------------------------------------\n"
		"1. 이 ATM의 거래 내역 열람 및 추출\n"
		"2. 나가기\n"
		"> ";

	msgMap["WritingHistoryToFile"]["English"] = "Exporting transaction history to enternal file.\n";
	msgMap["WritingHistoryToFile"]["Korean"] = "거래 기록을 외부 파일로 내보내는 중입니다.\n";

	msgMap["FileWritingSuccess"]["English"] = "File exported successfully\n";
	msgMap["FileWritingSuccess"]["Korean"] = "파일을 성공적으로 내보냈습니다.\n";

	msgMap["FileWritingFailure"]["English"] = "Failed to export file\n";
	msgMap["FileWritingFailure"]["Korean"] = "파일 내보내기에 실패했습니다.\n";

	msgMap["EnterCardPW"]["English"] =
		"\nEnter card password\n"
		"> ";
	msgMap["EnterCardPW"]["Korean"] =
		"\n카드 비밀번호를 입력하세요.\n"
		"> ";

	msgMap["CorrectCardPW"]["English"] = "Correct password\n";
	msgMap["CorrectCardPW"]["Korean"] = "비밀번호 일치\n";

	msgMap["WrongCardPW"]["English"] = "Wrong password\n";
	msgMap["WrongCardPW"]["Korean"] = "잘못된 비밀번호\n";

	msgMap["PwIncorrect3Times"]["English"] = "Incorrect password entered three times in a row.\n";
	msgMap["PwIncorrect3Times"]["Korean"] = "비밀번호가 3번 연속으로 잘못 입력되었습니다.\n";

	msgMap["SessionMenu"]["English"] =
		"======================================================================================================\n"
		"[ Session Menu ]\n"
		"------------------------------------------------------------------------------------------------------\n"
		"1. Deposit\n"
		"2. Withdrawal\n"
		"3. Transfer\n"
		"Enter the transaction number (Enter 0 to quit).\n"
		"> ";
	msgMap["SessionMenu"]["Korean"] =
		"======================================================================================================\n"
		"[ 세션 메뉴 ]\n"
		"------------------------------------------------------------------------------------------------------\n"
		"1. 입금\n"
		"2. 출금\n"
		"3. 이체\n"
		"거래 번호를 입력하세요 (종료하려면 0 입력).\n"
		"> ";

	msgMap["InvalidSelection"]["English"] = "Invalid number.\n";
	msgMap["InvalidSelection"]["Korean"] = "유효하지 않은 번호입니다.\n";

	msgMap["SessionEnd"]["English"] = "Session is ended.\n";
	msgMap["SessionEnd"]["Korean"] = "세션이 종료되었습니다.\n";

	msgMap["GoBackToEnteringCardNumber"]["English"] = "Returning to the card insertion and card number entry step.\n";
	msgMap["GoBackToEnteringCardNumber"]["Korean"] = "새로운 세션인 카드 삽입 및 카드 번호 입력 단계로 돌아갑니다.\n";

	msgMap["AskDepositType"]["English"] =
		"1. Cash Deposit\n"
		"2. Check Deposit\n"
		"Enter deposit type number (Enter 0 to quit).\n"
		"> ";
	msgMap["AskDepositType"]["Korean"] =
		"1. 현금 입금\n"
		"2. 수표 입금\n"
		"입금 종류 번호를 입력하세요 (종료하려면 0 입력).\n"
		"> ";

	msgMap["Input50kCount"]["English"] =
		"Enter the number of 50000Won.\n"
		"> ";
	msgMap["Input50kCount"]["Korean"] =
		"50000원의 장수를 입력하세요.\n"
		"> ";

	msgMap["Input10kCount"]["English"] =
		"Enter the number of 10000Won.\n"
		"> ";
	msgMap["Input10kCount"]["Korean"] =
		"10000원의 장수를 입력하세요.\n"
		"> ";

	msgMap["Input5kCount"]["English"] =
		"Enter the number of 5000Won.\n"
		"> ";
	msgMap["Input5kCount"]["Korean"] =
		"5000원의 장수를 입력하세요.\n"
		"> ";

	msgMap["Input1kCount"]["English"] =
		"Enter the number of 1000Won.\n"
		"> ";
	msgMap["Input1kCount"]["Korean"] =
		"1000원의 장수를 입력하세요.\n"
		"> ";

	msgMap["InvalidInput_Number"]["English"] = "Please enter number.\n";
	msgMap["InvalidInput_Number"]["Korean"] = "숫자를 입력해 주십시오.\n";

	msgMap["InvalidInput_Negative"]["English"] = "Negative number can not be accepted.\n";
	msgMap["InvalidInput_Negative"]["Korean"] = "음수는 입력할 수 없습니다.\n";

	msgMap["ExceedCashLimit"]["English"] = "Cash insertion limit exceeded. You can insert up to 50 papers.\n";
	msgMap["ExceedCashLimit"]["Korean"] = "현금 삽입 제한을 초과했습니다. 현금은 최대 50장까지만 삽입할 수 있습니다.\n";

	msgMap["NoCashInserted"]["English"] = "Cash is not inserted.\n";
	msgMap["NoCashInserted"]["Korean"] = "현금이 삽입되지 않았습니다.\n";

	msgMap["CheckInsertionRule"]["English"] =
		"=================[ Check Deposit Rules ]=================\n"
		"- Minimum check amount: 100,000 KRW\n"
		"- Maximum checks per transaction: 30\n"
		"- Transaction fee must be paid in cash (1,000 KRW bills)\n"
		"=========================================================\n";
	msgMap["CheckInsertionRule"]["Korean"] =
		"=================[ 수표 입금 안내 ]=================\n"
		"- 수표 최소 금액: 100,000원\n"
		"- 1회 최대 입금 장수: 30장\n"
		"- 수수료는 반드시 현금(1,000원권)으로 투입해야 합니다.\n"
		"====================================================\n";

	msgMap["EnterCheckAmount"]["English"] =
		"Enter the check amount. (Enter 0 to finish insertion)\n"
		"> ";
	msgMap["EnterCheckAmount"]["Korean"] =
		"수표 금액을 입력해 주세요. (입력을 종료하려면 0을 입력하세요)\n"
		"> ";

	msgMap["InvalidCheckAmount"]["English"] = "Invalid check amount. Minimum is 100,000 KRW.\n";
	msgMap["InvalidCheckAmount"]["Korean"] = "유효하지 않은 수표 금액입니다. 10만원 이상만 가능합니다.\n";

	msgMap["EnterCheckAmount"]["English"] =
		"수표 장수 입력(Eng.ver)\n"
		"> ";
	msgMap["EnterCheckAmount"]["Korean"] =
		"수표 장수 입력\n"
		"> ";

	msgMap["NoCheckInputExit"]["English"] = "No checks entered. Transaction cancelled.\n";
	msgMap["NoCheckInputExit"]["Korean"] = "입력된 수표가 없습니다. 거래를 취소합니다.\n";

	msgMap["ExceedCheckLimit"]["English"] = "Check insertion limit exceeded. You can insert up to 30 sheets.\n";
	msgMap["ExceedCheckLimit"]["Korean"] = "수표 삽입 제한을 초과했습니다. 수표는 최대 30장까지만 삽입할 수 있습니다.\n";

	msgMap["CheckInsertionConfirm"]["English"] = "Checks inserted successfully.\n";
	msgMap["CheckInsertionConfirm"]["Korean"] = "수표 투입이 완료되었습니다.\n";


	msgMap["WonUnit"]["English"] = " KRW";
	msgMap["WonUnit"]["Korean"] = "원";
	msgMap["SheetUnit"]["English"] = " sheets";
	msgMap["SheetUnit"]["Korean"] = "장";
	msgMap["FeeLabel"]["English"] = "Fee: ";
	msgMap["FeeLabel"]["Korean"] = "수수료: ";


	msgMap["FeeNoticePart1"]["English"] = "Transaction Fee: ";
	msgMap["FeeNoticePart1"]["Korean"] = "수수료 ";
	msgMap["FeeNoticePart2"]["English"] = " KRW is required.";
	msgMap["FeeNoticePart2"]["Korean"] = "원이 필요합니다.";
	msgMap["FeePromptPart1"]["English"] = "Please insert ";
	msgMap["FeePromptPart1"]["Korean"] = "1000원권 ";
	msgMap["FeePromptPart2"]["English"] = " bill(s) of 1000 KRW for fee.\n";
	msgMap["FeePromptPart2"]["Korean"] = "장을 넣어주세요.\n";
	msgMap["InsufficientFee"]["English"] = "Insufficient fee inserted.\n";
	msgMap["InsufficientFee"]["Korean"] = "수수료가 부족합니다.\n";


	msgMap["DepositSuccess"]["English"] = "Deposit Successful!\n";
	msgMap["DepositSuccess"]["Korean"] = "입금 성공!\n";
	msgMap["DepositFailed"]["English"] = "Deposit Failed.\n";
	msgMap["DepositFailed"]["Korean"] = "입금 실패.\n";
	msgMap["DepositAmountLabel"]["English"] = "Deposited Amount: ";
	msgMap["DepositAmountLabel"]["Korean"] = "입금액: ";
	msgMap["TotalDepositAmountLabel"]["English"] = "Total Deposited: ";
	msgMap["TotalDepositAmountLabel"]["Korean"] = "총 입금액: ";


	msgMap["StartCheckDepositInfo"]["English"] = "Start Check Deposit process.\n";
	msgMap["StartCheckDepositInfo"]["Korean"] = "수표 입금을 시작합니다.\n";
	msgMap["PromptCheckAmountLoop"]["English"] = "Enter check amount (0 to finish)\n> ";
	msgMap["PromptCheckAmountLoop"]["Korean"] = "수표 금액 입력 (종료하려면 0)\n> ";
	msgMap["PromptCheckCount"]["English"] = "Enter number of checks\n> ";
	msgMap["PromptCheckCount"]["Korean"] = "수표 장수 입력\n> ";
	msgMap["CheckTotalInfo"]["English"] = "Current Total: "; // totalCheckInfo 함수에서 사용
	msgMap["CheckTotalInfo"]["Korean"] = "현재 총액: ";
	msgMap["FinalCheckTotalAmount"]["English"] = "Final Check Amount: ";
	msgMap["FinalCheckTotalAmount"]["Korean"] = "최종 수표 총액: ";
	msgMap["FinalCheckTotalCount"]["English"] = "Final Check Count: ";
	msgMap["FinalCheckTotalCount"]["Korean"] = "최종 수표 장수: ";
	msgMap["FinalCheckDepositConfirm"]["English"] = "Confirming deposit...\n";
	msgMap["FinalCheckDepositConfirm"]["Korean"] = "입금을 확정합니다...\n";
	msgMap["CheckDepositSuccess"]["English"] = "Check Deposit Successful!\n";
	msgMap["CheckDepositSuccess"]["Korean"] = "수표 입금 성공!\n";


	msgMap["CurrentBalance"]["English"] = "Current Balance: ";
	msgMap["CurrentBalance"]["Korean"] = "현재 잔액: ";
	msgMap["WithdrawalAmountPrompt"]["English"] = "Enter amount to withdraw\n> ";
	msgMap["WithdrawalAmountPrompt"]["Korean"] = "출금할 금액을 입력하세요\n> ";
	msgMap["ExceedWithdrawalLimit"]["English"] = "Exceeded max withdrawal limit (500,000 KRW).\n";
	msgMap["ExceedWithdrawalLimit"]["Korean"] = "1회 출금 한도(50만원)를 초과했습니다.\n";
	msgMap["InvalidAmountUnit"]["English"] = "Amount must be in 1,000 KRW units.\n";
	msgMap["InvalidAmountUnit"]["Korean"] = "1,000원 단위로 입력해주세요.\n";
	msgMap["InsufficientBalance"]["English"] = "Insufficient account balance.\n";
	msgMap["InsufficientBalance"]["Korean"] = "계좌 잔액이 부족합니다.\n";
	msgMap["InsufficientATMCash"]["English"] = "ATM has insufficient cash.\n";
	msgMap["InsufficientATMCash"]["Korean"] = "ATM 내 현금이 부족합니다.\n";
	msgMap["WithdrawalSuccess"]["English"] = "Withdrawal Successful!\n";
	msgMap["WithdrawalSuccess"]["Korean"] = "출금 성공!\n";
	msgMap["WithdrawalAmountLabel"]["English"] = "Withdrawn Amount: ";
	msgMap["WithdrawalAmountLabel"]["Korean"] = "출금액: ";
	msgMap["BalanceAfterTransaction"]["English"] = "Balance after transaction: ";
	msgMap["BalanceAfterTransaction"]["Korean"] = "거래 후 잔액: ";
	msgMap["MaxWithdrawalLimitReached"]["English"] = "Max withdrawal count (3 times) reached.\n";
	msgMap["MaxWithdrawalLimitReached"]["Korean"] = "세션 당 최대 출금 횟수(3회)에 도달했습니다.\n";
	msgMap["AskAdditionalWithdrawal"]["English"] = "1. Withdraw more\n2. Quit\n> ";
	msgMap["AskAdditionalWithdrawal"]["Korean"] = "1. 추가 출금\n2. 종료\n> ";
	msgMap["ReturningToMenu"]["English"] = "Returning to menu...\n";
	msgMap["ReturningToMenu"]["Korean"] = "메뉴로 돌아갑니다...\n";
	msgMap["TransactionCancelled"]["English"] = "Transaction Cancelled.\n";
	msgMap["TransactionCancelled"]["Korean"] = "거래가 취소되었습니다.\n";
	msgMap["TransactionFailed"]["English"] = "Transaction Failed.\n";
	msgMap["TransactionFailed"]["Korean"] = "거래 실패.\n";


	msgMap["TransferOptionMenu"]["English"] = "1. Cash Transfer\n2. Account Transfer\nSelect option (Enter 0 to quit).\n> ";
	msgMap["TransferOptionMenu"]["Korean"] = "1. 현금 송금\n2. 계좌 이체\n원하는 작업을 선택하세요 (종료하려면 0 입력).\n> ";
	msgMap["CashTransferInputGuide"]["English"] = "Please insert cash for transfer.\n";
	msgMap["CashTransferInputGuide"]["Korean"] = "송금할 현금을 투입해주세요.\n";
	msgMap["AccountNumPrompt"]["English"] = "Enter destination account number\n> ";
	msgMap["AccountNumPrompt"]["Korean"] = "받으실 분의 계좌번호를 입력하세요\n> ";
	msgMap["TransferToSelfError"]["English"] = "Cannot transfer to the same account.\n";
	msgMap["TransferToSelfError"]["Korean"] = "자신의 계좌로는 이체할 수 없습니다.\n";
	msgMap["InvalidAccount"]["English"] = "Invalid account number.\n";
	msgMap["InvalidAccount"]["Korean"] = "존재하지 않는 계좌입니다.\n";
	msgMap["TransferAmountPrompt"]["English"] = "Enter transfer amount\n> ";
	msgMap["TransferAmountPrompt"]["Korean"] = "이체할 금액을 입력하세요\n> ";
	msgMap["TransferSuccess"]["English"] = "Transfer Successful!\n";
	msgMap["TransferSuccess"]["Korean"] = "이체 성공!\n";
	msgMap["TransferAmountLabel"]["English"] = "Transferred Amount: ";
	msgMap["TransferAmountLabel"]["Korean"] = "이체 금액: ";
	msgMap["ReceiverLabel"]["English"] = "Receiver: ";
	msgMap["ReceiverLabel"]["Korean"] = "받는 분: ";

	msgMap["PressEnter"]["English"] = "Press Enter to continue...";
	msgMap["PressEnter"]["Korean"] = "계속하려면 엔터 키를 누르세요...";

}



//===================================================================
// 메세지 맵에 문구 추가 (parameter 필요)								<< 추가->출력보다는 인자로 무언가 받고 바로 출력하는 거로 고치기
//===================================================================
void Interface::addMainMenu(const vector<ATM*>& allATMs) {
	string message;
	message += "======================================================================================================\n";
	message += "[ Main Menu | 메인 메뉴 ]\n";
	message += "------------------------------------------------------------------------------------------------------\n";
	message += "- ATM List (ATM 목록) -\n";
	int index = 1;
	for (ATM* atm : allATMs) {
		message += "#" + to_string(index++) + "\n";
		message += "Primary Bank (주거래은행) : " + atm->getPrimaryBankName() + "\n"; // getPrimaryBank -> getPrimaryBankName
		message += "ATM Serial (ATM 고유번호) : " + atm->getSerialNumber() + "\n";
		message += "Type (유형)               : " + atm->getType() + "\n";
		message += "Language (언어)           : " + atm->getLanguageMode() + "\n\n";
	}
	msgMap["MainMenu"]["Unselected"] = message;
}

void Interface::addATMWelcome(const ATM* atm) {
	string message;
	message += "======================================================================================================\n";
	message += "[ Welcome. ATM ";
	message += atm->getSerialNumber();
	message += " activated. | 환영합니다. ATM ";
	message += atm->getSerialNumber();
	message += "이 활성화되었습니다. ]\n";
	message += "------------------------------------------------------------------------------------------------------\n";
	msgMap["ATMWelcome"]["Unselected"] = message;
}

void Interface::addLanguageModeNotification(string languageMode) {
	if (languageMode == "Bilingual") {
		msgMap["LanguageModeNotification"]["English"] = "Language is set to English.\n";
		msgMap["LanguageModeNotification"]["Korean"] = "언어가 한국어로 설정되었습니다.\n";
	}
	else if (languageMode == "Unilingual") {
		msgMap["LanguageModeNotification"]["English"] = "Since this is a unilingual ATM, language is set to English.\n";
		msgMap["LanguageModeNotification"]["Korean"] = "Since this is a unilingual ATM, language is set to English.\n"; // 한국어 모드에서도 영어 출력
	}
}

void Interface::displayMessage(string msgKey) {
	// 키가 없으면 에러 방지를 위해 키 자체를 출력하거나 안전 처리
	if (msgMap.find(msgKey) == msgMap.end()) {
		cout << "[Error: Message Key '" << msgKey << "' not found]" << endl;
		return;
	}
	cout << msgMap[msgKey][language];
}

void Interface::displayErrorMessage(string msgKey) {
	if (msgMap.find(msgKey) == msgMap.end()) {
		cout << "[Error: Message Key '" << msgKey << "' not found]" << endl;
		return;
	}
	cout << msgMap[msgKey][language];
}

// [수정] 정수 입력 (스냅샷 대응)
int Interface::inputInt(string msgKey) {
	string input;
	while (true) {
		if (!msgKey.empty()) displayMessage(msgKey);
		cin >> input;

		// [추가] -1 입력 시 예외 발생
		if (input == "-1") {
			throw SessionAbortException();
		}

		// 스냅샷 체크
		if (input == "/") {
			if (globalInitializer != nullptr) globalInitializer->printSnapshot();
			continue;
		}

		// 숫자 변환 시도
		try {
			int count = stoi(input);
			if (count < 0) {
				if (msgMap.find("InvalidInput_Negative") != msgMap.end())
					cout << msgMap["InvalidInput_Negative"][language];
				else cout << "Negative number not allowed.\n";
				continue;
			}
			return count;
		}
		catch (...) {
			if (msgMap.find("InvalidInput_Number") != msgMap.end())
				cout << msgMap["InvalidInput_Number"][language] << endl;
			else if (language == "Korean") cout << "유효한 숫자를 입력해주세요.\n";
			else cout << "Please enter a valid number.\n";
		}
	}
}

// [수정] 큰 정수(금액) 입력 (스냅샷 대응)
long Interface::inputCheckAmount(string msgKey) {
	string input;
	while (true) {
		if (!msgKey.empty()) displayMessage(msgKey);
		cin >> input;

		// [추가] -1 입력 시 예외 발생
		if (input == "-1") {
			throw SessionAbortException();
		}

		if (input == "/") {
			if (globalInitializer != nullptr) globalInitializer->printSnapshot();
			continue;
		}

		try {
			long long amount = stoll(input); // stoll 사용 (long long)
			if (amount < 0) {
				if (msgMap.find("InvalidInput_Negative") != msgMap.end())
					cout << msgMap["InvalidInput_Negative"][language];
				else if (language == "Korean") cout << "음수는 허용되지 않습니다.\n";
				else cout << "Negative number not allowed.\n";
				continue;
			}
			return (long)amount;
		}
		catch (...) {
			if (msgMap.find("InvalidInput_Number") != msgMap.end())
				cout << msgMap["InvalidInput_Number"][language] << endl;
			else if (language == "Korean") cout << "유효한 숫자를 입력해주세요.\n";
			else cout << "Please enter a valid number.\n";
		}
	}
}

// [추가] 문자열 입력 (스냅샷 대응)
string Interface::inputString(string msgKey) {
	string input;
	while (true) {
		if (!msgKey.empty()) displayMessage(msgKey);
		cin >> input;
		// ⭐️ 1. 이전 버퍼 잔여물(selectLanguage 등) 제거
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		// [추가] -1 입력 시 예외 발생
		if (input == "-1") {
			throw SessionAbortException();
		}

		// 3. 스냅샷 체크
		if (input == "/") {
			if (globalInitializer != nullptr) {
				globalInitializer->printSnapshot();
			}
			return "";
		}

		// 빈 문자열이 아닌 경우 (유효성 검사는 ATM에서 처리)
		if (!input.empty()) {
			return input;
		}
	}

}

void Interface::totalCheckInfo(int amount, int count) {
	if (language == "English") {
		cout << "Current Total: " << amount << " KRW (" << count << " sheets)\n";
	}
	else if (language == "Korean") {
		cout << "현재 총액: " << amount << "원 (" << count << "장)\n";
	}
}


void Interface::displayDispensedCash(const CashDenominations& cash) {
	cout << "--------------------------------------------------" << endl;
	if (language == "Korean") {
		cout << "[ 출금 명세 ]" << endl;
		cout << "50000원권 : " << cash.c50k << "장" << endl;
		cout << "10000원권 : " << cash.c10k << "장" << endl;
		cout << " 5000원권 : " << cash.c5k << "장" << endl;
		cout << " 1000원권 : " << cash.c1k << "장" << endl;
	}
	else {
		cout << "[ Dispensed Cash Detail ]" << endl;
		cout << "50000 KRW : " << cash.c50k << " sheet(s)" << endl;
		cout << "10000 KRW : " << cash.c10k << " sheet(s)" << endl;
		cout << " 5000 KRW : " << cash.c5k << " sheet(s)" << endl;
		cout << " 1000 KRW : " << cash.c1k << " sheet(s)" << endl;
	}
	cout << "--------------------------------------------------" << endl;
}

void Interface::wait() {
	if (msgMap.find("PressEnter") != msgMap.end())
		cout << msgMap["PressEnter"][language];
	else if (language == "Korean") cout << "계속하려면 Enter 누르기...\n";
	else cout << "Press Enter to continue...";

	if (cin.rdbuf()->in_avail() > 0) {
		cin.ignore(10000, '\n'); // 10000자 무시하고 엔터만 찾음
	}
	cin.get();
}
