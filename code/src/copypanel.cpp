#include "copypanel.h"

CopyPanel::CopyPanel(QWidget *parent)
    : QWidget{parent}
{
    // 去除标题栏和背景
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    setFixedSize(tilePixel * PanelWidth, tilePixel * PanelHeight);
    tiles = loadTileset(":/qss/ui/texture.png");
}

CopyPanel::~CopyPanel()
{
    for (QPixmap *tile : tiles) {
        delete tile;
    }
    tiles.clear();
}


QVector<QPixmap*> CopyPanel::loadTileset(const QString &imagePath)
{
    QVector<QPixmap*> tileset;

    // 加载图片
    QPixmap image(imagePath);

    // 检查图片大小是否是 48x48，如果不是则返回空数组
    if (image.width() != 48 || image.height() != 48) {
        return tileset;
    }

    // 切分图片为 16x16 的瓦片并存储到数组中
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            QPixmap *tile = new QPixmap(image.copy(y * tilePixel, x * tilePixel, tilePixel, tilePixel));
            tileset.append(tile);
        }
    }
    return tileset;
}

void CopyPanel::setPixmapIcon()
{
    if(isMousePressed || initdown)
    {
        QPixmap pixmap(size());
        pixmap.fill(Qt::transparent);
        // 使用 QPainter 将九张瓦片绘制到 QPixmap 上
        QPainter painter(&pixmap);
        drawPanelBackground(painter);
        painter.end();
        // 设置显示的 QPixmap
        displayPixmap = pixmap;
        // 将 QPixmap 绘制到 QWidget 上
        QPainter widgetPainter(this);
        widgetPainter.drawPixmap(0, 0, pixmap);
        qDebug() << "Paint";
    }

}

void CopyPanel::drawPanelBackground(QPainter &painter)
{
    DrawTailMapBorder(painter, QPoint(0,0), size());
    dragY = height() - tilePixel;
    dragX = width() - tilePixel;
}

void CopyPanel::paintEvent(QPaintEvent *event)
{
    setPixmapIcon();
}

void CopyPanel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isMousePressed = true;
        mousePressPos = event->pos();
        mousePressPos_global = event->globalPos();
        if(event->pos().y() >=  dragY && event->pos().x() >= dragX)
        {
            isDrag = true;
        }
    }
}

void CopyPanel::mouseMoveEvent(QMouseEvent *event)
{
    if (isMousePressed) {

        if(isDrag)
        {
            DragEvent(event);
        }
    }
    if(event->pos().y() >=  dragY && event->pos().x() >= dragX)
    {
        setCursor(Qt::SizeFDiagCursor);
    }else
    {
        setCursor(Qt::CustomCursor);

    }

}

void CopyPanel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isMousePressed) {

            update();
            isMousePressed = false;
        }
    }
}

void CopyPanel::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y()>0)
    {
        qDebug()<<"向前滚";
        // 放大业务逻辑
    }
    else
    {
        qDebug()<<"向后滚";
        // 缩小业务逻辑

    }
}

void CopyPanel::DragEvent(QMouseEvent *event)
{

    auto tmppos = event->pos() - mousePressPos;
    int newHeight = height() + tmppos.y();
    int newWidth = width() + tmppos.x();
    setFixedHeight(newHeight > 48? newHeight : 48);
    setFixedWidth(newWidth > 48? newWidth : 48);
    mousePressPos = event->pos();
}

bool CopyPanel::isOverEdge(QPoint &t_point)
{
    return (t_point.x() > width() || t_point.y() > height());
}

void CopyPanel::DrawTailMapBorder(QPainter &painter, QPoint item_pos, QSize item_size)
{
    for (int x = 1; ((((x+1) * tilePixel) + item_pos.x()) < width()) && ((((x+1) * tilePixel) + item_pos.x()) < item_size.width()); x++)
    {
        for (int y = 1; ((((y+1) * tilePixel) + item_pos.y()) < height()) && ((((y+1) * tilePixel) + item_pos.y()) < item_size.height()); y++) {
            painter.drawPixmap(item_pos.x() +  x * tilePixel, item_pos.y() + y * tilePixel, *tiles[TileMiddleCenter]);
        }
    }

    // 上下边框
    for (int x = 1; ((((x+1) * tilePixel) + item_pos.x()) < width()) && ((((x+1) * tilePixel) + item_pos.x()) < item_size.width()); x++) {
        painter.drawPixmap(item_pos.x() +  x * tilePixel, item_pos.y(), *tiles[TileTopCenter]);
        painter.drawPixmap(item_pos.x() +  x * tilePixel, item_pos.y() + item_size.height() - tilePixel, *tiles[TileBottomCenter]);
    }
    // 左右边框
    for (int y = 1; ((((y+1) * tilePixel) + item_pos.y()) < height()) && ((((y+1) * tilePixel) + item_pos.y()) < item_size.height()); y++) {
        painter.drawPixmap(item_pos.x(), item_pos.y() + y * tilePixel, *tiles[TileMiddleLeft]);
        painter.drawPixmap(item_pos.x() + item_size.width() - tilePixel, item_pos.y() + y * tilePixel, *tiles[TileMiddleRight]);
    }


    // 左上角
    painter.drawPixmap(item_pos.x(), item_pos.y(), *tiles[TileTopLeft]);
    // 右上角
    painter.drawPixmap(item_pos.x() + item_size.width() - tilePixel, item_pos.y(), *tiles[TileTopRight]);
    // 左下角
    painter.drawPixmap(item_pos.x(), item_pos.y() + item_size.height() - tilePixel, *tiles[TileBottomLeft]);
    // 右下角
    painter.drawPixmap(item_pos.x() + item_size.width() - tilePixel, item_pos.y() + item_size.height() - tilePixel, *tiles[TileBottomRight]);

}
