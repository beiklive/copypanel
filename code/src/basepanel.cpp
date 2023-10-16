#include "basepanel.h"

BasePanel::BasePanel(QWidget *parent)
    : QWidget{parent}
{
    // 去除标题栏和背景
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    setFixedSize(tilePixel * PanelWidth, tilePixel * PanelHeight);
    tiles = loadTileset(":/qss/ui/texture.png");
    pixmapSkinInit();
    // 检查字体是否可用
    fontId = QFontDatabase::addApplicationFont(":/qss/ui/ark-pixel-12px.ttf");
    if (fontId != -1) {
        fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    }
    QObject::connect(this, &BasePanel::itemDeleteSignal, this, &BasePanel::itemDeleteSlot);

    PixmapItemSet();
//    createCopyItem();

}

BasePanel::~BasePanel()
{
    for (QPixmap *tile : tiles) {
        delete tile;
    }
    tiles.clear();
}


QVector<QPixmap*> BasePanel::loadTileset(const QString &imagePath)
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

bool BasePanel::isChineseOrEnglishCharacter(const QChar &character) {
    ushort unicode = character.unicode();

    if ((unicode >= 0x0020 && unicode <= 0x007E)) {
        return false; // 是英文字符或标点符号
    } else {
        return true; // 不是英文字符或标点符号
    }
}

void BasePanel::drawPixmapPanel()
{
    if(isMousePressed || initdown)
    {
        QPixmap pixmap(size());
        pixmap.fill(Qt::transparent);
        // 使用 QPainter 将九张瓦片绘制到 QPixmap 上
        QPainter painter(&pixmap);
        // 绘制背景
        drawBackground(painter, QPoint(0,0), size());
        // 绘制内部Item
        for (PixmapItem_t* tempItem : pixmapItems) {
            tempItem->Update(tempItem);
            setPixmapItem(painter, tempItem);
        }

        // 绘制边框
        drawPanelBorder(painter);

        painter.end();
        // 将 QPixmap 绘制到 QWidget 上
        QPainter widgetPainter(this);
        widgetPainter.drawPixmap(0, 0, pixmap);
    }

}

void BasePanel::drawPanelBorder(QPainter &painter)
{
    DrawTailMapBorder(painter, QPoint(0,0), size());
    dragY = height() - tilePixel;
    dragX = width() - tilePixel;
}

void BasePanel::drawBackground(QPainter &painter, QPoint item_pos, QSize item_size)
{
    for (int x = 1; ((((x+1) * tilePixel) + item_pos.x()) < width()) && ((((x+1) * tilePixel) + item_pos.x()) < item_size.width()); x++)
    {
        for (int y = 1; ((((y+1) * tilePixel) + item_pos.y()) < height()) && ((((y+1) * tilePixel) + item_pos.y()) < item_size.height()); y++) {
            painter.drawPixmap(item_pos.x() +  x * tilePixel, item_pos.y() + y * tilePixel, *tiles[TileMiddleCenter]);
        }
    }
}

void BasePanel::paintEvent(QPaintEvent *event)
{
    // 背景绘制
    drawPixmapPanel();

}

void BasePanel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isMousePressed = true;
        mousePressPos = event->pos();
        mousePressPos_global = event->globalPos();
        if(event->pos().y() >=  dragY && event->pos().x() >= dragX)
        {
            isDrag = true;
        }
        if(isMouseMoveIn)
        {
            itemClickEvent();
        }
        itemTracking(event);
    }
}

void BasePanel::mouseMoveEvent(QMouseEvent *event)
{
    if (isMousePressed) {
        if(isDrag)
        {
            DragEvent(event);
            // item状态更新
            itemsUpdate();
        }
    }
    if(event->pos().y() >=  dragY && event->pos().x() >= dragX)
    {
        setCursor(Qt::SizeFDiagCursor);
    }else
    {
        if(!isMouseMoveIn)
        {
            setCursor(Qt::CustomCursor);
        }
    }
    itemTracking(event);
}

void BasePanel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isMousePressed) {
            update();
            isMousePressed = false;
            if(isMouseMoveIn)
            {
                itemClickEvent();
            }
        }
        itemTracking(event);
    }
}

void BasePanel::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y()>0)
    {
        qDebug()<<"向前滚";
        // 向上逻辑
    }
    else
    {
        qDebug()<<"向后滚";
        // 向下逻辑
    }
}

void BasePanel::DragEvent(QMouseEvent *event)
{

    auto tmppos = event->pos() - mousePressPos;
    int newHeight = height() + tmppos.y();
    int newWidth = width() + tmppos.x();
    setFixedHeight(newHeight > 80? newHeight : 80);
    setFixedWidth(newWidth > 160? newWidth : 160);
    mousePressPos = event->pos();
}

bool BasePanel::isOverEdge(QPoint &t_point)
{
    return (t_point.x() > width() || t_point.y() > height());
}

void BasePanel::DrawTailMapBorder(QPainter &painter, QPoint item_pos, QSize item_size)
{

    // 上下边框
    for (int x = 1; ((((x+1) * tilePixel) + item_pos.x()) < width()) && ((((x+1) * tilePixel) + item_pos.x()) < item_size.width()); x++) {
        painter.drawPixmap(item_pos.x() +  x * tilePixel, item_pos.y(), *tiles[TileTopCenter]);
        painter.drawPixmap(item_pos.x() +  x * tilePixel, item_pos.y() + item_size.height() - tilePixel, *tiles[TileBottomCenter]);

        // 分割线
        painter.drawPixmap(item_pos.x() +  x * tilePixel, item_pos.y() + PanelSplitY, *tiles[TileTopCenter]);
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

void BasePanel::pixmapSkinInit()
{
    btnSkinDict["DEFAULT"] = {":/qss/ui/arrow.png", ":/qss/ui/arrow.png", ":/qss/ui/arrow.png"};
    btnSkinDict["ADD"] = {":/qss/ui/btn_add.png", ":/qss/ui/btn_add_hover.png", ":/qss/ui/btn_add_click.png"};
    btnSkinDict["DELETE"] = {":/qss/ui/btn_delete.png", ":/qss/ui/btn_delete_hover.png", ":/qss/ui/btn_delete_click.png"};
    btnSkinDict["CHECKBOX"] = {":/qss/ui/btn_checkbox_off.png", ":/qss/ui/btn_checkbox_on.png",":/qss/ui/btn_checkbox_off.png"};
}

PixmapItem_t *BasePanel::createPixmapItemByUrl(QString itemName, QPoint resPos, QString resUrl, QString reshoverUrl, QString resclickUrl, QString resTempUrl)
{
    PixmapItem_t *tempItem = new PixmapItem_t();
    tempItem->id = itemName;
    tempItem->resUrl = resUrl;
    tempItem->hoverResUrl = reshoverUrl;
    tempItem->clickResUrl = resclickUrl;
    tempItem->TempResUrl  = resTempUrl;
    tempItem->pos.setX(resPos.x());
    tempItem->pos.setY(resPos.y());
    setPixmapSkin(tempItem, NormalSkin);

    return tempItem;
}



PixmapItem_t *BasePanel::createPixmapItemByText(QString itemName, QPoint resPos, QString resText, int fontSize)
{
    PixmapItem_t *tempItem = new PixmapItem_t();
    tempItem->id = itemName;
    tempItem->pos.setX(resPos.x());
    tempItem->pos.setY(resPos.y());


    reDrawTextPixmap(tempItem, Qt::transparent, resText);




    return tempItem;
}

PixmapItem_t *BasePanel::createPixmapItemByPixmap(QString itemName, QPoint resPos)
{
    PixmapItem_t *tempItem = new PixmapItem_t();
    tempItem->id = itemName;
    tempItem->pos.setX(resPos.x());
    tempItem->pos.setY(resPos.y());

    if(itemName == "SPLITLINE")
    {
        QPixmap pixmap(width(), tilePixel);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        // 上下边框
        for (int x = 1; (((x+1) * tilePixel)) < width(); x++) {
            painter.drawPixmap(x * tilePixel, 0, *tiles[TileTopCenter]);
        }
        tempItem->res = pixmap;
    }

    return tempItem;
}

void BasePanel::reDrawTextPixmap(PixmapItem_t *item, const QColor &fillColor, const QString &resText)
{
    QPixmap pixmap(tilePixel * resText.length(), tilePixel);
    pixmap.fill(fillColor);
    QPainter painter(&pixmap);

    QFont font(fontName, 10, QFont::Thin);
    painter.setFont(font);
    painter.setPen(QColor(0, 0, 0));
    int posNum = 0;

    int prePixel{0};
    int curPixel{0};
    for(int i = 0; i < resText.length(); i++)
    {
        QChar character = resText[i];
        bool isChinese = isChineseOrEnglishCharacter(character);
        curPixel = (isChinese?zhPixel:enPixel);
        posNum += prePixel;
        prePixel = curPixel;
        painter.drawText(posNum, 0, curPixel, 16, Qt::AlignCenter, resText[i]);
    }
    item->res = pixmap;
}

void BasePanel::PixmapItemSet()
{

    PixmapItem_t *addBtn = createPixmapItemByUrl("ADD_BTN",QPoint(1*tilePixel, 1*tilePixel), btnSkinDict["ADD"][0], btnSkinDict["ADD"][1], btnSkinDict["ADD"][2]);
    addBtn->MouseMoveIn = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveIn " << item->id;
        setPixmapSkin(item, HoverSkin);
    };
    addBtn->MouseMoveOut = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveOut " << item->id;
        setPixmapSkin(item, NormalSkin);
    };
    addBtn->MouseClick = [&](PixmapItem_t *item) {
        qDebug() << "MouseClick " << item->id << " == " << isMousePressed;
        setPixmapSkin(item, isMousePressed ? ClickSkin : HoverSkin);
        if(isMousePressed){

        }else{
            // 鼠标抬起
            int index = copyItems.length();
            const QString boardtext = getTextFromClipboard();
            if(boardtext != ""){
                CompositePixmapItem_t *copyitem = createCompositeItem("index"+QString::number(index), QPoint(itemStartX + (0.5)*tilePixel, itemStartY + (index)*(tilePixel + 0.25*tilePixel)), boardtext);

                addItemToTrack(copyitem->btn);
                addItemToTrack(copyitem->text);

                copyItems.append(copyitem);
            }
        }


    };
    addBtn->Update = [&](PixmapItem_t *item){
        item->pos.setX(1*tilePixel);
        item->pos.setY(1*tilePixel);
    };

    addItemToTrack(addBtn);
    //====================================================================
    PixmapItem_t *CheckBox = createPixmapItemByUrl("CHECKBOX",QPoint(this->width() - tilePixel*3, 1*tilePixel), btnSkinDict["CHECKBOX"][0], btnSkinDict["CHECKBOX"][1],btnSkinDict["CHECKBOX"][2]);
    CheckBox->MouseMoveIn = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveIn " << item->id;
        setPixmapSkin(item, HoverSkin);
        setCursor(Qt::PointingHandCursor);
    };
    CheckBox->MouseMoveOut = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveOut " << item->id;
        setPixmapSkin(item, NormalSkin);
    };
    CheckBox->MouseClick = [&](PixmapItem_t *item) {
        qDebug() << "MouseClick " << item->id << " == " << isMousePressed;
        setPixmapSkin(item, isMousePressed ? ClickSkin : HoverSkin);

        if(isMousePressed){

        }else{

        }
    };
    CheckBox->Update = [&](PixmapItem_t *item){
        item->pos.setX(this->width() - tilePixel*3);
        item->pos.setY(1*tilePixel);
    };
    addItemToTrack(CheckBox);

    //====================================================================
    PixmapItem_t *CheckBoxLabel = createPixmapItemByText("CHECKBOXLABEL", QPoint(this->width() - tilePixel*(3 + 4.5), 1*tilePixel), "开机自启");
    CheckBoxLabel->MouseMoveIn = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveIn " << item->id;
    };
    CheckBoxLabel->MouseMoveOut = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveOut " << item->id;
    };
    CheckBoxLabel->Update = [&](PixmapItem_t *item){
        item->pos.setX(this->width() - tilePixel*(3 + 4.5));
        item->pos.setY(1*tilePixel);
    };
    CheckBoxLabel->MouseClick = [&](PixmapItem_t *item) {
        qDebug() << "MouseClick " << item->id;
        if(isMousePressed){

        }else{

        }
    };
    addItemToTrack(CheckBoxLabel);
    //=====================================================================

}

void BasePanel::setPixmapSkin(PixmapItem_t *item, int skinflag)
{
    QString skinUrl;
    switch (skinflag) {
    case NormalSkin:
        skinUrl = item->resUrl;
        break;
    case ClickSkin:
        skinUrl = item->clickResUrl;
        break;
    case HoverSkin:
        skinUrl = item->hoverResUrl;
        break;
    case TempSkin:
        skinUrl = item->TempResUrl;
        break;
    default:
        skinUrl = item->resUrl;
        break;
    }
    qDebug() << item->id << " : " << skinUrl;
    QPixmap icon(skinUrl);
    icon.setMask(icon.createHeuristicMask());  // 设置透明掩码
    item->res = icon;
    update();
}

void BasePanel::setPixmapItem(QPainter &painter, PixmapItem_t *item)
{
    painter.drawPixmap(item->pos.x(), item->pos.y(), item->res);
//    qDebug() << item->id << "  " << item->parentId << "  Pos : " << item->pos;
}

void BasePanel::itemsUpdate()
{
    for (PixmapItem_t* tempItem : pixmapItems) {
        tempItem->Update(tempItem);
    }
}

void BasePanel::itemTracking(QMouseEvent *event)
{
    for (PixmapItem_t* tempItem : pixmapItems) {
        auto point1 = event->pos();
        auto point2 = tempItem->pos;
        auto point3 = QPoint(tempItem->pos.x() + tempItem->res.width(), tempItem->pos.y() + tempItem->res.height());

        int minX = qMin(point2.x(), point3.x());
        int maxX = qMax(point2.x(), point3.x());
        int minY = qMin(point2.y(), point3.y());
        int maxY = qMax(point2.y(), point3.y());

        if (point1.x() >= minX && point1.x() <= maxX && point1.y() >= minY && point1.y() <= maxY) {
            if(!isMouseMoveIn)
            {
                // point1 在矩形内
                itemMoveInEvent(tempItem);
                isMouseMoveIn = true;
                itemID = tempItem->id;
            }
            break;
        } else {
            if(itemID == tempItem->id && isMouseMoveIn)
            {
                // point1 不在矩形内
                itemID = "NULL";
                isMouseMoveIn = false;
                itemMoveOutEvent(tempItem);
            }
        }
    }
}

void BasePanel::addItemToTrack(PixmapItem_t *item)
{
    // 遍历 items，检查是否具有相同 id 的项目
    bool itemExists = false;
    for (PixmapItem_t* tempItem : pixmapItems) {
        if (tempItem->id == item->id) {
            itemExists = true;
            break;
        }
    }
    // 如果 itemExists 为 false，将新项目添加到 items
    if (!itemExists) {
        pixmapItems.append(item);
    }
}

void BasePanel::itemMoveInEvent(PixmapItem_t *item)
{
    for (PixmapItem_t* tempItem : pixmapItems) {
        if (tempItem->id == item->id) {
            tempItem->MouseMoveIn(item);
            break;
        }
    }
}

void BasePanel::itemMoveOutEvent(PixmapItem_t *item)
{
    for (PixmapItem_t* tempItem : pixmapItems) {
        if (tempItem->id == item->id) {
            tempItem->MouseMoveOut(item);
            break;
        }
    }
}

void BasePanel::itemClickEvent()
{
    for (PixmapItem_t* tempItem : pixmapItems) {
        if (tempItem->id == itemID) {
            tempItem->MouseClick(tempItem);
            break;
        }
    }
}

void BasePanel::createCopyItem()
{
    int itemStartY = tilePixel + PanelSplitY;
    int itemStartX = tilePixel;
    for(int i=0; i < 15; i++)
    {
        CompositePixmapItem_t *copyitem = createCompositeItem("index"+QString::number(i), QPoint(itemStartX + (0.5)*tilePixel, itemStartY + (i)*(tilePixel + 0.25*tilePixel)), "测试文字测试文字测试文字测试文字测试文字测试文字测试文字");

        addItemToTrack(copyitem->btn);
        addItemToTrack(copyitem->text);

        copyItems.append(copyitem);
    }



}

CompositePixmapItem_t *BasePanel::createCompositeItem(QString itemName, QPoint resPos, QString text)
{
    CompositePixmapItem_t *tempItem = new CompositePixmapItem_t();
    tempItem->id = itemName;
    tempItem->pos.setX(resPos.x());
    tempItem->pos.setY(resPos.y());
    //========== btn ============
    PixmapItem_t *addBtn = createPixmapItemByUrl(tempItem->id +"DELETE_BTN",QPoint(tempItem->pos.x() + 0*tilePixel, tempItem->pos.y() + 0*tilePixel), btnSkinDict["DELETE"][0], btnSkinDict["DELETE"][1], btnSkinDict["DELETE"][2], btnSkinDict["DEFAULT"][0]);
    addBtn->parentId = tempItem->id;
    addBtn->MouseMoveIn = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveIn " << item->id;
        setPixmapSkin(item, HoverSkin);
    };
    addBtn->MouseMoveOut = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveOut " << item->id;
        setPixmapSkin(item, NormalSkin);
    };
    addBtn->MouseClick = [&](PixmapItem_t *item) {
        qDebug() << "MouseClick " << item->id << " == " << isMousePressed;
        setPixmapSkin(item, isMousePressed ? ClickSkin : HoverSkin);
        if(isMousePressed){

        }else{
            emit itemDeleteSignal(item->parentId);
        }
    };
    addBtn->Update = [&](PixmapItem_t *item){
        item->pos.setX(getCopyItemById(item->parentId)->pos.x() + 0*tilePixel);
        item->pos.setY(getCopyItemById(item->parentId)->pos.y() + 0*tilePixel);
    };
    tempItem->btn = addBtn;
    //========== text ===========
    PixmapItem_t *BoxLabel = createPixmapItemByText(tempItem->id + "BOXLABEL", QPoint(tempItem->pos.x() + tilePixel * 2, tempItem->pos.y() + 0 * tilePixel), text);
    BoxLabel->parentId = tempItem->id;
    BoxLabel->text = text;
    BoxLabel->MouseMoveIn = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveIn " << item->id;

        reDrawTextPixmap(item, Qt::gray, item->text);
        setPixmapSkin(getCopyItemById(item->parentId)->btn, TempSkin);
    };
    BoxLabel->MouseMoveOut = [&](PixmapItem_t *item) {
        qDebug() << "MouseMoveOut " << item->id;

        reDrawTextPixmap(item, Qt::transparent, item->text);
        setPixmapSkin(getCopyItemById(item->parentId)->btn, NormalSkin);
    };
    BoxLabel->Update = [&](PixmapItem_t *item){
        item->pos.setX(getCopyItemById(item->parentId)->pos.x() + tilePixel * 2);
        item->pos.setY(getCopyItemById(item->parentId)->pos.y() + 0 * tilePixel);
    };
    BoxLabel->MouseClick = [&](PixmapItem_t *item) {
        qDebug() << "MouseClick " << item->id;
        if(isMousePressed){

        }else{
            QApplication::clipboard()->setText(item->text);
        }
    };
    tempItem->text = BoxLabel;

    return tempItem;
}

CompositePixmapItem_t* BasePanel::getCopyItemById(QString id)
{
    for (CompositePixmapItem_t* tempItem : copyItems) {
        if (tempItem->id == id) {
            return tempItem;
        }
    }
    return nullptr;
}

void BasePanel::deleteCopyItemByID(QString id)
{
    CompositePixmapItem_t* tmpItem{nullptr};
    for (CompositePixmapItem_t* tempItem : copyItems) {
        if (tempItem->id == id) {
            qDebug() << "find : " << tempItem->id;
            tmpItem = tempItem;
            break;
        }
    }
    if(nullptr != tmpItem)
    {
        removePixmapItemById(tmpItem->btn->id);
        removePixmapItemById(tmpItem->text->id);
        removeCompositeItemById(tmpItem->id);
        isMouseMoveIn = false;
        copyitemIndex = 0;
        reArrangeCopyItems();
    }


}

void BasePanel::removeCompositeItemById(const QString &id) {
    int index = -1;
    for (int i = 0; i < copyItems.size(); ++i) {
        if (copyItems[i]->id == id) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        delete copyItems[index];
        copyItems.remove(index);
    }
}

void BasePanel::removePixmapItemById(const QString &id) {
    int index = -1;
    for (int i = 0; i < pixmapItems.size(); ++i) {
        if (pixmapItems[i]->id == id) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        delete pixmapItems[index];
        pixmapItems.remove(index);
    }
}

void BasePanel::reArrangeCopyItems()
{
    int ItemsLength = copyItems.length();

    for(int i = 0; i < ItemsLength; i++)
    {
        copyItems[i]->id = "index"+QString::number(i);
        copyItems[i]->pos.setX(itemStartX + (0.5)*tilePixel);
        copyItems[i]->pos.setY(itemStartY + (i)*(tilePixel + 0.25*tilePixel));
        copyItems[i]->btn->parentId = copyItems[i]->id;
        copyItems[i]->btn->id = copyItems[i]->id + "DELETE_BTN";
        copyItems[i]->text->parentId = copyItems[i]->id;
        copyItems[i]->text->id = copyItems[i]->id + "BOXLABEL";
    }


}

const QString BasePanel::getTextFromClipboard()
{
    const QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->mimeData()->hasImage()) {
//        // If clipboard has an image, set it to the cell
//        QImage image = clipboard->image();
//        // Save the image to a file in the desired directory
//        QString saveDir = QCoreApplication::applicationDirPath() + "/copypinner/img";
//        QDir().mkpath(saveDir);

//        QString savePath = saveDir + "/image_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png";
//        image.save(savePath);

//        QPixmap pixmap(savePath);
//        if (!pixmap.isNull()) {
//            int width = 100;
//            int height = 100;
//            QPixmap scaledPixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//            model->setData(model->index(row, 0), scaledPixmap, Qt::DecorationRole);
//        }
//        storage.add("image", savePath);
//        qDebug() << "image";
        return "";
    } else if (clipboard->mimeData()->hasText()) {
        // If clipboard has text, set it to the cell
        QString text = clipboard->text();
        return text;
    }else{
        return "";
    }

}

void BasePanel::itemDeleteSlot(const QString &text) {
    qDebug() << "itemDeleteSlot";
    deleteCopyItemByID(text);
}
