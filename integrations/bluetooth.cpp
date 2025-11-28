//This is just a very early poc for a bluetooth integration
//right now its just a switch to turn the adaper on/off
//Todo check for devices connected by bluetooth and create a sensor/switch for it with attributes showing battery and such
//todo implement cusom config to let the user decide what devices should be exposed to home assistant
#include "core.h"
#include <QCoreApplication>
#include <BluezQt/Manager>
#include <BluezQt/Adapter>
#include <BluezQt/InitManagerJob>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>

class BluetoothAdapterWatcher : public QObject
{
    Q_OBJECT

public:
    explicit BluetoothAdapterWatcher(QObject *parent = nullptr);


    
private:

    Switch *m_switch = nullptr;
    BluezQt::Manager *m_manager = nullptr;
    BluezQt::AdapterPtr m_adapter;
    bool m_initialized = false;
};

BluetoothAdapterWatcher::BluetoothAdapterWatcher(QObject *parent)
    : QObject(parent)
{
    m_switch = new Switch(this);
    m_switch->setId("bluetooth_adapter");
    m_switch->setName("Bluetooth Adapter");

    m_manager = new BluezQt::Manager(this);

    // Lag init-jobben
    BluezQt::InitManagerJob *job = m_manager->init();

    connect(job, &BluezQt::InitManagerJob::result, this, [this, job]() {
        if (job->error()) {
            qWarning() << "Bluez init failed:" << job->errorText();
            m_switch->setState(false);
            return;
        }

        auto adapters = job->manager()->adapters();
        if (!adapters.isEmpty()) {
            m_adapter = adapters.first(); // Første adapter
            m_initialized = true;

            qDebug() << "Adapter:" << m_adapter->name() << "Powered:" << m_adapter->isPowered();
            m_switch->setState(m_adapter->isPowered());

            connect(m_adapter.data(), &BluezQt::Adapter::poweredChanged, this, [this](bool powered){
                m_switch->setState(powered);
            });

        } else {
            qWarning() << "No adapters found";
            m_switch->setState(false);
        }
    });

    job->start();

    // Koble til stateChangeRequested for HA
    connect(m_switch, &Switch::stateChangeRequested, this, [this](bool requestedState){
        if (!m_initialized || !m_adapter)
            return;

        m_adapter->setPowered(requestedState);
        qDebug() << "Set adapter powered to" << requestedState;
    });
}




// Setup-funksjon som registreres i Kiot
void setupBluetoothAdapter()
{
    new BluetoothAdapterWatcher(qApp);
}

REGISTER_INTEGRATION("BluetoothAdapter", setupBluetoothAdapter, true)

#include "bluetooth.moc"
