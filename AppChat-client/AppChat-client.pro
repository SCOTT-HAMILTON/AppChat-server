QT += core
QT -= gui
QT += widgets

CONFIG += c++11

TARGET = AppChat-client
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    WinApp.cpp \
    WorkerReceiver.cpp \
    ThreadManager.cpp \
    DataType.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -LC:/SFML/lib

# LIBS += -lvorbisfile -lFLAC -lfreetype -ljpeg -logg -lopenal32 -lvorbis -lvorbisenc

CONFIG(release, debug|release): LIBS += -lsfml-audio -lsfml-graphics -lsfml-main -lsfml-network -lsfml-window -lsfml-system
CONFIG(debug, debug|release): LIBS += -lsfml-audio-d -lsfml-graphics-d -lsfml-main-d -lsfml-network-d -lsfml-window-d -lsfml-system-d

# CONFIG(release, debug|release): LIBS += -lsfml-audio-s -lsfml-graphics-s -lsfml-main -lsfml-network-s -lsfml-window-s -lsfml-system-s
# CONFIG(debug, debug|release): LIBS += -lsfml-audio-s-d -lsfml-graphics-s-d -lsfml-main-d -lsfml-network-s-d -lsfml-window-s-d -lsfml-system-s-d

INCLUDEPATH += C:/SFML/include
DEPENDPATH += C:/SFML/include

HEADERS += \
    WinApp.h \
    WorkerReceiver.h \
    ThreadManager.h \
    DataType.h

