#ifndef COMPAT_ACEALLOC_H
#define COMPAT_ACEALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Free a buffer back
 *
 *  This function free a buffer back with a specified module id and buffer type
 *
 *  @param[in] module_id The ACE module id
 *  @param[in] buf_type The memory buffer type
 *  @param[in] p Pointer to the buffer to be freed
 */
// void aceAlloc_free(aceModules_moduleId_t module_id, aceAlloc_bufferType_t buf_type, void* p);
void shadow_aceAlloc_free(void* p);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACEALLOC_H
