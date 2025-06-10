#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>

#define COLOR_RESET "\033[0m"
#define COLOR_DIR "\033[34m"
#define COLOR_LINK "\033[36m"
#define COLOR_EXEC "\033[32m"

typedef struct {
    char *name;
    struct stat st;
} FileEntry;

int cmp_name(const void *a, const void *b) {
    return strcmp(((FileEntry*)a)->name, ((FileEntry*)b)->name);
}

int cmp_name_rev(const void *a, const void *b) {
    return strcmp(((FileEntry*)b)->name, ((FileEntry*)a)->name);
}

int cmp_mtime(const void *a, const void *b) {
    time_t ta = ((FileEntry*)a)->st.st_mtime;
    time_t tb = ((FileEntry*)b)->st.st_mtime;
    return (tb - ta);
}

int cmp_size(const void *a, const void *b) {
    off_t sa = ((FileEntry*)a)->st.st_size;
    off_t sb = ((FileEntry*)b)->st.st_size;
    return (sb > sa) - (sb < sa);
}

int cmp_atime(const void *a, const void *b) {
    time_t ta = ((FileEntry*)a)->st.st_atime;
    time_t tb = ((FileEntry*)b)->st.st_atime;
    return (tb - ta);
}

int cmp_ctime(const void *a, const void *b) {
    time_t ta = ((FileEntry*)a)->st.st_ctime;
    time_t tb = ((FileEntry*)b)->st.st_ctime;
    return (tb - ta);
}

void print_permissions(mode_t mode) {
    char perms[11] = "----------";
    if (S_ISDIR(mode)) perms[0] = 'd';
    else if (S_ISLNK(mode)) perms[0] = 'l';
    else if (S_ISCHR(mode)) perms[0] = 'c';
    else if (S_ISBLK(mode)) perms[0] = 'b';
    else if (S_ISFIFO(mode)) perms[0] = 'p';
    else if (S_ISSOCK(mode)) perms[0] = 's';

    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';

    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';

    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';

    printf("%s ", perms);
}

void print_human_readable(off_t size) {
    const char *units[] = {"B", "K", "M", "G", "T"};
    int i = 0;
    double s = size;
    while (s >= 1024 && i < 4) {
        s /= 1024;
        i++;
    }
    printf("%.1f%s ", s, units[i]);
}

void print_inode(ino_t inode) {
    printf("%8lu ", (unsigned long)inode);
}

void print_mod_time(time_t mod_time) {
    char timebuf[64];
    struct tm *tm = localtime(&mod_time);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M", tm);
    printf(" %s", timebuf);
}

void print_filetype_suffix(mode_t mode) {
    if (S_ISDIR(mode))
        printf("/");
    else if (S_ISLNK(mode))
        printf("@");
    else if (mode & S_IXUSR)
        printf("*");
    else
        printf(" ");
}

void print_entry(FileEntry *fe, int long_format, int human_readable, int color,
                int show_inode, int show_modtime, int show_type_suffix) {
    if (show_inode) {
        print_inode(fe->st.st_ino);
    }

    if (long_format) {
        print_permissions(fe->st.st_mode);
        printf("%2ld ", (long)fe->st.st_nlink);
        struct passwd *pw = getpwuid(fe->st.st_uid);
        struct group *gr = getgrgid(fe->st.st_gid);
        printf("%s %s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        if (human_readable)
            print_human_readable(fe->st.st_size);
        else
            printf("%9ld ", (long)fe->st.st_size);
    }

    if (color) {
        if (S_ISDIR(fe->st.st_mode))
            printf(COLOR_DIR);
        else if (S_ISLNK(fe->st.st_mode))
            printf(COLOR_LINK);
        else if (fe->st.st_mode & S_IXUSR)
            printf(COLOR_EXEC);
    }

    printf("%s", fe->name);

    if (show_type_suffix) {
        print_filetype_suffix(fe->st.st_mode);
    }

    if (color)
        printf(COLOR_RESET);

    if ((long_format || show_modtime) && !show_type_suffix) {
        if (show_modtime) {
            print_mod_time(fe->st.st_mtime);
        }
    }

    if (long_format && S_ISLNK(fe->st.st_mode)) {
        char linktarget[1024];
        ssize_t len = readlink(fe->name, linktarget, sizeof(linktarget) - 1);
        if (len != -1) {
            linktarget[len] = '\0';
            printf(" -> %s", linktarget);
        }
    }

    printf("\n");
}

void list_dir(const char *path, int show_all, int long_format, int human_readable, int reverse,
              int sort_mtime, int sort_size, int recursive, int color, int show_dir_only,
              int show_inode, int show_modtime, int sort_atime, int sort_ctime, int show_type_suffix) {

    DIR *dp = opendir(path);
    if (!dp) {
        perror(path);
        return;
    }

    struct dirent *entry;
    FileEntry *files = NULL;
    int count = 0, capacity = 64;
    files = malloc(capacity * sizeof(FileEntry));
    if (!files) {
        perror("malloc");
        closedir(dp);
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (!show_all && entry->d_name[0] == '.')
            continue;

        if (count >= capacity) {
            capacity *= 2;
            files = realloc(files, capacity * sizeof(FileEntry));
            if (!files) {
                perror("realloc");
                closedir(dp);
                return;
            }
        }

        files[count].name = strdup(entry->d_name);

        // stat info
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        if (lstat(fullpath, &files[count].st) == -1) {
            perror(fullpath);
            free(files[count].name);
            continue;
        }

        if (show_dir_only && !S_ISDIR(files[count].st.st_mode)) {
            free(files[count].name);
            continue;
        }

        count++;
    }
    closedir(dp);

    // 정렬
    if (sort_atime)
        qsort(files, count, sizeof(FileEntry), reverse ? cmp_atime : cmp_atime);
    else if (sort_ctime)
        qsort(files, count, sizeof(FileEntry), reverse ? cmp_ctime : cmp_ctime);
    else if (sort_mtime)
        qsort(files, count, sizeof(FileEntry), reverse ? cmp_mtime : cmp_mtime);
    else if (sort_size)
        qsort(files, count, sizeof(FileEntry), reverse ? cmp_size : cmp_size);
    else
        qsort(files, count, sizeof(FileEntry), reverse ? cmp_name_rev : cmp_name);

    // 출력
    for (int i = 0; i < count; i++) {
        print_entry(&files[i], long_format, human_readable, color,
                    show_inode, show_modtime, show_type_suffix);
    }

    // 재귀 출력
    if (recursive) {
        for (int i = 0; i < count; i++) {
            if (S_ISDIR(files[i].st.st_mode) &&
                strcmp(files[i].name, ".") != 0 &&
                strcmp(files[i].name, "..") != 0) {
                printf("\n%s:\n", files[i].name);
                char nextpath[1024];
                snprintf(nextpath, sizeof(nextpath), "%s/%s", path, files[i].name);
                list_dir(nextpath, show_all, long_format, human_readable, reverse,
                         sort_mtime, sort_size, recursive, color, show_dir_only,
                         show_inode, show_modtime, sort_atime, sort_ctime, show_type_suffix);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(files[i].name);
    }
    free(files);
}

void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [options] [directory]\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -a       : Show all files including hidden\n");
    fprintf(stderr, "  -l       : Long listing format\n");
    fprintf(stderr, "  -h       : Human readable sizes\n");
    fprintf(stderr, "  -r       : Reverse order while sorting\n");
    fprintf(stderr, "  -t       : Sort by modification time\n");
    fprintf(stderr, "  -S       : Sort by file size\n");
    fprintf(stderr, "  -R       : Recursively list subdirectories\n");
    fprintf(stderr, "  --color  : Colorize the output\n");
    fprintf(stderr, "  -d       : List directories themselves, not contents\n");
    fprintf(stderr, "  -i       : Show inode number\n");
    fprintf(stderr, "  -m       : Show modification time after filename\n");
    fprintf(stderr, "  -u       : Sort by access time\n");
    fprintf(stderr, "  -c       : Sort by status change time\n");
    fprintf(stderr, "  -F       : Append indicator (one of */=@) to entries\n");
}

int main(int argc, char *argv[]) {
    int show_all = 0;
    int long_format = 0;
    int human_readable = 0;
    int reverse = 0;
    int sort_mtime = 0;
    int sort_size = 0;
    int recursive = 0;
    int color = 0;
    int show_dir_only = 0;

    int show_inode = 0;
    int show_modtime = 0;
    int sort_atime = 0;
    int sort_ctime = 0;
    int show_type_suffix = 0;

    char *path = ".";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) show_all = 1;
        else if (strcmp(argv[i], "-l") == 0) long_format = 1;
        else if (strcmp(argv[i], "-h") == 0) human_readable = 1;
        else if (strcmp(argv[i], "-r") == 0) reverse = 1;
        else if (strcmp(argv[i], "-t") == 0) sort_mtime = 1;
        else if (strcmp(argv[i], "-S") == 0) sort_size = 1;
        else if (strcmp(argv[i], "-R") == 0) recursive = 1;
        else if (strcmp(argv[i], "--color") == 0) color = 1;
        else if (strcmp(argv[i], "-d") == 0) show_dir_only = 1;
        else if (strcmp(argv[i], "-i") == 0) show_inode = 1;
        else if (strcmp(argv[i], "-m") == 0) show_modtime = 1;
        else if (strcmp(argv[i], "-u") == 0) sort_atime = 1;
        else if (strcmp(argv[i], "-c") == 0) sort_ctime = 1;
        else if (strcmp(argv[i], "-F") == 0) show_type_suffix = 1;
        else if (argv[i][0] == '-') {
            usage(argv[0]);
            return 1;
        } else {
            path = argv[i];
        }
    }

    list_dir(path, show_all, long_format, human_readable, reverse,
             sort_mtime, sort_size, recursive, color, show_dir_only,
             show_inode, show_modtime, sort_atime, sort_ctime, show_type_suffix);

    return 0;
}
