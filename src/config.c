#include "config.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <stddef.h>

Config* config_new(void) {
    Config *config = g_new0(Config, 1);
    config->port = DEFAULT_PORT;
    config->last_html_path = NULL;
    return config;
}

void config_free(Config *config) {
    if (!config) return;
    g_free(config->last_html_path);
    g_free(config);
}

gboolean config_load(Config *config, const gchar *path) {
    if (!config || !path) return FALSE;

    GKeyFile *key_file = g_key_file_new();
    GError *error = NULL;

    if (!g_key_file_load_from_file(key_file, path, G_KEY_FILE_NONE, &error)) {
        g_warning("Failed to load config file: %s", error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return FALSE;
    }

    config->port = g_key_file_get_integer(key_file, "server", "port", &error);
    if (error) {
        g_warning("Failed to read port: %s", error->message);
        g_error_free(error);
        config->port = DEFAULT_PORT;
    }

    g_free(config->last_html_path);
    config->last_html_path = g_key_file_get_string(key_file, "server", "last_html_path", &error);
    if (error) {
        g_warning("Failed to read last HTML path: %s", error->message);
        g_error_free(error);
        config->last_html_path = NULL;
    }

    g_key_file_free(key_file);
    return TRUE;
}

gboolean config_save(Config *config, const gchar *path) {
    if (!config || !path) return FALSE;

    GKeyFile *key_file = g_key_file_new();
    g_key_file_set_integer(key_file, "server", "port", config->port);
    
    if (config->last_html_path) {
        g_key_file_set_string(key_file, "server", "last_html_path", config->last_html_path);
    }

    gsize length;
    gchar *data = g_key_file_to_data(key_file, &length, NULL);
    gboolean result = g_file_set_contents(path, data, length, NULL);
    
    g_free(data);
    g_key_file_free(key_file);
    return result;
}
