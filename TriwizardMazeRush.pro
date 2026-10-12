QT       += core gui widgets opengl network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera.cpp \
    clientnetworkmanager.cpp \
    collisionsystem.cpp \
    discoveryservice.cpp \
    enemy.cpp \
    enemytypes.cpp \
    gameobject.cpp \
    gamewidget.cpp \
    highscoreswidget.cpp \
    hostnetworkmanager.cpp \
    leaderboard.cpp \
    main.cpp \
    mainmenuwidget.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    objective.cpp \
    obstacle.cpp \
    pickup.cpp \
    player.cpp \
    soundmanager.cpp \
    spritemanager.cpp \
    tile.cpp \
    tilemap.cpp

HEADERS += \
    camera.h \
    clientnetworkmanager.h \
    collisionsystem.h \
    discoveryservice.h \
    enemy.h \
    enemytypes.h \
    gameobject.h \
    gamewidget.h \
    highscoreswidget.h \
    hostnetworkmanager.h \
    leaderboard.h \
    mainmenuwidget.h \
    mainwindow.h \
    networkmanager.h \
    objective.h \
    obstacle.h \
    pickup.h \
    player.h \
    soundmanager.h \
    spritemanager.h \
    tile.h \
    tilemap.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc
