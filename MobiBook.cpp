#include <QMessageBox>
#include <QString>

#include "MobiBook.h"

static unsigned char TOKEN_CODE[256] = {
        0, 1, 1, 1,		1, 1, 1, 1,		1, 0, 0, 0,		0, 0, 0, 0,
        0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,
        0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,
        0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,
        0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,
        0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,
        0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,
        0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,		0, 0, 0, 0,
        3, 3, 3, 3,		3, 3, 3, 3,		3, 3, 3, 3,		3, 3, 3, 3,
        3, 3, 3, 3,		3, 3, 3, 3,		3, 3, 3, 3,		3, 3, 3, 3,
        3, 3, 3, 3,		3, 3, 3, 3,		3, 3, 3, 3,		3, 3, 3, 3,
        3, 3, 3, 3,		3, 3, 3, 3,		3, 3, 3, 3,		3, 3, 3, 3,
        2, 2, 2, 2,		2, 2, 2, 2,		2, 2, 2, 2,		2, 2, 2, 2,
        2, 2, 2, 2,		2, 2, 2, 2,		2, 2, 2, 2,		2, 2, 2, 2,
        2, 2, 2, 2,		2, 2, 2, 2,		2, 2, 2, 2,		2, 2, 2, 2,
        2, 2, 2, 2,		2, 2, 2, 2,		2, 2, 2, 2,		2, 2, 2, 2,
};


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

bool MobiBook::docDecompress(const QByteArray compressedBuffer) {
    QByteArray buffer;
    buffer.resize(m_maxRecordSize * 2);

    const char *srcBuff = compressedBuffer.data();
    const char *srcBuffStart = srcBuff;
    int srcBuffSize = compressedBuffer.size();
    const char *srcBuffEnd = srcBuff + compressedBuffer.size() - 1; // last byte of src buffer
    char *dstBuff = buffer.data();
    char *dstBuffStart = dstBuff;
    int dstBuffSize = buffer.size();
    char * dstBuffEnd = dstBuff + buffer.size() - 1; // last byte of dst buffer
    char token;
    while(srcBuff < srcBuffEnd) {
        token = *(srcBuff++);

        switch(TOKEN_CODE[(unsigned char)token]) {
        case 0:
            *(dstBuff++) = token;
            break;

        case 1:
            if((srcBuff + token > srcBuffEnd) || (dstBuff + token > dstBuffEnd))
                return setErrorCode(COMPRESSION_ERROR);

            memcpy(dstBuff, srcBuff, token);
            srcBuff += token;
            dstBuff += token;
            break;

        case 2:
            if(dstBuff + 1 > dstBuffEnd)
                return setErrorCode(COMPRESSION_ERROR);

            *(dstBuff++) = ' ';
            *(dstBuff++) = 0x80 ^ token;
            break;

        case 3:
            if(srcBuff + 1 > srcBuffEnd)
                return setErrorCode(COMPRESSION_ERROR);

            quint16 n = 0x100 * token + *(unsigned char *)(srcBuff++);
            quint16 copyLen = (n & 7) + 3;
            if(dstBuff + copyLen > dstBuffEnd)
                return setErrorCode(COMPRESSION_ERROR);

            quint16 shift = (n & 0x3fff) / 8;
            char *shiftedSrcdBuff = dstBuff - shift;
            memcpy(dstBuff, shiftedSrcdBuff, copyLen);
            dstBuff += copyLen;
            break;
        }
    }

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

    buffer.resize(m_maxRecordSize);

    if(size != m_stream.readRawData(buffer.data(), size))
        return setErrorCode(RECORD_PROCCESS_ERROR);

    switch(m_compression) {
    case 1:
        // No encryption, no compression
        m_bookText += buffer;
        break;

    case 2:
        // Doc compression, no encryption
        return docDecompress(buffer);
        break;

    case 17480:
        // HuffCDic
        return setErrorCode(HUFFCDIC_NOT_YET_SUPPORTED);

    default:
        return setErrorCode(UNK_COMPRESSION_SCHEMA_ERROR);

    }

    return true;
}
