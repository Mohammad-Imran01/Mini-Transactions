#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <QWidget>
#include <QString>
#include <QMessageBox>


#define USER_CREDENTIALS_FILE_ADDRESS QString("./resource/Clients.txt")

#define USER_CREDENTIALS_LINE_ITEM_COUNT 5
#define USER_CREDENTIALS_CARDNUMBER_INDEX 0
#define USER_CREDENTIALS_PIN_INDEX 1
#define USER_CREDENTIALS_FIRSTNAME_INDEX 2
#define USER_CREDENTIALS_LASTNAME_INDEX 3
#define USER_CREDENTIALS_PHONENUMBER_INDEX 4


namespace Ui {
class authentication;
}

namespace trans {
struct Customer;
}

class authentication : public QWidget
{
    Q_OBJECT

public:
    explicit authentication(QWidget *parent = nullptr);
    ~authentication();

private slots:
    bool on_pushButton_signin_clicked();


private:
    Ui::authentication *ui;
    trans::Customer* m_customer = nullptr;

    void fillCustomerDetails(const QStringList&);
};

#endif // AUTHENTICATION_H
