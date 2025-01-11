#ifndef BOOKLIST_ADMIN_H
#define BOOKLIST_ADMIN_H

#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QWidget>
#include <QCompleter>
#include <QMap>
#include <QVector>
#include <QTableWidgetItem>

#define USER_ACC_DETAILS_FILE_ADDRESS QString("resource/Transactions.txt")

namespace Ui {
class BookList_Admin;
}

namespace trans {
enum TransactionFilterType {
    SHOW_ALL = 0,
    SHOW_CHEQUING,
    SHOW_SAVINGS
};
}

class BookList_Admin : public QWidget
{
    Q_OBJECT

public:
    explicit BookList_Admin(Ui::MainWindow *ui_dash, QWidget *dash, QWidget *parent = nullptr);
    int loadData();
    ~BookList_Admin();


protected:
    void showEvent(QShowEvent *event) override final; // Proper declaration


private slots:
    void on_pushButton_backtodash_clicked();
    void on_pushButton_refresh_clicked();

private:
    Ui::BookList_Admin *ui;
    QWidget *admin_dash;
    Ui::MainWindow *ui_admindash;
    QVector<QCompleter *> completer;
    trans::TransactionFilterType shownListType = trans::SHOW_ALL;

    void setupTableHeaders();
    int populateTable(const QVector<QStringList> &data);
    // void setupCompleters(const QVector<QStringList> &data)
    // {
    //     // Clear existing completers
    //     for (QCompleter *comp : qAsConst(completer))
    //         delete comp;
    //     completer.clear();

    //     // Prepare column data for completers
    //     int columnCount = ui->tableWidget->columnCount();
    //     QVector<QStringList> columnData(columnCount);

    //     for (const QStringList &row : data)
    //     {
    //         for (int col = 0; col < row.size() && col < columnCount; ++col)
    //         {
    //             columnData[col].append(row.at(col));
    //         }
    //     }

    //     // Create and set completers
    //     for (const QStringList &colData : columnData)
    //     {
    //         QCompleter *comp = new QCompleter(colData, this);
    //         comp->setCaseSensitivity(Qt::CaseInsensitive);
    //         completer.append(comp);
    //     }
    // }
};

#endif // BOOKLIST_ADMIN_H
