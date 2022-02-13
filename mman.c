#include "mman.h"

/*
============================================================================
                                  Atomical                                  
============================================================================
*/

/**
 * @brief Atomically add to a given number
 * 
 * @param target Target number to add to
 * @param value Value to add
 * @return size_t New value of the variable
 */
INLINED static size_t atomic_add(volatile size_t *target, const size_t value)
{
  size_t old, new;

  // Try to compare and swap atomically until succeeded
  do {
    old = *target;
    new = old + value;
  } while (!__sync_bool_compare_and_swap(target, old, new));

  // Return the new value
  return new;
}

/**
 * @brief Atomically add one to a given number
 * 
 * @param target Target number to increment
 * @return size_t New value of the variable
 */
INLINED static size_t atomic_increment(volatile size_t *target)
{
  return atomic_add(target, 1);
}

/**
 * @brief Atomically remove one from a given number
 * 
 * @param target Target number to decrement
 * @return size_t New value of the variable
 */
INLINED static size_t atomic_decrement(volatile size_t *target)
{
  return atomic_add(target, -1);
}

/*
============================================================================
                                 Allocation                                 
============================================================================
*/

/**
 * @brief Allocate a new meta-info structure as well as it's trailing data block
 * 
 * @param size Size of the data block in bytes
 * @param cf Cleanup function
 * @return mman_meta_t Pointer to the meta-info
 */
INLINED static mman_meta_t *mman_create(size_t size, mman_cleanup_f_t cf)
{
  mman_meta_t *meta = (mman_meta_t *) malloc(sizeof(mman_meta_t) + size);

  *meta = (mman_meta_t) {
    .ptr = meta + 1, // Resource starts right after metadata
    .cf = cf, // Set cleanup function
    .refs = 1 // Starting out with one reference
  };

  return meta;
}

void *mman_alloc(size_t size, mman_cleanup_f_t cf)
{
  // Create new meta-info and return a pointer to the data block
  return mman_create(size, cf)->ptr;
}

void *mman_realloc(void *ptr_ptr, size_t new_size)
{
  // Receiving a pointer to the pointer to the reference, deref once
  void *ptr = *((void **) ptr_ptr);

  // Fetch the meta info allocated before the data block
  mman_meta_t *meta = ptr - sizeof(mman_meta_t);
  if (ptr != meta->ptr)
  {
    fprintf(stderr, "Invalid resource passed to \"mman_realloc\"!\n");
    return NULL;
  }

  // Reallocate whole meta object
  meta = realloc(meta, new_size);

  // Update pointer and return updated pointer
  meta->ptr = meta + 1;
  return meta->ptr;
}

/*
============================================================================
                                Deallocation                                
============================================================================
*/

void mman_dealloc(void *ptr_ptr)
{
  // Receiving a pointer to the pointer to the reference, deref once
  void *ptr = *((void **) ptr_ptr);

  // Do nothing for already free'd resources
  if (ptr == NULL) return;

  // Fetch the meta info allocated before the data block
  mman_meta_t *meta = ptr - sizeof(mman_meta_t);
  if (ptr != meta->ptr)
  {
    fprintf(stderr, "Invalid resource passed to \"mman_dealloc\"!\n");
    return;
  }

  // Decrease number of active references
  // Do nothing as long as active references remain
  if (atomic_decrement(&meta->refs) > 0) return;

  // Free the resource, also call cleanup on it
  if (meta->cf) meta->cf(ptr);
  free(meta);
  ptr = NULL;
}

/*
============================================================================
                                 Referencing                                
============================================================================
*/

void *mman_ref(void *ptr)
{
  // Fetch the meta info allocated before the data block
  mman_meta_t *meta = ptr - sizeof(mman_meta_t);
  if (ptr != meta->ptr)
  {
    fprintf(stderr, "Invalid resource passed to \"cmm_dealloc\"!\n");
    return NULL;
  }

  // Increment number of references and return pointer to the data block
  atomic_increment(&meta->refs);
  return meta->ptr;
}