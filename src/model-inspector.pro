#
# This file is part of the GAMS Studio project.
#
# Copyright (c) 2017-2018 GAMS Software GmbH <support@gams.com>
# Copyright (c) 2017-2018 GAMS Development Corp. <support@gams.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

QT       += core gui widgets

TARGET = modelinspector
TEMPLATE = app
DESTDIR = bin

CONFIG += c++14

# Setup and include the GAMS distribution
include(../gamsdependency.pri)

macx {
# ! The icns-file is created from a folder named gams.iconset containing images in multiple sizes.
# ! On mac osX type the command: iconutil -c icns [base-folder]/gams.iconset to create gams.icns
#    ICON = studio.icns
#    QMAKE_INFO_PLIST=Info.plist
    HEADERS += macoscocoabridge.h \
               macospathfinder.h

    SOURCES += macospathfinder.cpp

    OBJECTIVE_SOURCES += macoscocoabridge.mm

    QMAKE_INFO_PLIST = ../platform/macos/info.plist

    LIBS += -framework AppKit
}
unix {
    LIBS += -ldl
}
win32 {
#    RC_FILE += studio.rc
    LIBS += -luser32
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    exception.cpp \
    main.cpp \
    mainwindow.cpp \
    gamsprocess.cpp \
    commonpaths.cpp \
    gamslibprocess.cpp \
    modelinspector/hierarchicalheaderview.cpp \
    modelinspector/modelinstance.cpp    \
    modelinspector/modelinspector.cpp \
    modelinspector/modelinstancetablemodel.cpp \
    modelinspector/modelstatistic.cpp \
    modelinspector/sectiontreemodel.cpp \
    modelinspector/sectiontreeview.cpp \
    modelinspector/statisticedit.cpp \
    modelinspector/statisticswidget.cpp

HEADERS += \
    exception.h \
    mainwindow.h \
    gamsprocess.h \
    commonpaths.h \
    gamslibprocess.h    \
    modelinspector/hierarchicalheaderview.h \
    modelinspector/modelinstance.h  \
    modelinspector/modelinspector.h \
    modelinspector/modelinstancetablemodel.h \
    modelinspector/modelstatistic.h \
    modelinspector/sectiontreemodel.h \
    modelinspector/sectiontreeview.h \
    modelinspector/statisticedit.h \
    modelinspector/statisticswidget.h

FORMS += \
    mainwindow.ui \
    modelinspector/modelinspector.ui \
    modelinspector/statisticwidget.ui

OTHER_FILES +=          \
    ../platform/macos/modelinspector.entitlements.plist \
    ../platform/linux/modelinspector.desktop            \
    ../jenkinsfile-ci                                   \
    ../version
