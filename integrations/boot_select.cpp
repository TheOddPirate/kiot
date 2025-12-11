// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/* THis needs sudo to work for me on manjaro så i dont think this would be okay for kiot?
#include "core.h"
#include "entities/entities.h"
#include <QFile>
#include <QRegularExpression>
#include <QProcess>
#include <QDebug>
class BootSelect : public QObject
{
    Q_OBJECT
public:
    explicit BootSelect(QObject *parent = nullptr)
        : QObject(parent)
    {
        m_select = new Select(this);
        m_select->setId("boot_select");
        m_select->setName("Boot Selector");
        connect(m_select, &Select::optionSelected, this, &BootSelect::rebootToEntry);
        m_select->setOptions(loadOptions());
        m_select->setState(getCurrentEntry());
        
    }


private:
    QStringList m_entries;
    Select *m_select;
    QStringList loadOptions()
    {
        QString grubPath;
        if (QFile::exists("/boot/grub/grub.cfg"))
            grubPath = "/boot/grub/grub.cfg";
        else if (QFile::exists("/boot/grub2/grub.cfg"))
            grubPath = "/boot/grub2/grub.cfg";

        if (grubPath.isEmpty())
        {
            qDebug() << "No grub config found";
            return {};
        }
        QFile file(grubPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open grub config file";
            return  {};
        }
        QRegularExpression menuEntryPattern("^menuentry '([^']*)'");
        QRegularExpression submenuPattern("^submenu '([^']*)'");
        
        while (!file.atEnd()) {
            const QString line = QString::fromUtf8(file.readLine()).trimmed();
            QRegularExpressionMatch match = menuEntryPattern.match(line);
            if (match.hasMatch())
                m_entries.append(match.captured(1));
            else {
                match = submenuPattern.match(line);
                if (match.hasMatch())
                    m_entries.append(match.captured(1));
            }
        }

        return  m_entries;
    }

    QString getCurrentEntry()
    {
        QProcess proc;
        proc.start("grub-editenv", QStringList() << "list");
        proc.waitForFinished(2000);
        QString output = proc.readAllStandardOutput();

        QRegularExpression nextEntryPattern("^next_entry=(\\d+)");
        QRegularExpressionMatch match = nextEntryPattern.match(output);
        int index = 0;
        if (match.hasMatch())
            index = match.captured(1).toInt();

        if (index < m_entries.size())
            return m_entries.at(index);
        return {};
    }

    void rebootToEntry(const QString &option)
    {
        int index = m_entries.indexOf(option);
        if (index < 0)
            return;

        QProcess::execute("sudo", QStringList() << "-n" << "grub-reboot" << QString::number(index));
        m_select->setState(option);
        qDebug() << "Next GRUB boot entry set to:" << option;
    }
};

// Setup integrasjon
void setupBootSelect()
{
    new BootSelect(qApp);
}

REGISTER_INTEGRATION("Boot Select", setupBootSelect, true)

#include "boot_select.moc"
*/
