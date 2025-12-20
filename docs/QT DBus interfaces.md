# Creating a Qt DBus Interface Step by Step

This guide explains how to create a Qt DBus interface wrapper for a service on the session or system bus. The example uses `org.kde.Solid.PowerManagement.PolicyAgent` as a reference, but the steps are general and can be applied to other services like MPRIS.

---

## Prerequisites

Ensure you have the necessary Qt development tools installed:
```bash
# On Debian/Ubuntu
sudo apt-get install qt6-tools-dev qt6-dbus-dev

# On Fedora/RHEL
sudo dnf install qt6-qttools-devel qt6-qtbase-devel
```

---

## Step 1: Introspect the DBus Service

First, get the XML description of the DBus service:

```bash
qdbus org.kde.Solid.PowerManagement \
  /org/kde/Solid/PowerManagement/PolicyAgent \
  org.freedesktop.DBus.Introspectable.Introspect > org.kde.Solid.PowerManagement.PolicyAgent.xml
```

**Explanation:**
* `org.kde.Solid.PowerManagement` is the service name.
* `/org/kde/Solid/PowerManagement/PolicyAgent` is the object path.
* `org.freedesktop.DBus.Introspectable.Introspect` is the standard introspect method.

**Alternative using `dbus-send`:**
```bash
dbus-send --session --type=method_call \
  --print-reply --dest=org.kde.Solid.PowerManagement \
  /org/kde/Solid/PowerManagement/PolicyAgent \
  org.freedesktop.DBus.Introspectable.Introspect
```

This generates an XML file describing the interface.

---

## Step 2: Generate C++ Proxy Classes

Use `qdbusxml2cpp` to generate C++ files:

```bash
qdbusxml2cpp -c PolicyAgentInterface -p policyagentinterface dbus/org.kde.Solid.PowerManagement.PolicyAgent.xml
```

**Options:**
* `-c PolicyAgentInterface` sets the class name.
* `-p policyagentinterface` sets the prefix for `.h` and `.cpp` files (generates `policyagentinterface.h` and `policyagentinterface.cpp`).
* `-a` to generate adaptor classes (for implementing services, not just clients).
* `-v` for verbose output.

**Generated files contain:**
* A `QDBusAbstractInterface` wrapper with methods, properties, and signals.
* Proper Qt signal/slot connections.
* Type-safe method calls.

---

## Step 3: Include the Interface in Your Project

Copy the generated `.h` and `.cpp` files into your project and include the header:

```cpp
#include "policyagentinterface.h"
```

**CMake configuration:**
```cmake
# Add DBus module
find_package(Qt6 REQUIRED COMPONENTS DBus)

# Add generated files to your target
target_sources(your_target PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/dbus/policyagentinterface.cpp
)

target_link_libraries(your_target PRIVATE
    Qt6::DBus
)
```

**qmake configuration:**
```qmake
QT += dbus
SOURCES += dbus/policyagentinterface.cpp
HEADERS += dbus/policyagentinterface.h
```

---

## Step 4: Create an Instance

Instantiate the interface pointing to the correct service, path, and connection:

```cpp
PolicyAgentInterface *m_policyAgent = new PolicyAgentInterface(
    "org.kde.Solid.PowerManagement",
    "/org/kde/Solid/PowerManagement/PolicyAgent",
    QDBusConnection::sessionBus(),  // or QDBusConnection::systemBus()
    this  // QObject parent for memory management
);
```

**Important considerations:**
* Use `QDBusConnection::sessionBus()` for user session services.
* Use `QDBusConnection::systemBus()` for system-wide services.
* Always check if the service is available:
  ```cpp
  if (!m_policyAgent->isValid()) {
      qWarning() << "Failed to connect to DBus service:" << m_policyAgent->lastError();
  }
  ```

---

## Step 5: Connect to Signals

Connect DBus signals to your slots:

```cpp
connect(
    m_policyAgent,
    &PolicyAgentInterface::InhibitionsChanged,
    this,
    &YourClass::yourSlot
);
```

**Signal signature matching:**
* Slot signatures must **match the signal exactly**, including argument types.
* Use `Q_DECLARE_METATYPE` for custom types if needed.
* Example with proper slot:
  ```cpp
  private slots:
      void onInhibitionsChanged(const QList<QStringList> &added, const QStringList &removed);
  ```

---

## Step 6: Call Methods and Access Properties

**Synchronous method calls:**
```cpp
uint cookie = m_policyAgent->AddInhibition(types, "AppName", "Reason");
```

**Asynchronous method calls (recommended for UI responsiveness):**
```cpp
QDBusPendingCall call = m_policyAgent->asyncCall("AddInhibition", types, "AppName", "Reason");
QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
connect(watcher, &QDBusPendingCallWatcher::finished, this, &YourClass::onCallFinished);
```

**Access properties:**
```cpp
auto active = m_policyAgent->activeInhibitions();
bool hasInhibition = m_policyAgent->property("HasInhibition").toBool();
```

---

## Step 7: Debugging and Troubleshooting

**Common issues and solutions:**

1. **Service not found:**
   ```bash
   # List available services
   qdbus --session  # or qdbus --system
   ```

2. **Check if service is running:**
   ```bash
   qdbus --session org.kde.Solid.PowerManagement
   ```

3. **Slot signature mismatch:**
   * Check generated header for exact signal signature
   * Use `qDBusRegisterMetaType` for custom types

4. **Build issues:**
   * Ensure `QtDBus` is linked
   * Include generated `.cpp` file in build

5. **Runtime debugging:**
   ```cpp
   QLoggingCategory::setFilterRules("qt.dbus.debug=true");
   ```

---

## Step 8: Applying to Other Services (like MPRIS)

**Example for MPRIS2:**

1. **Find the service:**
   ```bash
   qdbus --session | grep org.mpris.MediaPlayer2
   ```

2. **Introspect:**
   ```bash
   qdbus org.mpris.MediaPlayer2.vlc \
     /org/mpris/MediaPlayer2 \
     org.freedesktop.DBus.Introspectable.Introspect > mpris.xml
   ```

3. **Generate interface:**
   ```bash
   qdbusxml2cpp -c MprisInterface -p mprisinterface mpris.xml
   ```

4. **Use in code:**
   ```cpp
   MprisInterface *player = new MprisInterface(
       "org.mpris.MediaPlayer2.vlc",
       "/org/mpris/MediaPlayer2",
       QDBusConnection::sessionBus(),
       this
   );
   
   connect(player, &MprisInterface::PlaybackStatusChanged,
           this, &YourClass::onPlaybackStatusChanged);
   ```

---

## Best Practices

1. **Error handling:** Always check `lastError()` after DBus calls.
2. **Async calls:** Use asynchronous calls for UI applications.
3. **Resource management:** Use QObject parent hierarchy for automatic cleanup.
4. **Thread safety:** DBus calls are not thread-safe; use signals/slots for cross-thread communication.
5. **Service availability:** Handle service disappearance with `QDBusServiceWatcher`.

**Example with service watcher:**
```cpp
QDBusServiceWatcher *watcher = new QDBusServiceWatcher(
    "org.kde.Solid.PowerManagement",
    QDBusConnection::sessionBus(),
    QDBusServiceWatcher::WatchForOwnerChange,
    this
);

connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged,
        this, &YourClass::onServiceOwnerChanged);
```

---

## Additional Resources

* [Qt DBus Documentation](https://doc.qt.io/qt-6/qtdbus-index.html)
* [D-Bus Specification](https://dbus.freedesktop.org/doc/dbus-specification.html)
* [KDE DBus Tutorial](https://techbase.kde.org/Development/Tutorials/D-Bus)