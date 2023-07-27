#ifndef QTINFORMATIONSTORAGE_H
#define QTINFORMATIONSTORAGE_H


#include <QString>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>

class QTInformationStorage
{
public:
    QTInformationStorage();
    QTInformationStorage(const QString &filename, const QString &timestampFormat = "yyyy-MM-dd hh:mm:ss");
    void add(const QString &type, const QString &content);
    void removeAt(int index);
    QString getInfo(int index) const;
    QString getType(int index) const;
    int size() const;

private:
    QString filename_;
    QString timestampFormat_;
};

#endif // QTINFORMATIONSTORAGE_H
