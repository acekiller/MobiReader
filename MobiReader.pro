TEMPLATE = app
TARGET = MobiReader
symbian {
    QT += core \
        gui
}
QT += webkit
HEADERS += MobiReader.h \
    MobiBook.h \
    PdbHeader.h \
    PdbStream.h
SOURCES += main.cpp \
    MobiReader.cpp \
    MobiBook.cpp \
    PdbHeader.cpp \
    PdbStream.cpp
FORMS += MobiReader.ui
RESOURCES += 
symbian { 
    TARGET.UID3 = 0xE7B59B86
    SOURCES += MobiReader_reg.rss
}
