#include <stdio.h>
#include <sys/statvfs.h>
#include <mntent.h>
#include <string.h>
#include <stdbool.h>

// Colores ANSI
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

double to_gb(unsigned long long bytes) {
    return (double)bytes / (1024 * 1024 * 1024);
}

void print_bar(double percent) {
    int width = 20;
    int pos = (int)(width * (percent / 100.0));
    
    printf(" [");
    for (int i = 0; i < width; i++) {
        if (i < pos) printf("#");
        else printf("-");
    }
    printf("] %.1f%%", percent);
}

bool is_real_disk(struct mntent *ent) {
    if (strncmp(ent->mnt_fsname, "/dev/", 5) == 0) return true;
    if (strcmp(ent->mnt_type, "drvfs") == 0 || strcmp(ent->mnt_type, "9p") == 0) {
        if (strncmp(ent->mnt_dir, "/mnt/", 5) == 0) return true;
    }
    if (strcmp(ent->mnt_dir, "/") == 0) return true;
    return false;
}

int main(int argc, char *argv[]) {
    bool show_bar = (argc > 1 && strcmp(argv[1], "--t") == 0);
    struct mntent *ent;
    struct statvfs s;
    FILE *mounts;

    printf(BOLD "%-15s %-10s %10s %10s %10s", "DRIVE", "TYPE", "USED", "FREE", "TOTAL");
    if (show_bar) printf("   PROGRESS");
    printf(RESET "\n----------------------------------------------------------------------\n");

    mounts = setmntent("/proc/mounts", "r");
    if (!mounts) return 1;

    while ((ent = getmntent(mounts)) != NULL) {
        if (is_real_disk(ent)) {
            if (statvfs(ent->mnt_dir, &s) == 0) {
                unsigned long long total = (unsigned long long)s.f_blocks * s.f_frsize;
                unsigned long long avail = (unsigned long long)s.f_bavail * s.f_frsize;
                unsigned long long used = total - avail;

                if (total > 0) {
                    double percent = ((double)used / total) * 100.0;
                    
                    // Color según el uso
                    if (percent > 90.0) printf(RED);
                    else if (percent > 70.0) printf(YELLOW);
                    else printf(GREEN);

                    printf("%-15s %-10s %9.1fG %9.1fG %9.1fG", 
                           ent->mnt_dir, ent->mnt_type, to_gb(used), to_gb(avail), to_gb(total));

                    if (show_bar) print_bar(percent);
                    
                    printf(RESET "\n");
                }
            }
        }
    }

    endmntent(mounts);
    return 0;
}
