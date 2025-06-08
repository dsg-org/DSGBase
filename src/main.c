#include <stdbool.h>
#include <stdio.h>

#include "./headers/file_handling.h"
#include "./headers/user_handling.h"

#define MAX_STR_LEN 64

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        // If no search flags are provided, show usage
        fprintf(stderr, "Usage: %s [OPTIONS]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -b <json_file>    Convert JSON file to binary\n");
        fprintf(stderr, "  -f <binary_file>  Specify binary file to search\n");
        fprintf(stderr, "  -n <name>         Search by name\n");
        fprintf(stderr, "  -s <surname>      Search by surname\n");
        fprintf(stderr, "  -i <id>           Search by ID\n");
        fprintf(stderr, "  -r <region>       Search by region\n");
        fprintf(stderr, "  -p                \n");

        return EXIT_FAILURE;
    }

    bool bflag = false;
    bool fflag = false;
    bool nflag = false;
    bool sflag = false;
    bool iflag = false;
    bool pflag = false;
    bool rflag = false;

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
                        bflag = true;
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
                        fflag = true;
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
                        nflag = true;
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
                        sflag = true;
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
                        iflag = true;
                        s->id = strtoll(argv[i], NULL, 10);
                        s->id_set = true;
                    }
                    break;
                case 'p':
                    pflag = true;
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
                        rflag = true;
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

    if (pflag)
    {
        if (fflag && s->fname)
        {
            printf("Loading users from binary file: %s\n", s->fname);
            load_users_from_json(s->fname);
        }
        print_surname();
        goto cleanup;
    }

    if (nflag || sflag || iflag || rflag)
    {
        // Load the binary file if filename is provided
        if (fflag && s->fname)
        {
            printf("Loading users from binary file: %s\n", s->fname);
            load_users_from_json(s->fname);
        }
        else
        {
            fprintf(stderr, "Error: No binary file specified with -f flag for searching.\n");
            goto cleanup;
        }

        puts("Performing filtered search...\n");
        puts("[");
        search_users(s);
        puts("\n]");
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

    // Clean up the hash table
    cleanup_hash_table();

    return EXIT_SUCCESS;
}
