#include "bodywindow.h"
#include "ui_bodywindow.h"

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
    tableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    tableView->setColumnWidth(1, 2 * width());
}

void bodyWindow::onAddButtonClicked()
{
    int row = model->rowCount();
    model->insertRow(row);

    const QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->mimeData()->hasImage()) {
        // If clipboard has an image, set it to the cell
        QImage image = clipboard->image();
        // Save the image to a file in the desired directory
        QString saveDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/copypinner/img";
        QDir().mkpath(saveDir);

        QString savePath = saveDir + "/image_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png";
        image.save(savePath);

        QPixmap pixmap(savePath);
        if (!pixmap.isNull()) {
            int width = 100;
            int height = 100;
            QPixmap scaledPixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            model->setData(model->index(row, 0), scaledPixmap, Qt::DecorationRole);
        }
        storage.add("image", savePath);
        qDebug() << "image";
    } else if (clipboard->mimeData()->hasText()) {
        // If clipboard has text, set it to the cell
        QString text = clipboard->text();
        model->setData(model->index(row, 0), text);
        storage.add("text", text);
        qDebug() << "text";
    }
}

void bodyWindow::onTableViewClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        if( "text" == storage.getType(index.row())){
            QApplication::clipboard()->setText(storage.getInfo(index.row()));
            qDebug() << "Text saved to clipboard successfully!";
        }
        else if( "image" == storage.getType(index.row()))
        {
            bool result = saveImageToClipboard(storage.getInfo(index.row()));
            if (result) {
                qDebug() << "Image saved to clipboard successfully!";
            } else {
                qDebug() << "Failed to save image to clipboard.";
            }
        }
    }
}

bool bodyWindow::saveImageToClipboard(const QString& imagePath)
{
    QImage image(imagePath);
    if (image.isNull()) {
        return false;
    }
    QPixmap pixmap = QPixmap::fromImage(image);
    if (pixmap.isNull()) {
        return false;
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setPixmap(pixmap);
    return true;
}

void bodyWindow::onTopCheckBoxToggled(bool checked)
{
    setWindowTopmost(this, checked);
}

void bodyWindow::setWindowTopmost(QWidget* widget, bool isTopmost)
{
    SetWindowPos((HWND)widget->winId(), isTopmost ? HWND_TOPMOST : HWND_NOTOPMOST,
                 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void bodyWindow::loadFromFile()
{
    loadDataFromFile();
}


void bodyWindow::loadDataFromFile()
{
    int dataSize = storage.size();
    for (int i = 0; i < dataSize; ++i) {
        QString info = storage.getInfo(i);
        QString type = storage.getType(i);
        int row = model->rowCount();
        if("image" == type)
        {
            QPixmap pixmap(info);
            if (!pixmap.isNull()) {
                int width = 100;
                int height = 100;
                QPixmap scaledPixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                model->insertRow(row);
                model->setData(model->index(row, 0), scaledPixmap, Qt::DecorationRole);
            }
        }
        else if("text" == type)
        {
            model->insertRow(row);
            model->setData(model->index(row, 0), info);
        }
    }
}
