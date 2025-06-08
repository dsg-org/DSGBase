#include <stdbool.h>
#include <stdlib.h>
#include <uthash.h>

/**
 * Struct representing a user loaded in memory.
 * Used as entries in a hash table, keyed by `id`.
 */
typedef struct data
{
    int64_t id;        // Unique identifier
    char* name;        // First name
    char* surname;     // Last name
    char* region;      // Region or locality
    UT_hash_handle hh; // uthash handle for hashing by ID
} Data;

/**
 * User filter struct for searching users.
 * Fields are used as search criteria, any NULL/false means "don't filter by that".
 */
typedef struct user
{
    char* fname;   // Optional field name (not used in logic, likely legacy or reserved)
    char* name;    // Name filter
    char* surname; // Surname filter
    char* region;  // Region filter
    int64_t id;    // ID to filter by
    bool id_set;   // Whether the ID filter is active
} User;

/**
 * Temporary struct used to track unique surnames.
 * Used in `print_surname` to avoid duplicates.
 */
typedef struct SurnameSet
{
    char* surname;     // A surname string
    UT_hash_handle hh; // uthash handle
} SurnameSet;

/**
 * Opens a binary file and reads `PackedUser` structs one by one.
 * Converts them into heap-allocated `Data` structs and adds to hash table.
 * Prints the number of users loaded.
 */
void load_users_from_json(const char*);

/**
 * Adds a user with given ID, name, surname, and region to the global hash table.
 * Prevents duplicate IDs. On memory allocation failure, cleans up and exits.
 */
void add_user(const int64_t id, const char* name, const char* surname, const char* region);

/**
 * Searches through all users in the hash table.
 * Applies filters from the given `User*` struct.
 * Fields that are NULL or false (`id_set == false`) are ignored in filtering.
 * Matching users are printed in JSON-like format.
 */
void search_users(const User*);

/**
 * Frees all memory associated with the main user hash table.
 * Each `Data` node and its strings are individually freed.
 */
void cleanup_hash_table(void);

/**
 * Iterates through all users and prints each unique surname once.
 * Internally uses a temporary uthash `SurnameSet` to track which surnames were printed.
 */
void print_surname(void);
