#!/bin/bash
# I am running manjaro with kde myself so i have not gotten to test this on debian
# but with the help of google and a llm i found every package name except for "PulseAudioQt" 
# but i included a backup compile from git for PulseAudioQt i hope this should make it
# easier for everyone with 0 cpp skills like myself to get started with testing 
# and maybe contributing on the project =)
set -e

# Detect package manager
detect_pkg_manager() {
    if command -v apt >/dev/null 2>&1; then
        echo "apt"
    elif command -v pacman >/dev/null 2>&1; then
        echo "pacman"
    else
        echo "unknown"
    fi
}

# Install package
install_package() {
    local pkg="$1"
    local pm=$(detect_pkg_manager)
    if [ "$pm" = "apt" ]; then
        if ! dpkg -s "$pkg" >/dev/null 2>&1; then
            sudo apt update
            sudo apt install -y "$pkg"
        fi
    elif [ "$pm" = "pacman" ]; then
        if ! pacman -Qi "$pkg" >/dev/null 2>&1; then
            sudo pacman -S --noconfirm "$pkg"
        fi
    else
        echo "Unsupported package manager"
        exit 1
    fi
}

echo "Installing system dependencies..."

PM=$(detect_pkg_manager)
if [ "$PM" = "unknown" ]; then
    echo "Cannot detect package manager. Aborting."
    exit 1
fi

# Common dependencies
if [ "$PM" = "apt" ]; then
    sudo apt update && sudo apt upgrade
    # So i have not gotten to test this on debian, but i had to trust gpt here for the package names
    # missing packages qt6-mqtt-dev (manually compiling needed?)
    packages=(cmake build-essential git extra-cmake-modules qt6-base-dev qt6-base-private-dev   \
          libkf6coreaddons-dev libkf6config-dev libkf6dbusaddons-dev \
          libkf6notifications-dev libkf6idletime-dev libkf6globalaccel-dev \
          libkf6pulseaudioqt-dev libudev-dev)

elif [ "$PM" = "pacman" ]; then
    # Tested on manjaro linux, i can not say for sure that its a match for other pacman using distroes
    packages=(cmake extra-cmake-modules qt6-base qt6-mqtt \
        kcoreaddons kconfig kdbusaddons \
        knotifications kidletime kglobalaccel pulseaudio-qt systemd-libs)
fi

# loops over and installs each dependency separatly so it stops on the error making it simpler to know what failed
for pkg in "${packages[@]}"; do
    install_package "$pkg"
done

echo "System packages installed."
if [ "$PM" = "apt" ]; then 
    echo "as a debian user you are missing qt6-mqtt-dev you will have to manually compile it from source"
elif [ "$PM" = "pacman" ]; then
    echo "Every dependencies should now be installed."
fi
