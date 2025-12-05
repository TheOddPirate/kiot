#include <QApplication>
#include "core.h"
#include <KAboutData>
#include <KDBusService>
#include <KSignalHandler>
// Is there a way to get the signal directly from

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KAboutData aboutData(QStringLiteral("kiot"), "KDE IOT", QStringLiteral("0.1"), "KDE Internet of Things Connection", KAboutLicense::GPL_V3, "© 2024");
    KDBusService service(KDBusService::Unique);
    HaControl appControl;

    KSignalHandler::self()->watchSignal(2);
    KSignalHandler::self()->watchSignal(15);
    QObject::connect(KSignalHandler::self(), &KSignalHandler::signalReceived, [](int sig) {
        qDebug() << "Got signal" << sig;
        QApplication::quit();
        
    });

    app.exec();
}
// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later
