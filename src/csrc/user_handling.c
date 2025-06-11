#include "user_handling.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthash.h>

#include "file_handling.h"

// Global hash table pointer
Data* person = NULL;

// Load users from a binary file (generated elsewhere) and populate hash table.
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
        add_user(p.id, p.name, p.surname, p.region);
        count++;
    }

    printf("Loaded %d users from binary file.\n", count);
    fclose(fp);
}

// Adds a user to the hash table, ensuring uniqueness by ID.
void add_user(const int64_t id, const char* name, const char* surname, const char* region)
{
    Data* s;

    HASH_FIND(hh, person, &id, sizeof(int64_t), s);
    if (s == NULL)
    {
        s = malloc(sizeof *s);
        if (!s)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        s->name = s->surname = s->region = NULL;
        s->id = id;

        HASH_ADD(hh, person, id, sizeof(int64_t), s); /* id: name of key field */
    }
    else
    {
        return;
    }
    s->name = strdup(name);
    if (!s->name)
        goto cleanup;
    s->surname = strdup(surname);
    if (!s->surname)
        goto cleanup;
    s->region = strdup(region);
    if (!s->region)
        goto cleanup;
    return;

cleanup:
    if (s->name)
        free(s->name);
    if (s->surname)
        free(s->surname);
    if (s->region)
        free(s->region);
    HASH_DEL(person, s);
    free(s);
    exit(EXIT_FAILURE);
}

// Searches the in-memory users by given filters and prints matches.
void search_users(const User* filters)
{
    Data *current, *tmp;
    int found_count = 0;

    HASH_ITER(hh, person, current, tmp)
    {
        bool match = true;

        if (filters->id_set && current->id != filters->id)
            match = false;
        if (filters->name && strcmp(current->name, filters->name) != 0)
            match = false;
        if (filters->surname && strcmp(current->surname, filters->surname) != 0)
            match = false;
        if (filters->region && strcmp(current->region, filters->region) != 0)
            match = false;

        if (match)
        {
            if (found_count > 0)
                puts(",");
            printf("  {\n");
            printf("    \"id\": \"%lld\",\n", (long long) current->id);
            printf("    \"name\": \"%s\",\n", current->name);
            printf("    \"surname\": \"%s\",\n", current->surname);
            printf("    \"region\": \"%s\" \n", current->region);
            printf("  }");
            found_count++;
        }

        // printf("Found %d matching users.\n", found_count);
    }
}

// Prints a list of all unique surnames found in the user hash table.
void print_surname(void)
{
    Data* current_user;
    SurnameSet* surname_set = NULL;
    SurnameSet* s;

    puts("Unique surname list:\n");

    for (current_user = person; current_user != NULL; current_user = current_user->hh.next)
    {
        // Check if surname is already in set
        HASH_FIND_STR(surname_set, current_user->surname, s);
        if (!s)
        {
            // Not found, print and add to set
            printf("%s\n", current_user->surname);

            s = malloc(sizeof(SurnameSet));
            s->surname = strdup(current_user->surname);
            HASH_ADD_KEYPTR(hh, surname_set, s->surname, strlen(s->surname), s);
        }
    }

    // Cleanup: free the set
    SurnameSet* tmp;
    HASH_ITER(hh, surname_set, s, tmp)
    {
        HASH_DEL(surname_set, s);
        free(s->surname);
        free(s);
    }
}

// Frees memory used by the main user hash table.
void cleanup_hash_table(void)
{
    Data *current, *tmp;
    HASH_ITER(hh, person, current, tmp)
    {
        HASH_DEL(person, current);
        free(current->name);
        free(current->surname);
        free(current->region);
        free(current);
    }
}
