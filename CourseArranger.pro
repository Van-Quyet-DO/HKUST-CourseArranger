QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    class.cpp \
    course.cpp \
    helper.cpp \
    impl.cpp \
    main.cpp \
    mainwindow.cpp \
    timetable.cpp

HEADERS += \
    class.h \
    course.h \
    helper.h \
    impl.h \
    mainwindow.h \
    msxml.h \
    timetable.h \
    urlmon.h \
    winapifamily.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
# https://stackoverflow.com/questions/35110064/using-winapi-functions-in-qt-wininet

win32: LIBS += -L$$PWD/./ -lurlmon

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/./liburlmon.a
else:win32-g++: PRE_TARGETDEPS += $$PWD/./liburlmon.a
RESOURCES +=
