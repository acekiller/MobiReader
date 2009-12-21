#ifndef PDBHEADER_H
#define PDBHEADER_H

#include <QDataStream>
#include <QObject>
#include <QString>
#include <QVector>

class PdbHeader : public QObject
{
Q_OBJECT
public:
    explicit PdbHeader(QObject *parent = 0);
    const QString &bookName() const { return m_bookName; }
    const QString &id() const { return m_id; }
    bool read(QDataStream &stream);
    const QVector<quint32> &recordsOffsets() const { return m_recordsOffsets; }

signals:

public slots:
    void setBookName(QString newName) { m_bookName = newName; }
    void setId(QString id) { m_id = id; }

private:
    QString m_bookName, m_id;
    quint16 m_flags;
    quint16 m_numOfRecords;
    QVector<quint32> m_recordsOffsets;

};

#endif // PDBHEADER_H
