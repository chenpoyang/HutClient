#-------------------------------------------------
#
# Project created by QtCreator 2011-11-04T17:49:42
#
#-------------------------------------------------

QT       += core gui sql

TARGET = HutClient
TEMPLATE = app


SOURCES += main.cpp\
        hutclient.cpp \
    logindlg.cpp \
    controlpanel.cpp \
    datawidget.cpp \
    student.cpp

HEADERS  += hutclient.h \
    logindlg.h \
    controlpanel.h \
    datawidget.h \
    student.h

FORMS    += hutclient.ui \
    logindlg.ui \
    controlpanel.ui \
    datawidget.ui \
    student.ui

RESOURCES += \
    Image.qrc

OTHER_FILES += \
    note/sql.txt \
    resource/hutIcon.ico \
    images/login.jpg \
    images/hutIcon.ico

RC_FILE = resource/mypp.rc










