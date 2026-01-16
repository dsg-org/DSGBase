#include "file_handling.h"

#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user_handling.h"

void trim_and_copy(char* dest, const char* src)
{
    strncpy(dest, src, MAX_STR_LEN - 1);
    dest[MAX_STR_LEN - 1] = '\0';
}

void convert_to_bin(char* fname)
{
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

    char* data = malloc(fsize + 1);
    if (fread(data, 1, fsize, in) != fsize)
    {
        perror("fread failed");
        fclose(in);
        free(data);
        exit(EXIT_FAILURE);
    }

    data[fsize] = '\0';
    fclose(in);

    cJSON* root = cJSON_Parse(data);
    free(data);

    if (!root || !cJSON_IsArray(root))
    {
        fprintf(stderr, "Invalid JSON\n");
        exit(EXIT_FAILURE);
    }

    FILE* out = fopen("src/output.bin", "wb");
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

    enum user_fields
    {
        USER_NAME,
        USER_SURNAME,
        USER_ID,
        DISTRICT,
        FATHER,
        FATHER_ID,
        MOTHER,
        MOTHER_ID
    };

    while (user)
    {
        if (!cJSON_IsObject(user))
        {
            user = user->next;
            continue;
        }

        PackedUser p;
        memset(&p, 0, sizeof(PackedUser));

        const char* keys[] = {"სახელი",
                              "გვარი",
                              "პირადი ნომერი",
                              "რაიონი",
                              "მამის სახელი",
                              "მამის პირადი",
                              "დედის სახელი",
                              "დედის პირადი"};

        cJSON* items[8];
        for (int k = 0; k < 8; k++)
            items[k] = cJSON_GetObjectItem(user, keys[k]);

        if (cJSON_IsString(items[USER_ID]))
            p.id = strtoll(items[USER_ID]->valuestring, NULL, 10);

        if (cJSON_IsString(items[FATHER_ID]))
            p.father_id = strtoll(items[FATHER_ID]->valuestring, NULL, 10);

        if (cJSON_IsString(items[MOTHER_ID]))
            p.mother_id = strtoll(items[MOTHER_ID]->valuestring, NULL, 10);

        int string_fields[] = {USER_NAME, USER_SURNAME, DISTRICT, FATHER, MOTHER};

        for (int k = 0; k < 5; k++)
        {
            int field_idx = string_fields[k];
            if (cJSON_IsString(items[field_idx]))
            {
                char* dest = (char*) &p + STRING_OFFSETS[k];
                trim_and_copy(dest, items[field_idx]->valuestring);
            }
        }

        fwrite(&p, sizeof(PackedUser), 1, out);

        if (++i % 1000 == 0)
            print_progress(i, total);
        user = user->next;
    }

    fclose(out);
    cJSON_Delete(root);
    printf("Conversion complete.\n");
}

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
