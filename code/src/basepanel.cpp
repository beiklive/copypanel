#include "basepanel.h"

BasePanel::BasePanel(QWidget *parent)
    : QWidget{parent}
{
    // 去除标题栏和背景
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    setFixedSize(tilePixel * PanelWidth, tilePixel * PanelHeight);
    // 读取材质
    tiles = loadTileset(":/qss/ui/texture.png");
    // 初始化 item 皮肤路径
    pixmapSkinInit();
    // 检查字体是否可用并加载
    fontId = QFontDatabase::addApplicationFont(":/qss/ui/ark-pixel-12px.ttf");
    if (fontId != -1) {
        fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    }

    // 初始化 item
    PixmapItemSet();

    //测试用例    createCopyItem();
    ReadClipboardStorage();
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

// 每次重绘触发
void BasePanel::drawPixmapPanel()
{
    if(isMousePressed || initdown)
    {
        QPixmap pixmap(size());
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        // 绘制背景
        drawBackground(painter, QPoint(0,0), size());

        // 绘制所有Item
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

        // 窗口大小调整是否启用
        isDrag = (event->pos().y() >=  dragY && event->pos().x() >= dragX);

        // 是否在 Item 内部点击
        if(isMouseMoveIn)
        {
            // 轮询找到被按下的item , 并调用 item 的 click 函数
            itemClickEvent();
        }


        itemTracking(event);
    }
}

void BasePanel::mouseMoveEvent(QMouseEvent *event)
{
    // 如果鼠标处于拖拽移动
    if (isMousePressed) {
        //允许拖动改变窗口大小
        if(isDrag)
        {
            // 根据鼠标移动改变窗口大小   会触发重绘事件
            DragEvent(event);
            // item状态更新
            itemsUpdate();
        }
    }

    // 如果鼠标在可拖拽区域
    if(event->pos().y() >=  dragY && event->pos().x() >= dragX)
    {
        setCursor(Qt::SizeFDiagCursor);
    }else
    {
    // 不在可拖拽区域
        // 如果不在 item 内部
        if(!isMouseMoveIn)
        {
            // 恢复成普通的鼠标状态
            setCursor(Qt::CustomCursor);
        }
        // else 在 item 内部时， 由 item 自己处理
    }
    // 保持 item 监听
    itemTracking(event);
}

void BasePanel::mouseReleaseEvent(QMouseEvent *event)
{
    // 鼠标左键松开
    if (event->button() == Qt::LeftButton) {
        // 如果松开时 在item内部
        if (isMousePressed) {
            isMousePressed = false;
            if(CurPixmapItem != nullptr)
            {
                // 执行当前 item 的点击事件
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

void BasePanel::TriggerMouseMoveEvent()
{

//    QMouseEvent *event = new QMouseEvent(QEvent::MouseMove,
//                                         QPointF(mousePressPos.x(), mousePressPos.y()),
//                                         Qt::LeftButton,
//                                         Qt::LeftButton,
//                                         Qt::NoModifier);

//    QApplication::postEvent(this, event);
}

// 窗口大小调整事件
void BasePanel::DragEvent(QMouseEvent *event)
{

    auto tmppos = event->pos() - mousePressPos;
    int newHeight = height() + tmppos.y();
    int newWidth = width() + tmppos.x();
    // 最小值限定
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

// 界面功能按键 初始化
void BasePanel::PixmapItemSet()
{
    // item生成按键
    PixmapItem_t *addBtn = createPixmapItemByUrl("ADD_BTN",QPoint(1*tilePixel, 1*tilePixel), btnSkinDict["ADD"][0], btnSkinDict["ADD"][1], btnSkinDict["ADD"][2]);

    // item 的事件函数绑定
    ITEM_EVENT_MOVEIN(addBtn)
    {
        setPixmapSkin(item, HoverSkin);
    }
    ITEM_EVENT_END(addBtn)
    ITEM_EVENT_MOVEOUT(addBtn)
    {
        setPixmapSkin(item, NormalSkin);
    }
    ITEM_EVENT_END(addBtn)
    ITEM_EVENT_CLICK(addBtn)
    {
        setPixmapSkin(item, isMousePressed ? ClickSkin : HoverSkin);
        if(isMousePressed){

        }else{
            // 鼠标抬起
            const QString boardtext = getTextFromClipboard();
            createClipboardItem(boardtext);
            if(boardtext != "")
            {
                fileStorage.add("text", boardtext);
            }
        }
    }
    ITEM_EVENT_END(addBtn)
    ITEM_EVENT_UPDATE(addBtn)
    {
        item->pos.setX(1*tilePixel);
        item->pos.setY(1*tilePixel);
    }
    ITEM_EVENT_END(addBtn)
    // 追加 item 到监听列表
    addItemToTrack(addBtn);
    //====================================================================
    PixmapItem_t *CheckBox = createPixmapItemByUrl("CHECKBOX",QPoint(this->width() - tilePixel*3, 1*tilePixel), btnSkinDict["CHECKBOX"][0], btnSkinDict["CHECKBOX"][1],btnSkinDict["CHECKBOX"][2]);
    ITEM_EVENT_MOVEIN(CheckBox)
    {
        setPixmapSkin(item, HoverSkin);
        setCursor(Qt::PointingHandCursor);
    }
    ITEM_EVENT_END(CheckBox)
    ITEM_EVENT_MOVEOUT(CheckBox)
    {
        setPixmapSkin(item, NormalSkin);
    }
    ITEM_EVENT_END(CheckBox)
    ITEM_EVENT_CLICK(CheckBox)
    {
        setPixmapSkin(item, isMousePressed ? ClickSkin : HoverSkin);
        if(isMousePressed){

        }else{

        }
    }
    ITEM_EVENT_END(addBtn)
    ITEM_EVENT_UPDATE(CheckBox)
    {
        item->pos.setX(this->width() - tilePixel*3);
        item->pos.setY(1*tilePixel);
    }
    ITEM_EVENT_END(CheckBox)
    addItemToTrack(CheckBox);

    //====================================================================
    PixmapItem_t *CheckBoxLabel = createPixmapItemByText("CHECKBOXLABEL", QPoint(this->width() - tilePixel*(3 + 4.5), 1*tilePixel), "开机自启");
    ITEM_EVENT_MOVEIN(CheckBoxLabel)
    {

    }
    ITEM_EVENT_END(CheckBoxLabel)
    ITEM_EVENT_MOVEOUT(CheckBoxLabel)
    {

    }
    ITEM_EVENT_END(CheckBoxLabel)
    ITEM_EVENT_CLICK(CheckBoxLabel)
    {

    }
    ITEM_EVENT_END(CheckBoxLabel)
    ITEM_EVENT_UPDATE(CheckBoxLabel)
    {
        item->pos.setX(this->width() - tilePixel*(3 + 4.5));
        item->pos.setY(1*tilePixel);
    }
    ITEM_EVENT_END(CheckBoxLabel)
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

// 调用所有item的更新事件
void BasePanel::itemsUpdate()
{
    for (PixmapItem_t* tempItem : pixmapItems) {
        tempItem->Update(tempItem);
    }
}

// 每次鼠标事件（按下、 抬起、 移动） 时触发
// 原来找到鼠标是否在item内部
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

        // 判断鼠标是否在 item 内部
        if (point1.x() >= minX && point1.x() <= maxX && point1.y() >= minY && point1.y() <= maxY) {
        // 鼠标在 item 内部
            // 一次只允许进入一个 item， 防止出现两个 item 重合的 bug
            if(!isMouseMoveIn)
            {
                // 触发 item 的鼠标进入函数
                itemMoveInEvent(tempItem);
                // 状态位打开
                isMouseMoveIn = true;
                // 记录当前所在 item 的 id
                itemID = tempItem->id;
            }
            // 发现鼠标确实在 item 内部，可以直接 break 不用继续检查后面的 item
            break;
        } else {
        // 鼠标移出 item
            // 如果发现 鼠标是 movein 的状态， 并且 记录id 为当前的 item 的 id
            // 说明鼠标是执行的移出操作
            if(itemID == tempItem->id && isMouseMoveIn)
            {
                // 清空 记录id
                itemID = "NULL";
                // 触发 item 的鼠标移出函数
                itemMoveOutEvent(tempItem);
                // 状态位关闭
                isMouseMoveIn = false;

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
    CurPixmapItem = item;
    qDebug() << "CurItem : " << CurPixmapItem->id;
    item->MouseMoveIn(item);
}

void BasePanel::itemMoveOutEvent(PixmapItem_t *item)
{
    item->MouseMoveOut(item);
    CurPixmapItem = nullptr;
    qDebug() << "CurItem clear";
}

void BasePanel::itemClickEvent()
{
    if(CurPixmapItem != nullptr)
    {
        CurPixmapItem->MouseClick(CurPixmapItem);
        clearCompositeItemTrash();
    }
}

// 测试用例
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
    ITEM_EVENT_MOVEIN(addBtn)
    {
        setPixmapSkin(item, HoverSkin);
    }
    ITEM_EVENT_END(addBtn)
    ITEM_EVENT_MOVEOUT(addBtn)
    {
        setPixmapSkin(item, NormalSkin);
    }
    ITEM_EVENT_END(addBtn)
    ITEM_EVENT_CLICK(addBtn)
    {
        setPixmapSkin(item, isMousePressed ? ClickSkin : HoverSkin);
        if(isMousePressed){

        }else{
            removeClipboardItem(item);
        }
    }
    ITEM_EVENT_END(addBtn)
    ITEM_EVENT_UPDATE(addBtn)
    {
        item->pos.setX(getCopyItemById(item->parentId)->pos.x() + 0*tilePixel);
        item->pos.setY(getCopyItemById(item->parentId)->pos.y() + 0*tilePixel);
    }
    ITEM_EVENT_END(addBtn)
    tempItem->btn = addBtn;
    //========== text ===========
    PixmapItem_t *BoxLabel = createPixmapItemByText(tempItem->id + "BOXLABEL", QPoint(tempItem->pos.x() + tilePixel * 2, tempItem->pos.y() + 0 * tilePixel), text);
    BoxLabel->parentId = tempItem->id;
    BoxLabel->text = text;
    ITEM_EVENT_MOVEIN(BoxLabel)
    {
        reDrawTextPixmap(item, Qt::gray, item->text);
        setPixmapSkin(getCopyItemById(item->parentId)->btn, TempSkin);
    }
    ITEM_EVENT_END(BoxLabel)
    ITEM_EVENT_MOVEOUT(BoxLabel)
    {
        reDrawTextPixmap(item, Qt::transparent, item->text);
        setPixmapSkin(getCopyItemById(item->parentId)->btn, NormalSkin);
    }
    ITEM_EVENT_END(BoxLabel)
    ITEM_EVENT_CLICK(BoxLabel)
    {
        if(isMousePressed){

        }else{
            QApplication::clipboard()->setText(item->text);
        }
    }
    ITEM_EVENT_END(BoxLabel)
    ITEM_EVENT_UPDATE(BoxLabel)
    {
        item->pos.setX(getCopyItemById(item->parentId)->pos.x() + tilePixel * 2);
        item->pos.setY(getCopyItemById(item->parentId)->pos.y() + 0 * tilePixel);
    }
    ITEM_EVENT_END(BoxLabel)

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

    fileStorage.removeAt(index);

    if (index >= 0) {
        copyItemsTrash.append(copyItems[index]);
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

void BasePanel::clearCompositeItemTrash()
{
    for(int i=0; i < copyItemsTrash.length(); i++)
    {
        delete copyItemsTrash[i];
    }
    copyItemsTrash.clear();
}

void BasePanel::ReadClipboardStorage()
{
    int filesize = fileStorage.size();
    if(filesize > 0)
    {
        for(int i = 0; i < filesize; i++)
        {
            createClipboardItem(fileStorage.getInfo(i));
        }
    }

}

void BasePanel::createClipboardItem(const QString boardtext)
{
    int index = copyItems.length();
    if(boardtext != ""){
        CompositePixmapItem_t *copyitem = createCompositeItem("index"+QString::number(index), QPoint(itemStartX + (0.5)*tilePixel, itemStartY + (index)*(tilePixel + 0.25*tilePixel)), boardtext);
        addItemToTrack(copyitem->btn);
        addItemToTrack(copyitem->text);
        copyItems.append(copyitem);

    }
}

void BasePanel::removeClipboardItem(PixmapItem_t *item)
{
    deleteCopyItemByID(item->parentId);
    //CurPixmapItem = nullptr;
    qDebug() << "remove";
    TriggerMouseMoveEvent();
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
