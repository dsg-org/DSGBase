#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include <stdbool.h>
#include <stdlib.h>

#define MAX_STR_LEN 64 // Maximum length for string fields in PackedUser

// Structure to represent a single user in a packed binary format
typedef struct
{
    int64_t id;
    int64_t license_id;
    int8_t gender; // 1 = male | 2 = female
    char name[MAX_STR_LEN];
    char surname[MAX_STR_LEN];
    char district[MAX_STR_LEN];
    char street[MAX_STR_LEN];
    char father[MAX_STR_LEN];
    char date_of_birth[MAX_STR_LEN];
    char date_of_registration[MAX_STR_LEN];
} PackedUser;

/**
 * Safely trims and copies a source string into a fixed-size destination buffer.
 * Ensures null termination and prevents buffer overflows.
 *
 * @param dest Destination buffer to copy into.
 * @param src  Source string to copy.
 */
void trim_and_copy(char* dest, const char* src);

/**
 * Converts a JSON file containing user data into a binary format file ("output.bin").
 *
 * @param fname Filename of the JSON file to convert.
 */
void convert_to_bin(char* fname);

/**
 * Displays a visual progress bar in the terminal for long-running operations.
 *
 * @param current The current count of processed items.
 * @param total   The total number of items to process.
 */
void print_progress(int current, int total);

#endif // FILE_HANDLING_H
