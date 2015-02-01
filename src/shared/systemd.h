/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/*
 * system-plugin-common
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <dbus/dbus.h>
#include <gio/gio.h>

enum SystemdUnitType {
        SYSTEMD_UNIT_SERVICE,
        SYSTEMD_UNIT_SOCKET,
        SYSTEMD_UNIT_DEVICE,
        SYSTEMD_UNIT_MOUNT,
        SYSTEMD_UNIT_AUTOMOUNT,
        SYSTEMD_UNIT_SWAP,
        SYSTEMD_UNIT_TARGET,
        SYSTEMD_UNIT_PATH,
        SYSTEMD_UNIT_TIMER,
        SYSTEMD_UNIT_SNAPSHOT,
        SYSTEMD_UNIT_SLICE,
        SYSTEMD_UNIT_SCOPE,
        _SYSTEMD_UNIT_TYPE_MAX,
        _SYSTEMD_UNIT_TYPE_INVALID = -1
};

#define DBUS_SYSTEMD_DEST               "org.freedesktop.systemd1"
#define DBUS_SYSTEMD_ADDRESS            "/org/freedesktop/systemd1"
#define DBUS_INTERFACE_SYSTEMD_MANAGER  DBUS_SYSTEMD_DEST ".Manager"
#define DBUS_INTERFACE_SYSTEMD_UNIT     DBUS_SYSTEMD_DEST ".Unit"
#define DBUS_INTERFACE_SYSTEMD_SERVICE  DBUS_SYSTEMD_DEST ".Service"
#define DBUS_INTERFACE_SYSTEMD_TARGET   DBUS_SYSTEMD_DEST ".Target"

int systemd_get_unit_property(const char *unit, const char *property, GVariant **variant);
int systemd_get_service_property(const char* unit, const char* property, GVariant **variant);

int systemd_get_manager_property_as_int32(const char *iface, const char *property, int *result);
int systemd_get_manager_property_as_uint32(const char *iface, const char *property, unsigned int *result);
int systemd_get_manager_property_as_int64(const char *iface, const char *property, long long *result);
int systemd_get_manager_property_as_uint64(const char *iface, const char *property, unsigned long long *result);
int systemd_get_manager_property_as_string(const char *iface, const char *property, char **result);
int systemd_get_unit_property_as_int32(const char *unit, const char *property, int *result);
int systemd_get_unit_property_as_uint32(const char *unit, const char *property, unsigned int *result);
int systemd_get_unit_property_as_int64(const char *unit, const char *property, long long *result);
int systemd_get_unit_property_as_uint64(const char *unit, const char *property, unsigned long long *result);
int systemd_get_unit_property_as_string(const char *unit, const char *property, char **result);
int systemd_get_service_property_as_int32(const char *unit, const char *property, int *result);
int systemd_get_service_property_as_uint32(const char *unit, const char *property, unsigned int *result);
int systemd_get_service_property_as_int64(const char *unit, const char *property, long long *result);
int systemd_get_service_property_as_uint64(const char *unit, const char *property, unsigned long long *result);
int systemd_get_service_property_as_string(const char *unit, const char *property, char **result);
