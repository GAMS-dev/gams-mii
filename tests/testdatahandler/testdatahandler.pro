include(../tests.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$SRCPATH/mii

SOURCES +=  tst_testdatahandler.cpp                      \
            $$SRCPATH/mii/datahandler.cpp                \
            $$SRCPATH/mii/modelinstance.cpp              \
            $$SRCPATH/mii/abstractmodelinstance.cpp      \
            $$SRCPATH/mii/aggregation.cpp                \
            $$SRCPATH/mii/symbol.cpp                     \
            $$SRCPATH/mii/labeltreeitem.cpp              \
            $$SRCPATH/mii/viewconfigurationprovider.cpp  \
            $$SRCPATH/mii/common.cpp                     \
            $$SRCPATH/mii/postopttreeitem.cpp
