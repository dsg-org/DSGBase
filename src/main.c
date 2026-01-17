#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "file_handling.h"
#include "string.h"
#include "user_handling.h"

#define MAX_STR_LEN 64

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s [OPTIONS]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -b <json_file>    Convert JSON file to binary\n");
        fprintf(stderr, "  -f <binary_file>  Specify binary file to search\n");
        fprintf(stderr, "  -n <name>         Search by name\n");
        fprintf(stderr, "  -s <surname>      Search by surname\n");
        fprintf(stderr, "  -i <id>           Search by ID\n");
        fprintf(stderr, "  -r <district>     Search by district\n");
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

    char* fname = NULL;
    char* bname = NULL;

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
                    if (bname)
                    {
                        fprintf(stderr, "Can not convert multiple files to binary.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        bflag = true;
                        bname = strdup(argv[i]);
                    }
                    break;
                case 'f':
                    if (fname)
                    {
                        fprintf(stderr, "Can not accept multiple file names.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        fflag = true;
                        fname = strdup(argv[i]);
                    }
                    break;
                case 'n':
                    if (*s->packed_user.name)
                    {
                        fprintf(stderr, "Can not accept multiple names.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        nflag = true;
                        strcpy(s->packed_user.name, argv[i]);
                    }
                    break;
                case 's':
                    if (*s->packed_user.surname)
                    {
                        fprintf(stderr, "Can not accept multiple surnames.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        sflag = true;
                        strcpy(s->packed_user.surname, argv[i]);
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
                        s->packed_user.id = strtoll(argv[i], NULL, 10);
                        s->id_set = true;
                    }
                    break;
                case 'p':
                    pflag = true;
                    break;

                case 'r':
                    if (*s->packed_user.district)
                    {
                        fprintf(stderr, "Can not accept multiple districts.\n");
                        free(s);
                        return EXIT_FAILURE;
                    }
                    i++;
                    if (i < argc)
                    {
                        rflag = true;
                        strcpy(s->packed_user.district, strdup(argv[i]));
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
        convert_to_bin(bname);
        puts("Successfully converted JSON to Binary.\n");
        goto cleanup;
    }

    if (pflag)
    {
        if (fflag && fname)
        {
            fprintf(stderr, "Loading users from binary file: %s\n", fname);
            load_users_from_json(fname);
        }
        print_surname();
        goto cleanup;
    }

    if (nflag || sflag || iflag || rflag)
    {
        if (fflag && fname)
        {
            fprintf(stderr, "Loading users from binary file: %s\n", fname);
            load_users_from_json(fname);
        }
        else
        {
            fprintf(stderr, "Error: No binary file specified with -f flag for searching.\n");
            goto cleanup;
        }

        fprintf(stderr, "Performing filtered search...\n");
        puts("[");
        search_users(s);
        puts("\n]");
        goto cleanup;
    }

cleanup:
    free(fname);
    free(s);
    if (p)
        free(p);

    cleanup_hash_table();

    return EXIT_SUCCESS;
}
