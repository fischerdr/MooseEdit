#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T19:52:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MooseEdit
TEMPLATE = app

QT += opengl
INCLUDEPATH = C:\boost\boost_1_55_0

win32:RC_ICONS += cogMoose.ico

SOURCES += main.cpp\
        mainwindow.cpp \
    LsbReader.cpp \
    charactertab.cpp \
    finddialog.cpp \
    PakReader.cpp \
    tinyxml/tinystr.cpp \
    tinyxml/tinyxml.cpp \
    tinyxml/tinyxmlerror.cpp \
    tinyxml/tinyxmlparser.cpp \
    LsbWriter.cpp \
    TextureAtlas.cpp \
    GameCharacter.cpp \
    CharacterGroup.cpp \
    GameItem.cpp \
    ItemGroup.cpp \
    InventoryHandler.cpp \
    ItemLabel.cpp \
    TooltipFrame.cpp \
    GenStatsReader.cpp \
    ItemEditFrame.cpp \
    StatsView.cpp \
    ItemGeneral.cpp \
    RelativePositionLayout.cpp

HEADERS  += mainwindow.h \
    LsbReader.h \
    charactertab.h \
    finddialog.h \
    PakReader.h \
    tinyxml/tinystr.h \
    tinyxml/tinyxml.h \
    LsbWriter.h \
    LsbObject.h \
    TextureAtlas.h \
    GameCharacter.h \
    CharacterGroup.h \
    GameItem.h \
    ItemGroup.h \
    InventoryHandler.h \
    ItemLabel.h \
    TooltipFrame.h \
    GenStatsReader.h \
    ItemEditFrame.h \
    StatsView.h \
    ItemGeneral.h \
    RelativePositionLayout.h

FORMS    += mainwindow.ui \
    charactertab.ui \
    finddialog.ui \
    TooltipFrame.ui \
    ItemEditFrame.ui \
    StatsView.ui \
    ItemGeneral.ui

RESOURCES += \
    resources.qrc

OTHER_FILES +=