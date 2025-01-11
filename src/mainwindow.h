#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define USER_ACC_DETAILS_FILE_ADDRESS QString("resource/Transactions.txt")


#include "authentication.h"
#include "booklist_admin.h"

#include <QMainWindow>
#include <QDate>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

namespace trans {
enum EN_AccType{
    savings=0,
    chequing,
    accNone
};

enum En_Type {
    invalidTransaction,
    withdrawal,
    deposit,
};

enum EN_ColumnRoles {
    CUSTOMER_CARDNUMBER=0,
    CUSTOMER_PIN,
    CUSTOMER_FIRSTNAME,
    CUSTOMER_LASTNAME,
    CUSTOMER_PHONENUMBER,
    CUSTOMER_INFO_COUNT
};
enum EN_COLUMN_INDEX {
    index_cardnumber,
    index_accounttype,
    index_priorbalance,
    index_transactionamount,
    index_newbalance,
    index_date,
    index_transactiontype,
    accountDetailColumnCount
};

struct Customer {
    QString m_cardnumber;
    QString m_pin;
    QString m_firstname;
    QString m_lastname;
    QString m_phonenumber;

    Customer() : m_cardnumber(-1), m_pin(-1) { }

    Customer(QString cardnumber, QString pin, QString firstname, QString lastname, QString phonenumber)
        : m_cardnumber(cardnumber), m_pin(pin), m_firstname(firstname),
        m_lastname(lastname), m_phonenumber(phonenumber) {}

    // By default if not assigned yet or Validation method to check if the customer is valid
    bool isValid() const {
        return m_cardnumber.isEmpty() == false && m_pin.isEmpty() == false;
    }

    // Print customer details using qDebug
    void printDetails() const;
};
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *auth, QString user_logged_in, bool sex, QWidget *parent = nullptr);

    ~MainWindow();

    void setCustomerDetails(trans::Customer* customerParam);

private slots:


private:
    void requestAmountToDepositForAccountType(trans::EN_AccType);
    void requestAmountToWithdrawForAccountType(trans::EN_AccType);
    void depositAmountForAccountType(trans::EN_AccType, double);
    // void withdrawAmountForAccountType(trans::EN_AccType, double);
    Ui::MainWindow *ui;

    QPointF oldPos; // for draggable
    QString user;
    BookList_Admin* bookList_Admin;
    trans::Customer* m_customer = nullptr;

    QWidget *aui = nullptr;

    BookList_Admin* transactionListWindow = nullptr;

    double requestAmountForTransaction(trans::EN_AccType, trans::En_Type);

    // returns last transaction of the given type, spillted ',' and filled like column wise.
    QStringList getLastTransactionDetailsForAccountType(trans::EN_AccType);
    bool appendRowToFile(QString detailsOfTransaction);


    trans::EN_AccType getAccountTypeSelected();

    void onButtonDepositPressed();
    void onButtonWithdrawalPressed();
    void onButtonShowHistoryPressed();
    void onButtonLogoutPressed();
};
#endif // MAINWINDOW_H
