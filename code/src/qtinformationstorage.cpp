#include "qtinformationstorage.h"

QTInformationStorage::QTInformationStorage()
    : filename_(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/copypinner/copypinner.cache"), timestampFormat_("yyyy-MM-dd hh:mm:ss")
{
}


QTInformationStorage::QTInformationStorage(const QString &filename, const QString &timestampFormat)
    : filename_(filename), timestampFormat_(timestampFormat)
{
}

void QTInformationStorage::add(const QString &type, const QString &content)
{
    QFile file(filename_);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        QDateTime timestamp = QDateTime::currentDateTime();
        out << "RowPrefix##" << timestamp.toString(timestampFormat_) << "##custom_separator##" << type << "##custom_separator##" << content << "##RowSuffix\n";
        file.close();
    }
}

QString QTInformationStorage::getInfo(int index) const
{
    QFile file(filename_);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        QStringList lines = content.split("##RowSuffix\n", Qt::SkipEmptyParts);
        if (index >= 0 && index < lines.size()) {
            QString line = lines.at(index);
            int start = line.lastIndexOf("##custom_separator##") + QString("##custom_separator##").size();
            int end = line.size();
            if (start != -1 && end != -1) {
                return line.mid(start, end - start); // Return information content
            }
        }
        file.close();
    }
    return QString();
}



QString QTInformationStorage::getType(int index) const
{
    QFile file(filename_);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        QStringList lines = content.split("##RowSuffix\n", Qt::SkipEmptyParts);
        if (index >= 0 && index < lines.size()) {
            QString line = lines.at(index);
            int start = line.indexOf("##custom_separator##") + QString("##custom_separator##").size();
            int end = line.lastIndexOf("##custom_separator##");
            if (start != -1 && end != -1) {
                return line.mid(start, end - start); // Return information type
            }
        }
        file.close();
    }
    return QString();
}


int QTInformationStorage::size() const
{
    QFile file(filename_);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        QStringList lines = content.split("##RowSuffix\n", Qt::SkipEmptyParts);
        file.close();
        return lines.size();
    }
    return 0;
}
