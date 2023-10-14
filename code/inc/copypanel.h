#ifndef COPYPANEL_H
#define COPYPANEL_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPainter>
#include <QVector>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
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



class CopyPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CopyPanel(QWidget *parent = nullptr);
    ~CopyPanel();

    QVector<QPixmap*> loadTileset(const QString &imagePath);

protected:
    void setPixmapIcon();
    void drawPanelBackground(QPainter &painter);
    void paintEvent(QPaintEvent *event) override;


    bool isMousePressed{false};
    QPoint mousePressPos;
    QPoint mousePressPos_global;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void DragEvent(QMouseEvent *event);
    bool isOverEdge(QPoint& t_point);
    void DrawTailMapBorder(QPainter &painter, QPoint item_pos, QSize item_size);


private:
    QVector<QPixmap*> tiles{nullptr};
    QPixmap displayPixmap;

    const int PanelWidth{17};
    const int PanelHeight{30};
    const int tilePixel{16};

    int dragX{0};
    int dragY{0};
    bool isDrag{false};
    bool initdown{true};
signals:

};






#endif // COPYPANEL_H
