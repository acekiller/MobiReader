#ifndef MOBIBOOK_H
#define MOBIBOOK_H

#include <QObject>
#include <QFile>

#include "PdbHeader.h"
#include "PdbStream.h"

class MobiBook : public QObject
{
Q_OBJECT
    enum MobiBookError {
        NO_ERROR,
        OPEN_FILE_ERROR,
        HEADER_READ_ERROR,
        RECORD_PROCCESS_ERROR,
        DRM_ERROR,
        HUFFCDIC_NOT_YET_SUPPORTED,
        UNK_COMPRESSION_SCHEMA_ERROR,
        COMPRESSION_ERROR
    };

public:
    explicit MobiBook(QObject *parent = 0);
    ~MobiBook();
    MobiBook(QString fileName, QObject *parent = 0);
    bool readBook(QString fileName);
    bool docDecompress(const QByteArray &compressedBufferArray);
    bool setErrorCode(MobiBookError error);
    const QString &bookText() const { return m_bookText; }
    MobiBookError errorCode() const { return m_error; }

signals:

public slots:

private:
    bool processZeroRecord();
    bool processRecord(int recordIndex);

    MobiBookError m_error;
    QFile m_file;
    PdbHeader m_header;
    PdbStream m_stream;
    quint16 m_compression;
    quint32 m_wholeTextLen;
    quint16 m_numOfBookRecords;
    quint16 m_maxRecordSize;

    quint32 m_mobiHeaderLen;
    quint32 m_mobiDocType;
    quint32 m_mobiDocEncoding;

    QString m_bookText;
};

#endif // MOBIBOOK_H
