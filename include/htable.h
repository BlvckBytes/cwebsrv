#ifndef htable_h
#define htable_h

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mman.h"

#define htable_FNV_OFFSET 14695981039346656037UL
#define htable_FNV_PRIME 1099511628211UL

typedef enum
{
  htable_SUCCESS,
  htable_KEY_NOT_FOUND,
  htable_KEY_ALREADY_EXISTS,
  htable_FULL,
} htable_result_t;

/**
 * @brief Stringifies an individual entry's value for visualization
 */
typedef char *(*htable_value_stringifier_t)(void *);

/**
 * @brief Represents an individual k-v pair entry in the table
 */
typedef struct
{
  char *key;
  void *value;
} htable_entry_t;

/**
 * @brief Represents a table having it's entries and a fixed size
 */
typedef struct
{
  // Actual table, list of entries
  htable_entry_t *table;

  // Current allocated size of the table
  size_t _table_size;

  // Maximum size the table can grow to
  size_t _table_cap;

  // Cleanup function for the table items
  mman_cleanup_f_t _cf;
} htable_t;

/**
 * @brief Allocate a new, empty table
 * 
 * @param table_size Size of the table
 * @param table_max_size Maximum size of the table, set to table_size for no automatic growth
 * @param cf Cleanup function for the items
 * @return htable_t* Pointer to the new table
 */
htable_t *htable_alloc(size_t table_size, size_t table_max_size, mman_cleanup_f_t cf);

/**
 * @brief Free a previously allocated table
 * 
 * @param ref Table reference
 */
void htable_free(void *ref);

/**
 * @brief Insert a new item into the table
 * 
 * @param table Table reference
 * @param key Key to connect with the value
 * @param elem Pointer to the value
 * 
 * @return htable_result_t Result of this operation
 */
htable_result_t htable_insert(htable_t *table, char *key, void *elem);

/**
 * @brief Remove an element by it's key
 * 
 * @param table Table reference
 * @param key Key connected to the target value
 * 
 * @return htable_result_t Result of this operation
 */
htable_result_t htable_remove(htable_t *table, char *key);

/**
 * @brief Get an existing key's connected value
 * 
 * @param table Table reference
 * @param key Key connected to the target value
 * @param output Output pointer buffer
 * 
 * @return htable_result_t Result of this operation
 */
htable_result_t htable_fetch(htable_t *table, char *key, void **output);

/**
 * @brief Get a list of all existing keys inside the table
 * 
 * @param table Table reference
 * @param output String array pointer buffer
 */
void htable_list_keys(htable_t *table, char ***output);

/**
 * @brief Print the table's key value pairs
 * 
 * @param table Table reference
 * @param stringifier Value stringifier function
 */
void htable_visualize(htable_t *table, htable_value_stringifier_t stringifier);

#endif