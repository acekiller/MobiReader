#include "PdbHeader.h"

PdbHeader::PdbHeader(QObject *parent)
: QObject(parent), m_flags(0), m_numOfRecords(0) {
}

bool PdbHeader::read(QDataStream &stream) {
    QByteArray buffer;
    quint32 offset = 0;
    buffer.resize(32);

    if(32 != stream.readRawData(buffer.data(), 32)) // offet: 32
        return false;
    m_bookName = buffer;

    stream >> m_flags;

    if(26 != stream.skipRawData(26)) // offset: 60
        return false;

    buffer.resize(9);
    if( 8 != stream.readRawData(buffer.data(), 8)) // offset: 68
        return false;
    buffer[8] = 0;
    m_id = buffer;

    if(8 != stream.skipRawData(8)) // offset: 76
        return false;

    stream >> m_numOfRecords; // offset: 78

    for(int i = 0; i < m_numOfRecords; ++i) {
        stream >> offset;
        if(4 != stream.skipRawData(4))
            return false;

        m_recordsOffsets.append(offset);
    }

    return true;
}
