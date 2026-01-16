#include "user_handling.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>

#include "file_handling.h"

Data* person = NULL;

void load_users_from_json(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("Binary file open failed");
        exit(EXIT_FAILURE);
    }

    PackedUser p;
    int count = 0;
    while (fread(&p, sizeof(PackedUser), 1, fp) == 1)
    {
        int str_count = sizeof(STRING_OFFSETS) / sizeof(STRING_OFFSETS[0]);
        int id_count = sizeof(ID_OFFSETS) / sizeof(ID_OFFSETS[0]);
        const char* str_attrs[str_count + 1];
        int64_t ids[id_count + 1];

        for (int i = 0; i < str_count; i++)
            str_attrs[i] = (char*) &p + STRING_OFFSETS[i];
        str_attrs[str_count] = NULL;

        for (int i = 0; i < id_count; i++)
            ids[i] = *(int64_t*) ((char*) &p + ID_OFFSETS[i]);
        ids[id_count] = -1;

        add_user(ids, str_attrs);
        count++;
    }

    printf("Loaded %d users from binary file.\n", count);
    fclose(fp);
}

void add_user(const int64_t* ids, const char** user_strings)
{
    Data* s;

    HASH_FIND(hh, person, &ids[0], sizeof(int64_t), s);
    if (s != NULL)
        return; // User already exists

    s = malloc(sizeof(Data));
    if (!s)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(s, 0, sizeof(Data));

    int id_count = sizeof(ID_OFFSETS) / sizeof(ID_OFFSETS[0]);

    for (int i = 0; i < id_count; i++)
    {
        int64_t* dest_addr = (int64_t*) ((char*) &s->user + ID_OFFSETS[i]);
        *dest_addr = ids[i];
    }

    int str_count = sizeof(STRING_OFFSETS) / sizeof(STRING_OFFSETS[0]);
    for (int i = 0; i < str_count; i++)
    {
        if (user_strings[i] != NULL)
        {
            char* dest_buffer = (char*) &s->user + STRING_OFFSETS[i];

            trim_and_copy(dest_buffer, user_strings[i]);
        }
    }

    HASH_ADD_KEYPTR(hh, person, &s->user.id, sizeof(int64_t), s);
}

void search_users(const User* filters)
{
    Data *current, *tmp;
    int found_count = 0;

    int id_count = sizeof(ID_OFFSETS) / sizeof(ID_OFFSETS[0]);
    int str_count = sizeof(STRING_OFFSETS) / sizeof(STRING_OFFSETS[0]);

    const char* filter_str[str_count];
    int64_t filter_int[id_count] = {};

    for (int i = 0; i < str_count; i++)
        filter_str[i] = (char*) &filters->packed_user + STRING_OFFSETS[i];

    for (int i = 0; i < id_count; i++)
        filter_int[i] = *(int64_t*) ((char*) &filters->packed_user + ID_OFFSETS[i]);

    HASH_ITER(hh, person, current, tmp)
    {
        bool match = true;

        for (int i = 0; i < id_count && filter_int[i] != 0; i++)
        {
            int64_t val = *(int64_t*) ((char*) &current->user + ID_OFFSETS[i]);
            if (val != filter_int[i])
            {
                match = false;
                break;
            }
        }

        if (!match)
            continue;

        for (int i = 0; i < str_count; i++)
            if (filter_str[i][0] != '\0')
            {
                char* val = (char*) &current->user + STRING_OFFSETS[i];
                if (strcmp(val, filter_str[i]) != 0)
                {
                    match = false;
                    break;
                }
            }

        if (match)
        {
            if (found_count)
                puts(",");
            printf("  {\n");
            printf("    \"id\": \"%ld\",\n", current->user.id);
            printf("    \"name\": \"%s\",\n", current->user.name);
            printf("    \"surname\": \"%s\",\n", current->user.surname);
            printf("    \"district\": \"%s\", \n", current->user.district);
            printf("    \"father\": \"%s\", \n", current->user.father);
            printf("    \"father_id\": \"%ld\", \n", current->user.father_id);
            printf("    \"mother\": \"%s\", \n", current->user.mother);
            printf("    \"mother_id\": \"%ld\" \n", current->user.mother_id);
            printf("  }");
            found_count++;
        }
    }
}

void print_surname(void)
{
    Data* current_user;
    SurnameSet* surname_set = NULL;
    SurnameSet* s;

    puts("Unique surname list:\n");

    for (current_user = person; current_user != NULL; current_user = current_user->hh.next)
    {
        HASH_FIND_STR(surname_set, current_user->user.surname, s);
        if (!s)
        {
            printf("%s\n", current_user->user.surname);

            s = malloc(sizeof(SurnameSet));
            s->surname = strdup(current_user->user.surname);
            HASH_ADD_KEYPTR(hh, surname_set, s->surname, strlen(s->surname), s);
        }
    }

    SurnameSet* tmp;
    HASH_ITER(hh, surname_set, s, tmp)
    {
        HASH_DEL(surname_set, s);
        free(s->surname);
        free(s);
    }
}

void cleanup_hash_table(void)
{
    Data *current, *tmp;
    HASH_ITER(hh, person, current, tmp)
    {
        HASH_DEL(person, current);
        free(current);
    }
}
