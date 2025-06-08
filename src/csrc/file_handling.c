#include "../headers/file_handling.h"

#include <cjson/cJSON.h> // JSON parsing library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Copies a string into a fixed-size buffer with trimming and null termination.
void trim_and_copy(char* dest, const char* src)
{
    strncpy(dest, src, MAX_STR_LEN - 1); // Copy at most MAX_STR_LEN - 1 chars
    dest[MAX_STR_LEN - 1] = '\0';        // Ensure null termination
}

// Converts a JSON file of users into a binary file.
// Assumes JSON is an array of objects with fields: "piadi", "saxeli", "gvari", "raioni"
void convert_to_bin(char* fname)
{
    // Open input JSON file
    FILE* in = fopen(fname, "r");
    if (!in)
    {
        perror("Failed to open input");
        exit(EXIT_FAILURE);
    }

    // Get file size
    fseek(in, 0, SEEK_END);
    long fsize = ftell(in);
    rewind(in);

    // Allocate buffer and read file content
    char* data = malloc(fsize + 1);
    if (fread(data, 1, fsize, in) != fsize)
    {
        perror("fread failed");
        fclose(in);
        free(data);
        exit(EXIT_FAILURE);
    }
    data[fsize] = '\0'; // Null-terminate
    fclose(in);

    // Parse JSON content
    cJSON* root = cJSON_Parse(data);
    free(data);

    // Validate JSON is an array
    if (!root || !cJSON_IsArray(root))
    {
        fprintf(stderr, "Invalid JSON\n");
        exit(EXIT_FAILURE);
    }

    // Open binary output file
    FILE* out = fopen("output.bin", "wb");
    if (!out)
    {
        perror("Failed to open output");
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }

    // Iterate through JSON array
    cJSON* user = root->child;
    int total = cJSON_GetArraySize(root);
    int i = 0;
    while (user)
    {
        if (!cJSON_IsObject(user))
        {
            user = user->next;
            continue;
        }

        // Extract fields with Georgian names
        cJSON* name = cJSON_GetObjectItem(user, "saxeli");   // "name"
        cJSON* surname = cJSON_GetObjectItem(user, "gvari"); // "surname"
        cJSON* id = cJSON_GetObjectItem(user, "piadi");      // "ID"
        cJSON* region = cJSON_GetObjectItem(user, "raioni"); // "region"

        // Skip if any required field is invalid
        if (!cJSON_IsString(id) || !cJSON_IsString(name) || !cJSON_IsString(surname) || !cJSON_IsString(region))
        {
            user = user->next;
            continue;
        }

        // Convert ID to int64
        int64_t user_id = strtoll(id->valuestring, NULL, 10);

        // Fill PackedUser structure
        PackedUser p;
        p.id = user_id;
        trim_and_copy(p.name, name->valuestring);
        trim_and_copy(p.surname, surname->valuestring);
        trim_and_copy(p.region, region->valuestring);

        // Write struct to binary file
        fwrite(&p, sizeof(PackedUser), 1, out);

        // Optional progress info every 1000 users
        if (++i % 1000 == 0)
            printf("Processed %d users\n", i);

        // Display progress bar more frequently
        if (i % 100 == 0 || i == total)
            print_progress(i, total);

        user = user->next;
    }

    // Cleanup
    fclose(out);
    cJSON_Delete(root);
    printf("Conversion complete.\n");
}

// Renders a dynamic ASCII progress bar in the terminal
void print_progress(int current, int total)
{
    const int bar_width = 50;
    float progress = (float) current / total;
    int pos = bar_width * progress;

    printf("[");
    for (int i = 0; i < bar_width; ++i)
    {
        if (i < pos)
            printf("=");
        else if (i == pos)
            printf(">");
        else
            printf(" ");
    }
    printf("] %.1f%% (%d/%d)\r", progress * 100.0, current, total);
    fflush(stdout);
}
