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

int get_line_count(const char* fname)
{
    FILE* f = fopen(fname, "r");
    int lines = 0;
    while (!feof(f))
    {
        if (fgetc(f) == '\n')
            lines++;
    }
    fclose(f);
    return lines;
}

void convert_to_bin(char* fname)
{
    FILE* in = fopen(fname, "r");
    if (!in)
    {
        perror("Input fail");
        exit(1);
    }

    FILE* out = fopen("src/output.bin", "wb");
    if (!out)
    {
        fclose(in);
        perror("Output fail");
        exit(1);
    }

    char line[16384]; // Buffer for a single JSON object line
    int i = 0;
    int total_records = get_line_count(fname);

    // We can't get total count easily from a stream without a pre-scan,
    // but we can estimate or just print count.
    printf("Converting stream...\n");

    enum user_fields
    {
        USER_NAME,
        USER_SURNAME,
        USER_ID,
        STREET,
        FATHER,
        GENDER,
        DATE_OF_BIRTH,
        DATE_OF_REGISTRATION,
        LICENSE_ID,
        DISTRICT,
    };

    const char* keys[] = {"სახელი",
                          "გვარი",
                          "პირადი ნომერი",
                          "ქუჩა",
                          "მამის სახელი",
                          "სქესი",
                          "დაბადების თარიღი",
                          "რეგისტრაციის თარიღი",
                          "მოწმობის ნომერი",
                          "რეგისტრაციის ადგილი"};

    while (fgets(line, sizeof(line), in))
    {
        if (line[0] == '[' || line[0] == ']' || line[0] == ',' || line[0] == '\n')
            continue;

        cJSON* user = cJSON_Parse(line);
        if (!user)
            continue;

        PackedUser p;
        memset(&p, 0, sizeof(PackedUser));

        int num_keys = sizeof(keys) / sizeof(keys[0]);
        cJSON* items[num_keys];
        for (int k = 0; k < num_keys; k++)
            items[k] = cJSON_GetObjectItem(user, keys[k]);

        if (cJSON_IsString(items[USER_ID]))
            p.id = strtoll(items[USER_ID]->valuestring, NULL, 10);

        if (cJSON_IsString(items[LICENSE_ID]))
            p.license_id = strtoll(items[LICENSE_ID]->valuestring, NULL, 10);

        if (cJSON_IsNumber(items[GENDER]))
            p.gender = items[GENDER]->valueint;

        int string_fields[] = {USER_NAME, USER_SURNAME, STREET, FATHER, DATE_OF_BIRTH, DATE_OF_REGISTRATION, DISTRICT};
        int str_count = sizeof(string_fields) / sizeof(string_fields[0]);

        for (int k = 0; k < str_count; k++)
        {
            int field_idx = string_fields[k];
            if (cJSON_IsString(items[field_idx]))
            {
                char* dest = (char*) &p + STRING_OFFSETS[k];
                trim_and_copy(dest, items[field_idx]->valuestring);
            }
        }

        fwrite(&p, sizeof(PackedUser), 1, out);

        cJSON_Delete(user);

        if (++i % 1000 == 0)
            print_progress(i, total_records);
    }

    fclose(in);
    fclose(out);
    printf("Conversion complete. Processed %d records.\n", i);
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
