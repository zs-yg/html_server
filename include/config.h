#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>
#include <stddef.h>

#define DEFAULT_PORT 8080
#define CONFIG_FILE "html_server.conf"

typedef struct {
    gint port;
    gchar *last_html_path;
} Config;

Config* config_new(void);
void config_free(Config *config);
gboolean config_load(Config *config, const gchar *path);
gboolean config_save(Config *config, const gchar *path);

#endif // CONFIG_H
