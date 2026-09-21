#pragma once
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QFile>
#include <QDataStream>

class BinaryVdfParser {
public:
    static QVariantMap parseShortcutsFile(const QString &filePath);
    static bool writeShortcutsFile(const QString &filePath, const QVariantMap &data);

private:
    static QString readNullTerminatedString(QDataStream &stream);
    static QVariantMap parseMap(QDataStream &stream);
};