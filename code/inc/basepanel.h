﻿#ifndef BASEPANEL_H
#define BASEPANEL_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPainter>
#include <QVector>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <functional> // 包含 <functional> 头文件
#include <QFont>
#include <QFontDatabase>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QMap>
#include <QStringList>
#include <QTimer>
#include "qtinformationstorage.h"
enum TilesetIndex{
    TileTopLeft,
    TileTopCenter,
    TileTopRight,
    TileMiddleLeft,
    TileMiddleCenter,
    TileMiddleRight,
    TileBottomLeft,
    TileBottomCenter,
    TileBottomRight
};

enum PixmapItemSkin{
    NormalSkin,
    ClickSkin,
    HoverSkin,
    TempSkin
};

typedef struct PixmapItem {
    QPoint pos;
    QPixmap res;
    QString id;
    QString parentId{"NULL"};
    QString text;
    QString resUrl;
    QString hoverResUrl;
    QString clickResUrl;
    QString TempResUrl{"NULL"};
    bool isOverArea{false};
    // MouseMoveIn Lambda函数
    std::function<void(PixmapItem*)> MouseMoveIn;
    // MouseMoveOut Lambda函数
    std::function<void(PixmapItem*)> MouseMoveOut;
    // MouseClick Lambda函数
    std::function<void(PixmapItem*)> MouseClick;
    // Update Lambda函数
    std::function<void(PixmapItem*)> Update;

    std::function<void(PixmapItem*, QString targetUrl)> PixmapAnimation;

} PixmapItem_t;

#define ITEM_EVENT_END(pixmap_item) };
#define ITEM_EVENT_MOVEIN(pixmap_item) \
    pixmap_item->MouseMoveIn = [&](PixmapItem_t *item) { \
        qDebug() << "MouseMoveIn " << item->id; \


#define ITEM_EVENT_MOVEOUT(pixmap_item) \
    pixmap_item->MouseMoveOut = [&](PixmapItem_t *item) { \
            qDebug() << "MouseMoveOut " << item->id; \


#define ITEM_EVENT_CLICK(pixmap_item) \
    pixmap_item->MouseClick = [&](PixmapItem_t *item) { \
            qDebug() << "MouseClick " << item->id << " == " << isMousePressed; \


#define ITEM_EVENT_UPDATE(pixmap_item) \
    pixmap_item->Update = [&](PixmapItem_t *item){ \


#define ITEM_EVENT_ANIMATION(pixmap_item) \
    pixmap_item->PixmapAnimation = [&](PixmapItem_t *item, QString targetUrl){ \



typedef struct CompositePixmapItem {
    QPoint pos;
    QString id;
    PixmapItem_t *btn;
    PixmapItem_t *text;
}CompositePixmapItem_t;

class BasePanel : public QWidget
{
    Q_OBJECT
public:
    explicit BasePanel(QWidget *parent = nullptr);
    ~BasePanel();

    QVector<QPixmap*> loadTileset(const QString &imagePath);
    bool isChineseOrEnglishCharacter(const QChar& character);
protected:
    void drawPixmapPanel();
    void drawPanelBorder(QPainter &painter);
    void drawBackground(QPainter &painter, QPoint item_pos, QSize item_size);
    void paintEvent(QPaintEvent *event) override;

    // 鼠标相关
    bool isMouseMoveIn = false;
    bool isMousePressed{false};
    bool isMouseSrollon{false};
    QPoint mousePressPos;
    QPoint mousePressPos_global;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool isInSrollArea(QMouseEvent *event);
    bool isOverEdge(QPoint& t_point);
    void TriggerMouseMoveEvent();

    // 边框拖拽功能
    void DragEvent(QMouseEvent *event);

    void DrawTailMapBorder(QPainter &painter, QPoint item_pos, QSize item_size);

    // pixmap皮肤初始化
    void pixmapSkinInit();

    // pixitem相关
    PixmapItem_t* createPixmapItemByUrl(QString itemName, QPoint resPos, QString resUrl, QString reshoverUrl, QString resclickUrl, QString resTempUrl="NULL");
    PixmapItem_t* createPixmapItemByText(QString itemName, QPoint resPos, QString resText, int fontSize=16);
    PixmapItem_t* createPixmapItemByPixmap(QString itemName, QPoint resPos);
    PixmapItem_t* createScrollPixmapItem(QString itemName, QPoint resPos);
    void reDrawTextPixmap(PixmapItem_t *item, const QColor &fillColor, const QString& resText);

    void PixmapItemSet();
    void setPixmapSkin(PixmapItem_t* item, int skinflag);
    void setPixmapItem(QPainter &painter, PixmapItem_t* item);

    // item 跟踪
    void itemsUpdate();
    void itemTracking(QMouseEvent *event);
    void addItemToTrack(PixmapItem_t* item);
    void addItemToSrollArea(PixmapItem_t* item);
    void itemMoveInEvent(PixmapItem_t *item);
    void itemMoveOutEvent(PixmapItem_t *item);
    void itemClickEvent();

    // 复合pixItem
    void createCopyItem();
    CompositePixmapItem_t* createCompositeItem(QString itemName, QPoint resPos, QString text);
    CompositePixmapItem_t* getCopyItemById(QString id);
    void deleteCopyItemByID(QString id);
    // 函数用于查找并删除 CompositePixmapItem 元素
    void removeCompositeItemById(const QString& id);
    // 函数用于查找并删除 PixmapItem 元素
    void removePixmapItemById(const QString& id);
    void reArrangeCopyItems();
    void clearCompositeItemTrash();


    // 剪切板功能
    void ReadClipboardStorage();

    void createClipboardItem(const QString boardtext);
    void removeClipboardItem(PixmapItem_t *item);
    const QString getTextFromClipboard();
    QTInformationStorage fileStorage;

private:
    QMap<QString, QStringList> btnSkinDict;
    QTimer timer;
    // 定义过渡效果的参数
    int transitionSteps = 30;  // 过渡步数
    qreal transitionSpeed = 0.02; // 渐变速度

    // 创建一个 QTimer 用于触发渐变效果


    int currentStep = 0;
    qreal currentOpacity = 0.0;
    QString fontName{"黑体"};
    QString itemID{"NULL"};
    int fontId{-1};
    int copyitemIndex{0};

    QVector<QPixmap*> tiles{nullptr};
    QVector<PixmapItem_t*> pixmapItems;
    QVector<CompositePixmapItem_t*> copyItems;

    QVector<PixmapItem_t*> pixmapItemsTrash;
    QVector<CompositePixmapItem_t*> copyItemsTrash;
    PixmapItem_t *CurPixmapItem{nullptr};
    PixmapItem_t *SrollArea{nullptr};

    const int PanelWidth{17};
    const int PanelHeight{30};
    const int tilePixel{16};
    const int SrollPixel{10};
    const int PanelSplitY{16*3};
    const int zhPixel = 14;
    const int enPixel = 8;
    int itemStartY = tilePixel + PanelSplitY;
    int itemStartX = tilePixel;

    int dragX{0};
    int dragY{0};
    bool isDrag{false};
    bool initdown{true};
    int SrollPixelTotal{0};

    int ShowHeight{0};
    int TotalHeight{0};
signals:
    void itemDeleteSignal(const QString& text);
public slots:
    // 自定义槽函数，带有一个 QString 参数
    void itemDeleteSlot(const QString& text);
};






#endif // BASEPANEL_H
