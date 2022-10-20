#-------------------------------------------------
#
# Project created by QtCreator 2022-09-18T10:02:49
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NexusAI
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    socketserver/WebServer.cpp \
    socketserver/mongoose.c \
    ai-interface/cmessagequeuethread.cpp \
    ai-interface/fibernotice.cpp \
    ai-interface/watchermanager.cpp \
    email/cemailqueuethread.cpp \
    email/emailaddress.cpp \
    email/emailmanager.cpp \
    email/emailsender.cpp \
    email/mimeattachment.cpp \
    email/mimecontentformatter.cpp \
    email/mimefile.cpp \
    email/mimehtml.cpp \
    email/mimeinlinefile.cpp \
    email/mimemessage.cpp \
    email/mimemultipart.cpp \
    email/mimepart.cpp \
    email/mimetext.cpp \
    email/quotedprintable.cpp \
    email/smtpclient.cpp \
    sendsetting.cpp

HEADERS += \
        mainwindow.h \
    socketserver/mongoose.h \
    ai-interface/cmessagequeuethread.h \
    ai-interface/fibernotice.h \
    ai-interface/firesmokedetect.h \
    ai-interface/watchermanager.h \
    socketserver/WebServer.h \
    email/cemailqueuethread.h \
    email/emailaddress.h \
    email/emailmanager.h \
    email/emailsender.h \
    email/mimeattachment.h \
    email/mimecontentformatter.h \
    email/mimefile.h \
    email/mimehtml.h \
    email/mimeinlinefile.h \
    email/mimemessage.h \
    email/mimemultipart.h \
    email/mimepart.h \
    email/mimetext.h \
    email/quotedprintable.h \
    email/smtpclient.h \
    email/smtpexports.h \
    email/SmtpMime \
    sendsetting.h

FORMS += \
        mainwindow.ui \
    sendsetting.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
LIBS += -lole32 -loleAut32 -lstrmiids -luser32 -lws2_32 -lwlanapi
    INCLUDEPATH+= D:\opencv-openvino\include
    LIBS += D:\opencv-openvino\lib\opencv_*.lib
}

RC_ICONS = final.ico
