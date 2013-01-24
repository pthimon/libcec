QT += network

HEADERS += lirc.h
HEADERS += myth.h
HEADERS += server.h
HEADERS += conn.h

SOURCES += lirc.cpp
SOURCES += myth.cpp
SOURCES += server.cpp
SOURCES += conn.cpp
SOURCES += main.cpp

TARGET = cec-lirc-client

LIBS += -lrt -ldl -lpthread -llirc_client

INCLUDEPATH += ../../include
INCLUDEPATH += ../
