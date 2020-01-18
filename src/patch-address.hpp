#ifndef patch_address_hpp
#define patch_address_hpp

#include "memory.h"

extern vm_address_t patch_addr_begin, patch_addr_ptr, patch_addr_end;

void AllocPatchMemory(Memory& memory, vm_size_t size);

#endif /* patch_address_hpp */
