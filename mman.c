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

/**
 * @brief Allocate memory and get a wrapped reference to it
 * 
 * @param size Number of bytes to allocate
 * @param cf Cleanup function, NULL if none is required
 * @return void* Pointer to the resource, NULL if no space left
 */
void *mman_alloc(size_t size, mman_cleanup_f_t cf)
{
  // Create new meta-info and return a pointer to the data block
  return mman_create(size, cf)->ptr;
}

/**
 * @brief Extend previously allocated memory with memory management.
 * This destroys the original resource and creates a copy which will
 * then be wrapped
 * 
 * @param size Number of bytes of the original object
 * @param ptr Pointer to the original object
 * @param cf Cleanup function, NULL if none is required
 * @return void* Pointer to the resource, NULL if no space left
 */
void *mman_from(size_t size, void *ptr, mman_cleanup_f_t cf)
{
  // Create meta-info and data block to copy over to
  mman_meta_t *meta = mman_create(size, cf);

  // Copy over data block
  memcpy(meta->ptr, ptr, size);

  // Free the old duplicate, also call cleanup on it
  if (cf) cf(ptr);
  free(ptr);
  ptr = NULL;

  // Return a pointer to the data block
  return meta->ptr;
}

/*
============================================================================
                                Deallocation                                
============================================================================
*/

/**
 * @brief Deallocate a wrapped resource when it goes out of scope
 * 
 * @param ptr_ptr Pointer to the pointer to the resource
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
    fprintf(stderr, "Invalid resource passed to \"cmm_dealloc\"!\n");
    return;
  }

  // Decrease number of active references
  // Do nothing as long as active references remain
  if (atomic_decrement(&meta->refs) > 0) return;

  // Free the resource, also call cleanup on it
  if (meta->cf) meta->cf(ptr);
  free(meta);
  ptr = NULL;

  // WARNING: This is a dev-debug
  printf("Freed a mman resource!\n");
}

/*
============================================================================
                                 Referencing                                
============================================================================
*/

/**
 * @brief Create a new reference to be shared with other consumers
 * 
 * @param ptr Pointer to the managed resource
 * @return void* Pointer to be shared
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