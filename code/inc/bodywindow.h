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
#include <QTextStream>
#include <QStandardPaths>
#include <windows.h>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <QMimeData>
#include <QDebug>
#include <QDir>
#include "qtinformationstorage.h"

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
    void setWindowTopmost(QWidget* widget, bool isTopmost);
    bool saveImageToClipboard(const QString& imagePath);
    void loadDataFromFile();

private slots:
    void onAddButtonClicked();
    void onDelButtonClicked();
    void onTableViewClicked(const QModelIndex &index);
    void onTopCheckBoxToggled(bool checked);
//    void saveToFile();
    void loadFromFile();

private:
    Ui::bodyWindow *ui;

    QTableView *tableView;
    QStandardItemModel *model;
    QPushButton *addButton;
    QPushButton *delButton;
    QCheckBox *topCheckBox;

    qint16 loadItemMax{50};
    QString cacheFileName = "/copypinner/copypinner.cache";
    QTInformationStorage storage;

    int tableRowGlobal{-1};
};
#endif // BODYWINDOW_H
