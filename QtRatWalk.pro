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
    RatWalkCore/RatWalkTracker.cpp \
    RatWalkCore/RatWalkFrameObject.cpp \
    RatWalkCore/RatWalkTrackerVideoObject.cpp \
    RatWalkGui/ImageViewer.cpp \
    RatWalkCore/RatWalkFile.cpp

HEADERS  += \
    RatWalkCore/RatWalkControlPoint.h \
    RatWalkCore/RatWalkFrameObject.h \
    RatWalkCore/RatWalkTrackerVideoObject.h \
    RatWalkCore/Points.h \
    RatWalkCore/RatWalkFile.h \
    RatWalkCore/RatWalkTracker.h \
    RatWalkGui/MainWindow.h \
    RatWalkGui/cvMat2QtImage.h \
    RatWalkGui/ImageViewer.h \
    RatWalkCore/Constantes.h

FORMS    += RatWalkGui/MainWindow.ui

LIBS     += `pkg-config --libs opencv`

CONFIG   += c++11

OTHER_FILES += \
    Doxyfile
