include(../tests.pri)

QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$SRCPATH

HEADERS += $$SRCPATH/abstractprocess.h      \
           $$SRCPATH/gamslibprocess.h

SOURCES +=  tst_testgamslibprocess.cpp      \
            $$SRCPATH/commonpaths.cpp       \
            $$SRCPATH/exception.cpp         \
            $$SRCPATH/abstractprocess.cpp   \
            $$SRCPATH/gamslibprocess.cpp
