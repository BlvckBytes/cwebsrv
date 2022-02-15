#ifndef htable_h
#define htable_h

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "util/mman.h"
#include "util/atomanip.h"
#include "util/strclone.h"

#define HTABLE_FNV_OFFSET 14695981039346656037UL
#define HTABLE_FNV_PRIME 1099511628211UL
#define HTABLE_MAX_KEYLEN 128UL

/**
 * @brief Cleanup function for entry values, used on removal
 */
typedef void (*htable_cf_t)(void *);

typedef enum
{
  HTABLE_SUCCESS,
  HTABLE_KEY_NOT_FOUND,
  HTABLE_KEY_ALREADY_EXISTS,
  HTABLE_KEY_TOO_LONG,
  HTABLE_FULL,
  HTABLE_NULL_VALUE,
} htable_result_t;

/**
 * @brief Represents an individual k-v pair entry in the table
 */
typedef struct htable_entry
{
  char *key;
  void *value;

  // Next link for the linked-list on this slot
  struct htable_entry *_next;
} htable_entry_t;

/**
 * @brief Represents a table having it's entries and a fixed size
 */
typedef struct
{
  // Actual table, list of entries
  htable_entry_t **slots;

  // Allocated number of slots
  size_t _slot_count;

  // Current number of items in the table
  size_t _item_count;

  // Maximum number of slots to be allocated when growing
  size_t _item_cap;

  // Cleanup function for the table items
  htable_cf_t _cf;
} htable_t;

/**
 * @brief Allocate a new, empty table
 * 
 * @param slot_count Amount of slots to allocate
 * @param item_cap Maximum number of items stored
 * @param cf Cleanup function for the items
 * @return htable_t* Pointer to the new table
 */
htable_t *htable_make(size_t slot_count, size_t item_cap, htable_cf_t cf);

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

#endif