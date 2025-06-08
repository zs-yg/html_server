#include "main.h"
#include <gtk/gtk.h>
#include <locale.h>
#include <libintl.h>

static void on_file_select_response(GObject *source, GAsyncResult *result, gpointer user_data) {
    Application *app = (Application *)user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    
    if (file) {
        g_free(app->config->last_html_path);
        app->config->last_html_path = g_file_get_path(file);
        g_object_unref(file);
    }
    
    if (error) {
        g_error_free(error);
    }
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    Application *application = (Application *)user_data;
    
    // 确保只初始化一次
    if (!application->viewer) {
        application_init(application);
        gtk_application_add_window(app, application->viewer->window);
    }
    gtk_window_present(application->viewer->window);
}

void application_init(Application *app) {
    app->config = config_new();
    // 创建默认配置
    app->config->last_html_path = NULL;
    app->config->port = 8080;
    
    // 尝试加载配置，忽略失败
    config_load(app->config, CONFIG_FILE);
    
    // 创建视图但不立即显示
    app->viewer = html_viewer_new(app->app);
    
    // 设置窗口默认大小
    gtk_window_set_default_size(app->viewer->window, 800, 600);
}

void application_run(Application *app) {
    if (!app || !app->viewer) return;
    html_viewer_run(app->viewer);
}

void application_cleanup(Application *app) {
    if (!app) return;
    
    if (app->viewer) {
        GtkFileDialog *dialog = gtk_file_dialog_new();
        gtk_file_dialog_open(dialog, GTK_WINDOW(app->viewer->window), NULL, 
                          (GAsyncReadyCallback)on_file_select_response, app);
        g_object_unref(dialog);
        config_save(app->config, CONFIG_FILE);
        
        html_viewer_free(app->viewer);
    }
    
    if (app->config) {
        config_free(app->config);
    }
}

int main(int argc, char **argv) {
    // 设置本地化
    setlocale(LC_ALL, "");
    bindtextdomain("html_server", "locale");
    textdomain("html_server");

    // 初始化GTK (使用正确的调用方式)
    if (!gtk_init_check()) {
        g_error("Failed to initialize GTK");
        return 1;
    }

    Application app = {0};
    app.app = gtk_application_new("org.example.htmlserver", 
        G_APPLICATION_HANDLES_OPEN);
    
    // 确保应用激活信号连接成功
    g_signal_connect(app.app, "activate", G_CALLBACK(on_activate), &app);
    
    // 不在此处初始化，在activate回调中初始化
    
    // 运行主事件循环
    int status = g_application_run(G_APPLICATION(app.app), argc, argv);
    
    // 清理资源
    application_cleanup(&app);
    g_object_unref(app.app);
    
    return status;
}
