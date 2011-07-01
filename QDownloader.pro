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
DESTDIR	= bin
MOC_DIR	= .moc
unix:OBJECTS_DIR = .obj/unix
win32:OBJECTS_DIR = .obj/win32
macx:OBJECTS_DIR = .obj/macx

SOURCES += src/main.cpp \
	src/qdownloader.cpp

OTHER_FILES += \
    Design.txt

HEADERS += \
	src/qdownloader.h \
	src/qdownloader_p.h


TRANSLATIONS += i18n/$$TARGET-zh_CN.ts
