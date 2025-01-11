#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QInputDialog>

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QDoubleValidator>


MainWindow::MainWindow(QWidget *auth, QString user_logged_in, bool sex, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->user = user_logged_in;

    ui->label->setText("Admin Dashboard | " + QDate::currentDate().toString("dddd, MMMM dd, yyyy"));
    aui = auth;

    bookList_Admin = new BookList_Admin(ui, this);

    transactionListWindow = new BookList_Admin(ui, this);

    connect(ui->buttonDeposit, &QPushButton::pressed, this, &MainWindow::onButtonDepositPressed);
    connect(ui->buttonWithdraw, &QPushButton::pressed, this, &MainWindow::onButtonWithdrawalPressed);
    connect(ui->buttonTransactionHistory, &QPushButton::pressed, this, &MainWindow::onButtonShowHistoryPressed);
    connect(ui->buttonLogout, &QPushButton::pressed, this, &MainWindow::onButtonLogoutPressed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCustomerDetails(trans::Customer *customerParam) {
    if(!customerParam)
        return;
    m_customer = customerParam;

    qDebug().nospace().noquote() << "Customer details filled:"
                                 << "\nCard Number:" << m_customer->m_cardnumber
                                 << "\nName:" << m_customer->m_firstname << " " << m_customer->m_lastname
                                 << "\nPhone:" << m_customer->m_phonenumber;
}

double MainWindow::requestAmountForTransaction(trans::EN_AccType accType, trans::En_Type transType) {
    if (accType == trans::accNone)
        return .0;

    QString accountType = (accType == trans::savings) ? "Savings" : "Chequing";

    QString winTitle, label;
    if (transType == trans::deposit) {
        winTitle = QString("Deposit into %1 Account").arg(accountType);
        label = QString("Enter the amount you want to deposit to your %1 account:").arg(accountType);
    } else if (transType == trans::withdrawal) {
        winTitle = QString("Withdraw from %1 Account").arg(accountType);
        label = QString("Enter the amount you want to withdraw from your %1 account:").arg(accountType);
    } else {
        return 0.0;
    }

    // Create and configure the dialog
    QDialog dialog(this);
    dialog.setWindowTitle(winTitle);
    dialog.setFixedSize(400, 200);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLabel* infoLabel = new QLabel(label, &dialog);
    QLineEdit* amountInput = new QLineEdit(&dialog);

    amountInput->setPlaceholderText("Enter amount");
    amountInput->setValidator(new QDoubleValidator(0.01, 1e6, 2, &dialog));

    QPushButton* confirmButton = new QPushButton("Confirm", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(infoLabel);
    layout->addWidget(amountInput);
    layout->addLayout(buttonLayout);

    double inputAmount = 0.0;

    // Connect buttons to dialog actions
    connect(confirmButton, &QPushButton::clicked, [&]() {
        QString amountText = amountInput->text();
        if (amountText.isEmpty()) {
            QMessageBox::warning(&dialog, "Error", "Please enter an amount.");
            return;
        }

        inputAmount = amountText.toDouble();
        dialog.accept();
    });

    connect(cancelButton, &QPushButton::clicked, [&]() {
        dialog.reject();
    });

    // Execute the dialog and return the value
    if (dialog.exec() == QDialog::Accepted) {
        return inputAmount;
    }

    return 0.0; // Return 0.0 if the dialog is canceled
}

trans::EN_AccType MainWindow::getAccountTypeSelected() {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Select Account Type");
    msgBox.setText("Please select your account type:");

    QPushButton* chequingButton = msgBox.addButton("Chequing", QMessageBox::ActionRole);
    QPushButton* savingsButton = msgBox.addButton("Savings", QMessageBox::ActionRole);

    msgBox.exec();

    if (msgBox.clickedButton() == chequingButton)
        return trans::chequing;
    if(msgBox.clickedButton() == savingsButton)
        return trans::savings;
    return trans::accNone;
}

void MainWindow::onButtonDepositPressed() {
    trans::EN_AccType accType = getAccountTypeSelected();

    if(accType == trans::accNone)
        return;

    auto transactionAmount = requestAmountForTransaction(accType, trans::deposit);
    if(transactionAmount < 1)
        return;


    QStringList lastTransactionDetails = getLastTransactionDetailsForAccountType(accType);

    double currentBalance{ };
    if(lastTransactionDetails.length() == trans::accountDetailColumnCount) {
        currentBalance = QString(lastTransactionDetails[trans::index_newbalance]).toDouble();
    }

    QString transactionLog = QString("%1,%2,%3,%4,%5,%6,%7")
                                 .arg(m_customer->m_cardnumber)
                                 .arg(accType == trans::chequing ? "Chequing" : "Savings")
                                 .arg(currentBalance)
                                 .arg(transactionAmount)
                                 .arg(QString::number((currentBalance + transactionAmount), 'f', 2))
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"))
                                 .arg("Deposit");

    appendRowToFile(transactionLog);

    qDebug() << Q_FUNC_INFO << "Transaction info:\n"
             << transactionLog;
}

void MainWindow::onButtonWithdrawalPressed() {
    trans::EN_AccType accType = getAccountTypeSelected();

    if (accType == trans::accNone)
        return;

    auto transactionAmount = requestAmountForTransaction(accType, trans::withdrawal);

    if (transactionAmount < 1)  // Validate transaction amount
        return;

    QStringList lastTransactionDetails = getLastTransactionDetailsForAccountType(accType);

    double currentBalance{ };
    if(lastTransactionDetails.length() == trans::accountDetailColumnCount) {
        currentBalance = QString(lastTransactionDetails[trans::index_newbalance]).toDouble();
    }

    // Check if the withdrawal amount exceeds the current balance
    if (transactionAmount > currentBalance) {
        QMessageBox::warning(this, "Insufficient Funds",
                             "The withdrawal amount exceeds your current balance. Transaction aborted.");
        return;
    }

    // Prepare the transaction log
    QString transactionLog = QString("%1,%2,%3,%4,%5,%6,%7")
                                 .arg(m_customer->m_cardnumber)
                                 .arg(accType == trans::chequing ? "Chequing" : "Savings")
                                 .arg(lastTransactionDetails[trans::index_newbalance])
                                 .arg(transactionAmount)
                                 .arg(QString::number((currentBalance - transactionAmount), 'f', 2))
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"))
                                 .arg("Withdrawal");

    // Append transaction to the log file
    appendRowToFile(transactionLog);

    // Log the transaction details
    qDebug() << Q_FUNC_INFO << "Transaction info:\n"
             << transactionLog;
}

QStringList MainWindow::getLastTransactionDetailsForAccountType(trans::EN_AccType accType) {
    if (!m_customer || !m_customer->isValid())
        return QStringList(); // Return empty list if customer is invalid.

    QFile file(USER_ACC_DETAILS_FILE_ADDRESS);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << USER_ACC_DETAILS_FILE_ADDRESS;
        return QStringList();
    }

    QString accountType = (accType == trans::savings) ? "Savings" : "Chequing";

    QTextStream in(&file);
    QStringList result;


    while (!in.atEnd()) {
        QStringList rowCurr = in.readLine().split(",");
        if (rowCurr.length() > trans::index_accounttype &&
            rowCurr[trans::index_accounttype] == accountType)
        {
            result = rowCurr; // Keep updating to ensure we get the last matching row.
        }
    }

    file.close();
    return result;
}

bool MainWindow::appendRowToFile(QString detailsOfTransaction) {
    if (!m_customer || !m_customer->isValid()) {
        qWarning() << "Invalid customer object.";
        return false;
    }

    if (detailsOfTransaction.isEmpty()) {
        qWarning() << "Empty transaction details provided.";
        return false;
    }

    QFile file(USER_ACC_DETAILS_FILE_ADDRESS);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open file for appending:" << USER_ACC_DETAILS_FILE_ADDRESS;
        return false;
    }

    QTextStream out(&file);
    out << detailsOfTransaction << "\n";
    file.close();

    return true;
}

void MainWindow::onButtonShowHistoryPressed() {
    // qDebug() << Q_FUNC_INFO << " ";
    if(!transactionListWindow) {
        transactionListWindow = new BookList_Admin(ui, this);
        transactionListWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    }
    this->close();
    transactionListWindow->show();
}

void MainWindow::onButtonLogoutPressed() {
    qDebug() << Q_FUNC_INFO << " ";
    int ret = QMessageBox::question(nullptr, "Confirm Logout", "Are you sure you want to logout?");
    if (ret == QMessageBox::Yes)
    {
        this->close();
        aui->show();
    }
}

void trans::Customer::printDetails() const {
    qDebug() << "Customer Details:";
    qDebug() << "Card Number: " << m_cardnumber;
    qDebug() << "PIN: " << m_pin;
    qDebug() << "First Name: " << m_firstname;
    qDebug() << "Last Name: " << m_lastname;
    qDebug() << "Phone Number: " << m_phonenumber;
}
