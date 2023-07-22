#ifndef URITOPATH_H
#define URITOPATH_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Converts a file uri in format "file://C:\Users\User%201" or "file:///home/User%201"
 * to a system path like "C:\Users\User 1" or "/home/User 1"
 * 
 * @param uri URI to decode
 * @return Returns a pointer to the decoded uri or NULL if allocation fails or the uri is not valid fileuri
*/
char* dyn_decuri(const char *uri);

/**
 * Converts a path string in format "C:\Users\User 1" or "/home/User 1"
 * into a file uri like "file://C:\Users\User%201" or "file:///home/User%201"
 * 
 * @param path Path to encode
 * @return Returns a pointer to the encoded path or NULL if allocation fails
*/
char* dyn_encpath(const char* path);

#ifdef __cplusplus
}
#endif

#endif
