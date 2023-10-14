#include "desktopmenu.h"
#include "qcoreapplication.h"

DesktopMenu::DesktopMenu(QWidget *parent)
    : QMainWindow{parent}
{
    // 1. 去除标题栏和背景
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);

    // 设置窗口大小为图标大小
    QPixmap icon(":/qss/ui/logo.ico");
    setFixedSize(48, 48);

    // 显示图片
    icon.setMask(icon.createHeuristicMask());  // 设置透明掩码
    setPixmapIcon(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation), ":/qss/ui/logo.ico");

    this->installEventFilter(this); // 安装事件过滤器以捕获鼠标进入和离开事件
    // 2. 隐藏任务栏图标，显示托盘图标
    createSystemTrayIcon();
}

void DesktopMenu::OpenPanel()
{
    qDebug() << "OpenPanel";
    isPanelOpen = !isPanelOpen;
    if(isPanelOpen)
    {
        try {
            if(panel == nullptr)
            {
                panel = new CopyPanel();
            }
            PanelPosition();
            panel->show();
        } catch (...) {
            closeProgram();
        }

    }else
    {
        panel->close();
    }

}

void DesktopMenu::setPixmapIcon(const QPixmap &pixmap, const QString &path)
{
    if(nullptr == label){
        label = new QLabel(this);
    }
    if(movie != nullptr)
    {
        delete movie;
        movie = nullptr;
    }

    if(":/qss/ui/logo.gif" == path)
    {
        movie = new QMovie(path);
        movie->setScaledSize(QSize(48, 48));
        label->setMovie(movie);
        movie->start();
    }else if(":/qss/ui/logo.ico" == path)
    {
        label->setPixmap(pixmap);
    }

    label->setAlignment(Qt::AlignCenter);
    label->setFixedSize(pixmap.size());
    label->setAttribute(Qt::WA_TransparentForMouseEvents);
    label->setCursor(Qt::PointingHandCursor);

    label->show();
}

void DesktopMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isMousePressed = true;
        mousePressPos = event->pos();
        mousePressPos_global = event->globalPos();
    }
    else if (event->button() == Qt::RightButton) {
        // 处理右键单击事件，可以在这里调用右键菜单
        showContextMenu(event->pos());
    }
}

void DesktopMenu::mouseMoveEvent(QMouseEvent *event)
{
    if (isMousePressed) {
        move(event->globalPos() - mousePressPos);
        PanelPosition();
    }
}

void DesktopMenu::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isMousePressed) {

            int distance = (event->globalPos() - mousePressPos_global).manhattanLength();
            if (distance <= 1) { // 设置一个小的移动阈值
                // 鼠标按下后没有移动，执行 noaction 函数
                OpenPanel();
            }
            isMousePressed = false;
        }
    }
}

bool DesktopMenu::eventFilter(QObject *obj, QEvent *event)
{

    if (obj == this && event->type() == QEvent::Enter && isMouseEntered == false) {
        QPixmap icon(":/qss/ui/logo.gif");
        // 显示图片
        icon.setMask(icon.createHeuristicMask());  // 设置透明掩码
        setPixmapIcon(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation), ":/qss/ui/logo.gif");
        isMouseEntered = true;
    } else if (obj == this && event->type() == QEvent::Leave  && isMouseEntered == true && isPanelOpen == false) {
        QPixmap icon(":/qss/ui/logo.ico");
        // 显示图片
        icon.setMask(icon.createHeuristicMask());  // 设置透明掩码
        setPixmapIcon(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation), ":/qss/ui/logo.ico");
        isMouseEntered = false;
    }
    return false;
}

void DesktopMenu::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    QAction *settingsAction = contextMenu.addAction("设置");
    QAction *quitAction = contextMenu.addAction("退出");

    connect(settingsAction, &QAction::triggered, this, &DesktopMenu::showSettings);
    connect(quitAction, &QAction::triggered, this, &DesktopMenu::closeProgram);
    contextMenu.exec(mapToGlobal(pos));
}

// 改变面板出现的位置
void DesktopMenu::PanelPosition()
{
    if(panel != nullptr && isPanelOpen == true)
    {
        panel->move(pos() + QPoint(this->width()+10, 0));
    }
}

void DesktopMenu::createSystemTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/qss/ui/logo.ico"));
    trayIcon->show();

    // 创建右键菜单
    QMenu *trayMenu = new QMenu(this);
    QAction *settingsAction = trayMenu->addAction("设置");
    QAction *quitAction = trayMenu->addAction("退出");

    connect(settingsAction, &QAction::triggered, this, &DesktopMenu::showSettings);
    connect(quitAction, &QAction::triggered, this, &DesktopMenu::closeProgram);

    trayIcon->setContextMenu(trayMenu);
}

void DesktopMenu::showSettings()
{
    // 实现打开设置的逻辑
}

void DesktopMenu::closeProgram()
{
    this->close();
    QCoreApplication::quit();

}
