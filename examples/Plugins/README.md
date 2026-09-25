# KIOT Plugin Templates

## Navigation

* [Overview](#overview)
* [Templates](#templates)
* [1. Native Plugin Template](#1-native-plugin-template)
* [2. Flatpak Extension Plugin Template](#2-flatpak-extension-plugin-template)
* [Which one should I choose?](#which-one-should-i-choose)
* [See Also](#see-also)

---

## Overview

Welcome to the KIOT plugin development playground!

New folder added, Combined, for both flatpak and native builds, documentation will be updated later

Depending on how you plan to distribute your integration, KIOT supports two different approaches for writing plugins. Whether you want to bake it directly into the main repository or package it as an independent, modular Flatpak extension (à la OBS Studio) for the community, we have a boilerplate template ready for you.

---

## Templates

### 1. Native-Plugin-Template

* **Directory:** [`Native-Plugin-Template/`](https://www.google.com/search?q=./Native-Plugin-Template/&utm_source=gemini)
* **Best used for:** Core integrations, local development, or if your plugin lives directly inside the main KIOT source tree.
* **How it works:** It compiles directly alongside the main application structure using standard CMake configurations and integrates natively with the core binary path.
* **Native plugins works in flatpak to:** Yes, you can install the native plugins under '/home/theoddpirate/.var/app/org.davidedmundson.kiot/data/kiot/plugins/' and it should load


### 2. Flatpak-Extension-Plugin-Template

* **Directory:** [`Flatpak-Extension-Plugin-Template/`](https://www.google.com/search?q=./Flatpak-Extension-Plugin-Template/&utm_source=gemini)
* **Best used for:** Third-party developers who want to create independent integrations (like Home Assistant connectors, custom hardware controls, etc.) without modifying the core repository or needing a centralized plugin store.
* **Key Features:**
* Packed as a standalone Flatpak extension (`build-extension: true`).
* Automatically discovered by KIOT core via `/app/plugins`.
* Includes AppStream Metainfo (`.xml`) and Qt plugin metadata (`plugin.json`).



> ⚠️ **Developer Warning for Flatpak Extensions:**
> When creating a Flatpak extension, your manifest ID **must** strictly follow the core naming convention:
> `org.davidedmundson.kiot.plugin.YourPluginName`
> If you deviate or miss `.kiot.`, Flatpak-builder will reject it with a `No extension point matching...` error.

---

## Which one should I choose?

| Feature | Native Plugin | Flatpak Extension |
| --- | --- | --- |
| **Where does it live?** | Inside the KIOT source tree | Standalone repository / folder |
| **Distribution** | Bundled with core | Installed via Flatpak separate extension |
| **Maintenance** | Updated with core releases | Independent lifecycle |
| **Best for** | Core contributors | 3rd-party community devs |

Grab a template, copy it over, adjust the names, and start building something awesome! If you run into issues, check the existing code examples or reach out. =)


* [KIOT Main](/README.md) for project overview and setup
* [KIOT Shared](../../Shared/README.md)
* [KIOT Shared/Entities](/Shared/entities/README.md) for information about the shared lib entites part of the project
* [KIOT Integrations](../integrations/README.md) for creating new integrations
* [KIOT Example](../examples/README.md) for config examples and some inspiration
* [KIOT Helper Scripts](/scripts/README.md) for information about the helper scripts