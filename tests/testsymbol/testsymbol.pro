include(../tests.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$SRCPATH/mii

SOURCES +=  $$SRCPATH/mii/symbol.cpp        \
            $$SRCPATH/mii/labeltreeitem.cpp \
            tst_testsymbol.cpp
