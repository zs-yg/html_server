#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <glib.h>
#include <stdbool.h>

typedef struct {
    gchar *str;
    gsize length;
} String;

String* string_new(const gchar *src);
void string_free(String *str);
String* string_concat(String *str1, String *str2);
bool string_equals(String *str1, String *str2);
String* string_dup(const String *src);
gchar* string_to_cstr(String *str);

#endif // STRING_UTILS_H
