#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define FILEPREFIX "file://"
#define FILEPREFIXSIZE strlen(FILEPREFIX)

char* dyn_decuri(const char *uri) {
    // Check if the file contains a fileprefix
    if (strncmp(uri, FILEPREFIX, FILEPREFIXSIZE) != 0) {
        return NULL;
    }
    // Skip the prefix
    uri+=FILEPREFIXSIZE;
    // Initial allocate some mem
    size_t cap = strlen(uri) + 1;
    size_t ix_path = 0;
    char* path = malloc(cap);
    if (!path) {
        return NULL;
    }

    for (char c; (c = *uri) != '\0'; uri++) {
        if (ix_path + 2 >= cap) {
            cap *= 1.5;
            path = realloc(path, cap);
            if (!path) {
                return NULL;
            }
        }
        if (c == '%') {
            int val;
            // Checks if first and second digit is letter and then parses it (e.g. %20)
            if (isxdigit(*(uri + 1)) && isxdigit(*(uri + 2)) && sscanf(uri + 1, "%02x", &val) == 1) {
                *(path+ix_path) = val;
                uri+=2;
            }
        } else if (c == '+') {
            *(path+ix_path) = ' ';
        } else {
            *(path+ix_path) = c;
        }
        ix_path++;
    }
    *(path+ix_path) = '\0';
    return path;
}

char* dyn_encpath(const char* path) {
    // Initial allocate some mem
    size_t cap = strlen(path) + FILEPREFIXSIZE + 1;
    // index uri is the index of the block in the uri string
    size_t ix_uri = 0;
    char* uri = malloc(cap);
    if (!uri) {
        return NULL;
    }
    // Add Prefix
    strcpy(uri, FILEPREFIX);
    ix_uri += FILEPREFIXSIZE;
    
    for (char c; (c = *path) != '\0'; path++) {
        if (ix_uri + 4 >= cap) {
            cap *= 2;
            uri = realloc(uri, cap);
            if (!uri) {
                return NULL;
            }
        }
        if (!isalnum(c) && c != '/' && c != '.') {
            sprintf(uri+ix_uri, "%%%02X", (unsigned char)c);
            ix_uri += 3;
        } else {
            *(uri + ix_uri) = c;
            ix_uri++;
        }
    }
    *(uri + ix_uri) = '\0';
    return uri;
}
