#include "desktopmenu.h"

#include <QApplication>
#include <QFile>
#include <QPixmap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/qss/ui/logo.ico");
    pixmap.setMask(pixmap.createHeuristicMask());  // 设置透明掩码
    a.setWindowIcon(QIcon(pixmap));


    DesktopMenu w;
    w.show();
    return a.exec();
}
