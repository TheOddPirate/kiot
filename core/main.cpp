#include "core.h"
#include "logging/messagehandler.h"
#include "ui_qt/mainwindow.h"
#include <KIOTShared/kiotshared.h>
#include <QApplication>
#include <QTranslator>
#include <csignal>

#include <KAboutData>
#include <KDBusService>
#include <KSignalHandler>

DEFINE_LOGGER(main_cpp, Core.Main)
/**
 * @brief Main entry point for the kiot application
 * @param argc Argument count
 * @param argv Argument vector
 * @return Application exit code
 * 
 * Initializes the Qt application, sets up custom logging, handles KDE
 * integration, and watches for termination signals (SIGTERM, SIGINT).
 */
int main(int argc, char **argv)
{
    QApplication::setDesktopFileName(PlatformHelper::generateServiceName());
    QApplication::setApplicationName(QStringLiteral(PROJECT_NAME));
    QApplication::setApplicationVersion(QStringLiteral(PROJECT_VERSION));
  //  QApplication::setOrganizationName(QStringLiteral(PROJECT_NAME));
    QString domain = PlatformHelper::resolveOrganizationDomain(QStringLiteral(PROJECT_DOMAIN));
    QApplication::setOrganizationDomain( domain);
    QApplication app(argc, argv);


    QTranslator translator;
    QTranslator libTranslator;

    QString locale = QLocale::system().name(); // F.eks. "nb_NO" eller "nn_NO"
    
    // Prøver å laste inn en .qm-fil fra Qt sin ressursfil (f.eks. ":/i18n/kiot_nb_NO.qm")
    if (translator.load("kiot_fr" , QStringLiteral(":/translations"))) {
        QCoreApplication::installTranslator(&translator);
    }

    QString libPath = "/mnt/Development/Clones/kiot/build/Shared/"; 
    if (libTranslator.load("kiotshared_fr",   QStringLiteral(":/kiotshared/translations"))) {
        QCoreApplication::installTranslator(&libTranslator);
    }else {
        qWarning() << "Failed to load library translations";
    }

    initLogging();
    
    KAboutData aboutData(
        QStringLiteral(PROJECT_NAME),
        "KDE IOT",
        QStringLiteral(PROJECT_VERSION),
        QStringLiteral(PROJECT_DESCRIPTION),
        KAboutLicense::GPL_V3,
        "© 2024-"+QStringLiteral(CURRENT_YEAR)
    );
    
    KDBusService service(KDBusService::Unique | KDBusService::Replace);
    //qCInfo(main_cpp) << QCoreApplication::translate("Starting") << PROJECT_NAME << QCoreApplication::translate("version:") << PROJECT_VERSION;
    qCInfo(main_cpp) << QCoreApplication::translate("main", "Starting") 
                     << PROJECT_NAME 
                     << QCoreApplication::translate("main", "version:") 
                     << PROJECT_VERSION;
    
    MainWindow mainWindow;
    HaControl appControl;

    KSignalHandler::self()->watchSignal(SIGTERM);
    KSignalHandler::self()->watchSignal(SIGINT);
    QObject::connect(KSignalHandler::self(), &KSignalHandler::signalReceived, [](int sig) {
        if (sig == SIGTERM || sig == SIGINT) {
            qCInfo(main_cpp) << QCoreApplication::translate("main", "Shutting down") << QStringLiteral(PROJECT_NAME);
            QApplication::quit();
        }
    });

    return app.exec();
}
// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later
