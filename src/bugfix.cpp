#include "bugfix.hpp"
using namespace asmjit;
using namespace asmjit::x86;
void BugFix::FixUseCard() {
    vm_address_t len;
    /*
     patch:
     cmp dword [ebp+0x14], 0x0
     js 2a266       // 右键取消卡片选择
     mov eax, dword [ebp+0x8]
     mov dword [esp], eax
     call get_hold_card_id_117ec
     cmp eax, 0x34  // card_id <= 0x34: plant
     jle 2a2cf      // 放植物
     cmp eax, 0x3c
     jl 2a266       // 取消放置
     cmp eax, 0x4a
     jg 2a266       // 取消放置
     jmp 2a2a4      // 放僵尸
     */
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = *patch.as;
        
        as.cmp(dword_ptr(ebp, 0x14), 0x0);
        as.js(0x2a266);
        
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call(0x117ec);
        as.cmp(eax, 0x34);
        as.jle(0x2a2cf);
        as.cmp(eax, 0x3c);
        as.jl(0x2a266);
        as.cmp(eax, 0x4a);
        as.jg(0x2a266);
        as.jmp(0x2a2a4);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    /*
     2a260: (length=6)
     jmp patch
     */
    {
        Patch patch(memory, 0x2a260);
        auto& as = *patch.as;
        
        as.jmp(patch_addr_ptr);
        
        patch.patch();
    }
    patch_addr_ptr += len;
}

void BugFix::FixClickCard() {
    vm_address_t len;
    /*
     patch:
     jle f96c6 // id not exist
     
     cmp eax, 0x38
     je sun
     cmp eax, 0x39
     je diamond
     cmp eax, 0x3a
     je zombie
     cmp eax, 0x3b
     je cup
     
     cmp eax, 0x4a
     jg f96c6 // id not exist
     cmp eax, 0x34
     jg not_plant
     jmp f8aa4 // plant
     not_plant:
     cmp eax, 0x3b
     jg f8aa4 // zombie
     
     mov eax, dword [ebp+0x8]
     mov eax, dword [eax]
     mov eax, dword [eax+0x7c0]
     cmp eax, 0x14 // 宝石迷阵
     je f8aa4
     cmp eax, 0x18 // 宝石迷阵2
     je f8aa4
     jne f96c6 // id may crash game
     
     cup:
     mov eax, dword [ebp+0x8]
     mov eax, dword [eax+0x4]
     mov eax, dword [eax+0x154]
     mov dword [esp], eax
     mov dword [esp+0x4], 0x2
     mov dword [esp+0x8], 0x0
     call a6164
     jmp f96c6
     sun:
     mov eax, dword [ebp+0x8]
     mov eax, dword [eax+0x4]
     mov edx, dword [eax+0x5554]
     lea edx, dword [edx+1000]
     mov dword [eax+0x5554], edx
     jmp f96c6
     diamond:
     mov eax, dword [ebp+0x8]
     mov eax, dword [eax+0x4]
     mov dword [esp], eax
     mov dword [esp+0x4], 400
     mov dword [esp+0x8], 300
     mov dword [esp+0xc], 3
     mov dword [esp+0x10], 3
     call 28e02
     jmp f96c6
     zombie:
     mov eax, dword [ebp+0x8]
     mov eax, dword [eax]
     mov eax, dword [eax+0x7c0]
     cmp eax, 0x17
     je f8aa4
     mov dword [esp+0xc], 0
     mov dword [esp+0x8], -2
     mov dword [esp+0x4], 0xb
     mov eax, dword [ebp+0x8]
     mov eax, dword [eax+0x4]
     mov eax, dword [eax+0x154]
     mov dword [esp], eax
     call ac672
     jmp f96c6
     */
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label cup = as.newLabel();
        Label not_plant = as.newLabel();
        Label sun = as.newLabel();
        Label diamond = as.newLabel();
        Label zombie = as.newLabel();
        uint32_t discard = 0xf96c6;
        uint32_t go_on = 0xf8aa4;
        
        as.jle(discard);
        as.cmp(eax, 0x38);
        as.je(sun);
        as.cmp(eax, 0x39);
        as.je(diamond);
        as.cmp(eax, 0x3a);
        as.je(zombie);
        as.cmp(eax, 0x3b);
        as.je(cup);
        
        as.cmp(eax, 0x4a);
        as.jg(discard);
        as.cmp(eax, 0x34);
        as.jg(not_plant);
        as.jmp(go_on);
        
        as.bind(not_plant);
        as.cmp(eax, 0x3b);
        as.jg(go_on);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(eax, dword_ptr(eax));
        as.mov(eax, dword_ptr(eax, 0x7c0));
        as.cmp(eax, 0x14);
        as.je(go_on);
        as.cmp(eax, 0x18);
        as.je(go_on);
        as.jmp(discard);
        
        as.bind(cup);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(eax, dword_ptr(eax, 0x4));
        as.mov(eax, dword_ptr(eax, 0x154));
        as.mov(dword_ptr(esp), eax);
        as.mov(dword_ptr(esp, 0x4), 0x2);
        as.mov(dword_ptr(esp, 0x8), 0x0);
        as.call(0xa6164);
        as.jmp(discard);
        
        as.bind(sun);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(eax, dword_ptr(eax, 0x4));
        as.mov(edx, dword_ptr(eax, 0x5554));
        as.lea(edx, dword_ptr(edx, 1000));
        as.mov(dword_ptr(eax, 0x5554), edx);
        as.jmp(discard);
        
        as.bind(diamond);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(eax, dword_ptr(eax, 0x4));
        as.mov(dword_ptr(esp), eax);
        as.mov(dword_ptr(esp, 0x4), 400);
        as.mov(dword_ptr(esp, 0x8), 300);
        as.mov(dword_ptr(esp, 0xc), 3);
        as.mov(dword_ptr(esp, 0x10), 3);
        as.call(0x28e02);
        as.jmp(discard);
        
        as.bind(zombie);
        /*
         mov eax, dword [ebp+0x8]
         mov eax, dword [eax]
         mov eax, dword [eax+0x7c0]
         cmp eax, 0x17
         je f8aa4
         */
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(eax, dword_ptr(eax));
        as.mov(eax, dword_ptr(eax, 0x7c0));
        as.cmp(eax, 0x17);
        as.je(go_on);
        as.mov(dword_ptr(esp, 0xc), 0);
        as.mov(dword_ptr(esp, 0x8), -2);
        as.mov(dword_ptr(esp, 0x4), 0xb);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(eax, dword_ptr(eax, 0x4));
        as.mov(eax, dword_ptr(eax, 0x154));
        as.mov(dword_ptr(esp), eax);
        as.call(0xac672);
        as.jmp(discard);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    /*
     0xf8a9e: (length=6)
     jmp patch
     */
    {
        Patch patch(memory, 0xf8a9e);
        auto& as = patch.get_assembler();
        
        as.jmp(patch_addr_ptr);
        
        patch.patch();
    }
    patch_addr_ptr += len;
}
void BugFix::FixMushroomWeakUp() {
    /*
     3d4b7:
     jmp patch
     
     patch:
     // eax = mushroom
     mov edx, dword [ebp+0x8] // coffee
     cmp eax, edx
     jl rectify // mushroom < coffee
     mov dword [eax+0x130], 0x64
     jmp 3d4c1
     
     rectify:
     mov dword [eax+0x130], 0x63
     jmp 3d4c1
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label rectify = as.newLabel();
        
        as.mov(edx, dword_ptr(ebp, 0x8));
        as.cmp(eax, edx);
        as.jl(rectify);
        as.mov(dword_ptr(eax, 0x130), 0x64);
        as.jmp(0x3d4c1);
        
        as.bind(rectify);
        as.mov(dword_ptr(eax, 0x130), 0x63);
        as.jmp(0x3d4c1);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0x3d4b7);
        auto& as = patch.get_assembler();
        as.jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}
