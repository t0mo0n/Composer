#include "PlayWindow.h"
#include "ui_PlayWindow.h"
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>
#include "GlobalState.h"

int PlayWindow::initSpeed=1;
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
PlayWindow::ttype_ PlayWindow::toolType = PlayWindow::belt;
QList<Belt*> PlayWindow::belts_;
QList<Composer*> PlayWindow::composers_;
QList<Cutter*> PlayWindow::cutters_;
QList<Bin*> PlayWindow::bins_;
QList<SpeedSwitcher*> PlayWindow::speedSwitchers_;

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

    updateTimer = new QTimer();
    connect(updateTimer,&QTimer::timeout, this,&PlayWindow::updateGame);
    updateTimer->start(100);
}

PlayWindow::~PlayWindow()
{
    delete ui;
}

void PlayWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint pressedPoint = mapFromViewToGameMap(event->pos());
    pressedPos_ = pressedPoint;
    if (gameMap[pressedPoint.y()][pressedPoint.x()]->occupied){
        // 左键对组件进行旋转；右键则是删除
        if (event->button() == Qt::LeftButton){
            // 准备旋转
            toSpin = true;
        } else{
            // 准备删除
            toDelete = true;
        }
    } else{
        // 左键新增方块或者开始铺设；右键无效
        if (event->button() == Qt::LeftButton){
            toPut = true;
            if (toolType == belt){
                // 开始放置轨道，记录位置，并以此来判断轨道放置的方向
            }
        }
    }
}

void PlayWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint releasedPoint = mapFromViewToGameMap(event->pos());
    if (releasedPoint == pressedPos_){
        if (toSpin){
            // 处理旋转
        } else if(toDelete){
            // deal with this situation;

        } else if(toPut){
            switch (PlayWindow::toolType) {
            case PlayWindow::belt:
                // 结束放置轨道
                break;
            case PlayWindow::composer:{
                Composer *new_composer = new Composer(composerLevel,pressedPos_);
                PlayWindow::composers_.push_back(new_composer);
                new_composer->setZValue(1);
                scene()->addItem(new_composer);
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = true;
                break;
            }
            case PlayWindow::cutter:{
                Cutter *new_cutter = new Cutter(cutterLevel,pressedPos_);
                PlayWindow::cutters_.push_back(new_cutter);
                new_cutter->setZValue(1);
                scene()->addItem(new_cutter);
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = true;
                break;
            }
            case PlayWindow::bin:{

                break;
            }
            case PlayWindow::speedSwitcher:{

                break;
            }
            }
        }
    }
    toSpin = false;
    toDelete = false;
    toPut = false;
    // pressedPos_ = QPoint(-100,-100);
}

void PlayWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 结合lastPos判断当前放置的方向，同时要判定目前方块是否已被占据；结束后更新lastPos
    if (toPut){

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
    QWidget::closeEvent(event);
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
        // center = new Center(GlobalState::centerLevel,QPoint((5+22)*BLOCK_WIDTH,(3.5+10)*BLOCK_HEIGHT));
        // 中心以外，画出地图方块，注意使用随机算法分配有用的块位置；
        int emptyR = QRandomGenerator::global()->bounded(3,5);
        int emptyC = QRandomGenerator::global()->bounded(3,6);
        int noteR1 = QRandomGenerator::global()->bounded(2,6);
        int noteC1 = QRandomGenerator::global()->bounded(3,8);
        if (GlobalState::noteBlockLevel == 1){
            int noteR2 = 9 - noteR1;
            int noteC2 = 11 - noteC1;
            QRect totalArea(0,0,MAP_COLS,MAP_ROWS);
            QList<QRect> regularBlocks1 = {QRect(7,16,noteC1,noteR1),QRect(34,2,noteC2,noteR2)};
            QList<QRect> regularBlocks2 = {QRect(37,17,emptyC,emptyR)};
            remainedAreaConstruct(totalArea,regularBlocks1,regularBlocks2);
        } else if (GlobalState::noteBlockLevel == 2){
            int noteR2 = QRandomGenerator::global()->bounded(2,6);
            int noteC2 = QRandomGenerator::global()->bounded(3,7);
            int noteR3 = 13 - noteR1 - noteR2;
            int noteC3 = 16 - noteC1 - noteC2;
            QRect totalArea(0,0,MAP_ROWS,MAP_COLS);
            QList<QRect> regularBlocks1 = {QRect(7,16,noteC1,noteR1),
            QRect(34,2,noteC2,noteR2), QRect(9,3,noteC3,noteR3)};
            QList<QRect> regularBlocks2 = {QRect(37,17,emptyC,emptyR)};
            remainedAreaConstruct(totalArea,regularBlocks1,regularBlocks2);
        }
        // center->setZValue(3);
        // mainScene->addItem(center);
        for (int i = 0; i < MAP_ROWS; i++){
            for (int j = 0; j < MAP_COLS; j++){
                if (gameMap[i][j] != nullptr){
                    gameMap[i][j]->setZValue(0);
                    mainScene->addItem(gameMap[i][j]);
                }
            }
        }

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

