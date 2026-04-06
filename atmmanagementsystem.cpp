#include <iostream>
#include <fstream>
#include <cstdlib>
#include <conio.h> 
using namespace std;

void bankmenu()
{
	cout << "------Welcome to my bank------" << endl;
	cout << "1-Customer mode" << endl;
	cout << "2-Manager mode" << endl;
	cout << "3-Exit program" << endl;
}
const int account_numbers = 9;
long long accounts[account_numbers] = {};
int pin[account_numbers] = {};
int balance[account_numbers] = {};
char status[account_numbers] = {};
int failedAttempts[account_numbers] = {};
int locked[account_numbers] = {};
int txn_id = 100;

int getpin()
{
	int pin = 0;
	char ch;
	int digits = 0;
	cout << "Enter your pin = ";
	pin = 0;
	while (digits < 4)
	{
		ch = _getch();
		if (ch >= '0' && ch <= '9')
		{
			cout << '*';
			pin = pin * 10 + (ch - '0');
			digits++;
		}
	}
	cout << endl;
	return pin;
}

void fetchdata()
{
	ifstream accountfile("account.txt");
	if (!accountfile)
	{
		cout << "Error: file not found." << endl;
		return;
	}
	int index = 0;
	while (index < account_numbers && (accountfile >> accounts[index] >> pin[index] >> balance[index] >> status[index] >> failedAttempts[index] >> locked[index]))
	{
		index++;
	}
	accountfile.close();
}

int findaccount(long long acc_number)
{
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] == acc_number)
		{
			return i;
		}
	}
	return -1;
}

bool backup_account_file()
{
	ifstream file1("account.txt");
	ofstream file2("account_backup.txt");

	if (!file2)
	{
		cout << "Backup failed; your data is safe but not updated." << endl;
		if (file1)
		{
			file1.close();
		}
		return false;
	}

	if (file1)
	{
		long long accountNumber;
		int accountPin;
		int accountBalance;
		char accountStatus;
		int failedPinAttempts;
		int accountLocked;

		while (file1 >> accountNumber >> accountPin >> accountBalance >> accountStatus >> failedPinAttempts >> accountLocked)
		{
			file2 << accountNumber << " " << accountPin << " " << accountBalance << " " << accountStatus << " " << failedPinAttempts << " " << accountLocked << endl;
			if (!file2)
			{
				cout << "Backup failed; your data is safe but not updated." << endl;
				file1.close();
				file2.close();
				return false;
			}
		}

		file1.close();
		file2.close();
		return true;
	}
	else
	{
		for (int index = 0; index < account_numbers; index++)
		{
			if (accounts[index] != 0)
			{
				file2 << accounts[index] << " " << pin[index] << " " << balance[index] << " " << status[index] << " " << failedAttempts[index] << " " << locked[index] << endl;
				if (!file2)
				{
					cout << "Backup failed; your data is safe but not updated." << endl;
					file2.close();
					return false;
				}
			}
		}
		file2.close();
		return true;
	}
}

void deposit(long long acc_number, int entered_pin)
{
	int index = findaccount(acc_number);
	if (status[index] == 'C')
	{
		cout << "This account is closed. Deposit not allowed." << endl;
		return;
	}
	int deposit_amount;
	cout << "Enter amount you want to deposit = ";
	cin >> deposit_amount;
	if (deposit_amount <= 0)
	{
		cout << "Invalid amount entered" << endl;
		return;
	}
	balance[index] = balance[index] + deposit_amount;
	cout << "Amount " << deposit_amount << " has been successfully deposited." << endl;
	cout << "New balance = " << balance[index] << endl;
	ofstream file("account.txt");
	for (int i = 0; i < account_numbers; i++)
	{
		file << accounts[i] << " " << pin[i] << " " << balance[i] << " " << status[i] << " " << failedAttempts[i] << " " << locked[i] << endl;
	}
	file.close();
	backup_account_file();
	ofstream transactionfile("transaction.txt", ios::app);
	transactionfile << "Account: " << acc_number << " | Deposit: " << deposit_amount << " | New Balance: " << balance[index] << endl;
	transactionfile.close();
}

void withdraw_menu(long long acc_number, int entered_pin)
{
	int index = findaccount(acc_number);
	if (status[index] == 'C')
	{
		cout << "This account is closed cannot withdraw" << endl;
		return;
	}
	int withdraw_amount;
	cout << "Enter amount you want to withdraw = ";
	cin >> withdraw_amount;
	if (withdraw_amount <= 0 || withdraw_amount > balance[index])
	{
		cout << "Cant withdraw this amount!!";
		return;
	}
	balance[index] = balance[index] - withdraw_amount;
	cout << "Amount " << withdraw_amount << " has been successfully withdrawn." << endl;
	cout << "New balance = " << balance[index] << endl;
	ofstream file("account.txt");
	for (int i = 0; i < account_numbers; i++)
	{
		file << accounts[i] << " " << pin[i] << " " << balance[i] << " " << status[i] << " " << failedAttempts[i] << " " << locked[i] << endl;
	}
	file.close();
	backup_account_file();
	ofstream transactionfile("transaction.txt", ios::app);
	transactionfile << "Account: " << acc_number << " | Withdraw: " << withdraw_amount << " | New Balance: " << balance[index] << endl;
	transactionfile.close();
}

void transfer_money(long long acc_number, int entered_pin)
{
	int sender_index = findaccount(acc_number);
	int reciever_acc;
	cout << "Enter account you wish to send money to = ";
	cin >> reciever_acc;
	int reciever_index = findaccount(reciever_acc);
	if (status[reciever_index] == 'C')
	{
		cout << "This account is closed cant transfer money to this account" << endl;
		return;
	}
	if (reciever_index == -1)
	{
		cout << "This account doesnt exist. Enter valid account try again." << endl;
		return;
	}
	if (reciever_index == sender_index)
	{
		cout << "Same account entered. Cant send money to this account" << endl;
		return;
	}
	int transfer_amount;
	cout << "Enter amount you want to transfer = ";
	cin >> transfer_amount;
	if (transfer_amount <= 0)
	{
		cout << "Invalid amount entered!!" << endl;
		return;
	}
	if (balance[sender_index] < transfer_amount)
	{
		cout << "Unsufficent balance cant send money from this account" << endl;
		return;
	}
	if (status[reciever_index] == 'D')
	{
		cout << "Current account is deactivated cant send money to this account" << endl;
		return;
	}
	int corelation_ID = txn_id++;
	balance[sender_index] = balance[sender_index] - transfer_amount;
	balance[reciever_index] = balance[reciever_index] + transfer_amount;
	cout << "Amount " << transfer_amount << " transferred successfully!" << endl;
	cout << "Your new balance = " << balance[sender_index] << endl;
	cout << "Your new balance = " << balance[reciever_index] << endl;
	ofstream file("account.txt");
	for (int i = 0; i < account_numbers; i++)
	{
		file << accounts[i] << " " << pin[i] << " " << balance[i] << " " << status[i] << " " << failedAttempts[i] << " " << locked[i] << endl;
	}
	file.close();
	backup_account_file();
	ofstream transactionfile("transaction.txt", ios::app);
	transactionfile << "[TXN ID: " << corelation_ID << "] " << "Sender Account: " << acc_number << " | Transferred: " << transfer_amount << " | New Balance: " << balance[sender_index] << endl;
	transactionfile << "[TXN ID: " << corelation_ID << "] " << "Receiver Account: " << reciever_acc << " | Received: " << transfer_amount << " | New Balance: " << balance[reciever_index] << endl;
}

void changepin(long long acc_number, int entered_pin)
{
	int initial_pin_index = findaccount(acc_number);
	if (status[initial_pin_index] == 'C')
	{
		cout << "This account is closed. Deposit not allowed." << endl;
		return;
	}
	int newpin;
	while (true)
	{
		newpin = getpin();
		if (newpin >= 1000 && newpin <= 9999) break;
		cout << "PIN must be 4 digits. Enter again: ";
	}
	if (newpin == pin[initial_pin_index])
	{
		cout << "You have entered the same pin again try again" << endl;
		return;
	}
	for (int i = 0; i < account_numbers; i++)
	{
		if (i != initial_pin_index && pin[i] == newpin)
		{
			cout << "Entered pin already exists for another account. Choose a different pin." << endl;
			return;
		}
	}
	int comfirmnewpin;
	comfirmnewpin = getpin();
	if (comfirmnewpin != newpin)
	{
		cout << "Pin donot match" << endl;
		return;
	}
	int oldpin = pin[initial_pin_index];
	pin[initial_pin_index] = newpin;
	cout << "Pin successfully changed from " << oldpin << " to " << newpin << endl;
	ofstream file("account.txt");
	for (int i = 0; i < account_numbers; i++)
	{
		file << accounts[i] << " " << pin[i] << " " << balance[i] << " " << status[i] << " " << failedAttempts[i] << " " << locked[i] << endl;
	}
	file.close();
	backup_account_file();
	ofstream transactionfile("transaction.txt", ios::app);
	transactionfile << "Account: " << acc_number << " | Pin changed successfully" << endl;
	transactionfile.close();
}
void checkbalance(long long acc_number, int pin)
{
	int index = findaccount(acc_number);
	cout << "The balance of account" << " " << acc_number << " " << "is" << " " << balance[index] << endl;
}
void ministatements(long long acc_number, int pin)
{
	int index = findaccount(acc_number);
	if (status[index] == 'C')
	{
		cout << "This account is closed no statements in this" << endl;
		return;
	}
	int k;
	cout << "Enter how many transactions you'd like to see = ";
	cin >> k;
	if (k <= 0)
	{
		cout << "Inavalid input entered!!" << endl;
		return;
	}
	ifstream file("transaction.txt");
	if (!file)
	{
		cout << "No transactions found for this account" << endl;
		return;
	}
	char line[200];
	long long temp_acc;
	int total = 0;
	while (file.getline(line, 200))
	{
		temp_acc = 0;
		for (int i = 0; line[i] != 0; i++)
		{
			if (line[i] >= '0' && line[i] <= '9')
			{
				temp_acc = temp_acc * 10 + (line[i] - '0');
				if (temp_acc == acc_number)
				{
					total++;
					break;
				}
			}
			else
			{
				temp_acc = 0;
			}
		}
	}
	file.close();
	if (total == 0)
	{
		cout << "No transactions found for this account" << endl;
		return;
	}
	file.open("transaction.txt");
	int shown = 0, count = 0;
	cout << "Last " << k << " Transactions for Account " << acc_number << endl;
	while (file.getline(line, 200))
	{
		temp_acc = 0;
		for (int i = 0; line[i] != '\0'; i++)
		{
			if (line[i] >= '0' && line[i] <= '9')
			{
				temp_acc = temp_acc * 10 + (line[i] - '0');
				if (temp_acc == acc_number)
				{
					count++;
					if (count > total - k)
					{
						cout << line << endl;
						shown++;
					}
					break;
				}
			}
			else
			{
				temp_acc = 0;
			}
		}
	}
	file.close();
	if (shown == 0)
	{
		cout << "No transactions to show " << endl;
	}
}
void cmenu()
{
	cout << "1-Deposit" << endl;
	cout << "2-Withdraw" << endl;
	cout << "3-Transfer money" << endl;
	cout << "4-Check Balance" << endl;
	cout << "5-Change Pin" << endl;
	cout << "6-Mini statements" << endl;
}

int customer_menu()
{
	fetchdata();
	long long acc_number;
	cout << "Enter your account number = ";
	cin >> acc_number;
	int index = findaccount(acc_number);
	if (index == -1)
	{
		cout << "Account not found" << endl;
		return 0;
	}
	if (locked[index] == 1)
	{
		cout << "Account is locked due to multiple failed PIN attempts. Contact manager to unlock." << endl;
		return 0;
	}
	int entered_pin;
	int tries = 0;
	do
	{
		entered_pin = getpin();
		if (entered_pin == pin[index]) break;
		tries++;
		failedAttempts[index]++;
		if (tries < 3)
		{
			cout << "Invalid pin. You have " << (3 - tries) << " tries left" << endl;
		}
	} while (tries < 3);

	if (entered_pin != pin[index])
	{
		locked[index] = 1;
		cout << "Account has been locked due to 3 invalid PIN attempts! Contact manager." << endl;

		ofstream file("account.txt");
		for (int i = 0; i < account_numbers; i++)
		{
			if (accounts[i] != 0)
			{
				file << accounts[i] << " " << pin[i] << " " << balance[i] << " " << status[i] << " " << failedAttempts[i] << " " << locked[i] << endl;
			}
		}
		file.close();
		return 0;
	}
	if (status[index] == 'D' || status[index] == 'C')
	{
		cout << "Current account is deactivated or closed , cannot use any services except checking balances" << endl;
		int choice;
		do
		{
			cout << "1-Check Balance" << endl;
			cout << "3-Exit" << endl;
			cin >> choice;
			switch (choice)
			{
			case 1: checkbalance(acc_number, entered_pin); break;
			case 3: cout << "Logging out of the account" << endl; break;
			default:cout << "Invalid choice enter again" << endl; break;
			}
		} while (choice != 3);
		return 0;
	}
	int choice;
	do
	{
		cmenu();
		cout << "Enter your choice = ";
		cin >> choice;
		system("cls");
		switch (choice)
		{
		case 1: deposit(acc_number, entered_pin); break;
		case 2: withdraw_menu(acc_number, entered_pin); break;
		case 3: transfer_money(acc_number, entered_pin); break;
		case 4: checkbalance(acc_number, entered_pin); break;
		case 5: changepin(acc_number, entered_pin); break;
		case 6: ministatements(acc_number, entered_pin); break;
		case 0:cout << "Logging out" << endl; break;
		default:cout << "Invalid choice, try again" << endl; break;
		}
	} while (choice != 0);
}

void mmenu()
{
	cout << "1-Open Account" << endl;
	cout << "2-Close Account" << endl;
	cout << "3-Deactivate / Reopen Account" << endl;
	cout << "4-Search Accounts" << endl;
	cout << "5-Display All Accounts" << endl;
	cout << "6-Unlock Account" << endl;
	cout << "7-Logout" << endl;
}
int getManagerPin()
{
	int pin = 0;
	char ch;
	int digits = 0;
	cout << "Enter manager pin = ";
	pin = 0;
	while (digits < 4)
	{
		ch = _getch();
		if (ch >= '0' && ch <= '9')
		{
			cout << '*';
			pin = pin * 10 + (ch - '0');
			digits++;
		}
	}
	cout << endl;
	return pin;
}
void openaccount()
{
	fetchdata();

	long long highestaccount = 0;
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] > highestaccount)
		{
			highestaccount = accounts[i];
		}
	}
	if (highestaccount == 0)
	{
		highestaccount = 10000;
	}
	long long newaccount = highestaccount + 1;
	int filledaccounts = 0;
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] != 0)
		{
			filledaccounts++;
		}
	}
	if (filledaccounts >= account_numbers)
	{
		cout << "Bank limit reached! Cannot open more accounts." << endl;
		return;
	}
	cout << "New account number: " << newaccount << endl;
	int manager_pin;
	ifstream admin("admin.txt");
	if (!admin)
	{
		cout << "Error: admin.txt not found!" << endl;
		return;
	}
	admin >> manager_pin;
	admin.close();

	int newpin;
	bool unique;
	do
	{
		unique = true;
		newpin = getManagerPin();

		if (newpin < 1000 || newpin > 9999)
		{
			cout << "PIN must be 4 digits. Enter again." << endl;
			unique = false;
			continue;
		}
	} while (!unique);

	int choice;
	int newbalance = 0;
	cout << "Do you want to deposit an initial amount? (1 = YES, 2 = NO): ";
	cin >> choice;

	if (choice == 1)
	{
		cout << "Enter initial deposit amount: ";
		cin >> newbalance;

		if (newbalance < 0)
		{
			cout << "Invalid amount entered. Account not created." << endl;
			return;
		}
	}
	else if (choice == 2)
	{
		newbalance = 0;
	}
	else
	{
		cout << "Invalid choice cannot craeate account" << endl;
		return;
	}

	char newstatus;
	do
	{
		cout << "Enter account status (A for Active / D for Deactivated): ";
		cin >> newstatus;
		if (newstatus != 'A' && newstatus != 'D')
		{
			cout << "Invalid input! Please enter only A or D." << endl;
		}
	} while (newstatus != 'A' && newstatus != 'D');

	int newIndex = -1;
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] == 0)
		{
			newIndex = i;
			break;
		}
	}
	accounts[newIndex] = newaccount;
	pin[newIndex] = newpin;
	balance[newIndex] = newbalance;
	status[newIndex] = newstatus;
	failedAttempts[newIndex] = 0;
	locked[newIndex] = 0;

	ofstream file("account.txt");
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] != 0)
		{
			file << accounts[i] << " " << pin[i] << " " << balance[i] << " " << status[i] << " " << failedAttempts[i] << " " << locked[i] << endl;
		}
	}
	file.close();
	cout << "\nAccount created successfully!" << endl;
	cout << "Account number: " << newaccount << " | PIN: " << newpin << " | Balance: " << newbalance << " | Status: " << newstatus << endl;

	ofstream log("manageractivity.txt", ios::app);
	log << "Manager created new account: " << newaccount << " | Balance: " << newbalance << " | Status: " << newstatus << endl;
	log.close();
}
void closeaccount()
{
	fetchdata();
	long long targetaccount;
	cout << "Enter account number to close = ";
	cin >> targetaccount;

	bool found = false;
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] == targetaccount)
		{
			found = true;
			if (status[i] == 'C')
			{
				cout << "Account is already closed" << endl;
				return;
			}
			cout << "Are you sure you want to close this account? (1 = YES, 2 = NO) = ";
			int confirm;
			cin >> confirm;
			if (confirm != 1)
			{
				cout << "Account closure cancelled" << endl;
				return;
			}
			status[i] = 'C';

			ofstream file("account.txt");
			for (int j = 0; j < account_numbers; j++)
			{
				if (accounts[j] != 0)
				{
					file << accounts[j] << " " << pin[j] << " " << balance[j] << " " << status[j] << " " << failedAttempts[j] << " " << locked[j] << endl;
				}
			}
			file.close();
			cout << "Account " << targetaccount << " closed successfully." << endl;
			ofstream log("manageractivity.txt", ios::app);
			log << "Manager closed account: " << targetaccount << endl;
			log.close();
			return;
		}
	}
	if (!found)
	{
		cout << "Account not found!" << endl;
	}
}
void activate_deactivate_account()
{
	fetchdata();
	long long targetaccount;
	cout << "Enter account number to deactivate/reopen = ";
	cin >> targetaccount;

	bool found = false;
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] == targetaccount)
		{
			found = true;

			if (status[i] == 'A')
			{
				cout << "Account is currently Active. Do you want to Deactivate it? (1 = YES, 2 = NO) = ";
				int confirm;
				cin >> confirm;
				if (confirm != 1)
				{
					cout << "Operation cancelled." << endl;
					return;
				}
				status[i] = 'D';
				cout << "\nAccount " << targetaccount << " deactivated successfully." << endl;
				ofstream log("manageractivity.txt", ios::app);
				log << "Manager deactivated account:" << targetaccount << endl;
				log.close();
			}
			else if (status[i] == 'D' || status[i] == 'C')
			{
				cout << "Account is currently Deactivated or Closed. Do you want to Reopen it? (1 = YES, 2 = NO) = ";
				int confirm;
				cin >> confirm;
				if (confirm != 1)
				{
					cout << "Operation cancelled." << endl;
					return;
				}
				status[i] = 'A';
				cout << "Account " << targetaccount << " reopened successfully." << endl;

				ofstream log("manageractivity.txt", ios::app);
				log << "Manager reopened account: " << targetaccount << endl;
				log.close();
			}
			ofstream file("account.txt");
			for (int j = 0; j < account_numbers; j++)
			{
				if (accounts[j] != 0)
				{
					file << accounts[j] << " " << pin[j] << " " << balance[j] << " " << status[j] << " " << failedAttempts[j] << " " << locked[j] << endl;
				}
			}
			file.close();
			return;
		}
	}
	if (!found)
	{
		cout << "Account not found!" << endl;
	}
}
void search_account()
{
	fetchdata();
	long long targetaccount;
	cout << "Enter account number to search = ";
	cin >> targetaccount;
	bool found = false;
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] == targetaccount)
		{
			found = true;
			cout << "Account found!" << endl;
			cout << "Account number: " << accounts[i] << endl;
			cout << "PIN: " << pin[i] << endl;
			cout << "Balance: " << balance[i] << endl;
			cout << "Status: " << status[i] << endl;

			ofstream log("manageractivity.txt", ios::app);
			log << "Manager searched account: " << targetaccount << endl;
			log.close();
			break;
		}
	}
	if (!found)
	{
		cout << "Account not found!" << endl;
	}
}
void displayaccounts()
{
	fetchdata();

	for (int i = 0; i < account_numbers - 1; i++)
	{
		for (int j = 0; j < account_numbers - i - 1; j++)
		{
			if (balance[j] < balance[j + 1])
			{
				int temp_balance = balance[j];
				balance[j] = balance[j + 1];
				balance[j + 1] = temp_balance;

				long long temp_account = accounts[j];
				accounts[j] = accounts[j + 1];
				accounts[j + 1] = temp_account;

				int temp_pin = pin[j];
				pin[j] = pin[j + 1];
				pin[j + 1] = temp_pin;

				char temp_status = status[j];
				status[j] = status[j + 1];
				status[j + 1] = temp_status;
			}
		}
	}
	cout << "All accounts(balance high to low):" << endl;
	cout << "Account Number | PIN | Balance | Status" << endl;
	bool any = false;
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] != 0)
		{
			cout << accounts[i] << " | " << pin[i] << " | " << balance[i] << " | " << status[i] << endl;
			any = true;
		}
	}
	if (!any)
	{
		cout << "No accounts found" << endl;
	}
	ofstream log("manageractivity.txt", ios::app);
	log << "Manager viewed all accounts" << endl;
	log.close();
}
void unlock_account()
{
	fetchdata();
	long long acc;
	cout << "Enter account number to unlock: ";
	cin >> acc;
	bool found = false;
	for (int i = 0; i < account_numbers; i++)
	{
		if (accounts[i] == acc)
		{
			found = true;

			if (status[i] == 'C')
			{
				cout << "Account is closed and cannot be unlocked." << endl;
				return;
			}
			if (locked[i] == 0)
			{
				cout << "Account is already unlocked." << endl;
				return;
			}
			locked[i] = 0;
			failedAttempts[i] = 0;
			cout << "Account " << acc << " has been unlocked successfully!" << endl;

			ofstream file("account.txt");
			for (int j = 0; j < account_numbers; j++)
			{
				if (accounts[j] != 0)
				{
					file << accounts[j] << " " << pin[j] << " " << balance[j] << " " << status[j] << " " << failedAttempts[j] << " " << locked[j] << endl;
				}
			}
			file.close();
			ofstream log("manageractivity.txt", ios::app);
			log << "Manager unlocked account: " << acc << endl;
			log.close();
			return;
		}
	}
	if (!found)
	{
		cout << "Account not found!" << endl;
	}
}
int verifyManagerPin()
{
	int stored;
	ifstream fin("admin.txt");
	if (!fin)
	{
		cout << "File not found\n";
		return 0;
	}

	fin >> stored;
	fin.close();

	int decrypted = 0;
	int temp = stored;

	while (temp > 0)
	{
		int dec = temp % 10;
		dec = dec ^ 5;
		decrypted = dec + decrypted * 10;
		temp /= 10;
	}
	int tries = 0;
	int userPin;

	while (tries < 3)
	{
		userPin = getManagerPin();
		if (userPin == decrypted)
		{
			cout << "Access granted successfully!" << endl;
			return 1;
		}
		else
		{
			tries++;
			if (tries < 3)
				cout << "Invalid PIN. You have " << (3 - tries) << " tries left." << endl;
		}
	}

	cout << "Too many invalid attempts. Why are you trying to hack manager?" << endl;
	return 0;
}

int managermode()
{
	if (!verifyManagerPin())
	{
		return 0;
	}
	int choice;
	do
	{
		mmenu();
		cout << "Enter your choice = ";
		cin >> choice;
		system("cls");
		switch (choice)
		{
		case 1: openaccount(); break;
		case 2: closeaccount(); break;
		case 3: activate_deactivate_account(); break;
		case 4: search_account(); break;
		case 5: displayaccounts(); break;
		case 6: unlock_account(); break;
		case 7: cout << "Logging out of manager mode" << endl; break;
		default: cout << "Invalid choice. Try again." << endl; break;
		}
	} while (choice != 0);
}
int main()
{
	int choice;
	do
	{
		bankmenu();
		cin >> choice;
		system("cls");
		switch (choice)
		{
		case 1: customer_menu(); break;
		case 2: managermode(); break;
		case 3: cout << "Thank you for visiting our bank"; break;
		default:cout << "Invalid choice, try again" << endl; break;
		}
	} while (choice != 3);
	return 0;
}
