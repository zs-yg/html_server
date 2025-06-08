#include "http_server.h"
#include <gio/gio.h>
#include <glib.h>
#include <string.h>
#include <gio/gnetworking.h>

static gboolean handle_connection(GSocketService *service,
                                GSocketConnection *connection,
                                GObject *source_object,
                                gpointer user_data) {
    (void)service; // 明确标记未使用参数
    (void)source_object; // 明确标记未使用参数
    HttpServer *server = (HttpServer *)user_data;
    GInputStream *input;
    GOutputStream *output;
    gchar buffer[1024];

    input = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    output = g_io_stream_get_output_stream(G_IO_STREAM(connection));

    g_input_stream_read(input, buffer, sizeof(buffer), NULL, NULL);

    const gchar *response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "\r\n"
                           "%s";

    gchar *full_response = g_strdup_printf(response, server->html_content);
    g_output_stream_write(output, full_response, strlen(full_response), NULL, NULL);
    g_free(full_response);

    return TRUE;
}

static gpointer server_thread_func(gpointer data) {
    HttpServer *server = (HttpServer *)data;
    GSocketService *service = g_socket_service_new();
    GError *error = NULL;
    GInetAddress *inet_addr = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);

    // 创建socket地址
    GSocketAddress *socket_addr = g_inet_socket_address_new(inet_addr, server->port);
    g_object_unref(inet_addr);

    // 设置服务属性
    g_socket_listener_set_backlog(G_SOCKET_LISTENER(service), 10);
    g_signal_connect(service, "incoming", G_CALLBACK(handle_connection), server);
    server->running = TRUE;

    // 绑定端口
    if (!g_socket_listener_add_address(G_SOCKET_LISTENER(service),
                                     socket_addr,
                                     G_SOCKET_TYPE_STREAM,
                                     G_SOCKET_PROTOCOL_DEFAULT,
                                     NULL,
                                     NULL,
                                     &error)) {
        g_warning("Failed to bind port %d: %s", server->port, error->message);
        g_error_free(error);
        g_object_unref(socket_addr);
        g_object_unref(service);
        server->running = FALSE;
        return NULL;
    }

    g_object_unref(socket_addr);

    // 启动服务
    g_socket_service_start(service);

    g_print("Server started successfully on port %d\n", server->port);

    // 创建独立事件循环
    GMainContext *context = g_main_context_new();
    GMainLoop *loop = g_main_loop_new(context, FALSE);
    g_main_context_push_thread_default(context);

    // 运行服务
    while (server->running) {
        if (g_main_context_iteration(context, FALSE) == FALSE) {
            g_usleep(10000); // 10ms
        }
    }

    // 清理资源
    g_main_loop_unref(loop);
    g_main_context_unref(context);
    g_object_unref(service);
    return NULL;
}

HttpServer* http_server_new(int port, const gchar *html_path) {
    HttpServer *server = g_new0(HttpServer, 1);
    server->port = port;
    
    GError *error = NULL;
    gchar *contents = NULL;
    gsize length = 0;
    
    if (g_file_get_contents(html_path, &contents, &length, &error)) {
        server->html_content = contents;
    } else {
        g_warning("Failed to load HTML file: %s", error->message);
        g_error_free(error);
        server->html_content = g_strdup("<html><body><h1>Error loading HTML file</h1></body></html>");
    }
    
    return server;
}

void http_server_free(HttpServer *server) {
    if (!server) return;
    
    if (server->server_thread) {
        http_server_stop(server);
    }
    
    g_free(server->html_content);
    g_free(server);
}

gboolean http_server_start(HttpServer *server) {
    if (!server) return FALSE;
    
    server->server_thread = g_thread_new("http-server", server_thread_func, server);
    return server->server_thread != NULL;
}

void http_server_stop(HttpServer *server) {
    if (!server || !server->server_thread) return;
    
    g_thread_join(server->server_thread);
    server->server_thread = NULL;
}
