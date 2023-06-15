include(../tests.pri)

QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$SRCPATH/modelinspector

SOURCES +=  tst_testviewconfigurationprovider.cpp                   \
            $$SRCPATH/modelinspector/abstractmodelinstance.cpp      \
            $$SRCPATH/modelinspector/modelinstance.cpp              \
            $$SRCPATH/modelinspector/datahandler.cpp                \
            $$SRCPATH/modelinspector/labeltreeitem.cpp              \
            $$SRCPATH/modelinspector/symbol.cpp                     \
            $$SRCPATH/modelinspector/aggregation.cpp                \
            $$SRCPATH/modelinspector/viewconfigurationprovider.cpp

