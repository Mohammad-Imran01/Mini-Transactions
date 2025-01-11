#include "booklist_admin.h"
#include "ui_booklist_admin.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

BookList_Admin::BookList_Admin(Ui::MainWindow *ui_dash, QWidget *dash, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BookList_Admin),
    admin_dash(dash),
    ui_admindash(ui_dash),
    shownListType(trans::SHOW_ALL)
{
    ui->setupUi(this);

    // Disable editing on table widget
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Load initial data
    this->loadData();

    // Connect filter combo box to reload data
    connect(ui->transactionListType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        shownListType = static_cast<trans::TransactionFilterType>(idx);
        loadData();
    });


}

int BookList_Admin::loadData()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QFile file(USER_ACC_DETAILS_FILE_ADDRESS);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->label->setText("Books List | Failed to Load Records");
        qWarning() << "Could not open file:" << USER_ACC_DETAILS_FILE_ADDRESS;
        return 0;
    }

    QTextStream in(&file);
    QVector<QStringList> data;

    bool isFirstRow = true;
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        if (isFirstRow)
        {
            isFirstRow = false; // Skip header row
            // qDebug() << "Skipped header row:" << line;
            continue;
        }

        QStringList fields = line.split(',');
        if (fields.size() < 2)
        {
            qWarning() << "Malformed line skipped:" << line;
            continue;
        }

        // Apply filter
        if (shownListType == trans::SHOW_ALL ||
            (shownListType == trans::SHOW_CHEQUING && fields.at(1) == "Chequing") ||
            (shownListType == trans::SHOW_SAVINGS && fields.at(1) == "Savings"))
        {
            data.append(fields);
        }
    }

    file.close();

    // Populate table and set up completers
    int recordsLoaded = populateTable(data);
    // setupCompleters(data);

    ui->label->setText("Books List | " + QString::number(recordsLoaded) + " Records Loaded");
    return recordsLoaded;
}

int BookList_Admin::populateTable(const QVector<QStringList> &data)
{
    int rowCount = 0;
    for (const QStringList &row : data)
    {
        ui->tableWidget->insertRow(rowCount);

        for (int col = 0; col < row.size(); ++col)
        {
            if (col < ui->tableWidget->columnCount())
                ui->tableWidget->setItem(rowCount, col, new QTableWidgetItem(row.at(col)));
        }

        ++rowCount;
    }

    return rowCount;
}



void BookList_Admin::on_pushButton_backtodash_clicked()
{
    this->hide();
    admin_dash->show();
}

void BookList_Admin::on_pushButton_refresh_clicked()
{
    this->loadData();
}

BookList_Admin::~BookList_Admin()
{
    // for (QCompleter *comp : qAsConst(completer))
    //     delete comp;
    delete ui;
}

void BookList_Admin::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event); // Call the base class implementation
    loadData();                // Refresh data whenever the widget is shown
}
