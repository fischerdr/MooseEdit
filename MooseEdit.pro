#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T19:52:57
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MooseEdit
TEMPLATE = app

INCLUDEPATH += C:\boost\boost_1_55_0
INCLUDEPATH += C:\glew-1.11.0\include\
INCLUDEPATH += C:\glm

LIBS += "-LC:\boost\boost_1_55_0\stage\lib"
LIBS += "-LC:\boost\stage\lib" -lboost_filesystem-mgw48-mt-1_55
LIBS += "-LC:\boost\stage\lib" -lboost_system-mgw48-mt-1_55
LIBS += "-LC:\Users\Jason\MooseEdit\granny\lib\win32"
LIBS += "-lgranny2"

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
    RelativePositionLayout.cpp \
    CheckboxListItem.cpp \
    AbilityGroupItem.cpp \
    ExpandableGroupBox.cpp \
    TraitWidget.cpp \
    SkillEditFrame.cpp \
    EquipmentHandler.cpp \
    GamePakData.cpp \
    ItemTemplateWidget.cpp \
    EditorSettings.cpp \
    CharacterLoader.cpp \
    AppearanceEditorFrame.cpp \
    zgranny.cpp \
    GlContextWidget.cpp \
    nv_dds.cpp \
    LsxReader.cpp \
    LsbObject.cpp \
    LsxWriter.cpp \
    GlShader.cpp \
    GlShaderProgram.cpp \
    glew.c \
    PakCompressor.cpp \
    lz4.cpp \
    PakWriter.cpp \
    md5.cpp \
    SanityHash.cpp \
    lz4hc.cpp \
    SettingsDialog.cpp

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
    RelativePositionLayout.h \
    CheckboxListItem.h \
    AbilityGroupItem.h \
    ExpandableGroupBox.h \
    TraitWidget.h \
    SkillEditFrame.h \
    EquipmentHandler.h \
    GamePakData.h \
    ItemTemplateWidget.h \
    EditorSettings.h \
    CharacterLoader.h \
    AppearanceEditorFrame.h \
    granny/granny.h \
    zgranny.h \
    GlContextWidget.h \
    DDSLoader.h \
    dds.h \
    nv_dds.h \
    LsxReader.h \
    LsxWriter.h \
    GlShader.h \
    GlShaderProgram.h \
    PakCompressor.h \
    lz4.h \
    PakWriter.h \
    PakInfo.h \
    SanityHash.h \
    md5.h \
    lz4hc.h \
    SettingsDialog.h

FORMS    += mainwindow.ui \
    charactertab.ui \
    finddialog.ui \
    TooltipFrame.ui \
    ItemEditFrame.ui \
    StatsView.ui \
    ItemGeneral.ui \
    AbilityGroupItem.ui \
    TraitWidget.ui \
    SkillEditFrame.ui \
    ItemTemplateWidget.ui \
    AppearanceEditorFrame.ui \
    SettingsDialog.ui

RESOURCES += \
    resources.qrc
