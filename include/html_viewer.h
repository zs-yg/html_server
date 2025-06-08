#ifndef HTML_VIEWER_H
#define HTML_VIEWER_H

#include <gtk/gtk.h>
#include <glib.h>
#include "http_server.h"

typedef struct HtmlViewer {
    GtkApplication *app;
    GtkWindow *window;
    GtkEntry *port_entry;
    GtkTextView *web_view;
    GtkButton *file_button;
    GtkButton *start_button;
    gchar *html_path;
    HttpServer *server;
} HtmlViewer;

HtmlViewer* html_viewer_new(GtkApplication *app);
void html_viewer_free(HtmlViewer *viewer);
void html_viewer_run(HtmlViewer *viewer);
void on_file_select(GtkWidget *widget, gpointer data);

#endif // HTML_VIEWER_H
