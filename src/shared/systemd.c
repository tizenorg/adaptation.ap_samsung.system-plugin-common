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

#include <stdio.h>
#include <gio/gio.h>

#include "util.h"
#include "dbus-common.h"
#include "systemd.h"

#define SYSTEMD_UNIT_OBJ_PATH_PREFIX    "/org/freedesktop/systemd1/unit/"
#define SYSTEMD_UNIT_ESCAPE_CHAR        ".-"

static char *get_systemd_unit_obj_path(const char* unit) {
        char *path = NULL;
        int i, j;
        size_t p, k, prefix_len, unit_len = strlen(unit);

        for (i=0, p=0; p<unit_len; i++) {
                k = strcspn(unit+p, SYSTEMD_UNIT_ESCAPE_CHAR);
                p += k+1;
        }

        prefix_len = strlen(SYSTEMD_UNIT_OBJ_PATH_PREFIX);
        /* assume we try to get object path of foo-bar.service then
         * the object path will be
         * "/org/freedesktop/systemd1/unit/foo_2dbar_2eservice\n". In
         * this case we can find two escape characters, so the total
         * length will be:
         * (PREFIX_length) + (unit_length - escape + 3*escape) + NULL */
        path = new0(char,
                    prefix_len
                    + (unit_len - (i-1)) + ((i-1) * 3 * sizeof(char))
                    + 1);
        strncpy(path, SYSTEMD_UNIT_OBJ_PATH_PREFIX, prefix_len+1);

        for (j=0, p=0; j < i; j++) {
                k = strcspn(unit+p, SYSTEMD_UNIT_ESCAPE_CHAR);
                strncpy(path+prefix_len, unit+p, k);
                if (k < strlen(unit+p)) {
                        sprintf(path+prefix_len+k, "_%x", *(unit+p+k) & 0xff);
                        prefix_len += k+3;
                        p += k+1;
                }
        }

        return path;
}

static int systemd_get_property(GBusType type,
                                const char *name,
                                const char *path,
                                const char *iface,
                                const char *method,
                                const char *interface,
                                const char *property,
                                GVariant **variant) {

        GDBusConnection *connection;
        GVariant *gvar;
        GError *error;
        GDBusProxy *proxy;

#if (GLIB_MAJOR_VERSION <= 2 && GLIB_MINOR_VERSION < 36)
        g_type_init ();
#endif

        error = NULL;
        proxy = g_dbus_proxy_new_for_bus_sync(type ? G_BUS_TYPE_SYSTEM : G_BUS_TYPE_SESSION,
                                              G_DBUS_PROXY_FLAGS_NONE,
                                              NULL, /* GDBusInterfaceInfo */
                                              name,
                                              path,
                                              iface,
                                              NULL, /* GCancellable */
                                              &error);

        if (proxy == NULL) {
                g_printerr("Error creating proxy: %s\n", error->message);
                g_error_free(error);
                return -1;
        }

        error = NULL;
        gvar = g_dbus_proxy_call_sync(proxy,
                                      method,
                                      g_variant_new ("(ss)",
                                                     interface,
                                                     property),
                                      G_DBUS_CALL_FLAGS_NONE,
                                      -1,
                                      NULL, /* GCancellable */
                                      &error);

        g_assert_no_error(error);
        g_assert(gvar != NULL);
        *variant = gvar;
        g_clear_error (&error);
        g_object_unref(proxy);

        return 0;
}

int systemd_get_manager_property(const char *iface,
                                 const char *property,
                                 GVariant **variant) {

        return systemd_get_property(G_BUS_TYPE_SYSTEM,
                                    DBUS_SYSTEMD_DEST,
                                    DBUS_SYSTEMD_ADDRESS,
                                    DBUS_INTERFACE_DBUS_PROPERTIES,
                                    "Get",
                                    iface,
                                    property,
                                    variant);
}

int systemd_get_unit_property(const char *unit,
                              const char *property,
                              GVariant **variant) {

        _cleanup_free_ char *systemd_unit_obj_path;

        systemd_unit_obj_path = get_systemd_unit_obj_path(unit);

        return systemd_get_property(G_BUS_TYPE_SYSTEM,
                                    DBUS_SYSTEMD_DEST,
                                    systemd_unit_obj_path,
                                    DBUS_INTERFACE_DBUS_PROPERTIES,
                                    "Get",
                                    DBUS_INTERFACE_SYSTEMD_UNIT,
                                    property,
                                    variant);
}

int systemd_get_service_property(const char *unit,
                                 const char *property,
                                 GVariant **variant) {

        _cleanup_free_ char *systemd_unit_obj_path;

        systemd_unit_obj_path = get_systemd_unit_obj_path(unit);

        return systemd_get_property(G_BUS_TYPE_SYSTEM,
                                    DBUS_SYSTEMD_DEST,
                                    systemd_unit_obj_path,
                                    DBUS_INTERFACE_DBUS_PROPERTIES,
                                    "Get",
                                    DBUS_INTERFACE_SYSTEMD_SERVICE,
                                    property,
                                    variant);
}

typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;

#define g_variant_type_int32    G_VARIANT_TYPE_INT32
#define g_variant_type_int64    G_VARIANT_TYPE_INT64
#define g_variant_type_uint32   G_VARIANT_TYPE_UINT32
#define g_variant_type_uint64   G_VARIANT_TYPE_UINT64
#define g_variant_type_string   G_VARIANT_TYPE_STRING

#define g_variant_get_function_int32(v)   g_variant_get_int32(v)
#define g_variant_get_function_int64(v)   g_variant_get_int64(v)
#define g_variant_get_function_uint32(v)  g_variant_get_uint32(v)
#define g_variant_get_function_uint64(v)  g_variant_get_uint64(v)
#define g_variant_get_function_string(v)  g_variant_dup_string(v, NULL)

#define DEFINE_SYSTEMD_GET_PROPERTY(iface, type, value)                 \
        int systemd_get_##iface##_property_as_##type(                   \
                        const char* target,                             \
                        const char* property,                           \
                        value* result) {                                \
                                                                        \
                GVariant *var;                                          \
                GVariant *inner;                                        \
                int r;                                                  \
                                                                        \
                r = systemd_get_##iface##_property(target, property, &var); \
                                                                        \
                if (r < 0) {                                            \
                        fprintf(stderr, "Failed to get property:\n  target: %s\n  property: %s\n", \
                                target, property);                      \
                        return r;                                       \
                }                                                       \
                                                                        \
                g_assert(g_variant_is_of_type(var, G_VARIANT_TYPE("(v)"))); \
                g_variant_get(var, "(v)", &inner);                      \
                g_assert(g_variant_is_of_type(inner, g_variant_type_##type)); \
                *result = g_variant_get_function_##type(inner);         \
                g_variant_unref(var);                                   \
                                                                        \
                return 0;                                               \
        }


DEFINE_SYSTEMD_GET_PROPERTY(manager, int32, int)
DEFINE_SYSTEMD_GET_PROPERTY(manager, uint32, uint)
DEFINE_SYSTEMD_GET_PROPERTY(manager, int64, long long)
DEFINE_SYSTEMD_GET_PROPERTY(manager, uint64, unsigned long long)
DEFINE_SYSTEMD_GET_PROPERTY(manager, string, char*)
DEFINE_SYSTEMD_GET_PROPERTY(unit, int32, int)
DEFINE_SYSTEMD_GET_PROPERTY(unit, uint32, uint)
DEFINE_SYSTEMD_GET_PROPERTY(unit, int64, long long)
DEFINE_SYSTEMD_GET_PROPERTY(unit, uint64, unsigned long long)
DEFINE_SYSTEMD_GET_PROPERTY(unit, string, char*)
DEFINE_SYSTEMD_GET_PROPERTY(service, int32, int)
DEFINE_SYSTEMD_GET_PROPERTY(service, uint32, uint)
DEFINE_SYSTEMD_GET_PROPERTY(service, int64, long long)
DEFINE_SYSTEMD_GET_PROPERTY(service, uint64, unsigned long long)
DEFINE_SYSTEMD_GET_PROPERTY(service, string, char*)
