#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T16:26:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtRatWalk
TEMPLATE = app

include(xlsx/qtxlsx.pri)

SOURCES += main.cpp \
    RatWalkGui/ImageViewer.cpp \
    RatWalkCore/RatFile.cpp \
    RatWalkCore/Frame.cpp \
    RatWalkCore/Video.cpp \
    RatWalkGui/Plotter.cpp \
    RatWalkGui/AnglePlotter.cpp \
    RatWalkCore/StepRegister.cpp \
    RatWalkCore/Constantes.cpp \
    RatWalkGui/MainWindow.cpp \
    RatWalkCore/Project.cpp \
    RatWalkCore/Corrector.cpp \
    RatWalkGui/ExportAnglesDialog.cpp \
    RatWalkGui/ShowImageDialog.cpp

HEADERS  += \
    RatWalkCore/Points.h \
    RatWalkGui/cvMat2QtImage.h \
    RatWalkGui/ImageViewer.h \
    RatWalkCore/Constantes.h \
    RatWalkCore/ControlPoint.h \
    RatWalkCore/Frame.h \
    RatWalkCore/RatFile.h \
    RatWalkCore/Video.h \
    RatWalkCore/RatWalkCore.h \
    RatWalkGui/Plotter.h \
    RatWalkGui/AnglePlotter.h \
    RatWalkCore/StepRegister.h \
    RatWalkGui/ExportAnglesDialog.h \
    RatWalkGui/MainWindow.h \
    RatWalkCore/Project.h \
    RatWalkGui/RatWalkGui.h \
    RatWalkCore/Corrector.h \
    RatWalkGui/ShowImageDialog.h

FORMS    += \
    RatWalkGui/AnglePlotter.ui \
    RatWalkGui/ExportAnglesDialog.ui \
    RatWalkGui/MainWindow.ui \
    RatWalkGui/ShowImageDialog.ui

LIBS     += `pkg-config --libs opencv`

CONFIG   += c++11

OTHER_FILES += \
    Doxyfile \
    .gitignore

RESOURCES += \
    icons.qrc
