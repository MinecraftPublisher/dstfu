#include <CoreServices/CoreServices.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int pointer = 0;

int EndsWith(const char *str, const char *suffix) {
    if (!str || !suffix) return 0;
    size_t lenstr    = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr) return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void callback(
    ConstFSEventStreamRef         streamRef,
    void                         *clientCallBackInfo,
    size_t                        numEvents,
    void                         *eventPaths,
    const FSEventStreamEventFlags eventFlags[],
    const FSEventStreamEventId    eventIds[]) {
    char **paths = eventPaths;
    for (size_t i = 0; i < numEvents; ++i) {
        if (strstr(paths[ i ], ".DS_Store") != NULL) {
            unlink(paths[ i ]);
            // printf("New .DS_Store file detected: %s\n", paths[ i ]);
        }
    }
}

#define LATENCY 1.0

void monitorDirectory(const char *path) {
    CFStringRef pathToWatch  = CFStringCreateWithCString(NULL, path, kCFStringEncodingUTF8);
    CFArrayRef  pathsToWatch = CFArrayCreate(NULL, (const void **) &pathToWatch, 1, NULL);

    FSEventStreamContext context = { 0, NULL, NULL, NULL, NULL };
    FSEventStreamRef     stream  = FSEventStreamCreate(
        NULL,
        &callback,
        &context,
        pathsToWatch,
        kFSEventStreamEventIdSinceNow,
        LATENCY,
        kFSEventStreamCreateFlagFileEvents);

    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
    FSEventStreamSetDispatchQueue(stream, queue);

    FSEventStreamStart(stream);

    CFRunLoopRun();

    FSEventStreamStop(stream);
    FSEventStreamInvalidate(stream);
    FSEventStreamRelease(stream);
    CFRelease(pathsToWatch);
    CFRelease(pathToWatch);
}

void delete_ds_store(const char *path) {
    pointer++;
    if (pointer > 5) pointer = 0;
    if (pointer == 0) sleep(1);

    DIR           *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL) {
        // fprintf(stderr, "Cannot open directory %s\n", path);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char full_path[ 4096 ];
        snprintf(full_path, sizeof(full_path), "%s%s/", path, entry->d_name);
        // printf("path %s\n", full_path);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        } else if (entry->d_type == DT_DIR) {
            delete_ds_store(full_path);
        } else if (strstr(entry->d_name, ".DS_Store") != NULL) {
            unlink(full_path);
            // printf("Deleted .DS_Store file: %s\n", full_path);
        }
    }

    closedir(dir);
}

#define ARG "__S_S_S_S__R_R_C__TD_F_E_G_T_G_V_E_"

typedef char *string;

int main(int argc, string *argv) {
    char buf[1024];
    // magic
    snprintf(buf, sizeof(buf), "(ps axco pid,command | grep dstfu | awk '{print $1}' | grep -v %i | xargs kill -9)", getpid());
    system(buf);

    // printf("Executable: %s (%i)\n", argv[ 0 ], getpid());
    const char *root_directory = "/";

    if (fork()) {
        exit(0);
    } else {
        printf("dstfu is running on the background.\n");
        if (fork()) {
            while (1) {
                delete_ds_store(root_directory);
                // printf("LOOP OVER FS COMPLETED\n");
                sleep(5);
            }
        } else {
            monitorDirectory(root_directory);
        }
    }

    return 0;
}