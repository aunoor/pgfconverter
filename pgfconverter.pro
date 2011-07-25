#-------------------------------------------------
#
# Project created by QtCreator 2011-07-22T11:33:45
#
#-------------------------------------------------

QT       += core gui xml

TARGET = pgfconverter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp \
    src/mainwindow.cpp

HEADERS += \
    src/mainwindow.h

FORMS += \
    ui/mainwindow.ui

INCLUDEPATH += src

UI_DIR = tmp/.ui
MOC_DIR = tmp/.moc
OBJECTS_DIR = tmp/.obj
RCC_DIR = tmp/.rcc
