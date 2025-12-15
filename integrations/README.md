# KIOT Integrations

This directory contains integrations that extend KIOT's functionality. Each integration provides one or more entities (sensors, buttons, etc.) that can be monitored or controlled through Home Assistant.

---

## Quick Navigation

* [Overview](#overview)
* [Creating a New Integration](#creating-a-new-integration)

  * [Template Setup](#1-start-with-the-template)
  * [Add to CMakeLists.txt](#2-add-to-cmakeliststxt)
  * [QObject-based Class Structure](#3-understanding-the-pattern-based-on-audiocpp)
* [Entity Types & Usage](#4-entity-types-and-their-usage)
* [Key Principles from audio.cpp](#5-key-principles-from-audiocpp)
* [Common Integration Patterns](#6-common-integration-patterns)
* [Existing Integrations Reference](#7-existing-integrations-reference)
* [Best Practices](#8-best-practices)
* [Testing Your Integration](#9-testing-your-integration)
* [Common Issues](#10-common-issues)
* [Example Implementation](#example-implementation)

---

## Overview

Integrations are C++ source files compiled into the KIOT binary. Primary pattern: **QObject-based classes** for complex integrations with state management, external resource monitoring, or multiple interconnected entities.

---

## Creating a New Integration

### 1. Start with the Template (Template / Instructions)

Use `integration_template.cpp` as a starting point, following the `audio.cpp` pattern:

```bash
cp integration_template.cpp my_integration.cpp
```

### 2. Add to CMakeLists.txt (Template / Instructions)

```cmake
set(KIOT_INTEGRATIONS_SRC
    # ... existing files ...
    my_integration.cpp
)
```

### 3. Understanding the Pattern (Template / Instructions)

#### QObject-based Class Structure

```cpp
class MyIntegration : public QObject
{
    Q_OBJECT
public:
    explicit MyIntegration(QObject *parent = nullptr);
    ~MyIntegration() override;
    
private slots:
    void onNumberValueChanged(int newValue);
    void onSelectOptionChanged(const QString &newOption);
    void onExternalSystemUpdate();
    
private:
    Number *m_number = nullptr;
    Select *m_select = nullptr;
    QDBusInterface *m_dbusInterface = nullptr;
};
```

**Key Components:**

1. Constructor – Create entities and connect signals
2. Destructor – Clean up external resources
3. Signal Handlers – Respond to Home Assistant changes
4. Entity Updates – Update entities when system state changes

---

## 4. Entity Types and Their Usage (Template / Instructions)

**Number (numeric values)**

```cpp
Number *volume = new Number(this);
volume->setId("output_volume");
volume->setName("Output Volume");
volume->setRange(0, 100, 1, "%");
connect(volume, &Number::valueChangeRequested,
        this, &MyIntegration::onVolumeChanged);
```

**Select (dropdown)**

```cpp
Select *deviceSelector = new Select(this);
deviceSelector->setId("output_device");
deviceSelector->setName("Output Device");
deviceSelector->setOptions({"Headphones", "Speakers", "HDMI"});
connect(deviceSelector, &Select::optionSelected,
        this, &MyIntegration::onDeviceSelected);
```

**Button (trigger actions)**

```cpp
Button *actionButton = new Button(this);
actionButton->setId("perform_action");
actionButton->setName("Perform Action");
connect(actionButton, &Button::triggered,
        this, &MyIntegration::onActionTriggered);
```

**Switch (toggle states)**

```cpp
Switch *featureSwitch = new Switch(this);
featureSwitch->setId("enable_feature");
featureSwitch->setName("Enable Feature");
connect(featureSwitch, &Switch::stateChangeRequested,
        this, &MyIntegration::onFeatureToggled);
```

**Sensor (string state)**

```cpp
Sensor *statusSensor = new Sensor(this);
statusSensor->setId("system_status");
statusSensor->setName("System Status");
statusSensor->setState("Running");
statusSensor->setAttributes({{"uptime", "5 hours"}});
```

**BinarySensor (boolean state)**

```cpp
BinarySensor *activitySensor = new BinarySensor(this);
activitySensor->setId("user_active");
activitySensor->setName("User Active");
activitySensor->setState(true);
```

---

## 5. Key Principles from audio.cpp (Reference)

* **Bidirectional State Sync**: HA → System via signals, System → HA via `setState()` / `setValue()`
* **Resource Management**: Initialize in constructor, cleanup in destructor
* **Error Handling**: Check DBus interfaces, log warnings, reflect availability in entities

---

## 6. Common Integration Patterns (Reference)

* **DBus Monitoring**: Connect to DBus signals, update entities accordingly
* **Timer-based Updates**: `QTimer` to refresh states regularly
* **Configuration Reading**: Use `KSharedConfig` for persistent settings

---

## 7. Existing Integrations Reference (Reference)

* `audio.cpp` – Audio volume and device control
* `activewindow.cpp` – Active window tracking
* `nightmode.cpp` – Night mode control
* `dndstate.cpp` – DBus property monitoring
* `active.cpp` – sensor with timer
* `suspend.cpp` – power control buttons

---

## 8. Best Practices (Template / Instructions)

1. Follow `audio.cpp` pattern
2. Ensure bidirectional state sync
3. Implement destructor for cleanup when needed
4. Use `qWarning` / `qInfo` / `qDebug` appropriately
5. Initialize entity states in constructor
6. Disconnect signals if needed

---

## 9. Testing Your Integration (Template / Instructions)

1. Add to CMakeLists.txt, rebuild KIOT
2. Check KIOT logs for initialization
3. Verify entities appear in Home Assistant
4. Test HA → System and System → HA

---

## 10. Common Issues (Reference)

* Missing `.moc` include
* No `REGISTER_INTEGRATION` macro
* State not syncing correctly
* DBus service or object path issues

---

## Example Implementation (Template / Instructions)

See `integration_template.cpp` for a full example following the `audio.cpp` pattern. Includes all entity types, bidirectional state management, and resource handling.
