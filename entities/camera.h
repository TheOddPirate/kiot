// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

//This is a camera entity implementation based on info from here
//https://www.home-assistant.io/integrations/camera.mqtt/
#pragma once
#include "entity.h"
#include <QObject>
#include <QByteArray>

class Camera : public Entity
{
    Q_OBJECT
public:
    Camera(QObject *parent = nullptr);



protected:
    void init() override;
};
