### uritopath

This library can simply encode filepaths into a URI format and vice versa.

#### Installation

To use it, just clone this repository into your projects folder and include the files to your build tool.

#### Usage

Encode Filepath as URI

```c
const char *path = "/home/user/Documents/I use Arch BTW.txt";
char *uri = dyn_encpath(path);
if (uri) {
    printf("%s\n", uri);
    free(uri);
}
// Outputs: file:///home/user/Documents/I%20use%20Arch%20BTW.txt
```

Decode URI to Filepath

```c
const char *uri = "file:///home/user/Documents/I%20use%20Arch%20BTW.txt";
char *path = dyn_decuri(uri);
if (path) {
    printf("%s\n", path);
    free(path);
}
// Outputs: /home/user/Documents/I use Arch BTW.txt
```