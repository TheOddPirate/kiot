// SPDX-FileCopyrightText: David Edmundson <davidedmundson@kde.org>
// SPDX-FileCopyrightText: 2025-2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
// All credits to david for this integration, without the work he put into Kgamma2 i would not have the blueprint on howto use KScreen
// https://invent.kde.org/davidedmundson/kgamma2

#include "Shared/Transport/transportmanager.h"
#include "core/core.h"
#include "Shared/entities/select.h"
#include "Shared/entities/switch.h"
#include "Shared/entities/number.h"
#include "Shared/platformhelper.h"

#include <QObject>
#include <QTimer>
#include <cstdlib>
#include <kscreen/config.h>
#include <kscreen/configoperation.h>
#include <kscreen/getconfigoperation.h>
#include <kscreen/output.h>
#include <kscreen/setconfigoperation.h>
#include <kscreen/configmonitor.h>
#include <kscreen/types.h>
#include <kscreen/mode.h>

DEFINE_LOGGER(screen_controller, Integrations.ScreenController)

/**
 * @class ScreenController
 * @brief Display controller integration for KIOT
 */
class ScreenController : public QObject
{
    Q_OBJECT

public:
    explicit ScreenController(QObject *parent = nullptr)
        : QObject(parent)
        , m_select(nullptr)
        , m_selectResolution(nullptr)
        , m_switchEnabled(nullptr)
        , m_switchPrimary(nullptr)
        , m_brightness(nullptr)
        , m_zoomScale(nullptr)
    {
        setup();
        loadConfigFromSystem();

        connect(KScreen::ConfigMonitor::instance(), &KScreen::ConfigMonitor::configurationChanged, 
                this, &ScreenController::loadConfigFromSystem);
    }

    void setup()
    {
        m_select = new Select(this);
        m_select->setId("displaylist");
        m_select->setName("Displays");
        m_select->setDiscoveryConfig("icon", "mdi:application-cog");

        QObject::connect(this, &ScreenController::screenNamesChanged, this, &ScreenController::onScreenNamesChanged);
        QObject::connect(m_select, &Select::optionSelected, this, &ScreenController::onOptionSelected);

        // Resolution select entity
        m_selectResolution = new Select(this);
        m_selectResolution->setId("displaylist_resolution");
        m_selectResolution->setName("Display Resolution");
        m_selectResolution->setDiscoveryConfig("icon", "mdi:monitor-screenshot");
        QObject::connect(m_selectResolution, &Select::optionSelected, this, &ScreenController::onResolutionSelected);

        // Enabled/disabled switch
        m_switchEnabled = new Switch(this);
        m_switchEnabled->setId("displaylist_enabled");
        m_switchEnabled->setName("Display Enabled");
        QObject::connect(m_switchEnabled, &Switch::stateChangeRequested, this, [this](bool state) {
            if (!m_activeoutput)
                return;
            if (m_activeoutput->isEnabled() == state)
                return;
            m_activeoutput->setEnabled(state);
            saveConfig();
        });

        // Primary screen switch
        m_switchPrimary = new Switch(this);
        m_switchPrimary->setId("displaylist_primary");
        m_switchPrimary->setName("Display Primary");
        QObject::connect(m_switchPrimary, &Switch::stateChangeRequested, this, [this](bool state) {
            if (!m_activeoutput || !m_config)
                return;
            if (!state) {
                m_switchPrimary->setState(true);
                return;
            }
            if (m_config->primaryOutput() != m_activeoutput) {
                m_config->setOutputPriority(m_activeoutput, 1);
                saveConfig();
            }
        });

        // Brightness scale
        m_brightness = new Number(this);
        m_brightness->setId("displaylist_brightness");
        m_brightness->setName("Display Brightness");
        m_brightness->setRange(0, 100, 1, "%");
        connect(m_brightness, &Number::valueChangeRequested, this, [this](int level) {
            if (!m_activeoutput)
                return;
            double result = static_cast<double>(level) / 100.0;
            qCDebug(screen_controller) << "Setting brightness to" << result;
            m_activeoutput->setBrightness(result);
            saveConfig();
        });
    
        // Zoom scale
        m_zoomScale = new Number(this);
        m_zoomScale->setId("displaylist_zoom");
        m_zoomScale->setName("Display Zoom");
        m_zoomScale->setRange(50, 300, 1, "%"); // 50% to 300%
        
        connect(m_zoomScale, &Number::valueChangeRequested, this, [this](int level) {
            if (!m_activeoutput)
                return;
            double result = static_cast<double>(level) / 100.0;
            qCDebug(screen_controller) << "Setting Zoom to" << result;
            m_activeoutput->setScale(result);
            saveConfig();
        });
    }

    QStringList screenNames() const
    {
        return m_screenNames;
    }

Q_SIGNALS:
    void screenNamesChanged();
    void lastErrorChanged();

private slots:
    void onOptionSelected(const QString &option)
    {
        if (m_activeoutput) {
            disconnect(m_activeoutput.data(), nullptr, this, nullptr);
        }
        if (m_config) {
            for (auto output : m_config->outputs()) {
                if (QString("%1 (%2)").arg(output->model(), output->name()) != option)
                    continue;
                m_activeoutput = output;
            }
        }
        if (!m_activeoutput)
            return;


        updateState();
    }

    void onResolutionSelected(const QString &option)
    {
        if (!m_activeoutput)
            return;

        QString modeId = m_resolutionMap.value(option);
        if (modeId.isEmpty())
            return;

        if (m_activeoutput->currentModeId() != modeId) {
            qCDebug(screen_controller) << "Setting resolution mode ID:" << modeId << "for option:" << option;
            m_activeoutput->setCurrentModeId(modeId);
            saveConfig();
        }
    }

    void onScreenNamesChanged()
    {
        if (!m_select)
            return;
        qCDebug(screen_controller) << "Screen names changed to:" << screenNames();
        m_select->setOptions(m_screenNames);
    
        if (!m_activeoutput && !m_screenNames.isEmpty()) {
            QTimer::singleShot(100, this, [this]() {
                m_select->setState(m_screenNames.first());
                onOptionSelected(m_screenNames.first());
            });
        }
    }

    void onPriorityChanged()
    {
        if (!m_switchPrimary || !m_activeoutput)
            return;
        qCDebug(screen_controller) << "Primary screen changed.";
        bool isPrimary = (m_activeoutput == m_config->primaryOutput());
        if (m_switchPrimary->state() == isPrimary)
            return;
        m_switchPrimary->setState(isPrimary);
    }

private:
    void loadConfigFromSystem()
    {
        auto *operation = new KScreen::GetConfigOperation(KScreen::ConfigOperation::NoOptions, this);
        connect(operation, &KScreen::ConfigOperation::finished, this, [this, operation] {
            if (m_config) {
                disconnect(m_config.data(), nullptr, this, nullptr);
                KScreen::ConfigMonitor::instance()->removeConfig(m_config);
            }

            m_config = operation->config();
            
            if (m_config) {
                KScreen::ConfigMonitor::instance()->addConfig(m_config);
                connect(m_config.data(), &KScreen::Config::prioritiesChanged, this, &ScreenController::onPriorityChanged);
            }

            if (m_activeoutput) {
                int activeId = m_activeoutput->id();
                m_activeoutput.clear();
                for (auto output : m_config->outputs()) {
                    if (output->id() == activeId) {
                        m_activeoutput = output;
                        break;
                    }
                }
                if (m_activeoutput) {
                    QObject::connect(m_activeoutput.data(), &KScreen::Output::isEnabledChanged, this, &ScreenController::updateState);
                    QObject::connect(m_activeoutput.data(), &KScreen::Output::brightnessChanged, this, &ScreenController::updateState);
                    QObject::connect(m_activeoutput.data(), &KScreen::Output::scaleChanged, this, &ScreenController::updateState);
                    QObject::connect(m_activeoutput.data(), &KScreen::Output::currentModeIdChanged, this, &ScreenController::updateState);
                }
            }

            refreshScreenNames();
            updateState();
        });
    }

    void updateState()
    {
        if (!m_activeoutput || !m_switchEnabled || !m_switchPrimary || !m_brightness || !m_zoomScale || !m_selectResolution || !m_config)
            return;
            
        m_switchEnabled->setState(m_activeoutput->isEnabled());
        m_switchPrimary->setState(m_activeoutput == m_config->primaryOutput());
        m_brightness->setValue(static_cast<int>(m_activeoutput->brightness() * 100));
        m_zoomScale->setValue(static_cast<int>(m_activeoutput->scale() * 100));

        m_resolutionMap.clear();
        QStringList modeOptions;
        auto modes = m_activeoutput->modes();
        
        for (auto mode : modes) {
            QString key = mode->id();
            QString value = mode->name(); // F.eks. "1920x1080@60Hz"
            
            if (!modeOptions.contains(value)) {
                modeOptions.append(value);
                m_resolutionMap.insert(value, key);
            }
        }
        
        m_selectResolution->setOptions(modeOptions);

        QString currentModeId = m_activeoutput->currentModeId();
        for (auto it = m_resolutionMap.begin(); it != m_resolutionMap.end(); ++it) {
            if (it.value() == currentModeId) {
                m_selectResolution->setState(it.key());
                break;
            }
        }

        QVariantMap attributes;
        attributes["model"] = m_activeoutput->model();
        attributes["name"] = m_activeoutput->name();
        attributes["id"] = m_activeoutput->id();
        attributes["vendor"] = m_activeoutput->vendor();
        attributes["HDR"] = m_activeoutput->isHdrEnabled();
        attributes["enabled"] = m_activeoutput->isEnabled();
        attributes["primary"] = (m_activeoutput == m_config->primaryOutput());
        attributes["current_resolution_id"] = currentModeId;
        
        m_select->setAttributes(attributes);
    }

    void saveConfig()
    {
        if (!m_config || !m_activeoutput)
            return;

        KScreen::ConfigPtr updatedConfig = m_config->clone();
        
        auto *operation = new KScreen::SetConfigOperation(updatedConfig, this);
        connect(operation, &KScreen::ConfigOperation::finished, this, [this, operation] {
            if (operation->hasError()) {
                qCWarning(screen_controller) << operation->errorString();
                return;
            }
            qCDebug(screen_controller) << "Screen Config saved successfully";
            loadConfigFromSystem();
        });
    }

    void refreshScreenNames()
    {
        QStringList names;
        m_screenIds.clear();

        if (m_config) {
            for (auto output : m_config->outputs()) {
                names.append(QString("%1 (%2)").arg(output->model(), output->name()));
                m_screenIds.append(output->id());
            }
        }

        if (m_screenNames == names) {
            return;
        }

        m_screenNames = names;
        Q_EMIT screenNamesChanged();
    }

private:
    Select *m_select;
    Select *m_selectResolution;
    Switch *m_switchEnabled;
    Switch *m_switchPrimary;
    Number *m_brightness;
    Number *m_zoomScale;
    KScreen::OutputPtr m_activeoutput;
    KScreen::ConfigPtr m_config;
    QList<int> m_screenIds;
    QStringList m_screenNames;
    QMap<QString, QString> m_resolutionMap; 
};

void setupScreenController()
{
    new ScreenController(qApp);
}

REGISTER_INTEGRATION("ScreenController", setupScreenController, true)

#include "screen_controller.moc"