#-------------------------------------------------
#
# Project created by QtCreator 2020-05-07T23:16:05
#
#-------------------------------------------------

QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AWAH-SIP_Desktop-GUI
TEMPLATE = app

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
    AWAHSipDesktopGUI.cpp \
    accountdialog.cpp \
    cmdfacade.cpp \
    generalsettings.cpp \
    iOsettings.cpp \
    sipStateModel.cpp \
    accountsettings.cpp \
    addfileplayer.cpp \
    addsounddev.cpp \
    audioroutemodel.cpp \
    callstatistic.cpp \
    codecsettings.cpp \
    editorwidget.cpp \
    logwindow.cpp \
    makecall.cpp \
    route.cpp \
    main.cpp \
    websocketclient.cpp

HEADERS += \
    AWAHSipDesktopGUI.h \
    accountdialog.h \
    cmdfacade.h \
    generalsettings.h \
    iOsettings.h \
    sipStateModel.h \
    accountsettings.h \
    addfileplayer.h \
    addsounddev.h \
    audioroutemodel.h \
    callstatistic.h \
    codecsettings.h \
    editorwidget.h \
    logwindow.h \
    makecall.h \
    route.h \
    lib/types.h \
    websocketclient.h

FORMS += \
    AWAHSipDesktopGUI.ui \
    accountdialog.ui \
    iOsettings.ui \
    generalsettings.ui \
    accountsettings.ui \
    addfileplayer.ui \
    addsounddev.ui \
    callstatistic.ui \
    codecsettings.ui \
    logwindow.ui \
    makecall.ui \
    route.ui


RESOURCES += \
    icons.qrc


DISTFILES +=
