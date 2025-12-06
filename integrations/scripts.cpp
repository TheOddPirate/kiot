// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core.h"
#include "entities/entities.h"
#include <QAction>
#include <KProcess>

#include <KConfigGroup>
void registerScripts()
{
    qInfo() << "Loading scripts";
    auto scriptConfigToplevel = KSharedConfig::openConfig()->group("Scripts");
    const QStringList scriptIds = scriptConfigToplevel.groupList();
    // TODO make sure this is the best way to support input variables
    Textbox *textb = nullptr;

    for (const QString &scriptId : scriptIds) {
        auto scriptConfig = scriptConfigToplevel.group(scriptId);
        const QString name = scriptConfig.readEntry("Name", scriptId);
        const QString exec = scriptConfig.readEntry("Exec");

        if (exec.isEmpty()) {
            qWarning() << "Could not find script Exec entry for" << scriptId;
            continue;
        }
        //Creates a shared textbox for input variables to script only if user has defined {arg} in the Exec line
        if (exec.contains("{arg}") && textb == nullptr) {
            textb = new Textbox(qApp);
            textb->setId("scripts_arguments");
            textb->setName("arguments");
            textb->setHaIcon("mdi:console");
        }
        auto button = new Button(qApp);
        button->setId(scriptId);
        button->setName(name);
        const QString icon = scriptConfig.readEntry("icon","mdi:script-text");
        button->setHaIcon(icon);
        // Home assistant integration supports payloads, which we could expose as args
        // maybe via some substitution in the exec line
        QObject::connect(button, &Button::triggered, qApp, [exec, scriptId,textb]() {
            qInfo() << "Running script " << scriptId;
            // TODO make this better
            QString ex = exec;
            if(exec.contains("{arg}"))
            {

                qDebug() << "customizing exec \""<< ex << "\" to: \""  << ex.replace("{arg}",textb->state()) << "\"";
                ex = ex.replace("{arg}",textb->state());
                textb->setState("");
            }
            // DAVE TODO flatpak escaping
            KProcess *p = new KProcess();
            p->setShellCommand(ex);
            p->startDetached();
            delete p;
        });
    }
}
REGISTER_INTEGRATION("Scripts",registerScripts,true)
