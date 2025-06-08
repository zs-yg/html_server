#include "string_utils.h"
#include <stdlib.h>
#include <string.h>

String* string_new(const gchar *src) {
    if (!src) return NULL;

    String *str = malloc(sizeof(String));
    if (!str) return NULL;

    str->length = strlen(src);
    str->str = malloc(str->length + 1);
    if (!str->str) {
        free(str);
        return NULL;
    }

    strcpy(str->str, src);
    return str;
}

void string_free(String *str) {
    if (!str) return;
    free(str->str);
    free(str);
}

String* string_concat(String *str1, String *str2) {
    if (!str1 || !str2) return NULL;

    String *result = malloc(sizeof(String));
    if (!result) return NULL;

    result->length = str1->length + str2->length;
    result->str = malloc(result->length + 1);
    if (!result->str) {
        free(result);
        return NULL;
    }

    strcpy(result->str, str1->str);
    strcat(result->str, str2->str);
    return result;
}

bool string_equals(String *str1, String *str2) {
    if (!str1 || !str2) return false;
    return strcmp(str1->str, str2->str) == 0;
}

String* string_dup(const String *src) {
    if (!src) return NULL;
    return string_new(src->str);
}

gchar* string_to_cstr(String *str) {
    if (!str) return NULL;
    return g_strdup(str->str);
}
