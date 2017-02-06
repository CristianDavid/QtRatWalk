#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T16:26:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtRatWalk
TEMPLATE = app

SOURCES += main.cpp \
    RatWalkGui/MainWindow.cpp \
    RatWalkGui/ImageViewer.cpp \
    RatWalkCore/RatFile.cpp \
    RatWalkCore/Frame.cpp \
    RatWalkCore/Tracker.cpp \
    RatWalkCore/Video.cpp \
    RatWalkGui/Plotter.cpp

HEADERS  += \
    RatWalkCore/Points.h \
    RatWalkGui/MainWindow.h \
    RatWalkGui/cvMat2QtImage.h \
    RatWalkGui/ImageViewer.h \
    RatWalkCore/Constantes.h \
    RatWalkCore/ControlPoint.h \
    RatWalkCore/Frame.h \
    RatWalkCore/Tracker.h \
    RatWalkCore/RatFile.h \
    RatWalkCore/Video.h \
    RatWalkCore/RatWalkCore.h \
    RatWalkGui/Plotter.h

FORMS    += RatWalkGui/MainWindow.ui

LIBS     += `pkg-config --libs opencv`

CONFIG   += c++11

OTHER_FILES += \
    Doxyfile
