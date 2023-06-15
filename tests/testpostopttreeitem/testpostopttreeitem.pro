include(../tests.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$SRCPATH/modelinspector

SOURCES +=  tst_testpostopttreeitem.cpp                     \
            $$SRCPATH/modelinspector/common.cpp             \
            $$SRCPATH/modelinspector/postopttreeitem.cpp
