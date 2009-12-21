#ifndef MOBIBOOK_H
#define MOBIBOOK_H

#include <QObject>
#include <QFile>

#include "PdbHeader.h"
#include "PdbStream.h"

class MobiBook : public QObject
{
Q_OBJECT
public:
    explicit MobiBook(QObject *parent = 0);
    MobiBook(QString fileName, QObject *parent = 0);
    bool readBook(QString fileName);

signals:

public slots:

private:
    QFile m_file;
    PdbHeader m_header;
    PdbStream m_stream;
    quint16 m_compression;
    quint32 m_wholeTextLen;
    quint16 m_numOfBookRecords;
    quint16 m_maxRecordSize;

};

#endif // MOBIBOOK_H
