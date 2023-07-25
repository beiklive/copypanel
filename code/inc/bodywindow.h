#ifndef BODYWINDOW_H
#define BODYWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <QHeaderView>
#include <QCheckBox>
#include <QFile>
QT_BEGIN_NAMESPACE
namespace Ui { class bodyWindow; }
QT_END_NAMESPACE

class bodyWindow : public QMainWindow
{
    Q_OBJECT

public:
    bodyWindow(QWidget *parent = nullptr);
    ~bodyWindow();


private:
    void setupUI();
    void resizeEvent(QResizeEvent *event);

    void saveDataToFile(const QString &fileName);
    void loadDataFromFile(const QString &fileName);

private slots:
    void onAddButtonClicked();
    void onTableViewClicked(const QModelIndex &index);
    void onTopCheckBoxToggled(bool checked);
    void saveToFile();
    void loadFromFile();

private:
    Ui::bodyWindow *ui;

    QTableView *tableView;
    QStandardItemModel *model;
    QPushButton *addButton;
    QCheckBox *topCheckBox;
};
#endif // BODYWINDOW_H
