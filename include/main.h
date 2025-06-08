#ifndef MAIN_H
#define MAIN_H

#include <gtk/gtk.h>
#include "config.h"
#include "html_viewer.h"
#include "http_server.h"
#include "string_utils.h"

typedef struct {
    GtkApplication *app;
    Config *config;
    struct HtmlViewer *viewer;
} Application;

int main(int argc, char **argv);
void application_init(Application *app);
void application_run(Application *app);
void application_cleanup(Application *app);

#endif // MAIN_H
