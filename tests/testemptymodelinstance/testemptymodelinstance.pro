include(../tests.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$SRCPATH/mii

SOURCES +=  tst_testemptymodelinstance.cpp           \
            $$SRCPATH/mii/abstractmodelinstance.cpp  \
            $$SRCPATH/mii/datamatrix.cpp             \
            $$SRCPATH/mii/symbol.cpp                 \
            $$SRCPATH/mii/common.cpp                 \
            $$SRCPATH/mii/postopttreeitem.cpp
