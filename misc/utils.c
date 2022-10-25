#include "../headers/utils.h"
#include "../headers/config.h"
#include "../headers/config_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

char * strnstr(const char *s, const char *find, size_t slen)
{
    char c, sc;
    size_t len;

    if ((c = *find++) != '\0') {
        len = strlen(find);
        do {
            do {
                if (slen-- < 1 || (sc = *s++) == '\0')
                    return (NULL);
            } while (sc != c);
            if (len > slen)
                return (NULL);
        } while (strncmp(s, find, len) != 0);
        s--;
    }
    return ((char *)s);
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    return !stat(path, &path_stat) &&
           (S_ISREG(path_stat.st_mode));
}

char* trim(const char *src)
{
    char *dst = malloc(sizeof(char)*strlen(src));
    int k=0;
    for (int j = 0; src[j] != '\0'; j++) {

        if (!(src[j] == ' ' || src[j] == '\t' || src[j] == '\n')) {
            dst[k] = src[j];
            k++;
        }
    }
    dst[k] = 0;

    return dst;
}