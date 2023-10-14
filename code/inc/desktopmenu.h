#ifndef DESKTOPMENU_H
#define DESKTOPMENU_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QPixmap>
#include <QLabel>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include <QEnterEvent>
#include <QMovie>
#include <QScreen>
#include <QApplication>
#include "copypanel.h"
class DesktopMenu : public QMainWindow
{
    Q_OBJECT
public:
    explicit DesktopMenu(QWidget *parent = nullptr);
    void OpenPanel();
protected:
    void setPixmapIcon(const QPixmap &pixmap, const QString &path);

    // 鼠标相关变量
    bool isMousePressed;
    QPoint mousePressPos;
    QPoint mousePressPos_global;
    bool isMouseEntered{false};

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    // 自定义右键菜单
    void showContextMenu(const QPoint &pos);


    // Panel相关
    bool isPanelOpen{false};
    void PanelPosition();


private:
    QSystemTrayIcon *trayIcon;
    QLabel *label{nullptr};
    QMovie *movie{nullptr};
    CopyPanel *panel{nullptr};
    void createSystemTrayIcon();

    void showSettings();
    void closeProgram();
signals:

};

#endif // DESKTOPMENU_H
