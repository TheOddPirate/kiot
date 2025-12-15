#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SCRIPTS_PATH="$SCRIPT_DIR/scripts"

pause() {
    echo
    read -rp "Press Enter to return to menu..."
}

while true; do
    clear
    echo "======================================="
    echo " KIOT Helper Menu"
    echo "======================================="
    echo
    echo "0 Quit"
    echo "1 Install dependencies (Arch/Debian, tested on Manjaro via pacman)"
    echo "2 Build kiot"
    echo "3 Build and install kiot (will ask for sudo)"
    # TODO ccreate native build and install menu
    echo "4 Flatpak build and install menu"
    echo
    echo "======================================="
    read -rp "Select an option: " choice

    case "$choice" in
        0)
            echo "Exiting."
            exit 0
            ;;
        1)
            echo "Installing dependencies..."
            bash "$SCRIPTS_PATH/install_dependencies.sh"
            pause
            ;;
        2)
            echo "Building kiot..."
            bash "$SCRIPTS_PATH/build.sh"
            pause
            ;;
        3)
            echo "Building and installing kiot..."
            bash "$SCRIPTS_PATH/install.sh"
            pause
            ;;
        4)
            echo "Building and installing kiot flatpak..."
            bash "$SCRIPTS_PATH/build_flatpak_installer.sh"
            pause
            ;;
        *)
            echo "Invalid option."
            sleep 1
            ;;
    esac
done
