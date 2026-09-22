// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "VdfParser.h"
#include <QRegularExpression>

QVariantMap VdfParser::parseFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QVariantMap();
    }
    
    QTextStream stream(&file);
    return parseBlock(stream);
}

QVariantMap VdfParser::parseString(const QString &content)
{
    QString mutableContent = content;
    QTextStream stream(&mutableContent);
    return parseBlock(stream);
}

QString VdfParser::cleanToken(const QString &token)
{
    QString trimmed = token.trimmed();
    // Fjern hermetegn rundt strenger om de finnes
    if (trimmed.startsWith('"') && trimmed.endsWith('"') && trimmed.length() >= 2) {
        trimmed = trimmed.mid(1, trimmed.length() - 2);
    }
    return trimmed;
}

QVariantMap VdfParser::parseBlock(QTextStream &stream)
{
    QVariantMap result;
    QString keyToken = "";

    // Regex for å hente ut enten "siterte strenger" eller vanlige ord
    static const QRegularExpression tokenRegex(R"("(?:[^"\\]|\\.)*"|\S+)");

    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        // Hopp over tomme linjer og C++ style kommentarer
        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }

        // Hvis vi treffer en lukke-brakett, er denne seksjonen/blokken ferdig
        if (line == "}") {
            break;
        }

        // Hent alle tokens på linjen (f.eks: `"appid"` og `"1623730"`)
        QRegularExpressionMatchIterator it = tokenRegex.globalMatch(line);
        QStringList tokens;
        while (it.hasNext()) {
            tokens.append(cleanToken(it.next().captured(0)));
        }

        if (tokens.isEmpty()) {
            continue;
        }

        // Tilfelle 1: Starten på en ny underblokk (f.eks "AppState" { eller InstalledDepots {)
        if (tokens.size() == 1) {
            if (tokens[0] == "{") {
                // Vi har allerede nøkkelen (f.eks "appstate"), parse underseksjonen rekursivt!
                if (!keyToken.isEmpty()) {
                    result[keyToken.toLower()] = parseBlock(stream);
                    keyToken.clear();
                }
            } else {
                // Dette er nøkkelen til en innkommende blokk eller verdi
                keyToken = tokens[0];
            }
        }
        // Tilfelle 2: Nøkkel og Verdi på samme linje (f.eks: "appid" "1623730")
        else if (tokens.size() >= 2) {
            QString key = tokens[0].toLower();   // Gjør nøkkelen case-insensitive
            QString value = tokens[1];

            // Sjekk om neste token på samme linje var en '{' (f.eks "InstalledDepots" {)
            if (value == "{") {
                result[key] = parseBlock(stream);
            } else {
                result[key] = value;
            }
            keyToken.clear();
        }
    }

    return result;
}