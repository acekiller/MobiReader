TEMPLATE = app
TARGET = MobiReader
QT += core \
    gui \
    webkit \
    webkit
HEADERS += MobiReader.h
SOURCES += main.cpp \
    MobiReader.cpp
FORMS += MobiReader.ui
RESOURCES += 
symbian { 
    TARGET.UID3 = 0xE7B59B86
    SOURCES += MobiReader_reg.rss
}
