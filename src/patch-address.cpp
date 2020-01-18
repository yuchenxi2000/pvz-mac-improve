#include "patch-address.hpp"
vm_address_t patch_addr_begin, patch_addr_ptr, patch_addr_end;

void AllocPatchMemory(Memory& memory, vm_size_t size) {
    patch_addr_begin = memory.Allocate(size);
    patch_addr_ptr = patch_addr_begin;
    patch_addr_end = patch_addr_begin + size;
}
