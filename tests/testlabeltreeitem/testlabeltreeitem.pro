include(../tests.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$SRCPATH/mii

SOURCES +=  tst_testlabeltreeitem.cpp                       \
            $$$$SRCPATH/mii/labeltreeitem.cpp
