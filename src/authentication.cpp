#include "authentication.h"
#include "ui_authentication.h"
#include "mainwindow.h"

#include <QDebug>
#include <QDir>

authentication::authentication(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::authentication)
{
    ui->setupUi(this);
}

authentication::~authentication()
{
    delete ui;
    ui = nullptr;

    delete m_customer;
    m_customer = nullptr;
}



bool authentication::on_pushButton_signin_clicked()
{
    bool isSuccesfull = false;

    QString username = ui->lineEdit_user->text();
    QString password = ui->lineEdit_pass->text();


    QFile file(USER_CREDENTIALS_FILE_ADDRESS);

    qDebug() << Q_FUNC_INFO << QDir::currentPath() + USER_CREDENTIALS_FILE_ADDRESS;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        QMessageBox::critical(nullptr, "Error", "Could not open user database file.");


    QTextStream lineReader(&file);

    while (!lineReader.atEnd()) {
        QString lineCurrent = lineReader.readLine();
        if (lineCurrent.isEmpty()) continue;

        QStringList parts = lineCurrent.split(',');
        if (parts.size() != USER_CREDENTIALS_LINE_ITEM_COUNT) continue;

        QString fileUsername = parts[USER_CREDENTIALS_CARDNUMBER_INDEX].trimmed();
        QString filePassword = parts[USER_CREDENTIALS_PIN_INDEX].trimmed();

        if (fileUsername == username && filePassword == password) {
            isSuccesfull = true; // Match found

            fillCustomerDetails(parts);
        }
    }
    file.close();

    if(isSuccesfull) {
        MainWindow * mainWindowATM = new MainWindow(this, username, false);
        this->close();
        mainWindowATM->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

        m_customer->printDetails();
        mainWindowATM->setCustomerDetails(m_customer);

        mainWindowATM->show();
    } else {
        qDebug() << "Unsuccessfull login attempt.";
    }
    return isSuccesfull;
}

void authentication::fillCustomerDetails(const QStringList &parts) {
    if (parts.size() < 5) {
        qWarning() << "Invalid data format. Expected at least 5 parts.";
        return;
    }

    m_customer = new trans::Customer();

    m_customer->m_cardnumber  = parts[USER_CREDENTIALS_CARDNUMBER_INDEX];
    m_customer->m_pin         = parts[USER_CREDENTIALS_PIN_INDEX];
    m_customer->m_firstname   = parts[USER_CREDENTIALS_FIRSTNAME_INDEX];
    m_customer->m_lastname    = parts[USER_CREDENTIALS_LASTNAME_INDEX];
    m_customer->m_phonenumber = parts[USER_CREDENTIALS_PHONENUMBER_INDEX];
}
