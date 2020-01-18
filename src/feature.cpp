#include "feature.hpp"
using namespace asmjit;
using namespace asmjit::x86;

void ModifyCardProperty(Memory& memory, int CardID, int cost, int cooldowntime, bool isBeghouledMode) {
    uint32_t CostAddress = 0;
    if (CardID <= 52) {
        if (isBeghouledMode) {
            if (CardID == 7)
                CostAddress = 0x3DA39;
            else if (CardID == 10)
                CostAddress = 0x3DA4B;
            else if (CardID == 23)
                CostAddress = 0x3DA5D;
        } else {
            CostAddress = 0x35CFC0 + 0x10 + CardID * 0x24;
        }
        if (cooldowntime >= 0) {
            memory.WriteMemory<int>(cooldowntime, {static_cast<unsigned long long>(0x35CFC0 + 0x14 + CardID * 0x24)});
        }
    } else if (CardID >= 56 && CardID <= 74) {
        CostAddress = memory.ReadMemory<uint32_t>({static_cast<unsigned long long>(0x2E8D30 + (CardID - 56) * 0x4)}) + 0x3;
    } else if (CardID == 54)
        CostAddress = 0x3DA6F;
    else if (CardID == 55)
        CostAddress = 0x3DA81;
    if (CostAddress && cost >= 0)
        memory.WriteMemory<int>(cost, {CostAddress});
}

void Feature::FeatureExplodeNut() {
    // card cost
//    0x35CFC0 + 0x10 + CardID * 0x24
//    memory.WriteMemory<int>(150, {0x35d6b4});
    /*
     d82cd:
     jmp patch
     
     patch:
     cmp edx, 0x40  (edx: plant+0x40)
     jg end
     mov eax, dword [ebp+0xc]
     mov eax, dword [eax+0x24]
     cmp eax, 0x31
     jne end
     mov eax, dword [ebp+0xc]
     mov dword [eax+0x24], 0x2
     mov dword [eax+0x50], 0x1
     end:
     mov eax, dword [ebp+0xc]
     mov dword [eax+0xb4], 0x32
     jmp d82da
     */
    // 被啃死时爆炸
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label end = as.newLabel();
        Label tall_nut = as.newLabel();
        
        as.cmp(edx, 0x40);
        as.jg(end);
        as.mov(eax, dword_ptr(ebp, 0xc));
        as.mov(eax, dword_ptr(eax, 0x24));
        as.cmp(eax, 0x3);
        as.jne(tall_nut);
        as.mov(eax, dword_ptr(ebp, 0xc));
        as.mov(dword_ptr(eax, 0x24), 0x2);
        as.mov(dword_ptr(eax, 0x50), 0x1);
        as.jmp(end);
        as.bind(tall_nut);
        as.cmp(eax, 0x17);
        as.jne(end);
        as.mov(eax, dword_ptr(ebp, 0xc));
        as.mov(dword_ptr(eax, 0x24), 0x11);
        as.bind(end);
        as.mov(eax, dword_ptr(ebp, 0xc));
        as.mov(dword_ptr(eax, 0xb4), 0x32);
        as.jmp(0xd82da);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xd82cd);
        auto& as = patch.get_assembler();
        as.jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    /*
     3d644:
     jmp patch
     
     patch:
     mov eax, dword [ebp+0x8]
     mov edx, dword [eax+0x24]
     cmp edx, 0x31
     jne other
     mov byte [eax+0x142], 0x0
     mov dword [eax+0x24], 0x2
     mov dword [eax+0x50], 0x1
     other:
     mov eax, dword [eax]
     jmp 3d649
     */
    // 被敲击、碾压时爆炸
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label other = as.newLabel();
//        Label tall_nut = as.newLabel();
        
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(edx, dword_ptr(eax, 0x24));
        as.cmp(edx, 0x3);
//        as.jne(tall_nut);
        as.jne(other);
        as.mov(byte_ptr(eax, 0x142), 0x0);
        as.mov(dword_ptr(eax, 0x24), 0x2);
        as.mov(dword_ptr(eax, 0x50), 0x1);
//        as.jmp(other);
//        as.bind(tall_nut);
//        as.cmp(edx, 0x17);
//        as.jne(other);
//        as.mov(byte_ptr(eax, 0x142), 0x0);
//        as.mov(dword_ptr(eax, 0x24), 0x11);
        as.bind(other);
        as.mov(eax, dword_ptr(eax));
        as.jmp(0x3d649);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0x3d644);
        auto& as = patch.get_assembler();
        as.jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    ModifyCardProperty(memory, 3, 100, -1, 0);
}

void Feature::FeatureFumeshroomOneLine() {
    memory.WriteMemory<int>(0x640, {0x3e3eb});
    ModifyCardProperty(memory, 0xa, 125, -1, 0);
}

void Feature::FeatureKelpPull() {
    // 可以杀陆地僵尸
    memory.WriteMemory<unsigned char>(0x80, {0x3e959});

    vm_address_t len;
    /*
     1ed0a:
     jmp patch

     patch:
     cmp dword [ebp+0x14], 0x13
     je 1ec47
     */
    // 可以放置在陆地上
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();

        as.cmp(dword_ptr(ebp, 0x14), 0x13);
        as.je(0x1ec47);
        as.jmp(0x1ed10);

        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0x1ed0a);
        auto& as = patch.get_assembler();
        as.jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    ModifyCardProperty(memory, 0x13, 50, -1, 0);
    
//    memory.WriteMemory<unsigned char, 6>({0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, {0x1ed0a});
//    // 1ee49
//    {
//        Patch patch(memory, patch_addr_ptr);
//        auto& as = patch.get_assembler();
//
//        as.cmp(dword_ptr(ebp, -0xc), 0x21);
//        as.je(0x1ee4f);
//        as.cmp(dword_ptr(ebp, -0xc), 0x13);
//        as.je(0x1ee4f);
//        as.jmp(0x1ee7c);
//
//        len = patch.code_length();
//        assert(patch_addr_ptr + len <= patch_addr_end);
//        patch.patch();
//    }
//    {
//        Patch patch(memory, 0x1ee49);
//        auto& as = patch.get_assembler();
//        as.jmp(patch_addr_ptr);
//        patch.patch();
//    }
//    patch_addr_ptr += len;
}
void Feature::FeatureBucketDoorZombie() {
    // ead0e: jmp eac81
    Patch patch(memory, 0xead0e);
    auto& as = patch.get_assembler();
    as.long_().jmp(0xeac81);
    patch.patch();
}
void Feature::FeatureFlagZombie() {
    /*
     ebfdf:
     jmp patch
     
     patch:
     jne ec108
     mov eax, dword [ebp+0x8]
     mov byte [eax+0xbc], 0x1
     mov eax, dword [ebp+0x8]
     mov dword [esp], eax
     call d0b08
     mov dword [esp+0x8], 0x0
     mov eax, 0x2e110c
     mov dword [esp+0x4], eax
     mov eax, dword [ebp+0x8]
     mov dword [esp], eax
     call d0538
     mov eax, dword[ebp+0x8]
     mov dword [eax+0xc4], 0x1
     mov dword [eax+0xd0], 0x172
     jmp ebffa
     */
    // 必须先设置第二类饰品再设置第一类，
    // 或者，先call d0b08再call d0538
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        as.jne(0xec108);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(byte_ptr(eax, 0xbc), 0x1);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0b08);
        as.mov(dword_ptr(esp, 0x8), 0);
        as.mov(eax, 0x2e1116);
        as.mov(dword_ptr(esp, 0x4), eax);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0538);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(eax, 0xc4), 0x2);
        as.mov(dword_ptr(eax, 0xd0), 0x44c);
        as.jmp(0xebffa);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xebfdf);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    // speed x 3
    // 没用。。
//    memory.WriteMemory<float>(2400.0, {0xec0fb + 1});
}
void Feature::FeaturePeaZombie() {
    /*
     ecb77:
     jmp patch
     
     patch:
     jne ecd27
     call d0b08
     call d0538
     set hat 1
     jmp ecb88
     */
    // 普通豌豆
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        as.jne(0xecd27);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(byte_ptr(eax, 0xbc), 0x1);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0b08);
        as.mov(dword_ptr(esp, 0x8), 0);
        as.mov(eax, 0x2e110c);
        as.mov(dword_ptr(esp, 0x4), eax);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0538);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(eax, 0xc4), 0x1);
        as.mov(dword_ptr(eax, 0xd0), 0x172);
        as.jmp(0xecb88);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xecb77);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    // 机枪
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        as.jne(0xed437);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(byte_ptr(eax, 0xbc), 0x1);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0b08);
        as.mov(dword_ptr(esp, 0x8), 0);
        as.mov(eax, 0x2e110c);
        as.mov(dword_ptr(esp, 0x4), eax);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0538);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(eax, 0xc4), 0x1);
        as.mov(dword_ptr(eax, 0xd0), 0x172);
        as.jmp(0xed298);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xed287);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}
void Feature::FeatureShooter() {
    /*
     40d34:
     jmp patch
     
     patch:
     ... (imp___jump_table__random，选择返回值决定子弹)
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label pea_pea = as.newLabel();
        Label pea_thorn = as.newLabel();
        Label pea_melon = as.newLabel();
        Label pea_cabbage = as.newLabel();
        
        Label kernel_pult = as.newLabel();
        Label kernel_cob = as.newLabel();
        
        int back = 0x40d43;
        int imp___jump_table__random = 0x42ed53;
        
        as.je(kernel_pult);
        as.cmp(eax, 0x0);
        as.jne(back);
        as.call(imp___jump_table__random);
        as.cmp(eax, (int)(0.8 * (double)INT_MAX));
        as.jl(pea_pea);
        as.cmp(eax, (int)(0.9 * (double)INT_MAX));
        as.jl(pea_thorn);
        as.cmp(eax, (int)(0.95 * (double)INT_MAX));
        as.jl(pea_cabbage);
        
        as.bind(pea_melon);
        as.mov(dword_ptr(ebp, -0x3c), 0x3);
        as.jmp(back);
        
        as.bind(pea_pea);
        as.mov(dword_ptr(ebp, -0x3c), 0x0);
        as.jmp(back);
        
        as.bind(pea_thorn);
        as.mov(dword_ptr(ebp, -0x3c), 0x8);
        as.jmp(back);
        
        as.bind(pea_cabbage);
        as.mov(dword_ptr(ebp, -0x3c), 0x2);
        as.jmp(back);
        
        // kernel_pult
        as.bind(kernel_pult);
        as.call(imp___jump_table__random);
        as.cmp(eax, (int)(0.99 * (double)INT_MAX));
        as.jge(kernel_cob);
        as.cmp(dword_ptr(ebp, 0x14), 0x1);
        as.jne(back);
        as.mov(dword_ptr(ebp, -0x3c), 0xc);
        as.jmp(back);
        
        as.bind(kernel_cob);
        as.mov(dword_ptr(ebp, -0x3c), 0xb);
        as.jmp(back);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0x40d34);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    // 40cb8: mov dword [ebp-0x3c], 0x1
    // 三线射冰豌豆
    memory.WriteMemory<int>(0x1, {0x40cbb});
}
void Feature::FeatureSmartDigZombie() {
    /*
     d8c3a:
     jmp patch
     
     patch:
     call _random
     cmp eax, P
     jge d8bda
     mov eax, dword [ebp+0x8]
     mov dword [eax+0x28], 0x25
     jmp d8c44
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        int imp___jump_table__random = 0x42ed53;
        
        as.call(imp___jump_table__random);
        as.cmp(eax, (int)(0.95 * (double)INT_MAX));
        as.jge(0xd8bda);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(eax, 0x28), 0x25);
        as.jmp(0xd8c44);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xd8c3a);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}
void Feature::FeatureNutZombie() {
    /*
     1b:
     ecd2b:
     jmp patch
     
     patch:
     jne eced7
     
     call d0b08
     jmp ecd3c
     
     
     1f:
     ecedb:
     jmp patch
     
     patch:
     jne ed0a3
     
     call d0b08
     jmp eceec
     */
    /*
     000eacde         mov        eax, dword [ebp+arg_0]
     000eace1         mov        dword [eax+0xd8], 0x1
     000eaceb         mov        eax, dword [ebp+arg_0]
     000eacee         mov        dword [eax+0xdc], 0x44c
     000eacf8         mov        eax, dword [ebp+arg_0]
     000eacfb         mov        dword [esp+0x2b8+var_2B8], eax                      ; argument #1 for method sub_d0b08
     000eacfe         call       sub_d0b08
     000ead03         mov        eax, dword [ebp+arg_0]
     000ead06         mov        dword [esp+0x2b8+var_2B8], eax                      ; argument #1 for method sub_d060a
     000ead09         call       sub_d060a
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        as.jne(0xeced7);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(eax, 0xd8), 0x1);
        as.mov(dword_ptr(eax, 0xdc), 0x44c);
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0b08);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd060a);
        as.jmp(0xecd3c);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xecd2b);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    // 机枪
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        as.jne(0xed0a3);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(eax, 0xd8), 0x1);
        as.mov(dword_ptr(eax, 0xdc), 0x44c);
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0b08);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd060a);
        as.jmp(0xeceec);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xecedb);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}

void Feature::FeatureJalapenoZombie() {
    /*
     1c:
     ed0ac:
     jmp patch
     
     patch:
     jne ed283
     call d0b08
     jmp ed0bd
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        as.jne(0xed283);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(eax, 0xd8), 0x1);
        as.mov(dword_ptr(eax, 0xdc), 0x44c);
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0b08);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd060a);
        as.jmp(0xed0bd);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xed0ac);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}

void Feature::FeatureWoguaZombie() {
    /*
     1e:
     ed43b:
     jmp patch
     
     patch:
     jne ed5e6
     
     call d0b08
     jmp ed44c
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        as.jne(0xed5e6);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(byte_ptr(eax, 0xbc), 0x1);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0b08);
        as.mov(dword_ptr(esp, 0x8), 0);
        as.mov(eax, 0x2e1116);
        as.mov(dword_ptr(esp, 0x4), eax);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(esp), eax);
        as.call((int)0xd0538);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(dword_ptr(eax, 0xc4), 0x2);
        as.mov(dword_ptr(eax, 0xd0), 0x44c);
        as.jmp(0xed44c);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xed43b);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}

void Feature::FeatureFootballZombie() {
    // 帽子不能被吸走
    Patch patch(memory, 0x36a79);
    auto& as = patch.get_assembler();
    as.jmp(0x36a87);
    patch.patch();
    
    // 帽子血量x2
    memory.WriteMemory<int>(0x578*2, {0xeb1a8+6});
}

void Feature::FeatureStar() {
    /*
     #define BACK 105263
     
     105261:
     jmp p
     
     p:
     jne other
     mov dword [ebp-0x1c], 0x14
     jmp BACK
     other:
     mov eax, dword [ebp+0x8]
     mov eax, dword [eax+0x5c]
     cmp eax, 0x7
     jne BACK
     call _random
     cmp eax, P1
     jl BACK
     cmp eax, P2
     jl FLY
     mov eax, dword [ebp+0xc]
     mov byte [eax+0xb8], 0x1
     jmp BACK
     FLY:
     mov eax, dword [ebp+0xc]
     mov byte [eax+0xb9], 0x1
     jmp BACK
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label fly = as.newLabel();
        Label other = as.newLabel();
        Label freeze = as.newLabel();
        
        int back = 0x10526a;
        int imp___jump_table__random = 0x42ed53;
        
        as.jne(other);
        as.mov(dword_ptr(ebp, -0x1c), 0x14);
        as.jmp(back);
        as.bind(other);
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(eax, dword_ptr(eax, 0x5c));
        as.cmp(eax, 0x7);
        as.jne(back);
        as.call(imp___jump_table__random);
        as.cmp(eax, (int)(0.95 * (double)INT_MAX));
        as.jl(back);
        as.cmp(eax, (int)(0.99 * (double)INT_MAX));
        as.jl(freeze);
        as.cmp(eax, (int)(0.995 * (double)INT_MAX));
        as.jl(fly);
        as.mov(eax, dword_ptr(ebp, 0xc));
        as.mov(byte_ptr(eax, 0xb8), 0x1);
        as.jmp(back);
        as.bind(fly);
        as.mov(eax, dword_ptr(ebp, 0xc));
        as.mov(byte_ptr(eax, 0xb9), 0x1);
        as.jmp(back);
        as.bind(freeze);
        as.mov(dword_ptr(esp, 0x4), 0);
        as.mov(eax, dword_ptr(ebp, 0xc));
        as.mov(dword_ptr(esp), eax);
        as.call(0xd8dea);
        as.jmp(back);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0x105261);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}

void Feature::FeatureMarigoldProduceCard() {
    // div ops
    // 双字除法 (edx, eax) / (ops) --> eax(商) edx(余数) ; edx为0
    /*
     0x352f2: jmp p
     
     p:
     00035315         mov        eax, dword [ebp+arg_0]          ; CODE XREF=sub_34ffa+786
     00035318         mov        edx, dword [eax+0xc]
     0003531e         mov        ecx, dword [eax+8]
     00035324         mov        ebx, dword [eax+4]
     00035327         mov        dword [esp+0x48+var_38], 0x3    ; argument #5 for method sub_28e02
     00035332         mov        dword [esp+0x48+var_3C], 0x10   ; argument #4 for method sub_28e02
     00035336         mov        dword [esp+0x48+var_40], edx    ; argument #3 for method sub_28e02
     0003533a         mov        dword [esp+0x48+var_44], ecx    ; argument #2 for method sub_28e02
     0003533e         mov        dword [esp+0x48+var_48], ebx    ; argument #1 for method sub_28e02
     00035341         call       sub_28e02
     mov ebx, eax
     call _random
     idiv 0x30
     mov eax, ebx
     mov dword [eax+0x68], edx
     jmp 35346
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        int imp___jump_table__random = 0x42ed53;
        
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(edx, dword_ptr(eax, 0xc));
        as.mov(ecx, dword_ptr(eax, 0x8));
        as.mov(ebx, dword_ptr(eax, 0x4));
        as.mov(dword_ptr(esp, 0x10), 0x3);
        as.mov(dword_ptr(esp, 0xc), 0x10);
        as.mov(dword_ptr(esp, 0x8), edx);
        as.mov(dword_ptr(esp, 0x4), ecx);
        as.mov(dword_ptr(esp), ebx);
        as.call((int)0x28e02);
        as.mov(ebx, eax);
        as.call(imp___jump_table__random);
        as.mov(ecx, 0x28);
        as.mov(edx, 0);
        as.idiv(ecx);
        as.mov(eax, ebx);
        as.mov(dword_ptr(eax, 0x68), edx);
        as.jmp(0x35346);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0x352f2);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    ModifyCardProperty(memory, 0x26, 400, -1, 0);
}

void Feature::FeatureBlover() {
    /*
     3cdbd:
     jmp p
     
     p:
     mov eax, dword [ebp-0xc]
     mov dword [esp], eax
     call e6bba
     mov eax, dword [ebp-0xc]
     mov dword [esp], eax
     push 3cdc8
     jmp d0cc4 // call d0cc4
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        
        Label other = as.newLabel();
        
        // 寒冰菇式冻结
//        as.mov(eax, dword_ptr(ebp, -0xc));
//        as.mov(dword_ptr(esp), eax);
//        as.call((int)0xe6bba);
        // 吹走半空小鬼
        // id=0x18, state=0x47
        // 减速
        as.mov(dword_ptr(esp, 0x4), 0);
        as.mov(eax, dword_ptr(ebp, -0xc));
        as.mov(dword_ptr(esp), eax);
        as.call(0xd8dea);
        as.mov(eax, dword_ptr(ebp, -0xc));
        as.cmp(dword_ptr(eax, 0x24), 0x18);
        as.jne(other);
        as.cmp(dword_ptr(eax, 0x28), 0x47);
        as.jne(other);
        as.mov(byte_ptr(eax, 0xb9), 0x1);
        
        as.bind(other);
        as.mov(dword_ptr(esp), eax);
        as.push(0x3cdc8);
        as.jmp(0xd0cc4); // call d0cc4
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0x3cdbd);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
    
    ModifyCardProperty(memory, 0x1b, 75, -1, 0);
}

void Feature::test() {
    /*
     d355f:
     jmp p
     
     p:
     mov eax, dword [ebp+0x8]
     mov eax, dword [eax+0x24]
     cmp eax, 0x1
     jne end
     movss xmm0, dword [ebp-0x10]
     mulss xmm0, 2.0
     cvtss2si xmm0, eax
     mov dword [ebp-0x10], eax
     end:
     movzx eax, byte [ebp-0x19]
     test al, al
     je d357e
     jmp d3567
     */
    vm_address_t len;
    {
        patch_addr_end -= 4;
        memory.WriteMemory<float>(1.5, {patch_addr_end});
//        std::cout << std::hex << patch_addr_end << std::endl;
        
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label end = as.newLabel();
        
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.mov(eax, dword_ptr(eax, 0x24));
        as.cmp(eax, 0x1);
        as.jne(end);
        as.movss(xmm0, dword_ptr(ebp, -0x10));
        as.mulss(xmm0, Mem(patch_addr_end));
        as.movss(dword_ptr(ebp, -0x10), xmm0);
//        as.cvtss2si(eax, xmm0);
//        as.mov(dword_ptr(ebp, -0x10), eax);
        
        as.bind(end);
        as.movzx(eax, byte_ptr(ebp, -0x19));
        as.test(al, al);
        as.je(0xd357e);
        as.jmp(0xd3567);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xd355f);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}

void Feature::test2() {
    /*
     db7b9:
     jmp p
     
     p:
     mov eax, dword [ebp+0x8]
     cmp dword [eax+0x68], 0x0
     je db83c
     mov dword [eax+0x68], 0x1
     movss xmm0, dword [eax+0x2c]
     jmp db7c1
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label second = as.newLabel();
        
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.cmp(dword_ptr(eax, 0x68), 0x0);
        as.jne(second);
        as.mov(dword_ptr(eax, 0x68), 0x100);
        as.jmp(0xdb3e7);
//        as.jmp(0xdb4d1);
        
        as.bind(second);
        as.movss(xmm0, dword_ptr(eax, 0x2c));
        as.jmp(0xdb7c1);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xdb7b9);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}

void Feature::test3() {
    /*
     db835:
     jmp p
     
     p:
     mov eax, dword [ebp+0x8]
     cmp dword [eax+0x68], 0x0
     jne second
     mov dword [eax+0x68], 0x7fffffff
     mov dword [eax+0x28], 0xc
     jmp db3e7
     
     second:
     call d6f40
     jmp db891
     */
    vm_address_t len;
    {
        Patch patch(memory, patch_addr_ptr);
        auto& as = patch.get_assembler();
        Label second = as.newLabel();
        
        as.mov(eax, dword_ptr(ebp, 0x8));
        as.cmp(dword_ptr(eax, 0x68), 0x0);
        as.jne(second);
        as.mov(dword_ptr(eax, 0x68), INT_MAX);
        as.mov(dword_ptr(eax, 0x28), 0xc);
        as.jmp(0xdb3e7);
        
        as.bind(second);
        as.call((int)0xd6f40);
        as.jmp(0xdb891);
        
        len = patch.code_length();
        assert(patch_addr_ptr + len <= patch_addr_end);
        patch.patch();
    }
    {
        Patch patch(memory, 0xdb835);
        auto& as = patch.get_assembler();
        as.long_().jmp(patch_addr_ptr);
        patch.patch();
    }
    patch_addr_ptr += len;
}
