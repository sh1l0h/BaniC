#include "../include/utils.h"

char *get_base_name(char *path)
{
    size_t path_len = strlen(path);

    if (path_len == 0)
        return path;

    size_t i;
    for (i = path_len - 1; i > 0; i--) {
        if (path[i] == '/') {
            i++;
            break;
        }
    }

    return path + i;
}
