// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core.h"
#include "entities/entities.h"
#include <QAction>

#include <KProcess>
#include <KConfigGroup>
#include <KSandbox>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(Scripts)
Q_LOGGING_CATEGORY(Scripts, "integration.Scripts")
void registerScripts()
{
    auto scriptConfigToplevel = KSharedConfig::openConfig()->group("Scripts");
    const QStringList scriptIds = scriptConfigToplevel.groupList();
    // TODO make sure this is the best way to support input variables
    Textbox *textb = nullptr;

    for (const QString &scriptId : scriptIds) {
        auto scriptConfig = scriptConfigToplevel.group(scriptId);
        const QString name = scriptConfig.readEntry("Name", scriptId);
        const QString exec = scriptConfig.readEntry("Exec");

        if (exec.isEmpty()) {
            qCWarning(Scripts) << "Could not find script Exec entry for" << scriptId;
            continue;
        }
        //Creates a shared textbox for input variables to script only if user has defined {arg} in the Exec line
        if (exec.contains("{arg}") && textb == nullptr) {
            textb = new Textbox(qApp);
            textb->setId("scripts_arguments");
            textb->setName("arguments");
            textb->setDiscoveryConfig("icon", "mdi:console");
        }
        auto button = new Button(qApp);
        button->setId(scriptId);
        button->setName(name);
        //Custom icon if set in config defaults to script icon
        QString icon = scriptConfig.readEntry("icon","mdi:script-text");
        button->setDiscoveryConfig("icon", icon);
        // Home assistant integration supports payloads, which we could expose as args
        // maybe via some substitution in the exec line
        // I tried to include some substitution and support for custom icons, it it okay?
        QObject::connect(button, &Button::triggered, qApp, [exec, scriptId,textb]() {
            // TODO make this better
            QString ex = exec;
            if(exec.contains("{arg}")  && textb != nullptr)
            {
                ex = ex.replace("{arg}",textb->state());
                textb->setState(""); //Clears the textbox after use, should it be kept?
            }
            qCInfo(Scripts) << "Running script " << scriptId << " with command " << ex;
            QStringList args = QProcess::splitCommand(ex);  
            if (exec.isEmpty()) {
               qCWarning(scripts) << "Could not find script Exec entry for" << scriptId;
                return;
            }
            QString program = args.takeFirst();           

            KProcess *p = new KProcess();
            p->setProgram(program);
            p->setArguments(args);

            if (KSandbox::isFlatpak()) {
                KSandbox::ProcessContext ctx = KSandbox::makeHostContext(*p);
                p->setProgram(ctx.program);
                p->setArguments(ctx.arguments);
            }

            p->startDetached();
            delete p;
        });
    }
    if( scriptIds.length() >= 1 )
        qCInfo(Scripts) << "Loaded" << scriptIds.length() << " scripts:" << scriptIds.join(", ");
}
REGISTER_INTEGRATION("Scripts",registerScripts,true)
