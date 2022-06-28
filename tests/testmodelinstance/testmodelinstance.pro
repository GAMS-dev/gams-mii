include(../tests.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$SRCPATH/modelinspector

SOURCES +=  tst_testmodelinstance.cpp                           \
            $$SRCPATH/modelinspector/abstractmodelinstance.cpp  \
            $$SRCPATH/modelinspector/modelinstance.cpp          \
            $$SRCPATH/modelinspector/datahandler.cpp            \
            $$SRCPATH/modelinspector/filtertreeitem.cpp         \
            $$SRCPATH/modelinspector/labeltreeitem.cpp          \
            $$SRCPATH/modelinspector/symbol.cpp                 \
            $$SRCPATH/modelinspector/aggregation.cpp
