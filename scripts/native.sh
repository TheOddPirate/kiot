#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

pause() {
    echo
    read -rp "Press Enter to return to menu..."
}

cleanup() {
    echo "Cleaning up build directory..."
    rm -rf build
}

build() {
    echo "Building kiot..."
    mkdir -p build
    cd build
    cmake ..
    make clean
    # make clang-format
    make
    cd ..
}

install() {
    if command -v pacman &> /dev/null; then
        echo "Detected Arch Linux. Building and installing via makepkg..."
        mkdir -p build
        cp scripts/PKGBUILD build/
        cd build
        makepkg -si
        cd ..
    elif command -v apt-get &> /dev/null; then
        echo "Detected Debian/Ubuntu. Building package using CPack..."
        mkdir -p build
        cd build
        cpack -G DEB
        
        pkg_file=$(find . -maxdepth 1 -name "*.deb" | head -n 1)
        if [ -n "$pkg_file" ]; then
            sudo apt install ./"$pkg_file"
        else
            echo "Error: Could not find generated Debian package."
            exit 1
        fi
        cd ..
    else
        echo "Installing kiot...(this requires sudo)"
        cd build
        sudo make install
        cd ..
    fi
}


uninstall() {
    if command -v pacman &> /dev/null; then
        echo "Uninstalling kiot via pacman..."
        sudo pacman -R kiot-git || sudo pacman -R kiot || echo "Package not found via pacman."
    elif command -v apt-get &> /dev/null; then
         echo "Uninstalling kiot via apt..."   
        sudo apt remove kiot || echo "Package not found via apt."
    else
        echo "Uninstalling kiot...(this requires sudo)"
    
        # Check if install manifest exists
        if [ -f "build/install_manifest.txt" ]; then
            echo "Using install manifest to uninstall..."
            # Read each line from manifest and remove the file
            while IFS= read -r file; do
                if [ -e "$file" ]; then
                    echo "Removing: $file"
                    sudo rm -f "$file"
                
                    # Remove empty parent directories
                    dir=$(dirname "$file")
                    while [ "$dir" != "/" ]; do
                        if rmdir "$dir" 2>/dev/null; then
                            echo "Removed empty directory: $dir"
                        else
                            break
                        fi
                        dir=$(dirname "$dir")
                    done
                fi
            done < "build/install_manifest.txt"
        
            # Also remove common known files as backup
            echo "Removing known installed files..."
            sudo rm -f /usr/bin/kiot
            sudo rm -f /etc/xdg/autostart/org.davidedmundson.kiot.desktop
            sudo rm -f /usr/share/applications/org.davidedmundson.kiot.desktop
            sudo rm -f /usr/share/kiot/activewindow_kwin.js
            sudo rm -f /usr/lib/qt6/plugins/plasma/kcms/systemsettings/kcm_kiot.so
            sudo rm -f /usr/share/applications/kcm_kiot.desktop
        
            # Remove empty directories
            sudo rmdir /usr/share/kiot 2>/dev/null || true
        
            echo "Uninstall completed!"
        else
            echo "Install manifest not found. Removing known files..."
            # Manual removal if manifest doesn't exist
            sudo rm -f /usr/bin/kiot
            sudo rm -f /etc/xdg/autostart/org.davidedmundson.kiot.desktop
            sudo rm -f /usr/share/applications/org.davidedmundson.kiot.desktop
            sudo rm -f /usr/share/kiot/activewindow_kwin.js
            sudo rm -f /usr/lib/qt6/plugins/plasma/kcms/systemsettings/kcm_kiot.so
            sudo rm -f /usr/share/applications/kcm_kiot.desktop
        
            # Remove empty directories
            sudo rmdir /usr/share/kiot 2>/dev/null || true
        
            echo "Uninstall completed (manual mode)!"
        fi
    
        # Update desktop database
        echo "Updating desktop database..."
        sudo update-desktop-database 2>/dev/null || true
    fi
    echo "Uninstall completed!"
}


build_and_install() {
    echo "Building and installing kiot..."
    build
    install
}


while true; do
    echo "======================================="
    echo " KIOT Native Installer Menu"
    echo "======================================="
    echo
    echo "0 Quit (runs cleanup before closing)"
    echo "1 Build kiot"
    echo "2 Install kiot (use 1 first, this requires sudo)"
    echo "3 Build and install kiot (AIO, this requires sudo)"
    echo "4 Uninstall (requires sudo, this will remove all file)"
    echo "5 Cleanup (Deletes the build folder)"
    echo
    echo "======================================="
    read -rp "Select an option: " choice

    case "$choice" in
        0)
            cleanup
            echo "Exiting."
            exit 0
            ;;
        1)
            build
            pause
            ;;
        2)
            echo "Installing kiot"
            install
            pause
            ;;
        3)
            echo "Building and installing kiot..."
            build_and_install
            pause
            ;;
        4)
            echo "Uninstalling kiot"
            uninstall
            pause
            ;;
        5)
            echo "Cleaning up after us"
            cleanup
            pause
            ;;
        *)
            echo "Invalid option. Please try again."
            pause
            ;;
    esac
done