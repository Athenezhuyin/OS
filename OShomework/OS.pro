#-------------------------------------------------
#
# Project created by QtCreator 2015-07-07T18:56:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OS
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    memory.cpp \
    file.cpp \
    process.cpp

HEADERS  += mainwindow.h \
    process.h \
    memory.h \
    file.h

FORMS    += mainwindow.ui
