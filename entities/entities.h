/**
 * @file entities.h
 * @brief Unified header for all KIOT entity types
 * 
 * @details
 * This header provides a single include point for all KIOT entity types.
 * It includes all individual entity headers, allowing integrations to
 * access the complete entity API through one include statement.
 * 
 * Usage in integrations:
 * @code
 * #include "entities/entities.h"
 * @endcode
 * 
 * This header includes:
 * - Entity base class
 * - All concrete entity types (Sensor, Switch, Button, etc.)
 * - Specialized entities (Camera, Notify, MediaPlayer)
 * 
 * @note Always use this header instead of including individual entity
 *       headers to ensure consistency and simplify dependency management.
 */

#pragma once

#include "entity.h"
#include "binarysensor.h"
#include "button.h"
#include "event.h"
#include "lock.h"
#include "number.h"
#include "select.h"
#include "sensor.h"
#include "switch.h"
#include "textbox.h"
#include "mediaplayer.h"
#include "notify.h"
#include "camera.h"