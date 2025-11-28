//This is just a very early poc for a bluetooth integration
//right now its just a switch to turn the adaper on/off
//Todo check for devices connected by bluetooth and create a sensor/switch for it with attributes showing battery and such
//todo implement cusom config to let the user decide what devices should be exposed to home assistant
#include "core.h"
#include <QCoreApplication>
#include <BluezQt/Manager>
#include <BluezQt/Adapter>
#include <BluezQt/InitManagerJob>

class BluetoothAdapterWatcher : public QObject
{
    Q_OBJECT

public:
    explicit BluetoothAdapterWatcher(QObject *parent = nullptr)
        : QObject(parent)
    {
        m_switch = new Switch(this);
        m_switch->setId("bluetooth_adapter");
        m_switch->setName("Bluetooth Adapter");
        m_manager = new BluezQt::Manager(this);

        // Lag init-jobben
        BluezQt::InitManagerJob *job = m_manager->init();
        connect(m_switch, &Switch::stateChangeRequested, this, [this](bool requestedState){
            auto adapters = m_manager->adapters(); // Hent powered adapter
            if (!adapters.isEmpty()) {
                auto adapter = adapters.first();
            if (requestedState) {
                adapter->setPowered(true);  // Skru på
                qDebug() << "Turned on bluetooth adaper: " << adapter->name();
            } else {
                adapter->setPowered(false); // Skru av
                qDebug() << "Turned off bluetooth adaper: " << adapter->name();
            }
            }
        });
        // Koble signal for når init er ferdig
        connect(job, &BluezQt::InitManagerJob::result, this, [this, job]() {
            if (job->error()) {
                qWarning() << "Bluez init failed:" << job->errorText();
                m_switch->setState(false);
                return;
            }

auto adapters = job->manager()->adapters();
if (!adapters.isEmpty()) {
    auto adapter = adapters.first(); // Hent første adapter uansett powered state
    qDebug() << "Adapter:" << adapter->name() << "Powered:" << adapter->isPowered();
    m_switch->setState(adapter->isPowered());
    
    connect(adapter.data(), &BluezQt::Adapter::poweredChanged, this, [this](bool powered){
        m_switch->setState(powered);
    });
} else {
    qWarning() << "No adapters found at all";
    m_switch->setState(false);
}
        });

        job->start();
    }

    
private:
    Switch *m_switch;
    BluezQt::Manager *m_manager;
};

void setupBluetoothAdapter()
{
    new BluetoothAdapterWatcher(qApp);
}

REGISTER_INTEGRATION("BluetoothAdapter", setupBluetoothAdapter, true)

#include "bluetooth.moc"
