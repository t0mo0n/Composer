#include "PlayWindow.h"
#include "ui_PlayWindow.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
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
MyObjectPool<Note>* PlayWindow::notePool = nullptr;
QList<QSharedPointer<Note>> PlayWindow::notes_;
QHash<QPoint, QSharedPointer<Belt>> PlayWindow::belts_;
QHash<QPoint, Composer*> PlayWindow::composers_;
QHash<QPoint, Cutter*> PlayWindow::cutters_;
QHash<QPoint, Bin*> PlayWindow::bins_;
QHash<QPoint, SpeedSwitcher*> PlayWindow::speedSwitchers_;
QGraphicsScene *PlayWindow::mainScene = nullptr;

PlayWindow::PlayWindow(const QString &archiveFileName,int chapnum,QWidget *parent)
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
    chapterNum = chapnum; // 非存档导入的关卡数初始化方法
    chapFinished = false;

    notePool = new MyObjectPool<Note>(100);
    PlayWindowBGM = new QMediaPlayer(this);
    pwBgmOutput = new QAudioOutput(this);
    PlayWindowBGM->setAudioOutput(pwBgmOutput);
    switch (bgGenerateNum) {
    case 1:
        PlayWindowBGM->setSource(QUrl("qrc:/music/resource/My-love.mp3"));
        break;
    case 2:
        PlayWindowBGM->setSource(QUrl("qrc:/music/resource/If-I-Can-Stop-One-Heart-From-Breaking.mp3"));
        break;
    case 3:
        PlayWindowBGM->setSource(QUrl("qrc:/music/resource/Florence.mp3"));
        break;
    }

    changeStyleSheet(ui->ShopBtn_);
    changeStyleSheet(ui->InfoBtn_);
    changeStyleSheet(ui->SaveBtn_);
    changeStyleSheet(ui->HelpBtn_);
    changeStyleSheet(ui->MuteBtn_);

    changeStyleSheet(ui->BeltTool_);
    changeStyleSheet(ui->ComposerTool_);
    changeStyleSheet(ui->CutterTool_);
    changeStyleSheet(ui->BinTool_);
    changeStyleSheet(ui->SpeedMachineTool_);

    if (archiveFileName == ""){
        initMap();
    } else{
        initMap(archiveFileName);// 打开对应存档文件
    }

    // 完成文字的显示，显示内容就是目前有的SpeedMachine数量
    ssnumTextItem = new QGraphicsTextItem();
    ssnumTextItem->setDefaultTextColor(QColor(255, 218, 185));
    ssnumTextItem->setFont(QFont("Eras Bold ITC", 20));
    mainScene->addItem(ssnumTextItem);
    ssnumTextItem->setPos(10, 620);
    QString ssnumText = "remain:" + QString::number(speedSwitcherNum);
    ssnumTextItem->setPlainText(ssnumText);

    pressedPos_ = QPoint(-100,-100);
    lastPos_ = QPoint(-200,-200);
    toPut = false;
    toSpin = false;
    toDelete = false;

    PlayWindowBGM->play();
    PlayWindowBGM->setLoops(-1);

    updateTimer = new QTimer();
    connect(updateTimer,&QTimer::timeout, this,&PlayWindow::updateGame);
    updateTimer->start(100);

    noteMoveTimer = new QTimer();
    connect(noteMoveTimer,&QTimer::timeout,this,&PlayWindow::noteMove);
    noteMoveTimer->start(50);
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
    if (gameMap[pressedPos_.y()][pressedPos_.x()]->getBlockContent() == 6){
        // 准备调用中心演奏函数
        if (pressedPos_.y() >= 10 && pressedPos_.y() <= 8+gameCenter->getCenterHeight()
            && pressedPos_.x() >= 22 && pressedPos_.x() <= 20+gameCenter->getCenterWidth()){
            toPlay = true;
        } else {toPlay = false;}
    } else if (gameMap[pressedPos_.y()][pressedPos_.x()]->occupied){
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
                // 开始放置轨道，记录位置(以此来判断下一轨道放置的方向)
                QSharedPointer<Belt> new_belt = beltPool.acquire();
                new_belt->initBelt(1,mapFromModeltoReal(pressedPos_));
                belts_.insert(pressedPos_,new_belt);
                new_belt.data()->setZValue(1);
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
        toPlay = false;
        toSpin = false;
        toDelete = false;
        toPut = false;
        return;
    }
    if (toPlay){
        if (releasedPoint.y() >= 10 && releasedPoint.y() <= 8+gameCenter->getCenterHeight()
            && releasedPoint.x() >= 22 && releasedPoint.x() <= 20+gameCenter->getCenterWidth()){
            // 演奏演示音乐
            if (chapterNum <= 3){
                PlayWindowBGM->stop();
                connect(gameCenter,&Center::playEnd,this,&PlayWindow::on_MuteBtn__clicked);
                gameCenter->centerPlay(chapterNum);
            }
        }
        toPlay = false;
        toSpin = false;
        toDelete = false;
        toPut = false;
        return;
    }
    if (releasedPoint == pressedPos_){
        if (toSpin){
            // 处理旋转
            QMutableListIterator<QSharedPointer<Note>> noteDeleter(notes_);
            while (noteDeleter.hasNext()) {
                if (mapFromViewToGameMap(noteDeleter.next()->boundedPos_) == pressedPos_){
                    mainScene->removeItem(noteDeleter.value().data());
                    noteDeleter.value()->noteDiscard();
                    notePool->release(noteDeleter.value());
                    noteDeleter.remove();
                }
            }
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
                spinBelt->occupied = false;
                break;
            }
            case composer:{
                Composer *spinComposer = composers_.value(pressedPos_);
                spinComposer->changeDir_(spinComposer->getDir()%4+1);
                spinComposer->occupied = false;
                break;
            }
            case cutter:{
                Cutter *spinCutter = cutters_.value(pressedPos_,nullptr);
                if (spinCutter == nullptr){
                    QList<QPoint> findSpinCtList= {QPoint(0,1),QPoint(0,-1),QPoint(1,0),QPoint(-1,0)};
                    for (auto point : findSpinCtList) {
                        spinCutter = cutters_.value(point+pressedPos_,nullptr);
                        if (spinCutter != nullptr && spinCutter->getPos_2() == mapFromModeltoReal(pressedPos_)) break;
                    }
                    QMutableListIterator<QSharedPointer<Note>> noteDeleter2(notes_);
                    while (noteDeleter2.hasNext()) {
                        if (noteDeleter2.next()->boundedPos_ == spinCutter->getPos_1()){
                            mainScene->removeItem(noteDeleter2.value().data());
                            noteDeleter2.value()->noteDiscard();
                            notePool->release(noteDeleter2.value());
                            noteDeleter2.remove();
                        }
                    }
                    if (spinCutter == nullptr){
                        QMessageBox::critical(this,"出错","ERROR！");
                        toPlay = false;
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        return;
                    }
                } else{
                    QMutableListIterator<QSharedPointer<Note>> noteDeleter2(notes_);
                    while (noteDeleter2.hasNext()) {
                        if (noteDeleter2.next()->boundedPos_ == spinCutter->getPos_2()){
                            mainScene->removeItem(noteDeleter2.value().data());
                            noteDeleter2.value()->noteDiscard();
                            notePool->release(noteDeleter2.value());
                            noteDeleter2.remove();
                        }
                    }
                }

                int new_dir = spinCutter->getDir()%4+1;
                switch (new_dir) {
                case 1:
                    if (gameMap[pressedPos_.y()-1][pressedPos_.x()]->occupied){
                        toPlay = false;
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        QMessageBox::warning(this,"放置警告","这个位置已经被占用！\n无法旋转过去");
                        return;
                    }
                    break;
                case 2:
                    if (gameMap[pressedPos_.y()][pressedPos_.x()+1]->occupied){
                        toPlay = false;
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        QMessageBox::warning(this,"放置警告","这个位置已经被占用！\n无法旋转过去");
                        return;
                    }
                    break;
                case 3:
                    if (gameMap[pressedPos_.y()+1][pressedPos_.x()]->occupied){
                        toPlay = false;
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        QMessageBox::warning(this,"放置警告","这个位置已经被占用！\n无法旋转过去");
                        return;
                    }
                    break;
                case 4:
                    if (gameMap[pressedPos_.y()][pressedPos_.x()-1]->occupied){
                        toPlay = false;
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
                spinCutter->occupied = 0;
                QPoint newCtPos2Maped = mapFromViewToGameMap(spinCutter->getPos_2());
                gameMap[newCtPos2Maped.y()][newCtPos2Maped.x()]->occupied = true;
                gameMap[newCtPos2Maped.y()][newCtPos2Maped.x()]->setBlockContent(3);
                break;
            }
            case speedSwitcher:
            {
                SpeedSwitcher *spinSS = speedSwitchers_.value(pressedPos_);
                spinSS->changeDir_(spinSS->getDir()%4+1);
                spinSS->occupied = false;
                break;
            }
            default:
                break;
            }
        } else if(toDelete){
            QMutableListIterator<QSharedPointer<Note>> noteDeleter(notes_);
            while (noteDeleter.hasNext()) {
                if (mapFromViewToGameMap(noteDeleter.next()->boundedPos_) == pressedPos_){
                    mainScene->removeItem(noteDeleter.value().data());
                    noteDeleter.value()->noteDiscard();
                    notePool->release(noteDeleter.value());
                    noteDeleter.remove();
                }
            }
            switch (gameMap[pressedPos_.y()][pressedPos_.x()]->getBlockContent()) {
            case belt:{
                QSharedPointer<Belt> deleteBelt_ = belts_.value(pressedPos_,nullptr);
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
                Cutter *deleteCutter_ = cutters_.value(pressedPos_,nullptr);
                if (deleteCutter_ == nullptr){
                    QList<QPoint> findDeleteCtList= {QPoint(0,1),QPoint(0,-1),QPoint(1,0),QPoint(-1,0)};
                    for (auto point : findDeleteCtList) {
                        deleteCutter_ = cutters_.value(point+pressedPos_,nullptr);
                        if (deleteCutter_ != nullptr && deleteCutter_->getPos_2() == mapFromModeltoReal(pressedPos_)) break;
                    }
                    QMutableListIterator<QSharedPointer<Note>> noteDeleter2(notes_);
                    while (noteDeleter2.hasNext()) {
                        if (noteDeleter2.next()->boundedPos_ == deleteCutter_->getPos_1()){
                            mainScene->removeItem(noteDeleter2.value().data());
                            noteDeleter2.value()->noteDiscard();
                            notePool->release(noteDeleter2.value());
                            noteDeleter2.remove();
                        }
                    }
                    if (deleteCutter_ == nullptr){
                        QMessageBox::critical(this,"出错","ERROR！");
                        toPlay = false;
                        toSpin = false;
                        toDelete = false;
                        toPut = false;
                        return;
                    }
                } else{
                    QMutableListIterator<QSharedPointer<Note>> noteDeleter2(notes_);
                    while (noteDeleter2.hasNext()) {
                        if (noteDeleter2.next()->boundedPos_ == deleteCutter_->getPos_2()){
                            mainScene->removeItem(noteDeleter2.value().data());
                            noteDeleter2.value()->noteDiscard();
                            notePool->release(noteDeleter2.value());
                            noteDeleter2.remove();
                        }
                    }
                }
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
                Composer *new_composer = new Composer(composerLevel,
                                    gameMap[pressedPos_.y()][pressedPos_.x()]->getBlockType(),mapFromModeltoReal(pressedPos_));
                composers_.insert(pressedPos_,new_composer);
                new_composer->setZValue(3);
                mainScene->addItem(new_composer);
                gameMap[pressedPos_.y()][pressedPos_.x()]->occupied = true;
                gameMap[pressedPos_.y()][pressedPos_.x()]->setBlockContent(2);
                break;
            }
            case cutter:{
                if (gameMap[pressedPos_.y()-1][pressedPos_.x()]->occupied){
                    QMessageBox::warning(this,"放置警告","这里放Cutter会产生空间\n冲突！请换一个地方放置\n或删除原有组件");
                } else{
                    Cutter *new_cutter = new Cutter(cutterLevel,mapFromModeltoReal(pressedPos_));
                    cutters_.insert(pressedPos_,new_cutter);
                    new_cutter->setZValue(3);
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
                new_bin->setZValue(3);
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
                    new_speedSwitcher->setZValue(3);
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
    toPlay = false;
    toSpin = false;
    toDelete = false;
    toPut = false;
}

// 后续优化：放置传送带的逻辑，将使用Belt工具时，从非Belt方块一直拖动到别的方块上也会持续放置传送带的逻辑完成
void PlayWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 结合lastPos判断当前放置的方向，同时要判定目前方块是否已被占据；结束后更新lastPos
    if (toPut && toolType == belt){
        QPoint nowPos_ = mapFromViewToGameMap(event->pos());
        if (nowPos_ != lastPos_){
            bool need_new = true;
            if (gameMap[nowPos_.y()][nowPos_.x()]->occupied && gameMap[nowPos_.y()][nowPos_.x()]->getBlockContent() != 1){
                // 强行打断
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
                        new_belt.data()->setZValue(1);
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
                        new_belt.data()->setZValue(1);
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
                        new_belt.data()->setZValue(1);
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
                        new_belt.data()->setZValue(1);
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

void PlayWindow::setNoteToScene(QSharedPointer<Note> np)
{
    np.data()->setZValue(2);
    mainScene->addItem(np.data());
    notes_.push_back(np);
}

void PlayWindow::closeEvent(QCloseEvent *event)
{
    // 停止定时器
    noteMoveTimer->stop();
    updateTimer->stop();
    delete noteMoveTimer;
    delete updateTimer;
    noteMoveTimer = nullptr;
    updateTimer = nullptr;

    delete gameCenter;
    gameCenter = nullptr;

    PlayWindowBGM->stop();
    delete PlayWindowBGM;
    PlayWindowBGM = nullptr;

    // 重置静态成员变量
    composerLevel = 0;
    cutterLevel = 0;
    speedSwitcherNum = 5;
    toolType = composer;
    Do_received = Re_received = Mi_received = Fa_received = So_received = La_received = Si_received = Empty_received = 0;

    // 清理存储指针的容器
    belts_.clear(); // QSharedPointer自动管理内存

    // 对于存储原始指针的容器，需要手动删除对象
    QHashIterator<QPoint, Composer*> composerIter(composers_);
    while (composerIter.hasNext()) {
        composerIter.next();
        delete composerIter.value();
    }
    composers_.clear();

    QHashIterator<QPoint, Cutter*> cutterIter(cutters_);
    while (cutterIter.hasNext()) {
        cutterIter.next();
        delete cutterIter.value();
    }
    cutters_.clear();

    QHashIterator<QPoint, Bin*> binIter(bins_);
    while (binIter.hasNext()) {
        binIter.next();
        delete binIter.value();
    }
    bins_.clear();

    // speedSwitcherNum += speedSwitchers_.size(); // 这是将ss数量保留，我暂时就不保留了，不然金币用不完
    QHashIterator<QPoint, SpeedSwitcher*> speedSwitcherIter(speedSwitchers_);
    while (speedSwitcherIter.hasNext()) {
        speedSwitcherIter.next();
        delete speedSwitcherIter.value();
    }
    speedSwitchers_.clear();

    // 下面这段代码很可能有问题
    // for (int i = notes_.size() - 1; i >= 0; --i) {
    //     auto note = notes_[i];
    //     if (note) {
    //         mainScene->removeItem(note.data());
    //     }
    // }
    notes_.clear();
    delete notePool;
    notePool = nullptr;

    // 清理gameMap中的Block对象
    for (int i = 0; i < MAP_ROWS; ++i) {
        for (int j = 0; j < MAP_COLS; ++j) {
            delete gameMap[i][j];
            gameMap[i][j] = nullptr;
        }
    }

    delete mainScene;
    mainScene = nullptr;
    
    emit playWindowClosed(chapterNum,chapFinished);
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

void PlayWindow::initMap(const QString& archiveFileName)
{
    if (archiveFileName == ""){
        // 画出中心方块；
        gameCenter = new Center(GlobalState::centerLevel,QPoint((5+21)*BLOCK_WIDTH,(3.5+9)*BLOCK_HEIGHT),chapterNum);
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
        // 建立对象池
        beltPool = MyObjectPool<Belt>(60);

    } else{
        // 读入地图与本局信息；
        QFile archiveFile(archiveFileName);
        if (!archiveFile.open(QIODevice::ReadOnly)) {
            qWarning("无法打开保存文件");
            return;
        }

        QByteArray saveData = archiveFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        QJsonObject jsonObject = loadDoc.object();

        // 读取强化信息和全局信息
        int nowCenterLv = jsonObject["nowCenterLv"].toInt();
        composerLevel = jsonObject["composerLevel"].toInt();
        cutterLevel = jsonObject["cutterLevel"].toInt();
        initSpeed = jsonObject["initSpeed"].toInt();
        Do_received = jsonObject["Do_received"].toInt();
        Re_received = jsonObject["Re_received"].toInt();
        Mi_received = jsonObject["Mi_received"].toInt();
        Fa_received = jsonObject["Fa_received"].toInt();
        So_received = jsonObject["So_received"].toInt();
        La_received = jsonObject["La_received"].toInt();
        Si_received = jsonObject["Si_received"].toInt();
        Empty_received = jsonObject["Empty_received"].toInt();
        speedSwitcherNum = jsonObject["speedSwitcherNum"].toInt();
        chapterNum = jsonObject["chapterNum"].toInt();
        int poolSize = jsonObject["beltPoolSize"].toInt();

        // 画出中心方块；
        gameCenter = new Center(nowCenterLv,QPoint((5+21)*BLOCK_WIDTH,(3.5+9)*BLOCK_HEIGHT),chapterNum);
        // 读取地图信息
        QJsonArray mapArray = jsonObject["map"].toArray();
        for (int i = 0; i < mapArray.size(); ++i) {
            QJsonObject blockObject = mapArray[i].toObject();
            int type = blockObject["type"].toInt();
            int content = blockObject["content"].toInt();
            QJsonArray posArray = blockObject["position"].toArray();
            int r = posArray[0].toInt();
            int c = posArray[1].toInt();
            gameMap[r][c] = new Block(type);
            gameMap[r][c]->setBlockPos_(mapFromModeltoReal(QPoint(c,r)));
            gameMap[r][c]->setBlockContent(content);
            if (content != 0){
                gameMap[r][c]->occupied = true;
            }
        }

        // 读取组件信息
        beltPool = MyObjectPool<Belt>(poolSize);
        QJsonArray beltsArray = jsonObject["belts"].toArray();
        for (int i = 0; i < beltsArray.size(); ++i) {
            QJsonObject beltObject = beltsArray[i].toObject();
            QJsonArray posArray = beltObject["position"].toArray();
            int y = posArray[0].toInt();
            int x = posArray[1].toInt();
            int direction = beltObject["direction"].toInt();
            // 根据位置和朝向恢复Belt
            auto belt = beltPool.acquire();
            belt->initBelt(1,QPoint(x,y));
            belt->changeDir_(direction);
            belts_.insert(mapFromViewToGameMap(QPoint(x,y)),belt);
            belt.data()->setZValue(1);
            mainScene->addItem(belt.data());
        }

        QJsonArray composersArray = jsonObject["composers"].toArray();
        for (int i = 0; i < composersArray.size(); ++i) {
            QJsonObject composerObject = composersArray[i].toObject();
            QJsonArray posArray = composerObject["position"].toArray();
            int y = posArray[0].toInt();
            int x = posArray[1].toInt();
            int direction = composerObject["direction"].toInt();
            int composerLv = composerObject["level"].toInt();

            QPoint posInMap = mapFromViewToGameMap(QPoint(x,y));
            Composer *composer = new Composer(composerLv,gameMap[posInMap.y()][posInMap.x()]->getBlockType(),QPoint(x,y));
            composer->changeDir_(direction);
            composers_.insert(posInMap,composer);
            composer->setZValue(3);
            mainScene->addItem(composer);
        }

        QJsonArray cuttersArray = jsonObject["cutters"].toArray();
        for (int i = 0; i < cuttersArray.size(); ++i) {
            QJsonObject cutterObject = cuttersArray[i].toObject();
            QJsonArray posArray = cutterObject["position"].toArray();
            int y = posArray[0].toInt();
            int x = posArray[1].toInt();
            int direction = cutterObject["direction"].toInt();
            int cutterLv = cutterObject["level"].toInt();

            Cutter *cutter = new Cutter(cutterLv,QPoint(x,y));
            cutter->changeDir_(direction);
            cutters_.insert(mapFromViewToGameMap(QPoint(x,y)),cutter);
            cutter->setZValue(3);
            mainScene->addItem(cutter);
        }

        QJsonArray binsArray = jsonObject["bins"].toArray();
        for (int i = 0; i < binsArray.size(); ++i) {
            QJsonObject binObject = binsArray[i].toObject();
            QJsonArray posArray = binObject["position"].toArray();
            int y = posArray[0].toInt();
            int x = posArray[1].toInt();

            Bin *bin = new Bin(QPoint(x,y));
            bins_.insert(mapFromViewToGameMap(QPoint(x,y)),bin);
            bin->setZValue(3);
            mainScene->addItem(bin);
        }

        QJsonArray speedSwitchersArray = jsonObject["speedSwitchers"].toArray();
        for (int i = 0; i < speedSwitchersArray.size(); ++i) {
            QJsonObject speedSwitcherObject = speedSwitchersArray[i].toObject();
            QJsonArray posArray = speedSwitcherObject["position"].toArray();
            int y = posArray[0].toInt();
            int x = posArray[1].toInt();
            int direction = speedSwitcherObject["direction"].toInt();

            SpeedSwitcher *speedSwitcher = new SpeedSwitcher(QPoint(x,y));
            speedSwitcher->changeDir_(direction);
            speedSwitchers_.insert(mapFromViewToGameMap(QPoint(x,y)),speedSwitcher);
            speedSwitcher->setZValue(3);
            mainScene->addItem(speedSwitcher);
        }

        archiveFile.close();
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
}
// 要保存和读取的是我已经在PlayWindow中添加的某些组件的某些重要信息. 具体来说，需要保存的有强化信息：composer等级，
// cutter等级，initSpeed；全局信息：各种音符的received数量，speedSwitcherNum，关卡信息chapterNum；游戏局内地图信息，
// 包括gameMap中两类Block方块的分布情况，center的等级；已经放置的各组件的信息，包括所有在belts_中的belt组件的位置和朝向，所有在
// composers_中的composer组件的位置和朝向, 所有cutters_中cutter的位置和朝向, bins_中所有bin的位置, speedSwitchers_所有speedSwitcher的位置和朝向
void PlayWindow::saveGame(const QString &saveFileName)
{
    QJsonObject saveData;

    // 保存强化信息和全局信息
    saveData["nowCenterLv"] = gameCenter->getLv();
    saveData["composerLevel"] = composerLevel;
    saveData["cutterLevel"] = cutterLevel;
    saveData["initSpeed"] = initSpeed;
    saveData["Do_received"] = Do_received;
    saveData["Re_received"] = Re_received;
    saveData["Mi_received"] = Mi_received;
    saveData["Fa_received"] = Fa_received;
    saveData["So_received"] = So_received;
    saveData["La_received"] = La_received;
    saveData["Si_received"] = Si_received;
    saveData["Empty_received"] = Empty_received;
    saveData["speedSwitcherNum"] = speedSwitcherNum;
    saveData["chapterNum"] = chapterNum;
    saveData["beltPoolSize"] = beltPool.get_pool_size();

    // // 保存地图信息
    QJsonArray mapArray;
    for (int i = 0; i < MAP_ROWS; ++i) {
        for (int j = 0; j < MAP_COLS; ++j) {
            if (gameMap[i][j] != nullptr) {
                QJsonObject blockObject;
                blockObject["type"] = gameMap[i][j]->getBlockType();
                blockObject["content"] = gameMap[i][j]->getBlockContent();
                blockObject["position"] = QJsonArray{i, j};
                mapArray.append(blockObject);
            }
        }
    }
    saveData["map"] = mapArray;

    // 保存组件信息
    QJsonArray beltsArray;
    for (auto& belt : belts_) {
        QJsonObject beltObject;
        beltObject["position"] = QJsonArray{belt->getPos().y(),belt->getPos().x()};
        beltObject["direction"] = belt->getDir();
        beltsArray.append(beltObject);
    }
    saveData["belts"] = beltsArray;

    QJsonArray composerArray;
    for (auto& composer : composers_) {
        QJsonObject composerObject;
        composerObject["position"] = QJsonArray{composer->getPos().y(),composer->getPos().x()};
        composerObject["direction"] = composer->getDir();
        composerObject["level"] = composer->getLv();
        composerArray.append(composerObject);
    }
    saveData["composers"] = composerArray;

    QJsonArray cutterArray;
    for (auto& cutter : cutters_) {
        QJsonObject cutterObject;
        cutterObject["position"] = QJsonArray{cutter->getPos_1().y(),cutter->getPos_1().x()};
        cutterObject["direction"] = cutter->getDir();
        cutterObject["level"] = cutter->getLv();
        cutterArray.append(cutterObject);
    }
    saveData["cutters"] = cutterArray;

    QJsonArray binArray;
    for (auto& bin : bins_) {
        QJsonObject binObject;
        binObject["position"] = QJsonArray{bin->getPos().y(),bin->getPos().x()};
        binArray.append(binObject);
    }
    saveData["bins"] = binArray;

    QJsonArray speedSwitcherArray;
    for (auto& speedSwitcher : speedSwitchers_) {
        QJsonObject speedSwitcherObject;
        speedSwitcherObject["position"] = QJsonArray{speedSwitcher->getPos().y(),speedSwitcher->getPos().x()};
        speedSwitcherObject["direction"] = speedSwitcher->getDir();
        speedSwitcherArray.append(speedSwitcherObject);
    }
    saveData["speedSwitchers"] = speedSwitcherArray;

    // 写入文件
    QFile saveFile(saveFileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("无法打开保存文件");
        return;
    }
    saveFile.write(QJsonDocument(saveData).toJson());
    saveFile.close();
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

QPoint PlayWindow::mapFromViewToGameMap(QPoint scenePos)
{
    scenePos.setX(scenePos.x() - 5 * BLOCK_WIDTH);
    scenePos.setY(scenePos.y() - 3.5 * BLOCK_HEIGHT);

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

void PlayWindow::updateGame()
{
    if (gameCenter->chapterFinished()){
        chapFinished = true;
        this->close();
    }
    this->update();
}

// Move可优化：如果在拖动传送带来改变传送带方向时，音符也刚好在上面，而且它碰到新的方向上是卡住的，那么不管后面本传送带方向又变成了什么，
// 它大概率会仍然卡在那个停滞不动的位置，且仍然绑定在这个传送带的位置上，这种时候我建议你把它绑着的传送带删除掉；不然这个bug我可有的优化了

// Move函数逻辑：
// 先判定当前绑定位置是否合法；再判定当前绑定位置有无方块，有什么方块？
// 结合方块类型做处理：分为当前绑定的是什么以及移动完绑定的是什么
// composer 和cutter 生成时，会把note放在自己的方块中间；所以如果note在它们上面，就会朝着出口方向移动
// 当前在belt上则先判定方向，再进行正常移动；
// 移动结束以后重新判定，重新绑定方块！判断碰到了什么方块，最后如果碰到cutter则先判定方向；再做消亡处理，然后马上调用其生成函数；
// 结束碰到ss则先判定方向，再进行跳转移动，其中改变速度直接对note做就行，压根用不着ss；
// center则被center类处理掉并增加计数与金币；bin直接处理掉
void PlayWindow::noteMove()
{
    for (int i = notes_.size()-1;i>=0;i--){
        auto note = notes_[i];
        QPoint mapBoundedPos_ = mapFromViewToGameMap(note->boundedPos_);
        QSharedPointer<Belt> beltToMove_ = nullptr;
        Composer *composerToMove = nullptr;
        Cutter *cutterToMove = nullptr;
        SpeedSwitcher *ssToMove = nullptr;
        int &typo__ = note->typo_;
        if (note->stop){
            ;
        } else{
            if (gameMap[mapBoundedPos_.y()][mapBoundedPos_.x()]->getBlockContent() == 1){
                // 在belt上的移动
                typo__ = 1;
                beltToMove_ = belts_.value(mapBoundedPos_);
                switch (beltToMove_->getDir()) {
                case 1:
                    note->moveBy(note->noteSpeed_,0);
                    break;
                case 2:
                    note->moveBy(-note->noteSpeed_,0);
                    break;
                case 3:
                    note->moveBy(0,note->noteSpeed_);
                    break;
                case 4:
                    note->moveBy(0,-note->noteSpeed_);
                    break;
                case 5:
                    if (note->mapToScene(note->boundingRect().center()).x() < beltToMove_->boundingRect().center().x()){
                        note->moveBy(note->noteSpeed_,0);
                    } else{
                        note->setNoteX((int)beltToMove_->boundingRect().center().x()-NOTE_WIDTH/2); // 是否能成功设置并显示x的改变？
                        note->moveBy(0,-note->noteSpeed_);
                    }
                    break;
                case 6:
                    if (note->mapToScene(note->boundingRect().center()).x() > beltToMove_->boundingRect().center().x()){
                        note->moveBy(-note->noteSpeed_,0);
                    } else{
                        note->setNoteX((int)beltToMove_->boundingRect().center().x()-NOTE_WIDTH/2);
                        note->moveBy(0,-note->noteSpeed_);
                    }
                    break;
                case 7:
                    if (note->mapToScene(note->boundingRect().center()).y() < beltToMove_->boundingRect().center().y()){
                        note->moveBy(0,note->noteSpeed_);
                    } else{
                        note->setNoteY((int)beltToMove_->boundingRect().center().y()-NOTE_HEIGHT/2);
                        note->moveBy(-note->noteSpeed_,0);
                    }
                    break;
                case 8:
                    if (note->mapToScene(note->boundingRect().center()).y() > beltToMove_->boundingRect().center().y()){
                        note->moveBy(0,-note->noteSpeed_);
                    } else{
                        note->setNoteY((int)beltToMove_->boundingRect().center().y()-NOTE_HEIGHT/2);
                        note->moveBy(-note->noteSpeed_,0);
                    }
                    break;
                case 9:
                    if (note->mapToScene(note->boundingRect().center()).x() < beltToMove_->boundingRect().center().x()){
                        note->moveBy(note->noteSpeed_,0);
                    } else{
                        note->setNoteX((int)beltToMove_->boundingRect().center().x()-NOTE_WIDTH/2);
                        note->moveBy(0,note->noteSpeed_);
                    }
                    break;
                case 10:
                    if (note->mapToScene(note->boundingRect().center()).x() > beltToMove_->boundingRect().center().x()){
                        note->moveBy(-note->noteSpeed_,0);
                    } else{
                        note->setNoteX((int)beltToMove_->boundingRect().center().x()-NOTE_WIDTH/2);
                        note->moveBy(0,note->noteSpeed_);
                    }
                    break;
                case 11:
                    if (note->mapToScene(note->boundingRect().center()).y() < beltToMove_->boundingRect().center().y()){
                        note->moveBy(0,note->noteSpeed_);
                    } else{
                        note->setNoteY((int)beltToMove_->boundingRect().center().y()-NOTE_HEIGHT/2);
                        note->moveBy(note->noteSpeed_,0);
                    }
                    break;
                case 12:
                    if (note->mapToScene(note->boundingRect().center()).y() > beltToMove_->boundingRect().center().y()){
                        note->moveBy(0,-note->noteSpeed_);
                    } else{
                        note->setNoteY((int)beltToMove_->boundingRect().center().y()-NOTE_HEIGHT/2);
                        note->moveBy(note->noteSpeed_,0);
                    }
                    break;
                default:
                    QMessageBox::critical(this,"出错","ERROR!建议\n保存后重启游戏");
                    break;
                }
            } else if(gameMap[mapBoundedPos_.y()][mapBoundedPos_.x()]->getBlockContent() == 2){
                typo__ = 2;
                composerToMove = composers_.value(mapBoundedPos_,nullptr);
                switch (composerToMove->getDir()) {
                case 1: //出口朝右
                    note->moveBy(note->noteSpeed_,0);
                    break;
                case 2:
                    note->moveBy(0,note->noteSpeed_);
                    break;
                case 3:
                    note->moveBy(-note->noteSpeed_,0);
                    break;
                case 4:
                    note->moveBy(0,-note->noteSpeed_);
                    break;
                default:
                    break;
                }
            } else if(gameMap[mapBoundedPos_.y()][mapBoundedPos_.x()]->getBlockContent() == 3){
                typo__ = 3;
                cutterToMove = cutters_.value(mapBoundedPos_,nullptr);
                if (cutterToMove == nullptr){
                    // start from pos2
                    QList<QPoint> findCutterList= {QPoint(0,1),QPoint(0,-1),QPoint(1,0),QPoint(-1,0)};
                    for (auto point : findCutterList) {
                        cutterToMove = cutters_.value(point+mapBoundedPos_,nullptr);
                        if (cutterToMove != nullptr && cutterToMove->getPos_2() == mapFromModeltoReal(mapBoundedPos_)) break;
                    }
                }
                if (cutterToMove == nullptr){
                    QMessageBox::critical(this,"ERROR","ERROR!");
                    continue;
                }
                switch (cutterToMove->getDir()) {
                case 1: //出口朝右
                    note->moveBy(note->noteSpeed_,0);
                    break;
                case 2:
                    note->moveBy(0,note->noteSpeed_);
                    break;
                case 3:
                    note->moveBy(-note->noteSpeed_,0);
                    break;
                case 4:
                    note->moveBy(0,-note->noteSpeed_);
                    break;
                default:
                    break;
                }
            } else if(gameMap[mapBoundedPos_.y()][mapBoundedPos_.x()]->getBlockContent() == 5){
                typo__ = 4;
                ssToMove = speedSwitchers_.value(mapBoundedPos_);
                switch (ssToMove->getDir()) {
                case 1: //出口朝右
                    note->moveBy(note->noteSpeed_,0);
                    break;
                case 2:
                    note->moveBy(0,note->noteSpeed_);
                    break;
                case 3:
                    note->moveBy(-note->noteSpeed_,0);
                    break;
                case 4:
                    note->moveBy(0,-note->noteSpeed_);
                    break;
                default:
                    break;
                }
            }
        }
        // 判定并重新绑定方块的函数！绑定到新的方块前看看它有没有被占据；如果没有，绑完修改新旧物体的occupied属性
        // 结合新的绑定位置的方块，进行析构或者跳转;
        if (abs(note->mapToScene(note->boundingRect().center()).x()-(note->boundedPos_.x()+BLOCK_WIDTH/2)) >= BLOCK_WIDTH/2
            || abs(note->mapToScene(note->boundingRect().center()).y()-(note->boundedPos_.y()+BLOCK_HEIGHT/2)) >= BLOCK_HEIGHT/2){
            QPoint nextBoundPos_; // 这是在地图中的位置！
            switch (note->speedDir_) {
            case 1:
                nextBoundPos_ = mapBoundedPos_ + QPoint(1,0);
                break;
            case 2:
                nextBoundPos_ = mapBoundedPos_ + QPoint(0,1);
                break;
            case 3:
                nextBoundPos_ = mapBoundedPos_ + QPoint(-1,0);
                break;
            case 4:
                nextBoundPos_ = mapBoundedPos_ + QPoint(0,-1);
                break;
            }
            if (nextBoundPos_.x() < 0 || nextBoundPos_.y() < 0){
                note->stop = true;
                continue;
            } else if(gameMap[nextBoundPos_.y()][nextBoundPos_.x()]->occupied){
                if (typo__ == 3){
                    cutterToMove = cutters_.value(mapBoundedPos_,nullptr);
                    if (cutterToMove == nullptr){
                        // start from pos2
                        QList<QPoint> findCutterList= {QPoint(0,1),QPoint(0,-1),QPoint(1,0),QPoint(-1,0)};
                        for (auto point : findCutterList) {
                            cutterToMove = cutters_.value(point+mapBoundedPos_,nullptr);
                            if (cutterToMove != nullptr && cutterToMove->getPos_2() == mapFromModeltoReal(mapBoundedPos_)) break;
                        }
                    }
                    if (cutterToMove == nullptr){
                        QMessageBox::critical(this,"ERROR","ERROR!");
                        continue;
                    }
                }

                switch (gameMap[nextBoundPos_.y()][nextBoundPos_.x()]->getBlockContent()) {
                case belt:{
                    QSharedPointer<Belt> newBoundBelt = belts_.value(nextBoundPos_);
                    if (newBoundBelt->occupied){
                        note->stop = true;
                        continue;
                    }
                    switch (newBoundBelt->getDir()%4) {
                    case 1:
                        if (note->speedDir_ == 1){
                            note->stop = false;
                            note->boundedPos_ = newBoundBelt->getPos();
                            newBoundBelt->occupied = true;
                            switch (typo__) {
                            case 0:
                                break;
                            case 1:
                                beltToMove_ = belts_.value(mapBoundedPos_);
                                beltToMove_->occupied = false;
                                break;
                            case 2:
                                composerToMove = composers_.value(mapBoundedPos_);
                                composerToMove->occupied = false;
                                break;
                            case 3:
                                cutterToMove->occupied -= 1;
                                break;
                            case 4:
                                ssToMove = speedSwitchers_.value(mapBoundedPos_);
                                ssToMove->occupied = false;
                                break;
                            }
                        } else{
                            note->stop = true;
                        }
                        break;
                    case 2:
                        if (note->speedDir_ == 3){
                            note->stop = false;
                            note->boundedPos_ = newBoundBelt->getPos();
                            newBoundBelt->occupied = true;
                            switch (typo__) {
                            case 0:
                                break;
                            case 1:
                                beltToMove_ = belts_.value(mapBoundedPos_);
                                beltToMove_->occupied = false;
                                break;
                            case 2:
                                composerToMove = composers_.value(mapBoundedPos_);
                                composerToMove->occupied = false;
                                break;
                            case 3:
                                cutterToMove->occupied -= 1;
                                break;
                            case 4:
                                ssToMove = speedSwitchers_.value(mapBoundedPos_);
                                ssToMove->occupied = false;
                                break;
                            }
                        } else{
                            note->stop = true;
                        }
                        break;
                    case 3:
                        if (note->speedDir_ == 2){
                            note->stop = false;
                            note->boundedPos_ = newBoundBelt->getPos();
                            newBoundBelt->occupied = true;
                            switch (typo__) {
                            case 0:
                                break;
                            case 1:
                                beltToMove_ = belts_.value(mapBoundedPos_);
                                beltToMove_->occupied = false;
                                break;
                            case 2:
                                composerToMove = composers_.value(mapBoundedPos_);
                                composerToMove->occupied = false;
                                break;
                            case 3:
                                cutterToMove->occupied -= 1;
                                break;
                            case 4:
                                ssToMove = speedSwitchers_.value(mapBoundedPos_);
                                ssToMove->occupied = false;
                                break;
                            }
                        } else{
                            note->stop = true;
                        }
                        break;
                    case 0:
                        if (note->speedDir_ == 4){
                            note->stop = false;
                            note->boundedPos_ = newBoundBelt->getPos();
                            newBoundBelt->occupied = true;
                            switch (typo__) {
                            case 0:
                                break;
                            case 1:
                                beltToMove_ = belts_.value(mapBoundedPos_);
                                beltToMove_->occupied = false;
                                break;
                            case 2:
                                composerToMove = composers_.value(mapBoundedPos_);
                                composerToMove->occupied = false;
                                break;
                            case 3:
                                cutterToMove->occupied -= 1;
                                break;
                            case 4:
                                ssToMove = speedSwitchers_.value(mapBoundedPos_);
                                ssToMove->occupied = false;
                                break;
                            }
                        } else{
                            note->stop = true;
                        }
                        break;
                    }
                    break;
                }
                case composer:{
                    note->stop = true;
                    break;
                }
                case cutter:{
                    Cutter *newBoundCutter = cutters_.value(nextBoundPos_,nullptr);
                    if (newBoundCutter == nullptr){
                        QList<QPoint> findCutterList= {QPoint(0,1),QPoint(0,-1),QPoint(1,0),QPoint(-1,0)};
                        for (auto point : findCutterList) {
                            newBoundCutter = cutters_.value(point+nextBoundPos_,nullptr);
                            if (newBoundCutter != nullptr && newBoundCutter->getPos_2() == mapFromModeltoReal(nextBoundPos_)) break;
                        }
                    }
                    if (newBoundCutter == nullptr){
                        QMessageBox::critical(this,"ERROR","ERROR!");
                        continue;
                    }
                    if (newBoundCutter->occupied || newBoundCutter->getDir() != note->speedDir_){
                        note->stop = true;
                        continue;
                    } else{
                        note->stop = false;
                        switch (typo__) {
                        case 0:
                            break;
                        case 1:
                            beltToMove_ = belts_.value(mapBoundedPos_);
                            beltToMove_->occupied = false;
                            break;
                        case 2:
                            composerToMove = composers_.value(mapBoundedPos_);
                            composerToMove->occupied = false;
                            break;
                        case 3:
                            cutterToMove->occupied -= 1;
                            break;
                        case 4:
                            ssToMove = speedSwitchers_.value(mapBoundedPos_);
                            ssToMove->occupied = false;
                            break;
                        }

                        mainScene->removeItem(note.data());
                        newBoundCutter->generateNote(note->noteType,note->noteSpeed_);
                        note->noteDiscard();
                        notePool->release(note);
                        notes_.removeAt(i);
                    }
                    break;
                }
                case bin:{
                    note->stop = false;
                    switch (typo__) {
                    case 0:
                        break;
                    case 1:
                        beltToMove_ = belts_.value(mapBoundedPos_);
                        beltToMove_->occupied = false;
                        break;
                    case 2:
                        composerToMove = composers_.value(mapBoundedPos_);
                        composerToMove->occupied = false;
                        break;
                    case 3:
                        cutterToMove->occupied -= 1;
                        break;
                    case 4:
                        ssToMove = speedSwitchers_.value(mapBoundedPos_);
                        ssToMove->occupied = false;
                        break;
                    }

                    mainScene->removeItem(note.data());
                    note->noteDiscard();
                    notePool->release(note);
                    notes_.removeAt(i);
                    break;
                }
                case speedSwitcher:{
                    SpeedSwitcher *newBoundSS = speedSwitchers_.value(nextBoundPos_);
                    if (newBoundSS->occupied || newBoundSS->getDir() != note->speedDir_) {
                        note->stop = true;
                        continue;
                    }
                    note->changeSpeed();
                    note->stop = false;
                    note->boundedPos_ = newBoundSS->getPos();
                    newBoundSS->occupied = true;
                    switch (typo__) {
                    case 0:
                        break;
                    case 1:
                        beltToMove_ = belts_.value(mapBoundedPos_);
                        beltToMove_->occupied = false;
                        break;
                    case 2:
                        composerToMove = composers_.value(mapBoundedPos_);
                        composerToMove->occupied = false;
                        break;
                    case 3:
                        assert(cutterToMove != nullptr);// 莫名其妙的编译错误？
                        cutterToMove->occupied -= 1;
                        break;
                    case 4:
                        ssToMove = speedSwitchers_.value(mapBoundedPos_);
                        ssToMove->occupied = false;
                        break;
                    }
                    switch (note->speedDir_) {
                    case 1: //出口朝右
                        note->moveBy(BLOCK_WIDTH,0);
                        break;
                    case 2:
                        note->moveBy(0,BLOCK_HEIGHT);
                        break;
                    case 3:
                        note->moveBy(-BLOCK_WIDTH,0);
                        break;
                    case 4:
                        note->moveBy(0,-BLOCK_HEIGHT);
                        break;
                    }
                    break;
                }
                case 6:{
                    // 这是处理中心的情况
                    note->stop = false;
                    switch (typo__) {
                    case 0:
                        break;
                    case 1:
                        beltToMove_ = belts_.value(mapBoundedPos_);
                        beltToMove_->occupied = false;
                        break;
                    case 2:
                        composerToMove = composers_.value(mapBoundedPos_);
                        composerToMove->occupied = false;
                        break;
                    case 3:
                        cutterToMove->occupied -= 1;
                        break;
                    case 4:
                        ssToMove = speedSwitchers_.value(mapBoundedPos_);
                        ssToMove->occupied = false;
                        break;
                    }

                    mainScene->removeItem(note.data());

                    if ((nextBoundPos_.y() >= 10 && nextBoundPos_.y() <= 8+gameCenter->getCenterHeight())
                        ||(nextBoundPos_.x() >= 22 && nextBoundPos_.x() <= 20+gameCenter->getCenterWidth())){
                        // 成功被中心接收，计入数量并演奏音乐
                        gameCenter->noteRecieved(note->noteType);
                    } else{
                        ;// 未成功接受，只会析构音符！
                        // 这个特性要改也容易，把这个条件去掉就行，但是为了防止入口过多，还是判定一下吧
                    }

                    note->noteDiscard();
                    notePool->release(note);
                    notes_.removeAt(i);
                    break;
                }
                default:
                    QMessageBox::critical(this,"Error","好像发生了一些意外:)");
                    break;
                }
            } else{
                note->stop = true;
                continue;
            }
        }
    }
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


void PlayWindow::on_MuteBtn__clicked()
{
    if (bgmPlaying){
        bgmPlaying = false;
        PlayWindowBGM->stop();
    } else{
        bgmPlaying = true;
        PlayWindowBGM->play();
        PlayWindowBGM->setLoops(-1);
    }
}


void PlayWindow::on_SaveBtn__clicked()
{
    // QString appDir = QCoreApplication::applicationDirPath();
    // QString targetPath = appDir + "/../files/Archives/save-your-archive.json";
    QString saveFileName = QFileDialog::getSaveFileName(this, "存档", "F:/advProgramProject/Composer/files/Archives/save-your-archive.json");
    saveGame(saveFileName);
}

