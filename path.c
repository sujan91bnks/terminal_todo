#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mach-o/dyld.h>

int main() {
    char path[PATH_MAX];
    uint32_t size = sizeof(path);

    if (_NSGetExecutablePath(path, &size) == 0) {
        printf("Executable path: %s\n", path);
    } else {
        // buffer too small, allocate required size
        char *path2 = malloc(size);
        if (_NSGetExecutablePath(path2, &size) == 0) {
            printf("Executable path: %s\n", path2);
        }
        free(path2);
    }

    return 0;
}
