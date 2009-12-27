#include <QMessageBox>
#include <QString>

#include "MobiBook.h"

MobiBook::MobiBook(QObject *parent)
: QObject(parent), m_error(NO_ERROR), m_compression(0), m_wholeTextLen(0),
m_numOfBookRecords(0), m_maxRecordSize(0) {
}

MobiBook::MobiBook(QString fileName, QObject *parent)
: QObject(parent), m_compression(0) {
    readBook(fileName);
}

MobiBook::~MobiBook() {
    m_file.close();
}

bool MobiBook::readBook(QString fileName) {
    m_file.setFileName(fileName);
    if(!m_file.open(QIODevice::ReadOnly))
        return setErrorCode(OPEN_FILE_ERROR);

    m_stream.setDevice(&m_file);

    if(!m_header.read(m_stream))
        return setErrorCode(HEADER_READ_ERROR);

    if(!processZeroRecord())
        return false;

    for(int i = 0; i < m_numOfBookRecords; ++i) {
        if(!processRecord(i)) {
            return false;
        }
    }

    return true;
}

bool MobiBook::docDecompress(const QByteArray &compressedBufferArray) {
    const int buffSize = m_maxRecordSize * 2;
    const int compressedBuffSize = compressedBufferArray.size();
    QByteArray buffer;
    buffer.resize(buffSize);
    unsigned char token;
    int srcIdx = 0;
    int dstIdx = 0;

    while(srcIdx < compressedBuffSize) {
        token = compressedBufferArray.at(srcIdx++);

        if(0 == token || (8 < token && token < 128)) {
            buffer[dstIdx++] = token;
        } else if(0 < token && token < 9) {
            if(srcIdx + token > compressedBuffSize)
                goto end_of_processing;

            for(unsigned short int i = 0; i < token; ++i)
                buffer[dstIdx++] = compressedBufferArray.at(srcIdx++);
        } else if(192 <= token) {
            buffer[dstIdx++] = ' ';
            buffer[dstIdx++] = 0x80 ^ token;
        } else {
            if(srcIdx + 1 > compressedBuffSize)
                goto end_of_processing;

            quint16 n = 0x100 * token + (unsigned char)compressedBufferArray.at(srcIdx++);
            quint16 copyLen = (n & 7) + 3;

            quint16 shift = (n & 0x3fff) / 8;
            int shiftedIdx = dstIdx - shift;

            for(int tmpIdx = dstIdx; tmpIdx < shiftedIdx + 2 * copyLen; ++tmpIdx)
                buffer[tmpIdx] = 0;

            for(quint16 i = 0; i < copyLen; ++i) {
                    buffer[dstIdx++] = buffer.at(shiftedIdx++);
            }

        }
    }

    // Truncate any possible additional bytes allocated by [] operator
    end_of_processing:    buffer.resize(dstIdx);
    m_bookText += buffer;

    return true;
}

inline bool MobiBook::setErrorCode(MobiBookError error) {
    m_error = error;
    return false;
}

bool MobiBook::processZeroRecord() {
    m_file.seek(m_header.recordsOffsets()[0]);
    m_stream >> m_compression;
    m_stream.skipRawData(2);

    m_stream >> m_wholeTextLen;
    m_stream >> m_numOfBookRecords;
    m_stream >> m_maxRecordSize;

    if(m_header.id() == "BOOKMOBI") {
        quint16 encrypted;
        m_stream >> encrypted;
        if(encrypted)
            return setErrorCode(DRM_ERROR);
    }

    return true;
}

bool MobiBook::processRecord(int recordIndex) {
    if(!m_file.seek(m_header.recordsOffsets()[recordIndex + 1])) {
        return setErrorCode(RECORD_PROCCESS_ERROR);
    }

    QByteArray buffer;

    quint16 size = m_maxRecordSize;
    if(recordIndex < m_header.recordsOffsets().size())
        size = m_header.recordsOffsets()[recordIndex + 2] - m_header.recordsOffsets()[recordIndex + 1];

    buffer.resize(size);

    if(size != m_file.read(buffer.data(), size))
        return setErrorCode(RECORD_PROCCESS_ERROR);

    switch(m_compression) {
    case 1:
        // No encryption, no compression
        m_bookText += buffer;
        break;

    case 2:
        // Doc compression, no encryption
        docDecompress(buffer);
        break;

    case 17480:
        // HuffCDic
        return setErrorCode(HUFFCDIC_NOT_YET_SUPPORTED);

    default:
        return setErrorCode(UNK_COMPRESSION_SCHEMA_ERROR);

    }

    return true;
}
