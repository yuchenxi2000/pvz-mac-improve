#include <iostream>
#include "patch-address.hpp"
#include "feature.hpp"
#include "bugfix.hpp"

#define MAKE_PATCH_DYLIB

// 使用dll注入（编译动态库！）
#ifdef MAKE_PATCH_DYLIB
__attribute__((constructor))
void patch_init(void) {
    printf("---------------------------\n");
    printf("PvZ Improve by YCX\n");
    printf("Game Version 1.0.40\n");
    printf("\n");
    printf("BiliBili: yuchenxi0_0\n");
    printf("GitHub: yuchenxi2000\n");
    printf("\n");
    
    // attach process
    Memory memory;
    memory.AttachSelf();
    
    AllocPatchMemory(memory, 1024*64);
    
    // fix bug
    BugFix bugfix(memory);
    bugfix.FixUseCard();
    bugfix.FixClickCard();
    bugfix.FixMushroomWeakUp();
    
    // add feature
    Feature feature(memory);
    feature.FeatureFlagZombie();
    feature.FeatureBucketDoorZombie();
    feature.FeaturePeaZombie();
    feature.FeatureExplodeNut();
    feature.FeatureFumeshroomOneLine();
    feature.FeatureKelpPull();
    feature.FeatureShooter();
    feature.FeatureSmartDigZombie();
    feature.FeatureNutZombie();
    feature.FeatureJalapenoZombie();
    feature.FeatureWoguaZombie();
    feature.FeatureFootballZombie();
    feature.FeatureStar();
    feature.FeatureMarigoldProduceCard();
    feature.FeatureBlover();
    
    printf("Code Injected Successfully!\n");
    printf("---------------------------\n");
}
// 运行时注入
#else
int main() {
    Memory memory;
    // attach process
    pid_t pid = memory.PidFromName("Plants vs. Zombi");
    memory.Attach(pid);
    
    AllocPatchMemory(memory, 1024*64);
    
    // fix bug
    BugFix bugfix(memory);
    bugfix.FixUseCard();
    bugfix.FixClickCard();
    bugfix.FixMushroomWeakUp();
    
    // add feature
    Feature feature(memory);
    feature.FeatureFlagZombie();
    feature.FeatureBucketDoorZombie();
    feature.FeaturePeaZombie();
    feature.FeatureExplodeNut();
    feature.FeatureFumeshroomOneLine();
    feature.FeatureKelpPull();
    feature.FeatureShooter();
    feature.FeatureSmartDigZombie();
    feature.FeatureNutZombie();
    feature.FeatureJalapenoZombie();
    feature.FeatureWoguaZombie();
    feature.FeatureFootballZombie();
    feature.FeatureStar();
    feature.FeatureMarigoldProduceCard();
    feature.FeatureBlover();
}

#endif
