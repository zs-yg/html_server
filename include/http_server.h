#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <glib.h>
#include <gtk/gtk.h>

typedef struct {
    gint port;
    gchar *html_content;
    GThread *server_thread;
    volatile gboolean running;
} HttpServer;

HttpServer* http_server_new(gint port, const gchar *html_path);
void http_server_free(HttpServer *server);
gboolean http_server_start(HttpServer *server);
void http_server_stop(HttpServer *server);

#endif // HTTP_SERVER_H
