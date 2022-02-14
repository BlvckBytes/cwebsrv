#include "datastruct/dynarr.h"

dynarr_t *dynarr_alloc(size_t array_size, size_t array_max_size, mman_cleanup_f_t cf)
{
  scptr dynarr_t *res = mman_alloc(sizeof(dynarr_t), dynarr_free);

  res->_array_size = array_size;
  res->_array_cap = array_max_size;
  res->_cf = cf;

  // Allocate all slots and initialize them to NULL
  res->items = (void *) mman_alloc(sizeof(void *) * array_size, free);
  for (size_t i = 0; i < array_size; i++)
    res->items[i] = NULL;

  return mman_ref(res);
}

void dynarr_free(void *ref)
{
  dynarr_t *dynarr = (dynarr_t *) ref;

  // Clean up items
  if (dynarr->_cf)
  {
    for (size_t i = 0; i < dynarr->_array_size; i++)
      dynarr->_cf(dynarr->items[i]);
  }

  // Clean up pointers and struct
  mman_dealloc(dynarr->items);
  dynarr->items = NULL;
  mman_dealloc(dynarr);
}

static void dynarr_resize_arr(dynarr_t *arr, size_t new_size)
{
  // Resize memory block of the array
  scptr void *new_arr = mman_realloc(&arr->items, sizeof(void *) * new_size);
  arr->items = mman_ref(new_arr);
  
  // Initialize new slots
  for (size_t i = arr->_array_size; i < new_size; i++)
    arr->items[i] = NULL;
  
  // Keep track of the new size
  arr->_array_size = new_size;
}

// TODO: Size down as well
static bool dynarr_try_resize(dynarr_t *arr)
{
  size_t rem_cap = arr->_array_cap - arr->_array_size;
  if (rem_cap > 0)
  {
    // Try to double the amount of slots, go straight to the cap otherwise
    size_t new_size = arr->_array_size * 2;
    if (new_size > rem_cap)
      new_size = arr->_array_size + rem_cap;

    // Resized
    dynarr_resize_arr(arr, new_size);
    return true;
  }

  // Can't go any further
  return false;
}

dynarr_result_t dynarr_push(dynarr_t *arr, void *item, size_t *slot)
{
  // Iterate from tail to head
  for (size_t i = 0; i < arr->_array_size; i++)
  {
    // Search for a free slot
    if (arr->items[i] != NULL) continue;

    // Set item
    arr->items[i] = item;
    if (slot) *slot = i;
    return dynarr_SUCCESS;
  }

  if (dynarr_try_resize(arr))
    return dynarr_push(arr, item, slot);

  // No more free slots
  return dynarr_FULL;
}

dynarr_result_t dynarr_set_at(dynarr_t *arr, size_t index, void *item)
{
  // Index range check
  if (index < 0 || index >= arr->_array_size) return dynarr_INDEX_NOT_FOUND;

  // Free old entry, if any
  void *slot = arr->items[index];
  if (slot) arr->_cf(slot);

  slot = item;
  return dynarr_SUCCESS;
}

dynarr_result_t dynarr_pop(dynarr_t *arr, void **out, size_t *slot)
{
  // Iterate from tail to head
  for (size_t i = arr->_array_size - 1; i >= 0; i--)
  {
    // Search for an occupied slot
    if (arr->items[i] == NULL) continue;

    // Remove this item
    if (slot) *slot = i;
    return dynarr_remove_at(arr, i, out);
  }

  // No occupied slots
  return dynarr_EMPTY;
}

dynarr_result_t dynarr_remove_at(dynarr_t *arr, size_t index, void **out)
{
  // Range check
  if (index < 0 || index >= arr->_array_size) return dynarr_INDEX_NOT_FOUND;

  // Write pointer to output buffer, clear slot
  void *slot = arr->items[index];
  if (out) *out = slot;
  slot = NULL;
  return dynarr_SUCCESS;
}