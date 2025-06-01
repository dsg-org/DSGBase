#include <cjson/cJSON.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>

#define MAX_STR_LEN 64

typedef struct
{
    int32_t id;
    char name[MAX_STR_LEN];
    char surname[MAX_STR_LEN];
    char region[MAX_STR_LEN];
    char* in;
} PackedUser;

typedef struct data
{
    int id;     // Person's ID
    char* name; // Person's name
    char* surname;
    char* region;
    UT_hash_handle hh; /* makes this structure hashable */
} Data;

Data* person = NULL;

typedef struct user
{
    char* fname;
    char* name;
    char* surname;
    char* region;
    int id;
    bool id_set;
} User;

void trim_and_copy(char*, const char*);
void convert_to_bin(char*);
void print_progress(int, int);
void load_users_from_json(const char*);
void add_user(const int, const char*, const char*, const char*);
Data* find_by_id(int);
void search_users_by_name(const char* name);
void search_users_by_surname(const char* surname);
void search_users_by_region(const char* region);

int bflag = 0;
int fflag = 0;
int nflag = 0;
int sflag = 0;
int iflag = 0;
int rflag = 0;

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "No file flag passed.\n");
        return EXIT_FAILURE;
    }

    User* s;
    PackedUser* p;
    s = calloc(1, sizeof(User));
    p = calloc(1, sizeof(PackedUser));

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
                case 'b':
                    if (p->in)
                    {
                        fprintf(stderr, "Can not convert multiple files to binary.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        bflag = 1;
                        p->in = strdup(argv[i]);
                    }
                    break;
                case 'f':
                    if (s->fname)
                    {
                        fprintf(stderr, "Can not accept multiple file names.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        fflag = 1;
                        s->fname = strdup(argv[i]);
                    }
                    break;
                case 'n':
                    if (s->name)
                    {
                        fprintf(stderr, "Can not accept multiple names.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        nflag = 1;
                        s->name = strdup(argv[i]);
                    }
                    break;
                case 's':
                    if (s->surname)
                    {
                        fprintf(stderr, "Can not accept multiple surnames.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        sflag = 1;
                        s->surname = strdup(argv[i]);
                    }
                    break;
                case 'i':
                    if (s->id_set)
                    {
                        fprintf(stderr, "Can not accept multiple IDs.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        iflag = 1;
                        s->id = atoi(argv[i]);
                        s->id_set = true;
                    }
                    break;
                case 'r':
                    if (s->region)
                    {
                        fprintf(stderr, "Can not accept multiple regions.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        rflag = 1;
                        s->region = strdup(argv[i]);
                    }
                    break;
                default:
                    fprintf(stderr, "Unknown flag: -%c\n", argv[i][1]);
                    free(s);
                    return EXIT_FAILURE;
            }
        }
    }

    if (bflag)
    {
        puts("Converting JSON to Binary.");
        convert_to_bin(p->in);
        puts("Successfully converted JSON to Binary.\n");
        goto cleanup;
    }

    if (fflag)
    {
        puts("Reading data from json.");
        load_users_from_json(s->fname);
        puts("Successfully loaded data from json.\n");
    }

    if (iflag)
    {
        puts("Searching for user by ID...\n");
        Data* found = find_by_id(s->id);
        if (found)
        {
            printf("Name: %s\nSurname: %s\nID: %d\nRegion: %s\n",
                   found->name,
                   found->surname,
                   found->id,
                   found->region);
        }
        else
        {
            printf("User with ID %d not found.\n", s->id);
        }
        goto cleanup;
    }

    if (nflag)
    {
        search_users_by_name(s->name);
        goto cleanup;
    }

    if (sflag)
    {
        search_users_by_surname(s->surname);
        goto cleanup;
    }

    if (rflag)
    {
        search_users_by_region(s->region);
        goto cleanup;
    }

cleanup:
    free(s->fname);
    free(s->name);
    free(s->surname);
    free(s->region);
    free(s);
    if (p)
    {
        if (p->in)
            free(p->in);
        free(p);
    }
    return EXIT_SUCCESS;
}

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

    FILE* out = fopen("output.bin", "wb");
    if (!out)
    {
        perror("Failed to open output");
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }

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

        cJSON* name = cJSON_GetObjectItem(user, "saxeli");
        cJSON* surname = cJSON_GetObjectItem(user, "gvari");
        cJSON* id = cJSON_GetObjectItem(user, "piadi");
        cJSON* region = cJSON_GetObjectItem(user, "raioni");

        if (!cJSON_IsString(id) || !cJSON_IsString(name) || !cJSON_IsString(surname) || !cJSON_IsString(region))
        {
            user = user->next;
            continue;
        }
        int32_t user_id = atoi(id->valuestring);

        PackedUser p;
        p.id = user_id;
        trim_and_copy(p.name, name->valuestring);
        trim_and_copy(p.surname, surname->valuestring);
        trim_and_copy(p.region, region->valuestring);

        fwrite(&p, sizeof(PackedUser), 1, out);

        if (++i % 1000 == 0)
            printf("Processed %d users\n", i);

        user = user->next;
        if (i % 100 == 0 || i == total)
            print_progress(i, total);
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

void load_users_from_json(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("Binary file open failed");
        exit(EXIT_FAILURE);
    }

    PackedUser p;
    while (fread(&p, sizeof(PackedUser), 1, fp) == 1)
    {
        add_user(p.id, p.name, p.surname, p.region);
    }

    fclose(fp);
};

void add_user(const int id, const char* name, const char* surname, const char* region)
{
    Data* s;

    HASH_FIND_INT(person, &id, s);
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

        HASH_ADD_INT(person, id, s); /* id: name of key field */
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

Data* find_by_id(int id)
{
    Data* s;

    HASH_FIND_INT(person, &id, s); /* s: output pointer */
    return s;
}

void search_users_by_name(const char* name)
{
    Data *s, *tmp;
    int found = 0;

    puts("Searching for user by Name...\n");

    HASH_ITER(hh, person, s, tmp)
    {
        if (strcmp(s->name, name) == 0)
        {
            printf("ID: %d\nName: %s\nSurname: %s\nRegion: %s\n\n", s->id, s->name, s->surname, s->region);
            found++;
        }
    }

    if (found == 0)
    {
        printf("No users found in Name: %s\n", name);
    }
}

void search_users_by_surname(const char* surname)
{
    Data *s, *tmp;
    int found = 0;

    puts("Searching for user by Surname...\n");

    HASH_ITER(hh, person, s, tmp)
    {
        if (strcmp(s->surname, surname) == 0)
        {
            printf("ID: %d\nName: %s\nSurname: %s\nRegion: %s\n\n", s->id, s->name, s->surname, s->region);
            found++;
        }
    }

    if (found == 0)
    {
        printf("No users found in Surname: %s\n", surname);
    }
}

void search_users_by_region(const char* region)
{
    Data *s, *tmp;
    int found = 0;

    puts("Searching for user by Region...\n");

    HASH_ITER(hh, person, s, tmp)
    {
        if (strcmp(s->region, region) == 0)
        {
            printf("ID: %d\nName: %s\nSurname: %s\nRegion: %s\n\n", s->id, s->name, s->surname, s->region);
            found++;
        }
    }

    if (found == 0)
    {
        printf("No users found in Region: %s\n", region);
    }
}
