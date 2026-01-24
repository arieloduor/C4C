/****************************************************************************************************\
 * FILE: find-clone.c                                                                               *
 * AUTHOR: Michael Kamau                                                                            *
 *                                                                                                  *
 * PURPOSE: The program counts all files in a given directory, if none is given, it counts all      *
 *          files in all directories and counts the number of lines in each file,                   *
 *          the program also counts the total number of lines as well in al the files combined.     *
 *          As of the creation of this program, multiple directories as arguments are not supported.*
 *                                                                                                  *
 *  USAGE: To use the program, you can compile it with gcc or your favorite C compiler:             *
 *                gcc -o count find-clone.c                                                         *                                                                                        *
 *                                                                                                  *
 *                 with argument:                                                                   *
 *                      ./count <path>                                                              *
 *                 without argument with which a default of "../" is used:                          *
 *                      ./count                                                                     *
 *                                                                                                  *
 *                                                                                                  *
 *                                                                                                  *
 *                                                                                                  *
 \***************************************************************************************************/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH 250 /* The maximum length of a given path in an argument*/

typedef struct
{
    size_t count;
    int total_lines;
} DirInventory;


/**
 * A helper function to check if the file is a .hpp file
 */
bool is_hpp(char *full_path)
{
    char *ext = strrchr(full_path, '.');

    if (!ext)
        return false;
    else if (strcmp(ext, ".hpp") == 0)
        return true;
    else
        return false;
    
}

/**
 * This function counts the number of lines in a file,
 * if the file is not found, it just prints out the error instead of halting the process
 *
 */
int count_lines_in_file(const char *file_name)
{
    FILE *file = fopen(file_name, "r");
    if (!file)
    {
        perror(file_name);
        return 0;
    }

    char buff[1080];
    int lines = 0;

    while (fgets(buff, sizeof(buff), file) != NULL)
        lines++;

    fclose(file);

    printf("%s : %d lines\n", file_name, lines);
    return lines;
}

/**
 * This is the function to traverses through the directories
 * Just like the above function, when a directory is invalid,
 * it just prints out the error.
 *
 * The function ignores ".", "..", and ".git" diroctories,
 * i ignore .git, because they are not part of the core project folders.
 *
 * As of the creation of this program, symlinks are ignored
 */
DirInventory *traverse_and_count_lines(char *path, DirInventory *dir_inventory)
{
    DIR *dir = opendir(path);

    if (!dir)
    {
        perror(path);
        return dir_inventory;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".git") == 0)
            continue;

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;

        if (lstat(full_path, &st) == -1)
            continue;

        if (S_ISREG(st.st_mode) && is_hpp(full_path))
        {
            dir_inventory->count++;
            dir_inventory->total_lines += count_lines_in_file(full_path);
        }
        else if (S_ISDIR(st.st_mode))
        {
            traverse_and_count_lines(full_path, dir_inventory);
        }

        /* Ignore symlinks and the rest*/
    }

    closedir(dir);
    return dir_inventory;
}

int main(int argc, char *argv[])
{
    DirInventory inv = {0};
    char path[4096] = "../";

    if (argc == 2)
        snprintf(path, sizeof(path), "%s", argv[1]);
    else if (argc > 2)
    {
        fprintf(stderr, "Only one directory supported\n");
        return 1;
    }

    traverse_and_count_lines(path, &inv);

    printf("Files: %zu\n", inv.count);
    printf("Total lines: %d\n", inv.total_lines);
}
