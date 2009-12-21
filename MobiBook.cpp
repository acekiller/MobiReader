#include <QString>

#include "MobiBook.h"

MobiBook::MobiBook(QObject *parent) :
    QObject(parent) {
}

MobiBook::MobiBook(QString fileName, QObject *parent)
: QObject(parent) {
    readBook(fileName);
}

bool MobiBook::readBook(QString fileName) {
    file.setFileName(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    stream.setDevice(&file);

    QByteArray name;
    name.resize(32);

    if(32 != stream.readRawData(name.data(), 32))
        return false;

    return true;
}
