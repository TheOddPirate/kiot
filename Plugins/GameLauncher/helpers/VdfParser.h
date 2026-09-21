#pragma once

#include <QString>
#include <QVariantMap>
#include <QTextStream>
#include <QFile>

class VdfParser
{
public:

    static QVariantMap parseFile(const QString &filePath);


    static QVariantMap parseString(const QString &content);

private:

    static QVariantMap parseBlock(QTextStream &stream);


    static QString cleanToken(const QString &token);
};