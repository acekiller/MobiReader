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
    QFile file;
    PdbHeader header;
    PdbStream stream;

};

#endif // MOBIBOOK_H
