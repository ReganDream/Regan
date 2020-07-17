QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network xml sql

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
LIBS+="D:\Qt\Qt5.14.1\5.14.1\mingw73_64\bin\Qt5Ftp.dll"
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    email_login.cpp \
    emailsend.cpp \
    ftpclient.cpp \
    global.cpp \
    main.cpp \
    mainwindow.cpp \
    server.cpp \


HEADERS += \
    client.h \
    email_login.h \
    emailsend.h \
    ftpclient.h \
    global.h \
    mainwindow.h \
    server.h \

FORMS += \
    client.ui \
    email_login.ui \
    emailsend.ui \
    ftpclient.ui \
    mainwindow.ui \
    server.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc

RC_ICONS=1.ico
