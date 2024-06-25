#include "PlayWindow.h"
#include "ui_PlayWindow.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>
#include "GlobalState.h"
#include "MyObjectPool.h"

int PlayWindow::initSpeed=2;
int PlayWindow::composerLevel=1;
int PlayWindow::cutterLevel=1;
int PlayWindow::Do_received=0;
int PlayWindow::Re_received=0;
int PlayWindow::Mi_received=0;
int PlayWindow::Fa_received=0;
int PlayWindow::So_received=0;
int PlayWindow::La_received=0;
int PlayWindow::Si_received=0;
int PlayWindow::Empty_received=0;
int PlayWindow::speedSwitcherNum=5;
PlayWindow::ttype_ PlayWindow::toolType = PlayWindow::composer;
QHash<QPoint, QSharedPointer<Belt>> PlayWindow::belts_;
QHash<QPoint, Composer*> PlayWindow::composers_;
QHash<QPoint, Cutter*> PlayWindow::cutters_;
QHash<QPoint, Bin*> PlayWindow::bins_;
QHash<QPoint, SpeedSwitcher*> PlayWindow::speedSwitchers_;
MyObjectPool<Note> PlayWindow::notePool;


PlayWindow::PlayWindow(QWidget *parent,int chapnum,QString archiveFileName)
    : QGraphicsView(parent)
    , ui(new Ui::PlayWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/img/resource/Icon.png"));
    this->setFixedSize(PW_WIDTH,PW_HEIGHT);
    this->setWindowTitle("Let's Play!");
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mainScene = new QGraphicsScene(this);
    mainScene->setSceneRect(0,0,PW_WIDTH,PW_HEIGHT);
    this->setScene(mainScene);
    this->setGeometry(0,0,PW_WIDTH,PW_HEIGHT);

    bgGenerateNum = QRandomGenerator::global()->bounded(1, 4);
    chapterNum = chapnum; //这是非导入存档的生成方式，如果导入了存档，后面还会进行更改

    notePool = MyObjectPool<Note>(80);

    changeStyleSheet(ui->ShopBtn_);
    changeStyleSheet(ui->InfoBtn_);
    changeStyleSheet(ui->SaveBtn_);
    changeStyleSheet(ui->HelpBtn_);

    changeStyleSheet(ui->BeltTool_);
    changeStyleSheet(ui->ComposerTool_);
    changeStyleSheet(ui->CutterTool_);
    changeStyleSheet(ui->BinTool_);
    changeStyleSheet(ui->SpeedMachineTool_);

    // 测试通关界面：
    // cGWindow = new Congratulation(this);
    // cGWindow->exec();

    QFile *archiveFile = nullptr;
    if (archiveFileName == ""){
        initMap();
    } else{
        // archiveFile = new QFile(...name);
        initMap(archiveFile);// 打开对应存档文件
    }

    // 完成文字的显示，显示内容就是目前有的SpeedMachine数量
    ssnumTextItem = new QGraphicsTextItem();
    ssnumTextItem->setDefaultTextColor(QColor(255, 218, 185));
    ssnumTextItem->setFont(QFont("Eras Bold ITC", 20));
    mainScene->addItem(ssnumTextItem);
    ssnumTextItem->setPos(10, 620);
    QString ssnumText = "remain:" + QString::number(speedSwitcherNum);
    ssnumTextItem->setPlainText(ssnumText);

    updateTimer = new QTimer();
    connect(updateTimer,&QTimer::timeout, this,&PlayWindow::updateGame);
    updateTimer->start(100);
}

PlayWindow::~PlayWindow()
{
    delete ui;
}

// 放置轨道：
// 第一段轨道的放置，取默认方向即可；马上放置
// 中间轨道，即鼠标移动但是没有到松开时：
//      首先根据相对于前一次点移动的方向变化，改变前一个点位置的轨道的方向；
//      然后结合上一次的方向，放置当前位置的轨道（不再是默认值）
// 松手时，轨道应该已经在moveEvent里放置好了，也不用再做什么
void PlayWindow::mousePressEvent(QMouseEvent *event)
{
    pressedPos_ = mapFromViewToGameMap(event->pos());
    if (pressedPos_.x() < 0 || pressedPos_.y() < 0){
        return;
    }
    if (gameMap[pressedPos_.y()][pressedPos_.x()]->occupied){
        // 左键对组件进行旋转；右键则是删除
        if (event->button() == Qt::LeftButton){
            // 准备旋转
            toSpin = true;
        } else if(event->button() == Qt::RightButton){
            // 准备删除
            toDelete = true;
        }
    } else{
        // 左键新增方块或者开始铺设；右键无效
        if (event->button() == Qt::LeftButton){
            toPut = true;
            if (toolType == belt){
                // TODO :开始放置轨道，记录位置(以此来判断下一轨道放置的方向)
                QSharedPointer<Belt> new_belt = beltPool.acquire();
                new_belt->initBelt(1,mapFromModeltoReal(pressedPos_));
                belts_.insert(pressedPos_,new_belt);
                new_belt->setZValue(1);
                mainScene->addItem(new_belt.data());
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = true;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(1);
                lastPos_ = pressedPos_;
            }
        }
    }
}

void PlayWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint releasedPoint = mapFromViewToGameMap(event->pos());
    if (releasedPoint.x() < 0 || releasedPoint.y() < 0){
        toSpin = false;
        toDelete = false;
        toPut = false;
        return;
    }
    if (releasedPoint == pressedPos_){
        if (toSpin){
            // TODO :处理旋转
            switch (gameMap[pressedPos_.y()][pressedPos_.x()]->getBlockContent()) {
            case belt:{
                QSharedPointer<Belt> spinBelt = belts_.value(pressedPos_);
                int dir = spinBelt->getDir();
                if (dir >= 1 && dir <= 3){
                    spinBelt->changeDir_(dir%3+2);
                } else if(dir == 4){
                    spinBelt->changeDir_(1);
                } else if(dir%2 == 1){
                    spinBelt->changeDir_(dir+1);
                } else if(dir%2 == 0){
                    spinBelt->changeDir_(dir-1);
                }
                break;
            }
            case composer:{
                Composer *spinComposer = composers_.value(pressedPos_);
                spinComposer->changeDir_(spinComposer->getDir()%4+1);
                break;
            }
            case cutter:{
                Cutter *spinCutter = cutters_.value(pressedPos_);
                int new_dir = spinCutter->getDir()%4+1;
                switch (new_dir) {
                case 1:
                    if (gameMap[pressedPos_.y()+1][pressedPos_.x()]->occupied){
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        QMessageBox::warning(this,"放置警告","这个位置已经被占用！\n无法旋转过去");
                        return;
                    }
                    break;
                case 2:
                    if (gameMap[pressedPos_.y()][pressedPos_.x()-1]->occupied){
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        QMessageBox::warning(this,"放置警告","这个位置已经被占用！\n无法旋转过去");
                        return;
                    }
                    break;
                case 3:
                    if (gameMap[pressedPos_.y()-1][pressedPos_.x()]->occupied){
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        QMessageBox::warning(this,"放置警告","这个位置已经被占用！\n无法旋转过去");
                        return;
                    }
                    break;
                case 4:
                    if (gameMap[pressedPos_.y()][pressedPos_.x()+1]->occupied){
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        QMessageBox::warning(this,"放置警告","这个位置已经被占用！\n无法旋转过去");
                        return;
                    }
                    break;
                default:
                    break;
                }
                QPoint oldCtPos2Maped = mapFromViewToGameMap(spinCutter->getPos_2());
                gameMap[oldCtPos2Maped.y()][oldCtPos2Maped.x()]->occupied = false;
                gameMap[oldCtPos2Maped.y()][oldCtPos2Maped.x()]->setBlockContent(0);
                spinCutter->changeDir_(new_dir);
                QPoint newCtPos2Maped = mapFromViewToGameMap(spinCutter->getPos_2());
                gameMap[newCtPos2Maped.y()][newCtPos2Maped.x()]->occupied = true;
                gameMap[newCtPos2Maped.y()][newCtPos2Maped.x()]->setBlockContent(3);
                break;
            }
            case speedSwitcher:
            {
                SpeedSwitcher *spinSS = speedSwitchers_.value(pressedPos_);
                spinSS->changeDir_(spinSS->getDir()%4+1);
                break;
            }
            default:
                break;
            }
        } else if(toDelete){
            // TODO :deal with this situation;
            switch (gameMap[pressedPos_.y()][pressedPos_.x()]->getBlockContent()) {
            case belt:{
                QSharedPointer<Belt> deleteBelt_ = belts_.value(pressedPos_);
                mainScene->removeItem(deleteBelt_.data());
                belts_.remove(pressedPos_);
                deleteBelt_->deleteBelt();
                beltPool.release(deleteBelt_);
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = false;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(0);
                break;
            }
            case composer:{
                Composer *deleteComposer_ = composers_.value(pressedPos_);
                mainScene->removeItem(deleteComposer_);
                composers_.remove(pressedPos_);
                delete deleteComposer_;
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = false;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(0);
                break;
            }
            case cutter:{
                Cutter *deleteCutter_ = cutters_.value(pressedPos_);
                mainScene->removeItem(deleteCutter_);
                cutters_.remove(pressedPos_);
                QPoint newCtPos2Maped = mapFromViewToGameMap(deleteCutter_->getPos_2());
                delete deleteCutter_;
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = false;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(0);
                gameMap[newCtPos2Maped.y()][newCtPos2Maped.x()]->occupied = false;
                gameMap[newCtPos2Maped.y()][newCtPos2Maped.x()]->setBlockContent(0);
                break;
            }
            case bin:{
                Bin *deleteBin_ = bins_.value(pressedPos_);
                mainScene->removeItem(deleteBin_);
                bins_.remove(pressedPos_);
                delete deleteBin_;
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = false;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(0);
                break;
            }
            case speedSwitcher:
            {
                SpeedSwitcher *deleteSS_ = speedSwitchers_.value(pressedPos_);
                mainScene->removeItem(deleteSS_);
                speedSwitchers_.remove(pressedPos_);
                delete deleteSS_;
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = false;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(0);
                speedSwitcherNum += 1;
                QString ssnumText = "remain:" + QString::number(speedSwitcherNum);
                ssnumTextItem->setPlainText(ssnumText);
            }
            default:
                break;
            }
        } else if(toPut){
            switch (toolType) {
            case belt:{
                break; // 为了保持完整性，其实完全没有意义
            }
            case composer:{
                Composer *new_composer = new Composer(composerLevel,mapFromModeltoReal(pressedPos_));
                composers_.insert(pressedPos_,new_composer);
                new_composer->setZValue(1);
                mainScene->addItem(new_composer);
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = true;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(2);
                break;
            }
            case cutter:{
                if (gameMap[pressedPos_.y()+1][pressedPos_.x()]->occupied){
                    QMessageBox::warning(this,"放置警告","这里放Cutter会产生空间\n冲突！请换一个地方放置\n或删除原有组件");
                } else{
                    Cutter *new_cutter = new Cutter(cutterLevel,mapFromModeltoReal(pressedPos_));
                    cutters_.insert(pressedPos_,new_cutter);
                    new_cutter->setZValue(1);
                    mainScene->addItem(new_cutter);
                    gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = true;
                    QPoint newCtPos2Maped = mapFromViewToGameMap(new_cutter->getPos_2());
                    gameMap[newCtPos2Maped.y()][newCtPos2Maped.x()]->occupied = true;
                    gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(3);
                    gameMap[newCtPos2Maped.y()][newCtPos2Maped.x()]->setBlockContent(3);
                }
                break;
            }
            case bin:{
                Bin *new_bin = new Bin(mapFromModeltoReal(pressedPos_));
                bins_.insert(pressedPos_,new_bin);
                new_bin->setZValue(1);
                mainScene->addItem(new_bin);
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = true;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(4);
                break;
            }
            case speedSwitcher:{
                if (speedSwitcherNum <= 0){
                    QMessageBox::warning(this,"放置警告","你没有多余的speedSwitcher\n了，请去购买或者回收已放置的");
                } else{
                    speedSwitcherNum -= 1;
                    SpeedSwitcher *new_speedSwitcher = new SpeedSwitcher(mapFromModeltoReal(pressedPos_));
                    speedSwitchers_.insert(pressedPos_,new_speedSwitcher);
                    new_speedSwitcher->setZValue(1);
                    mainScene->addItem(new_speedSwitcher);
                    gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = true;
                    gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(5);
                    QString ssnumText = "remain:" + QString::number(speedSwitcherNum);
                    ssnumTextItem->setPlainText(ssnumText);
                }
                break;
            }
            }
        }
    } else if(toPut && toolType == belt){
        // 结束放置轨道，什么也不用做
    }
    toSpin = false;
    toDelete = false;
    toPut = false;
}

void PlayWindow::mouseMoveEvent(QMouseEvent *event)
{
    // TODO: 结合lastPos判断当前放置的方向，同时要判定目前方块是否已被占据；结束后更新lastPos
    if (toPut && toolType == belt){
        QPoint nowPos_ = mapFromViewToGameMap(event->pos());
        if (nowPos_ != lastPos_){
            bool need_new = true;
            if (gameMap[nowPos_.y()][nowPos_.x()]->occupied && gameMap[nowPos_.y()][nowPos_.x()]->getBlockContent() != 1){
                //这之后强行打断，不会触发鼠标松开
                toPut = false;
                need_new = false;
            }
            int dx = nowPos_.x() - lastPos_.x();
            int dy = nowPos_.y() - lastPos_.y();
            if (dx+dy != 1 && dx+dy != -1){
                toPut = false;
                QMessageBox::critical(this,"放置警告","你的鼠标移动的太快了！\n请按行与列合理放置，\n不要对角放置传送带");
            } else{
                if (dx == 1){
                    // 对应相对右移
                    // 目前位置放置从右来的基础传送带
                    if (need_new){
                        QSharedPointer<Belt> new_belt = beltPool.acquire();
                        new_belt->initBelt(1,mapFromModeltoReal(nowPos_));
                        belts_.insert(nowPos_,new_belt);
                        new_belt->setZValue(1);
                        mainScene->addItem(new_belt.data());
                        gameMap[nowPos_.y()][nowPos_.x()]->occupied = true;
                        gameMap[nowPos_.y()][nowPos_.x()]->setBlockContent(1);
                    }
                    // 改变前一位置传送带的方向
                    QSharedPointer<Belt> last_belt = belts_.value(lastPos_);
                    if (last_belt->getDir() == 1 || last_belt->getDir() == 2){
                        last_belt->changeDir_(1);
                    } else{
                        last_belt->changeDir_(last_belt->getDir() + 8);
                    }
                } else if(dx == -1){
                    // 对应相对左移
                    if (need_new){
                        QSharedPointer<Belt> new_belt = beltPool.acquire();
                        new_belt->initBelt(2,mapFromModeltoReal(nowPos_));
                        belts_.insert(nowPos_,new_belt);
                        new_belt->setZValue(1);
                        mainScene->addItem(new_belt.data());
                        gameMap[nowPos_.y()][nowPos_.x()]->occupied = true;
                        gameMap[nowPos_.y()][nowPos_.x()]->setBlockContent(1);
                    }
                    QSharedPointer<Belt> last_belt = belts_.value(lastPos_);
                    if (last_belt->getDir() == 1 || last_belt->getDir() == 2){
                        last_belt->changeDir_(2);
                    } else{
                        last_belt->changeDir_(last_belt->getDir() + 4);
                    }
                } else if(dy == 1){
                    // 对应相对下移
                    if (need_new){
                        QSharedPointer<Belt> new_belt = beltPool.acquire();
                        new_belt->initBelt(3,mapFromModeltoReal(nowPos_));
                        belts_.insert(nowPos_,new_belt);
                        new_belt->setZValue(1);
                        mainScene->addItem(new_belt.data());
                        gameMap[nowPos_.y()][nowPos_.x()]->occupied = true;
                        gameMap[nowPos_.y()][nowPos_.x()]->setBlockContent(1);
                    }
                    QSharedPointer<Belt> last_belt = belts_.value(lastPos_);
                    if (last_belt->getDir() == 3 || last_belt->getDir() == 4){
                        last_belt->changeDir_(3);
                    } else{
                        last_belt->changeDir_(last_belt->getDir() + 8);
                    }
                } else if(dy == -1){
                    // 对应相对上移
                    if (need_new){
                        QSharedPointer<Belt> new_belt = beltPool.acquire();
                        new_belt->initBelt(4,mapFromModeltoReal(nowPos_));
                        belts_.insert(nowPos_,new_belt);
                        new_belt->setZValue(1);
                        mainScene->addItem(new_belt.data());
                        gameMap[nowPos_.y()][nowPos_.x()]->occupied = true;
                        gameMap[nowPos_.y()][nowPos_.x()]->setBlockContent(1);
                    }
                    QSharedPointer<Belt> last_belt = belts_.value(lastPos_);
                    if (last_belt->getDir() == 3 || last_belt->getDir() == 4){
                        last_belt->changeDir_(4);
                    } else{
                        last_belt->changeDir_(last_belt->getDir() + 4);
                    }
                }
            }
            lastPos_ = nowPos_;
        }
    }
}

void PlayWindow::closeEvent(QCloseEvent *event)
{
    Do_received = 0;
    Re_received = 0;
    Mi_received = 0;
    Fa_received = 0;
    So_received = 0;
    La_received = 0;
    Si_received = 0;
    Empty_received = 0;

    belts_.clear();
    composers_.clear();
    cutters_.clear();
    bins_.clear();
    speedSwitchers_.clear();

    emit playWindowClosed();
    QGraphicsView::closeEvent(event);
}

void PlayWindow::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    QPixmap pwBg;

    switch (bgGenerateNum) {
    case 1:
        pwBg.load(":/img/resource/PlayWindowBg.png");
        break;
    case 2:
        pwBg.load(":/img/resource/PlayWindowBg2.png");
        break;
    case 3:
        pwBg.load(":/img/resource/PlayWindowBg3.png");
        break;
    }

    painter->setOpacity(0.33);
    painter->drawPixmap(0,0, this->width(),this->height(), pwBg);
    painter->setOpacity(1.0);
}

void PlayWindow::changeStyleSheet(QToolButton *btn)
{
    btn->setStyleSheet(
        "QToolButton { background-color: rgba(0, 0, 0, 0); border: none; }"
        "QToolButton:hover { background-color: rgba(0,0,0, 0.1); }"
        "QToolButton:pressed { background-color: rgba(0,0,0, 0.2); }"
        );
}

void PlayWindow::initMap(QFile *file)
{
    if (file == nullptr){
        // 画出中心方块；
        gameCenter = new Center(GlobalState::centerLevel,QPoint((5+21)*BLOCK_WIDTH,(3.5+9)*BLOCK_HEIGHT));
        // 中心以外，画出地图方块，注意使用随机算法分配有用的块位置；
        int emptyR = QRandomGenerator::global()->bounded(3,6);
        int emptyC = QRandomGenerator::global()->bounded(4,7);
        int noteR1 = QRandomGenerator::global()->bounded(3,7);
        int noteC1 = QRandomGenerator::global()->bounded(4,9);
        if (GlobalState::noteBlockLevel == 1){
            int noteR2 = 10 - noteR1;
            int noteC2 = 12 - noteC1;
            QRect totalArea(0,0,MAP_COLS,MAP_ROWS);
            QList<QRect> regularBlocks1 = {QRect(7,16,noteC1,noteR1),QRect(34,4,noteC2,noteR2)};
            QList<QRect> regularBlocks2 = {QRect(37,17,emptyC,emptyR)};
            remainedAreaConstruct(totalArea,regularBlocks1,regularBlocks2);
        } else if (GlobalState::noteBlockLevel == 2){
            int noteR2 = QRandomGenerator::global()->bounded(3,7);
            int noteC2 = QRandomGenerator::global()->bounded(4,8);
            int noteR3 = 14 - noteR1 - noteR2;
            int noteC3 = 16 - noteC1 - noteC2;
            QRect totalArea(0,0,MAP_COLS,MAP_ROWS);
            QList<QRect> regularBlocks1 = {QRect(7,16,noteC1,noteR1),
            QRect(34,4,noteC2,noteR2), QRect(9,5,noteC3,noteR3)};
            QList<QRect> regularBlocks2 = {QRect(37,17,emptyC,emptyR)};
            remainedAreaConstruct(totalArea,regularBlocks1,regularBlocks2);
        }
        gameCenter->setZValue(3);
        mainScene->addItem(gameCenter);
        for (int i = 0; i < MAP_ROWS; i++){
            for (int j = 0; j < MAP_COLS; j++){
                if (gameMap[i][j] != nullptr){
                    gameMap[i][j]->setZValue(0);
                    mainScene->addItem(gameMap[i][j]);
                }
            }
        }
        for (int i = 0; i < gameCenter->getCenterHeight(); ++i) {
            for (int j = 0; j < gameCenter->getCenterWidth(); ++j) {
                gameMap[9+i][21+j]->occupied = true;
                gameMap[9+i][21+j]->setBlockContent(6);
            }
        }

        // 建立对象池
        beltPool = MyObjectPool<Belt>(80);

    } else{
        // 读入地图与本局信息；
    }
}

void PlayWindow::remainedAreaConstruct(const QRect &totalArea, const QList<QRect> &regularBlocks1,const QList<QRect> &regularBlocks2)
{
    QSet<QPoint> visitedPoints;

    // 遍历规律方块范围
    for (const QRect &block : regularBlocks1)
    {
      for (int x = block.left(); x <= block.right(); ++x)
      {
        for (int y = block.top(); y <= block.bottom(); ++y)
        {
          QPoint point(x, y);
          // 处理特殊方块
          gameMap[y][x] = new Block(1);
          gameMap[y][x]->setBlockPos_(mapFromModeltoReal(point));
          visitedPoints.insert(point);
        }
      }
    }

    for (const QRect &block : regularBlocks2)
    {
        for (int x = block.left(); x <= block.right(); ++x)
        {
            for (int y = block.top(); y <= block.bottom(); ++y)
            {
                QPoint point(x, y);
                // 处理特殊方块
                gameMap[y][x] = new Block(2);
                gameMap[y][x]->setBlockPos_(mapFromModeltoReal(point));
                visitedPoints.insert(point);
            }
        }
    }

    // 遍历剩余区域
    for (int x = totalArea.left(); x <= totalArea.right(); ++x)
    {
      for (int y = totalArea.top(); y <= totalArea.bottom(); ++y)
      {
        QPoint point(x, y);
        if (!visitedPoints.contains(point))
        {
            // 处理剩余的点
            gameMap[y][x] = new Block(0);
            gameMap[y][x]->setBlockPos_(mapFromModeltoReal(point));
        }
      }
    }
}

QPoint PlayWindow::mapFromViewToGameMap(QPoint viewPos)
{
    QPointF scenePos = this->mapToScene(viewPos);

    // 调整场景坐标，减去gameMap的偏移量
    scenePos.setX(scenePos.x() - 5 * BLOCK_WIDTH);
    scenePos.setY(scenePos.y() - 3.5 * BLOCK_HEIGHT);

    // 计算行列索引
    int colIndex = static_cast<int>(scenePos.x()) / BLOCK_WIDTH;
    int rowIndex = static_cast<int>(scenePos.y()) / BLOCK_HEIGHT;

    // 返回行列索引
    return QPoint(colIndex, rowIndex);
}

void PlayWindow::on_ShopBtn__clicked()
{
    shopWindow = new Shop(this,true);
    connect(shopWindow,&Shop::shopWindowClosed,this,&PlayWindow::on_ShopWindow__Closed);
    shopWindow->exec();
}


void PlayWindow::on_InfoBtn__clicked()
{
    infoWindow = new Infomation(this,chapterNum);
    infoWindow->exec();
}


void PlayWindow::on_BeltTool__clicked()
{
    toolType = belt;
}


void PlayWindow::on_ComposerTool__clicked()
{
    toolType = composer;
}


void PlayWindow::on_CutterTool__clicked()
{
    toolType = cutter;
}


void PlayWindow::on_BinTool__clicked()
{
    toolType = bin;
}


void PlayWindow::on_SpeedMachineTool__clicked()
{
    toolType = speedSwitcher;
    Do_received++;
}

void PlayWindow::on_ShopWindow__Closed()
{
    QString ssnumText = "remain:" + QString::number(speedSwitcherNum);
    ssnumTextItem->setPlainText(ssnumText);
    update();
}

