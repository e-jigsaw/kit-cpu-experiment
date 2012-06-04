TEMPLATE = subdirs
SUBDIRS =kue-chip2
TARGET = simulator
DEPENDPATH += .
INCLUDEPATH += .

HEADERS += data.h io.h property.h
SOURCES += io.c main.c property.c
OTHER_FILES+=CMakeLists.txt
