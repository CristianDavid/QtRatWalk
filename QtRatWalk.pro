#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T16:26:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtRatWalk
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    RatWalkTracker.cpp \
    RatWalkFrameObject.cpp \
    RatWalkTrackerVideoObject.cpp \
    ImageViewer.cpp \
    RatWalkFile.cpp

HEADERS  += MainWindow.h \
    RatWalkControlPoint.h \
    RatWalkFrameObject.h \
    RatWalkTrackerVideoObject.h \
    RatWalkTracker.h \
    cvMat2QtImage.h \
    ImageViewer.h \
    Points.h \
    RatWalkFile.h

FORMS    += MainWindow.ui

LIBS     += `pkg-config --libs opencv`

CONFIG   += c++11
