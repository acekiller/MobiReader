#include <QMessageBox>
#include <QString>

#include "MobiBook.h"

MobiBook::MobiBook(QObject *parent)
: QObject(parent), m_compression(0), m_wholeTextLen(0), m_numOfBookRecords(0),
m_maxRecordSize(0) {
}

MobiBook::MobiBook(QString fileName, QObject *parent)
: QObject(parent), m_compression(0) {
    readBook(fileName);
}

bool MobiBook::readBook(QString fileName) {
    m_file.setFileName(fileName);
    if(!m_file.open(QIODevice::ReadOnly))
        return false;

    m_stream.setDevice(&m_file);

    m_header.read(m_stream);
    m_file.seek(m_header.recordsOffsets()[0]);
    m_stream >> m_compression;
    m_stream.skipRawData(2);

    m_stream >> m_wholeTextLen;
    m_stream >> m_numOfBookRecords;
    m_stream >> m_maxRecordSize;

    if(m_header.id() == "BOOKMOBI") {
        quint16 encrypted;
        m_stream >> encrypted;
        if(encrypted) {
            QMessageBox::critical(0, tr("DRM error"), tr("DRM-encrypted books are not supported"));
        }
    }

    m_file.close();

    return true;
}
