#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDebug>

#include "KIOTShared/kiotshared_export.h"

namespace KIOTShared {

class KIOT_SHARED_EXPORT KiotSharedTranslatorInit {
public:

    KiotSharedTranslatorInit() {


        static QTranslator sharedTranslator;
        QString locale = QLocale::system().name(); // F.eks. "fr_FR" eller "nb_NO"
        qDebug() << "test da for faen";
        // Prøv å laste fra shared lib sin ressurs-prefix (f.eks. ":/kiotshared/translations")
        // Den vil lete etter filer som heter f.eks. "kiotshared_fr.qm"
        if (sharedTranslator.load("kiotshared_fr", QStringLiteral(":/kiotshared/translations"))) {
             qDebug() << "test da for faen";
            QCoreApplication::installTranslator(&sharedTranslator);
        } else {
            // Fallback til kun språkkode hvis full locale feiler (f.eks. "fr")
            QString shortLocale = locale.section('_', 0, 0);
            if (sharedTranslator.load("kiotshared_fr" , QStringLiteral(":/kiotshared/translations"))) {
                 qDebug() << "test da for faen";
                QCoreApplication::installTranslator(&sharedTranslator);
            }
        }
    }
};

// Dette objektet opprettes automatisk i det biblioteket lastes inn!
static KiotSharedTranslatorInit initializer;
}
