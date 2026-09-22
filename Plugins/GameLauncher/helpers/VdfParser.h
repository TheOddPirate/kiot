// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

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