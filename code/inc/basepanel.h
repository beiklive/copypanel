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

private:
    QString fontName{"黑体"};
    QString itemID{"NULL"};
    QVector<QPixmap*> tiles{nullptr};
    QVector<PixmapItem_t*> pixmapItems;

    const int PanelWidth{17};
    const int PanelHeight{30};
    const int tilePixel{16};
    const int PanelSplitY{16*3};

    int dragX{0};
    int dragY{0};
    bool isDrag{false};
    bool initdown{true};
signals:

};






#endif // BASEPANEL_H
