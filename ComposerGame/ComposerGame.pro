QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Belt.cpp \
    Bin.cpp \
    Block.cpp \
    Center.cpp \
    Composer.cpp \
    Congratulation.cpp \
    Cutter.cpp \
    GlobalState.cpp \
    Infomation.cpp \
    Note.cpp \
    PlayWindow.cpp \
    Shop.cpp \
    SpeedSwitcher.cpp \
    main.cpp \
    EntranceWindow.cpp

HEADERS += \
    Belt.h \
    Bin.h \
    Block.h \
    Center.h \
    Composer.h \
    Congratulation.h \
    Cutter.h \
    EntranceWindow.h \
    GlobalState.h \
    Infomation.h \
    MyObjectPool.h \
    Note.h \
    PlayWindow.h \
    Shop.h \
    SpeedSwitcher.h

FORMS += \
    Congratulation.ui \
    EntranceWindow.ui \
    PlayWindow.ui \
    Shop.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
