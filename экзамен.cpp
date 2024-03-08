#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <ctime>
#include <algorithm>
#include <fstream>
using namespace std;
class Transaction {
public:
    double amount;
    string category;
    time_t transaction_time;

    Transaction(double amt, string cat) : amount(amt), category(move(cat)) {
        transaction_time = time(nullptr);
    }
};
class BankAccount {
protected:
    double balance;
    vector<Transaction> transactions;
public:
    virtual ~BankAccount() {}
    BankAccount(double init_balance = 0.0) : balance(init_balance) {}
    virtual bool deposit(double amount) {
        if (amount <= 0) {
            return false;
        }
        balance += amount;
        transactions.push_back(Transaction(amount, "Deposit"));
        return true;
    }
    virtual bool withdraw(double amount) {
        if (amount <= 0 || amount > balance) {
            return false;
        }
        balance -= amount;
        transactions.push_back(Transaction(amount, "Withdrawal"));
        return true;
    }
    double getBalance() const {
        return balance;
    }
    vector<Transaction> getTransactionsForPeriod(time_t start, time_t end) const {
        vector<Transaction> filtered_transactions;
        copy_if(transactions.begin(), transactions.end(), back_inserter(filtered_transactions),
            [start, end](const Transaction& t) {
                return t.transaction_time >= start && t.transaction_time <= end;
            });
        return filtered_transactions;
    }
    void addTransaction(const Transaction& transaction) {
        if (transaction.category == "Deposit") {
            balance += transaction.amount;
        }
        else if (transaction.category == "Withdrawal" && balance >= transaction.amount) {
            balance -= transaction.amount;
        }
        else {
            cout << "Error: Invalid transaction or insufficient funds for withdrawal.\n";
            return; 
        }

        transactions.push_back(transaction);
        cout << "Transaction added." << endl;
    }
    vector<Transaction> getAllTransactions() const {
        return transactions;
    }
};
void generateReport(const vector<Transaction>& transactions) {
    unordered_map<string, double> category_totals;
    for (const auto& transaction : transactions) {
        category_totals[transaction.category] += transaction.amount;
    }

    for (const auto& pair : category_totals) {
        cout << "Category: " << pair.first << ", Total Spent: " << pair.second << endl;
    }
}
class Card {
private:
    string card_number;
    string card_type;
    BankAccount* linked_account;
public:
    Card(string number, std::string type, BankAccount* account) : card_number(number), card_type(type), linked_account(account) {}
    bool deposit(double amount) {
        if (linked_account->deposit(amount)) {
            cout << "Deposit successful through card" << endl;
            return true;
        }
        else {
            cout << "Deposit failed" << endl;
            return false;
        }
    }
    bool withdraw(double amount) {
        if (linked_account->withdraw(amount)) {
            cout << "Withdrawal successful through card" << endl;
            return true;
        }
        else {
            cout << "Withdrawal failed" << endl;
            return false;
        }
    }
};
class SavingAccount : public BankAccount {
public:
    SavingAccount(double init_balance = 0.0) : BankAccount(init_balance) {}
    void apply_interest() {
        double interest = balance * 0.05;
        deposit(interest);
    }
};
class Account {
public:
    string owner;
    string account_id;
    vector<Card*> cards;
    vector<BankAccount*> accounts;
    Account(string owner_name, string id) : owner(owner_name), account_id(id) {}
    void addCard(string number, string type, BankAccount* account) {
        cards.emplace_back(new Card(number, type, account));
    }
    void applyInterestToSavingAccounts() {
        for (auto& account : accounts) {
            SavingAccount* savingAccount = dynamic_cast<SavingAccount*>(account);
            if (savingAccount != nullptr) {
                savingAccount->apply_interest();
            }
        }
    }
};
void saveReportToFile(const unordered_map<string, double>& category_totals, const string& filepath) {
    ofstream file(filepath);
    if (file.is_open()) {
        for (const auto& pair : category_totals) {
            file << "Category: " << pair.first << ", spent: " << pair.second << '\n';
        }
        file.close();
        cout << "Report savd to " << filepath << endl;
    }
    else {
        cerr << "Unable to open file: " << filepath << endl;
    }
}
void loadReportFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file " << filename << std::endl;
        return;
    }
    std::string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}
int main() {
    BankAccount account;
    Card card("123456789", "Debit", &account);
    bool running = true;
    while (running) {
        cout << "\nBank Account Manager" << endl;
        cout << "1. Add Transaction" << endl;
        cout << "2. Generate report" << endl;
        cout << "3. Save report" << endl;
        cout << "4. Load  report" << endl;
        cout << "5. Deposit" << endl;
        cout << "6. Withdrawal" << endl;
        cout << "7. Get balance" << endl;
        cout << "8. Exit" << endl;
        cout << "Choose option: ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            double amount;
            string category;
            cout << "Enter amount: ";
            cin >> amount;
            cout << "Enter category: ";
            cin >> category;
            account.addTransaction(Transaction(amount, category));
            cout << "Transaction added." << endl;
        }
        else if (choice == 2) {
            vector<Transaction> transactions = account.getAllTransactions();
            unordered_map<string, double> category_totals;
            for (const auto& transaction : transactions) {
                category_totals[transaction.category] += transaction.amount;
            }

            for (const auto& pair : category_totals) {
                cout << "Category: " << pair.first << ", Total Spent: " << pair.second << endl;
            }
        }
        else if (choice == 3) {
            string filename;
            cout << "Enter filename to save report: ";
            cin >> filename;

            vector<Transaction> transactions = account.getAllTransactions();
            unordered_map<string, double> category_totals;
            for (const auto& transaction : transactions) {
                category_totals[transaction.category] += transaction.amount;
            }

            saveReportToFile(category_totals, filename);
            cout << "Report saved to " << filename << endl;
        } else if (choice == 4) {
            string filename;
            cout << "Enter filename to load report from: ";
            cin >> filename;
            loadReportFromFile(filename);
            cout << "Report loaded from " << filename << endl;
        } else if (choice == 5) {
            double amount;
            cout << "Enter deposit amount: ";
            cin >> amount;
            card.deposit(amount);
        }
        else if (choice == 6) {
            double amount;
            cout << "Enter withdrawal amount: ";
            cin >> amount;
            card.withdraw(amount);
        }
        else if (choice == 7) {
            cout << "Balance: " << account.getBalance();
        }
        else if (choice == 8) {
            running = false;
            cout << "Exiting program." << endl;
        }
        else {
            cout << "Invalid option. Please try again." << endl;
        }
    }
    return 0;
}


/*Создайте подобие банковского приложения. Система должна иметь следующие возможности: 

■ Наличие разных карт (дебетовых/кредитных) и счетов. Счета могут быть:
  • привязаны к карте;
  • существовать внутри аккаунта человека сами по себе. 

■ Пополнение счетов и карт;

■ Снятие с счетов и карт;  

■ Ведение списка затрат. Каждая затрата относится к определенной категории;
  К примеру - покупка, снятие наличных, оплата коммуналки или телефона         (последнее можно пометить одной категорией, типа, utility payment) 

■ Формирование отчетов по затратам и категориям: 

• день; 

• неделя; 

• месяц. 

■ Формирование рейтингов по максимальным суммам: 

• ТОП-3 затрат: 

• неделя; 

• месяц. 

• ТОП-3 категорий: 

• неделя; 

• месяц. 

■ Сохранение отчетов и рейтингов в файл.

Вы получите доп. баллы за реализацию сберегательных счетов, которые будут самостоятельно пополняться в начале каждого месяца. Для этого можно реализовать volatile переменную, которая будет получать информацию от календаря в ОС, сравниваться с датой и временем создания счёта, и раз в месяц выполнять функцию пополнения счёта на определённый процент. Я не показывал вам как это сделать, поэтому воспользуйтесь интернетом.

Не забудьте залить ваш код на Github, BitBucket или любой другой онлайн репозиторий по вашему выбору.*/