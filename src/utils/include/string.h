/****************************************************************************************************\
 * FILE: string.h                                                                                   *
 * AUTHOR: Michael Kamau                                                                            *
 *                                                                                                  *
 * PURPOSE: The program is an implementation of a dynamic string that can grow,                     *
 *          providing functions for creation, manipulation, and destruction of strings.             *
 *                                                                                                  *
 *  USAGE: To instantiate the program :                                                             *
 *                `String *str = string_create(capacity)`                                           *
 *                 capacity is the initial capacity of the string                                   *
 *                                                                                                  *
 *                 manipulating the string:                                                         *
 *                    -appending: string_append_cstr(str, cstr, len)                                *
 *                    -prepending: string_prepend_cstr(str, cstr, len)                              *
 *                    -substring: string_sub_str(str, from, to)                                     *
 *                    -get cstr: string_cstr(str)                                                   *
 *                    -destroy: string_destroy(str)                                                 *
 *                                                                                                  *
\****************************************************************************************************/

#ifndef C4C_STRINGS_H
#define C4C_STRINGS_H


#include "utils.hpp"

typedef struct String
{
    char *data;
    size_t length;
    size_t capacity;
} String;

/**
 * This function initializes a new string with given capacity
 * It takes one parameter:
 * 
 * - cap -> the initial capacity of the string
 * 
 * Returns a pointer to the new String or NULL on failure
 */

String *string_create(size_t cap)
{
    String *new_string = (String *)malloc(sizeof(String));
    if (!new_string)
    {
        return NULL;
    }

    new_string->capacity = cap;
    new_string->length = 0;
    new_string->data = (char *)calloc(cap, sizeof(char));
    if (!new_string)
    {
        free(new_string);
        return NULL;
    }

    return new_string;
}

/**
 * This function checks if the string is empty
 * It takes one parameter:
 * - s -> a pointer to the string to check
 * 
 * Returns true if the string is empty or NULL, false otherwise
 */

bool string_empty(const String *s)
{
    return s && s->length == 0;
}

/**
 * This function appends a C string to the end of the string
 * It takes three parameters:
 * - s -> a pointer to the string to append to
 * - cstr -> the C string to append
 * - len -> the length of the C string
 * 
 * Returns the modified string or NULL on failure
 */

String *string_append_cstr(String *s, const char *cstr, size_t len)
{
    if (!s || !cstr)
    {
        return s;
    }

    size_t needed = s->length + len + 1;

    if (needed > s->capacity)
    {
        size_t new_cap = needed * 2;
        char *tmp = (char *)realloc(s->data, new_cap);
        if (!tmp)
        {
            return NULL;
        }
        s->data = tmp;
        s->capacity = new_cap;
    }

    memcpy(s->data + s->length, cstr, len);
    s->length += len;
    s->data[s->length] = '\0';

    return s;
}

/**
 * This function prepends a C string to the beginning of the string
 * It takes three parameters:
 * - s -> a pointer to the string to prepend to
 * - cstr -> the C string to prepend
 * - len -> the length of the C string
 * 
 * Returns the modified string or NULL on failure
 */

String *string_prepend_cstr(String *s, const char *cstr, size_t len)
{
    if (!s || !cstr || len == 0)
    {
        return s;
    }

    size_t needed = s->length + len + 1;

    if (needed > s->capacity)
    {
        size_t new_cap = needed * 2;
        char *tmp = (char *)realloc(s->data, new_cap);
        if (!tmp)
        {
            return NULL;
        }
        s->capacity = new_cap;
    }

    memmove(s->data + len, s->data, s->length);
    memcpy(s->data, cstr, len);

    s->length += len;
    s->data[s->length] = '\0';

    return s;
}

/**
 * This function creates a substring from the given string
 * It takes three parameters:
 * - s -> a pointer to the source string
 * - from -> the starting index (inclusive)
 * - to -> the ending index (exclusive)
 * 
 * Returns a new String containing the substring or NULL on failure
 */
String *string_sub_str(const String *s, size_t from, size_t to)
{
    if (!s || from > to || to > s->length)
    {
        return NULL;
    }

    size_t len = to - from;
    String *out = string_create(len);
    if (!out)
    {
        return NULL;
    }

    memcpy(out->data, s->data + from, len);
    out->length = len;
    out->data[len] = '\0';

    return out;
}

/**
 * This function returns the C string representation of the string
 * It takes one parameter:
 * - s -> a pointer to the string
 * 
 * Returns the C string (char *) or NULL if s is NULL
 */
const char *string_cstr(const String *s)
{
    return s ? s->data : NULL;
}

/**
 * This function destroys the string and frees its memory
 * It takes one parameter:
 * - s -> a pointer to the string to destroy
 * 
 * Does nothing if s is NULL
 */
void string_destroy(String *s)
{
    if (!s)
    {
        return;
    }
    free(s->data);
    free(s);
}

#endif