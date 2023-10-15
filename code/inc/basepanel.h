#ifndef BASEPANEL_H
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
    HoverSkin
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
    // MouseMoveIn Lambda函数
    std::function<void(PixmapItem*)> MouseMoveIn;
    // MouseMoveOut Lambda函数
    std::function<void(PixmapItem*)> MouseMoveOut;
    // MouseClick Lambda函数
    std::function<void(PixmapItem*)> MouseClick;
    // Update Lambda函数
    std::function<void(PixmapItem*)> Update;

} PixmapItem_t;

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

protected:
    void drawPixmapPanel();
    void drawPanelBorder(QPainter &painter);
    void drawBackground(QPainter &painter, QPoint item_pos, QSize item_size);
    void paintEvent(QPaintEvent *event) override;

    // 鼠标相关
    bool isMouseMoveIn = false;
    bool isMousePressed{false};
    QPoint mousePressPos;
    QPoint mousePressPos_global;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    // 边框拖拽功能
    void DragEvent(QMouseEvent *event);
    bool isOverEdge(QPoint& t_point);
    void DrawTailMapBorder(QPainter &painter, QPoint item_pos, QSize item_size);


    // pixitem相关
    PixmapItem_t* createPixmapItemByUrl(QString itemName, QPoint resPos, QString resUrl, QString reshoverUrl, QString resclickUrl);
    PixmapItem_t* createPixmapItemByText(QString itemName, QPoint resPos, QString resText);
    PixmapItem_t* createPixmapItemByPixmap(QString itemName, QPoint resPos);

    void PixmapItemSet();
    void setPixmapSkin(PixmapItem_t* item, int skinflag);
    void setPixmapItem(QPainter &painter, PixmapItem_t* item);

    // item 跟踪
    void itemsUpdate();
    void itemTracking(QMouseEvent *event);
    void addItemToTrack(PixmapItem_t* item);
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



    // 剪切板功能
    const QString getTextFromClipboard();

private:
    QString fontName{"黑体"};
    QString itemID{"NULL"};
    int fontId{-1};
    int copyitemIndex{0};

    QVector<QPixmap*> tiles{nullptr};
    QVector<PixmapItem_t*> pixmapItems;
    QVector<CompositePixmapItem_t*> copyItems;

    QVector<PixmapItem_t*> pixmapItemsTrash;
    QVector<CompositePixmapItem_t*> copyItemsTrash;

    const int PanelWidth{17};
    const int PanelHeight{30};
    const int tilePixel{16};
    const int PanelSplitY{16*3};
    int itemStartY = tilePixel + PanelSplitY;
    int itemStartX = tilePixel;

    int dragX{0};
    int dragY{0};
    bool isDrag{false};
    bool initdown{true};
signals:
    void itemDeleteSignal(const QString& text);
public slots:
    // 自定义槽函数，带有一个 QString 参数
    void itemDeleteSlot(const QString& text);
};






#endif // BASEPANEL_H
