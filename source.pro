TEMPLATE = app
TARGET = simulator
DEPENDPATH += .
INCLUDEPATH += .

HEADERS += data.h io.h kue-chip2.h
SOURCES += io.c main.c kue-chip2.c
OTHER_FILES+=CMakeLists.txt
