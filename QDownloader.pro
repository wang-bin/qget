#-------------------------------------------------
#
# Project created by QtCreator 2011-06-30T13:47:27
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = QDownloader
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR	= ./
MOC_DIR	= .moc
unix:OBJECTS_DIR = .obj/unix
win32:OBJECTS_DIR = .obj/win32
macx:OBJECTS_DIR = .obj/macx

SOURCES += main.cpp \
    qdownloader.cpp

OTHER_FILES += \
    Design.txt

HEADERS += \
    qdownloader.h \
    qdownloader_p.h


TRANSLATIONS += $(TARGET)-zh_CN.ts
