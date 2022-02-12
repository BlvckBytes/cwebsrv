#include "htable.h"

htable_t *htable_alloc(size_t table_size)
{
  // Allocate the table itself
  htable_t *table = (htable_t *) malloc(sizeof(htable_t));

  // Set internal properties
  table->_table_size = table_size;

  // Allocate all slots and initialize them to NULL keys
  table->table = malloc(sizeof(htable_entry_t) * table_size);
  for (size_t i = 0; i < table_size; i++)
    table->table[i] = (htable_entry_t) { NULL, NULL };

  return table;
}

void htable_free(htable_t *table)
{
  // Free all table string keys
  for (size_t i = 0; i < table->_table_size; i++)
    free(table->table[i].key);

  // Free the actual table as well as it's container
  free(table->table);
  table->table = NULL;
  free(table);
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

  // Allocate space for the string
  entry->key = (char *) malloc(strlen(key) + 1);

  // Copy over and terminate
  const char *c = key;
  for (; *c; c++)
    entry->key[c - key] = *c;
  entry->key[c - key] = 0;

  // Set the value
  entry->value = elem;
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

    // Free slot reached, entry does not exist
    if (entry->key == NULL) return NULL;

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

  *output = (char **) malloc((num_keys + 1) * sizeof(char *));

  size_t output_index = 0;
  for (size_t i = 0; i < table->_table_size; ++i)
    if (table->table[i].key)
      (*output)[output_index++] = table->table[i].key;

  (*output)[output_index] = 0;
}