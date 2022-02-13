#include "htable.h"

htable_t *htable_alloc(size_t table_size, size_t table_max_size, mman_cleanup_f_t cf)
{
  htable_t *table = (htable_t *) mman_alloc(sizeof(htable_t), htable_free);

  table->_table_size = table_size;
  table->_table_cap = table_max_size;
  table->_cf = cf;

  // Allocate all slots and initialize them to NULL keys
  table->table = (htable_entry_t *) mman_alloc(sizeof(htable_entry_t) * table_size, free);
  for (size_t i = 0; i < table_size; i++)
    table->table[i] = (htable_entry_t) { NULL, NULL };

  return mman_ref(table);
}

void htable_free(void *ref)
{
  htable_t *table = (htable_t *) ref;

  // Free all table entries
  for (size_t i = 0; i < table->_table_size; i++)
  {
    // Call the item free function on all items
    if (table->_cf) table->_cf(table->table[i].value);

    // Free the duped string
    free(table->table[i].key);
  }

  // Free the actual table as well as it's container
  mman_dealloc(table->table);
  table->table = NULL;
  mman_dealloc(table);
}

size_t htable_hash(char *key, size_t table_size)
{
  // Start out at the specified offset
  size_t hash = htable_FNV_OFFSET;

  // Apply bitops for each char in the string
  for (char *c = key; *c; c++)
  {
    hash ^= (size_t)(*c);
    hash *= htable_FNV_PRIME;
  }

  return hash % table_size;
}

void htable_write_slot(htable_t *table, size_t slot, char *key, void *elem)
{
  htable_entry_t *entry = &table->table[slot];

  // Free old string in case it exists
  if (entry->key)
    free(entry->key);

  // Copy over key
  entry->key = strdup(key);

  // Set the value
  entry->value = elem;
}

/**
 * @brief Internal routine for resizing the hash table
 * 
 * @param table Table to resize
 * @param new_size New size of the table
 */
static void htable_resize(htable_t *table, size_t new_size)
{
  // Resize memory block of the table
  mman void *new_table = mman_realloc(&table->table, sizeof(htable_entry_t) * new_size);
  table->table = mman_ref(new_table);
  
  // Initialize new slots
  for (size_t i = table->_table_size; i < new_size; i++)
    table->table[i] = (htable_entry_t) { NULL, NULL };
  
  // Keep track of the new size
  table->_table_size = new_size;
}

htable_result_t htable_insert(htable_t *table, char *key, void *elem)
{
  size_t index = htable_hash(key, table->_table_size);

  // Key already exists
  if (table->table[index].key != NULL)
  {
    for (size_t i = 0; i < table->_table_size; i++)
    {
      size_t slot = (i + index) % table->_table_size;
      htable_entry_t *entry = &table->table[slot];

      // Slot already occupied
      if (entry->key != NULL)
      {
        // Key already in the table
        if (strcmp(entry->key, key) == 0)
          return htable_KEY_ALREADY_EXISTS;

        continue;
      }

      // Take up first available free slot
      htable_write_slot(table, slot, key, elem);
      return htable_SUCCESS;
    }

    // TODO: Size down as well (on removal)
    size_t rem_cap = table->_table_cap - table->_table_size;
    if (rem_cap > 0)
    {
      // Try to double the amount of slots, go straight to the cap otherwise
      size_t new_size = table->_table_size * 2;
      if (new_size > rem_cap)
        new_size = table->_table_size + rem_cap;

      htable_resize(table, new_size);

      // Retry insertion
      return htable_insert(table, key, elem);
    }

    // Could not find another empty slot
    return htable_FULL;
  }

  // Slot is not yet occupied, success
  htable_write_slot(table, index, key, elem);
  return htable_SUCCESS;
}

htable_entry_t *find_entry(htable_t *table, char *key)
{
  size_t slot = htable_hash(key, table->_table_size);

  for (size_t i = 0; i < table->_table_size; i++)
  {
    htable_entry_t *entry = &table->table[(i + slot) % table->_table_size];

    // Slot holds nothing
    if (entry->key == NULL) continue;

    // Not in current slot!
    if (strcmp(entry->key, key) != 0) continue;
    return entry;
  }

  // Not found
  return NULL;
}

htable_result_t htable_remove(htable_t *table, char *key)
{
  htable_entry_t *entry = find_entry(table, key);

  // Entry does not exist
  if (!entry) return htable_KEY_NOT_FOUND;

  // Remove by freeing the string key again and nulling the value pointer
  free(entry->key);
  entry->key = NULL;
  entry->value = NULL;
  return htable_SUCCESS;
}

htable_result_t htable_fetch(htable_t *table, char *key, void **output)
{
  htable_entry_t *entry = find_entry(table, key);

  if (entry)
  {
    *output = entry->value;
    return htable_SUCCESS;
  }

  *output = NULL;
  return htable_KEY_NOT_FOUND;
}

void htable_visualize(htable_t *table, htable_value_stringifier_t stringifier)
{
  for (size_t i = 0; i < table->_table_size; i++)
  {
    htable_entry_t entry = table->table[i];

    char *str_val = entry.value ? stringifier(entry.value) : "NULL";

    printf(
      "table[%s] = %s\n",
      (entry.key == NULL ? "NULL" : entry.key),
      str_val
    );

    if (entry.value)
      free(str_val);
  }
}

void htable_list_keys(htable_t *table, char ***output)
{
  size_t num_keys = 0;

  for (size_t i = 0; i < table->_table_size; ++i)
    if (table->table[i].key) num_keys++;

  *output = (char **) mman_alloc((num_keys + 1) * sizeof(char *), NULL);

  size_t output_index = 0;
  for (size_t i = 0; i < table->_table_size; ++i)
    if (table->table[i].key)
      (*output)[output_index++] = table->table[i].key;

  (*output)[output_index] = 0;
}