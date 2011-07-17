#-------------------------------------------------
#
# Project created by QtCreator 2011-06-30T13:47:27
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = qget
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR	= bin
INCLUDEPATH += src

MOC_DIR	= .moc/$$[QT_VERSION]
unix:OBJECTS_DIR = .obj/unix
win32:OBJECTS_DIR = .obj/win32
macx:OBJECTS_DIR = .obj/macx

SOURCES += src/main.cpp \
	src/qget.cpp \
    src/convert.cpp \
    src/ui/progressui.cpp \
    src/ui/cui/progresscui.cpp

OTHER_FILES += \
    Design.txt \
    TODO.txt \
    README

HEADERS += \
	src/qget.h \
	src/qget_p.h \
    src/convert.h \
    src/version.h \
    src/ui/progressui.h \
    src/ui/cui/progresscui.h


TRANSLATIONS += i18n/$$TARGET-zh_CN.ts
