#include <iostream>
#include "patch-address.hpp"
#include "feature.hpp"
#include "bugfix.hpp"
Memory memory;
int main() {
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
