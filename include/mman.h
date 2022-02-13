#ifndef mman_h
#define mman_h

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
============================================================================
                                   Macros                                   
============================================================================
*/

// Marks a memory managed variable
#define mman __attribute__((cleanup(mman_dealloc)))

// Marks an always inlined function
#define INLINED __attribute__((always_inline)) inline

/*
============================================================================
                                  Typedefs                                  
============================================================================
*/

/**
 * @brief Cleanup function used for external destructuring of a resource
 */
typedef void (*mman_cleanup_f_t)(void *);

/**
 * @brief Meta-information of a memory managed resource
 */
typedef struct
{
  // Pointer to the resource
  void *ptr;

  // Cleanup function invoked before the resource gets free'd
  mman_cleanup_f_t cf;

  // Number of active references pointing at this resource
  volatile size_t refs;
} mman_meta_t;

/*
============================================================================
                                 Allocation                                 
============================================================================
*/

/**
 * @brief Allocate memory and get a wrapped reference to it
 * 
 * @param size Number of bytes to allocate
 * @param cf Cleanup function, NULL if none is required
 * @return void* Pointer to the resource, NULL if no space left
 */
void *mman_alloc(size_t size, mman_cleanup_f_t cf);

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
void *mman_from(size_t size, void *ptr, mman_cleanup_f_t cf);

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
void mman_dealloc(void *ptr_ptr);

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
void *mman_ref(void *ptr);

#endif