#-------------------------------------------------
#
# Project created by QtCreator 2019-12-09T13:48:37
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QChat
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    login.cpp \
    register.cpp \
    autoLogin.cpp \
    findgroup.cpp \
    filetrans.cpp \
    updateinfo.cpp \
    creategroup.cpp \
    filerecv.cpp \
    chatdlg.cpp \
    chatdlgstack.cpp \
    resetpwd.cpp \
    friendmanage.cpp \
    groupmanage.cpp

HEADERS += \
        hosturl.h\
        mainwindow.h \
    login.h \
    register.h \
    connecttosql.h \
    autoLogin.h \
    findgroup.h \
    filetrans.h \
    updateinfo.h \
    creategroup.h \
    filerecv.h \
    chatdlg.h \
    chatdlgstack.h \
    resetpwd.h \
    openDB.h \
    friendmanage.h \
    groupmanage.h

FORMS += \
        mainwindow.ui \
    login.ui \
    register.ui \
    autologin.ui \
    findgroup.ui \
    filetrans.ui \
    updateinfo.ui \
    creategroup.ui \
    filerecv.ui \
    chatdlg.ui \
    chatdlgstack.ui \
    resetpwd.ui \
    friendmanage.ui \
    groupmanage.ui

RESOURCES += \
    images.qrc
