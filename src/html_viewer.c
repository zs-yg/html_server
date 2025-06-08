#include "html_viewer.h"
#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>

struct HtmlViewer {
    GtkApplication *app;
    GtkWindow *window;
    GtkEntry *port_entry;
    GtkTextView *web_view;
    GtkButton *file_button;
    GtkButton *start_button;
    gchar *html_path;
    HttpServer *server;
};

static void on_file_select_response(GObject *source, GAsyncResult *result, gpointer data) {
    HtmlViewer *viewer = (HtmlViewer *)data;
    GError *error = NULL;
    
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    if (file) {
        g_free(viewer->html_path);
        viewer->html_path = g_file_get_path(file);
        
        // 加载HTML内容预览
        gchar *contents = NULL;
        gsize length = 0;
        if (g_file_get_contents(viewer->html_path, &contents, &length, NULL)) {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(viewer->web_view);
            gtk_text_buffer_set_text(buffer, contents, length);
            g_free(contents);
        }
        
        g_object_unref(file);
    } else if (error) {
        g_warning("File selection failed: %s", error->message);
        g_error_free(error);
    }
}

void on_file_select(GtkWidget *widget, gpointer data) {
    HtmlViewer *viewer = (HtmlViewer *)data;
    (void)widget;
    
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "选择html文件");
    
    gtk_file_dialog_open(dialog, GTK_WINDOW(viewer->window), NULL, 
                       (GAsyncReadyCallback)on_file_select_response, data);
    g_object_unref(dialog);
}

static void on_start_server(GtkWidget *widget, gpointer data) {
    (void)widget;
    HtmlViewer *viewer = (HtmlViewer *)data;
    const char *port_text = gtk_editable_get_text(GTK_EDITABLE(viewer->port_entry));
    int port = atoi(port_text);

    if (viewer->server) {
        http_server_stop(viewer->server);
        http_server_free(viewer->server);
    }

    if (!viewer->html_path) {
        g_warning("No HTML file selected");
        return;
    }

    viewer->server = http_server_new(port, viewer->html_path);

    if (!http_server_start(viewer->server)) {
        g_warning("Failed to start HTTP server");
    }
}

HtmlViewer* html_viewer_new(GtkApplication *app) {
    HtmlViewer *viewer = g_new0(HtmlViewer, 1);
    viewer->app = app;

    // 创建主窗口
    viewer->window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(viewer->window, "HTML Server");
    gtk_window_set_default_size(viewer->window, 800, 600);

    // 创建主布局
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(viewer->window, box);

    // 创建控制面板
    GtkWidget *controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(box), controls);

    // 端口输入框
    viewer->port_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(viewer->port_entry, "端口（例如 8080）");
    gtk_box_append(GTK_BOX(controls), GTK_WIDGET(viewer->port_entry));

    // 文件选择按钮
    viewer->file_button = GTK_BUTTON(gtk_button_new_with_label("选择html文件"));
    g_signal_connect(viewer->file_button, "clicked", G_CALLBACK(on_file_select), viewer);
    gtk_box_append(GTK_BOX(controls), GTK_WIDGET(viewer->file_button));

    // 启动服务器按钮
    viewer->start_button = GTK_BUTTON(gtk_button_new_with_label("启动服务"));
    g_signal_connect(viewer->start_button, "clicked", G_CALLBACK(on_start_server), viewer);
    gtk_box_append(GTK_BOX(controls), GTK_WIDGET(viewer->start_button));

    // HTML预览区域
    viewer->web_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(viewer->web_view, FALSE);
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(viewer->web_view));
    gtk_box_append(GTK_BOX(box), scrolled_window);

    return viewer;
}

void html_viewer_free(HtmlViewer *viewer) {
    if (!viewer) return;

    if (viewer->server) {
        http_server_stop(viewer->server);
        http_server_free(viewer->server);
    }

    g_free(viewer->html_path);
    g_free(viewer);
}

void html_viewer_run(HtmlViewer *viewer) {
    if (!viewer) return;
    gtk_window_present(viewer->window);
}
