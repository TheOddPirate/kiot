This is my personal testing branch. It includes some extra integrations, has all
supported entities moved from core.cpp to the entities folder to make the code
easier to navigate, and comes with some helper scripts to make testing easier.

The current extra integrations in this build are:

- Battery: shows the battery entities of your computer in HA with extra attributes.
- Bluetooth: gives control over the Bluetooth adapter, with switches for each
  paired device to connect/disconnect and extra data in attributes.
- Docker: needs to be enabled per docker image (polling-based) in config file, but allows start/stop of containers and provides extra info as attributes.
- Systemd: needs to be enabled per user service; fills the config file with all
  available user services at startup and allows starting/stopping selected services.
- Scripts updated to suport input variables from a shared textbox in HA
- Camera updated to try to stop a crash i got every now and then (5 cameras connected)
This build also supports the following entity types in Home Assistant:

- Battery
- Select
- Textbox

The helper script `helper.sh` makes it easy to install all dependencies needed to
build and run Kiot (tested on Manjaro with pacman, but should also work on Debian).

If you test this branch, please provide feedback so I know what is stable enough
to consider for an upstream PR. Thank you! 🙂


# About

Kiot (KDE Internet Of Things) is a background daemon that exposes useful information and actions for your local desktop session to a home automation controller like Home Assistant.

This does not control smart home devices directly. i.e:
If you want a light to turn on when the PC is set to "Do not Disturb" mode, this app will not directly control the light. This app exposes the "Do not distrub" state to your controller (Home Assistant) so that you can create an automation there.

# Current State

This is pre-alpha software a level where if you're ok compiling things from source and meddling with config files by hand.

# Setup

## Dependencies

Make sure you have these packages installed:
- `cmake`
- `extra-cmake-modules`
- `qt6-base`/`qt6-base-dev`
- `qt6-mqtt`/`qt6-mqtt-dev`

Beware that depending on your distribution, these package names may vary slightly. If they simply don't exist, you will have to install them manually. 

## Download and install

Download this repo, for example, by cloning it: 
```sh
git clone https://github.com/davidedmundson/kiot.git  # downloads the repo to your system
cd kiot  # switches directory to the newly downloaded folder
```
Now, launch the following commands to proceed with installation:
```sh
mkdir build
cd build
cmake ..
make
make install  # might require `sudo`
```
Some dependencies might be missing, make sure you have 

# MQTT

In home assistant MQTT server must be enabled.
See https://www.home-assistant.io/integrations/mqtt/

The following configuration needs to be placed in `~/.config/kiotrc`,

```
 [general]
 host=some.host
 port=1883
 user=myUsername
 password=myPassword
 ```

> [!NOTE]
> If Kiot is running and you change the configuration, you will need to restart Kiot for the changes to take effect.

## Home Assistant Managed

- `host` should be your Home Assistant local address,
- `port` is correct at 1883 by default,
- `user` and `password` should be the username and password of a Home Assistant user (**recommended to create a specific user for MQTT connection**)

## Home Assistant Container

If running Home Assistant in a container:

- `host` should be the IP address where the MQTT broker is available
- `port` is correct at 1883 by default
- `user` and `password` should be a username and password setup for the MQTT broker

On the home assistant side everything should then work out-the-box with MQTT discovery.
Try rebooting Home Assistant, and then launch the `kiot` program and see it things go well. 

# Goals

Compared to other similar projects, I want to avoid exposing pointless system statistic information that's not useful in a HA context. There's no point having a sensor for "kernel version" for example. Instead the focus is towards tighter desktop integration with things that are practical and useful. This includes, but is not exclusive too some Plasma specific properties.

The other focus is on ensuring that device triggers and actions appear in an intuitive easy-to-use way in Home Assistant's configuration. 

# Supported Features (so far)

 - User activity (binary sensor)
 - Locked state (switch)
 - Suspend (button)
 - Camera in use (binary sensor)
 - Accent Colour (sensor)
 - Arbitrary Scripts (buttons)
 - Shortcuts (device_trigger)
 - Nightmode status (binary sensor)
 - Active window status (sensor)
 - Volume controller (number based slider)
 - Docker (switch with attributes)
 - Battery (battery sensor)
 - Systemd (switches)
 - GamePad (Binary sensor)
 - Bluetooth (switches with extra attributes)

# Additional Config

```
[general]
host=some.host
port=1883
user=myUsername
password=myPassword
useSSL=false

[Scripts][myScript1]
Name=Launch chrome
Exec=google-chrome

[Scripts][myScript2]
...

[Shortcuts][myShortcut1]
Name=Do a thing
# This then becomes available in global shortcuts KCM for assignment and will appear as a trigger in HA, so keys can be bound to HA actions

[docker] (this auto fills on first run with available images)
polltimer=30
imagename=true

[systemd](This auto fills with available user services on start)
kiot.service=true
```

# Flatpak build

Installing by flatpak is also possible

 - Clone this repo
 - `flatpak-builder build .flatpak-manifest.yaml --user --install --force-clean`
 - This will build and install kiot as a flatpak fetching all dependencies

## Notes:

The flatpak will not autostart.

## Future

Long term, flatpak is the only thing that matters, I'll push to Flathub once we have a have UI
 
