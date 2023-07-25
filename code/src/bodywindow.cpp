#include "bodywindow.h"
#include "ui_bodywindow.h"
#include <QTextStream>
#include <QStandardPaths>
bodyWindow::bodyWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::bodyWindow)
{
    ui->setupUi(this);

    QIcon icon(":/qss/top.ico"); // Replace with the actual icon file path if not using Qt resources
    setWindowIcon(icon);
    setupUI();
    loadFromFile();
}

bodyWindow::~bodyWindow()
{
    delete ui;
    delete tableView;
    delete model;
    delete addButton;
    delete topCheckBox;
}

void bodyWindow::setupUI()
{
    tableView = ui->tableView;
    addButton = ui->Add_btn;
    topCheckBox = ui->topcheckbox;

    this->setWindowTitle("PastePanel");

    model = new QStandardItemModel(this);
    model->setColumnCount(1);

    tableView->setModel(model);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 设置表头的resize mode为Stretch
    tableView->setColumnWidth(1, width());
    // 设置表头视图的ResizeMode为ResizeToContents
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setVisible(false);
    tableView->horizontalHeader()->setVisible(false);
    tableView->setShowGrid(false);
    connect(tableView, &QTableView::clicked, this, &bodyWindow::onTableViewClicked);
    connect(addButton, &QPushButton::clicked, this, &bodyWindow::onAddButtonClicked);
    connect(topCheckBox, &QCheckBox::toggled, this, &bodyWindow::onTopCheckBoxToggled);

}


void bodyWindow::resizeEvent(QResizeEvent *event)
{
//    bodyWindow::resizeEvent(event);
    // 更新表头视图的宽度为窗口的宽度
    tableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    tableView->setColumnWidth(1, width());
}

void bodyWindow::onAddButtonClicked()
{
    int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, 0), QApplication::clipboard()->text());
    saveToFile();
}

void bodyWindow::onTableViewClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString rowData = model->data(index, Qt::DisplayRole).toString();
        QApplication::clipboard()->setText(rowData);
    }
}

void bodyWindow::onTopCheckBoxToggled(bool checked)
{
    if (checked) {
        // 将窗口设置为置顶
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    } else {
        // 取消窗口的置顶属性
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    }

    // 重新设置窗口属性，使其生效
    show();
}


void bodyWindow::saveToFile()
{
    saveDataToFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "data.txt");
}

void bodyWindow::loadFromFile()
{
    loadDataFromFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "data.txt");
}

void bodyWindow::saveDataToFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int row = 0; row < model->rowCount(); ++row) {
            QString data = model->data(model->index(row, 0)).toString();
            out << data << Qt::endl;
        }
        file.close();
    }
}

void bodyWindow::loadDataFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QStringList dataList;
        while (!in.atEnd()) {
            dataList << in.readLine();
        }
        file.close();

        int rowsToLoad = qMin(dataList.size(), 20);
        int startRow = dataList.size() - rowsToLoad;

        for (int i = startRow; i < dataList.size(); ++i) {
            QString data = dataList.at(i);
            int row = model->rowCount();
            model->insertRow(row);
            model->setData(model->index(row, 0), data);

        }
    }
}
