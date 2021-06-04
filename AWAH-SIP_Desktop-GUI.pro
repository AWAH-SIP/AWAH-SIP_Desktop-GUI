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
    ../AWAH-SIP_Desktop-GUI/addgpio.cpp \
    AWAHSipDesktopGUI.cpp \
    accountdialog.cpp \
    buddydialog.cpp \
    buddysettings.cpp \
    cmdfacade.cpp \
    connectdialog.cpp \
    generalsettings.cpp \
    gpioroutemodel.cpp \
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
    ../AWAH-SIP_Desktop-GUI/addgpio.h \
    AWAHSipDesktopGUI.h \
    accountdialog.h \
    buddydialog.h \
    buddysettings.h \
    cmdfacade.h \
    connectdialog.h \
    generalsettings.h \
    gpioroutemodel.h \
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
    ../AWAH-SIP_Desktop-GUI/addgpio.ui \
    AWAHSipDesktopGUI.ui \
    accountdialog.ui \
    buddydialog.ui \
    buddysettings.ui \
    connectdialog.ui \
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

RC_ICONS = images/AWAH_logo_sm.ico

DISTFILES +=
