#include "datastruct/htable.h"

/**
 * @brief Clean up an individual slot
 */
INLINED static void htable_slot_cleanup(htable_entry_t *slot, cleanup_fn_t cf)
{
  // Call the item free function, if applicable
  if (cf && slot->value) cf(slot->value);

  // Free the cloned string key
  mman_dealloc(slot->key);

  // Free the next chain in the linked list, if applicable
  if (slot->_next) htable_slot_cleanup(slot->_next, cf);
}

/**
 * @brief Clean up a no longer needed htable struct and it's slots
 */
INLINED static void htable_cleanup(mman_meta_t *ref)
{
  htable_t *table = (htable_t *) ref->ptr;

  // Free all table slots
  for (size_t i = 0; i < table->_slot_count; i++)
  {
    // Skip empty slots
    htable_entry_t *slot = table->slots[i];
    if (!slot) continue;

    htable_slot_cleanup(slot, table->_cf);
  }

  // Free the slot pointers
  mman_dealloc(table->slots);
}

htable_t *htable_make(size_t slot_count, size_t item_cap, cleanup_fn_t cf)
{
  htable_t *table = (htable_t *) mman_alloc(sizeof(htable_t), 1, htable_cleanup);
  
  table->_item_count = 0; // No freeing
  table->_slot_count = slot_count; // No freeing
  table->_item_cap = item_cap; // No freeing
  table->_cf = cf; // No freeing

  // Allocate all slots and initialize them to nullptrs
  table->slots = (htable_entry_t **) mman_alloc(sizeof(htable_entry_t *), slot_count, NULL); // needs mman freeing
  for (size_t i = 0; i < slot_count; i++)
    table->slots[i] = NULL;

  return mman_ref(table);
}

/**
 * @brief Generate a hash based on a string-key within the table slot index range
 * 
 * @param key String key to calculate on
 * @param slot_count Count of slots this table has
 * @return size_t Matching index in hash table
 */
INLINED static size_t htable_hash(char *key, size_t slot_count)
{
  // Start out at the specified offset
  size_t hash = HTABLE_FNV_OFFSET;

  // Apply bitops for each char in the string
  for (char *c = key; *c; c++)
  {
    hash ^= (size_t)(*c);
    hash *= HTABLE_FNV_PRIME;
  }

  return hash % slot_count;
}

htable_result_t htable_insert(htable_t *table, char *key, void *elem)
{
  // Already containing as many items as allowed
  if (table->_item_count >= table->_item_cap) return HTABLE_FULL;

  // Tried to insert a null value
  if (!elem) return HTABLE_NULL_VALUE;

  // Try to clone the key using a max-length
  scptr char *slot_key = strclone(key, HTABLE_MAX_KEYLEN);
  if (!slot_key) return HTABLE_KEY_TOO_LONG;

  // Find the target slot and create a new entry
  size_t slot_id = htable_hash(key, table->_slot_count);
  htable_entry_t **slot = &table->slots[slot_id];
  htable_entry_t *entry = (htable_entry_t *) mman_alloc(sizeof(htable_entry_t), 1, NULL); // needs mman freeing

  entry->key = mman_ref(slot_key);
  entry->value = elem;
  entry->_next = *slot;
  *slot = entry;

  // Increment item counter
  atomic_increment(&table->_item_count);
  return HTABLE_SUCCESS;
}

INLINED static htable_entry_t *find_entry(htable_t *table, char *key)
{
  size_t slot_id = htable_hash(key, table->_slot_count);
  htable_entry_t *slot = table->slots[slot_id];

  // Traverse linked list
  while (slot)
  {
    // Search slot that contains this key
    if (strncmp(key, slot->key, HTABLE_MAX_KEYLEN) == 0)
      return slot;

    slot = slot->_next;
  }

  // Not found
  return NULL;
}

bool htable_contains(htable_t *table, char *key)
{
  size_t slot_id = htable_hash(key, table->_slot_count);
  htable_entry_t *slot = table->slots[slot_id];

  // Traverse linked list
  htable_entry_t *prev_slot = NULL;
  while (slot)
  {
    // Search slot that contains this key
    if (strncmp(key, slot->key, HTABLE_MAX_KEYLEN) == 0)
      return true;

    prev_slot = slot;
    slot = slot->_next;
  }

  // Not found
  return false;
}

htable_result_t htable_remove(htable_t *table, char *key)
{
  size_t slot_id = htable_hash(key, table->_slot_count);
  htable_entry_t *slot = table->slots[slot_id];

  // Traverse linked list
  htable_entry_t *prev_slot = NULL;
  while (slot)
  {
    // Search slot that contains this key
    if (strncmp(key, slot->key, HTABLE_MAX_KEYLEN) == 0)
    {
      // Not the head, remove from linked list
      if (prev_slot)
        prev_slot->_next = slot->_next;

      // This is the head, set table's pointer
      else
        slot = slot->_next;

      // Deallocate and decrement item counter
      if (table->_cf) table->_cf(slot->value);
      mman_dealloc(slot);
      atomic_decrement(&table->_item_count);
      return HTABLE_SUCCESS;
    }

    prev_slot = slot;
    slot = slot->_next;
  }

  // Not found
  return HTABLE_KEY_NOT_FOUND;
}

htable_result_t htable_fetch(htable_t *table, char *key, void **output)
{
  htable_entry_t *entry = find_entry(table, key);

  if (entry)
  {
    *output = entry->value;
    return HTABLE_SUCCESS;
  }

  *output = NULL;
  return HTABLE_KEY_NOT_FOUND;
}

htable_result_t htable_append_table(htable_t *dest, htable_t *src, htable_append_mode_t mode)
{
  // Get all keys from the source
  char **keys;
  htable_list_keys(src, &keys);

  // Check if there are any collisions beforehand
  if (mode == HTABLE_AM_DUPERR)
  {
    // Iterate all available keys
    for (char **key = keys; *key; key++)
    {
      if (htable_contains(dest, *key))
        return HTABLE_KEY_ALREADY_EXISTS;
    }
  }

  // Iterate all available keys
  for (char **key = keys; *key; key++)
  {
    // Fetch this key's value
    char *value;
    if (htable_fetch(src, *key, (void *) &value) != HTABLE_SUCCESS)
      return HTABLE_KEY_NOT_FOUND;

    // Decide what mode to execute on this key
    htable_result_t insertion_result;

    if (mode == HTABLE_AM_OVERRIDE)
    {
      // Remove key if exists (don't even check errors, faster)
      htable_remove(dest, *key);
    }

    if (mode == HTABLE_AM_SKIP)
    {
      // Skip duplicate
      if (htable_contains(dest, *key)) continue;
    }

    // Insert new value
    if ((insertion_result = htable_insert(dest, *key, value)) != HTABLE_SUCCESS)
      return insertion_result;
  }

  return HTABLE_SUCCESS;
}

void htable_list_keys(htable_t *table, char ***output)
{
  *output = (char **) mman_alloc(sizeof(char *), table->_item_count + 1, NULL);

  size_t output_index = 0;
  for (size_t i = 0; i < table->_slot_count; i++)
  {
    // Skip empty slots
    htable_entry_t *slot = table->slots[i];

    // Traverse linked list
    while (slot && slot->key)
    {
      (*output)[output_index++] = slot->key;
      slot = slot->_next;
    }
  }

  // Terminate list
  (*output)[output_index] = 0;
}

char *htable_dump_hr_strs(htable_t *table)
{
  return htable_dump_hr(table, NULL);
}

char *htable_dump_hr(htable_t *table, stringifier_t stringifier)
{
  // Create a buffer for all the lines
  size_t buf_offs = 0;
  scptr char *buf = mman_alloc(sizeof(char), 8, NULL);

  // Iterate all slots
  for (size_t slot = 0; slot < table->_slot_count; slot++)
  {
    htable_entry_t *curr = table->slots[slot];

    // Append slot position
    if (!strfmt(&buf, &buf_offs, "[%lu] ", slot)) return NULL;

    // Print linked list contents
    while (curr && curr->key)
    {
      // Stringify value, if applicable
      char *stringified = stringifier ? stringifier(curr->value) : (char *) curr->value;

      if (!strfmt(
        &buf, &buf_offs,
        "%s (k=\"%s\", v=\"%s\")\n",
        "\t=>",
        curr->key,
        stringifier ? stringifier(curr->value) : (char *) curr->value
      )) return NULL;

      // Dealloc stringifier result, if applicable
      if (stringifier) mman_dealloc(stringified);

      // Go to next link
      curr = curr->_next;
    }

    // Print trailing "NULL _next"
    if (!strfmt(&buf, &buf_offs, "\t=> NULL\n")) return NULL;
  }

  // Terminate whole string
  buf[++buf_offs] = 0;
  return mman_ref(buf);
}