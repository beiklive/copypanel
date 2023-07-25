#include "bodywindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon icon(":/qss/top.ico"); // Replace with the actual icon file path if not using Qt resources
    a.setWindowIcon(icon);
    // Load the QSS file
    QFile styleFile(":/qss/zgf.qss"); // Replace with the actual file path if not using Qt resources
    styleFile.open(QFile::ReadOnly);

    // Apply the QSS style to the application
    QString styleSheet = styleFile.readAll();
    a.setStyleSheet(styleSheet);



    bodyWindow w;
    w.show();
    return a.exec();
}
