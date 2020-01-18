#ifndef feature_hpp
#define feature_hpp

#include "patch.hpp"
#include "patch-address.hpp"

class Feature {
    Memory& memory;
public:
    Feature(Memory& memory) : memory(memory) {}
    void FeatureKelpPull();
    void FeatureExplodeNut();
    void FeatureFumeshroomOneLine();
    void FeatureBucketDoorZombie();
    void FeatureFlagZombie();
    void FeaturePeaZombie();
    void FeatureShooter();
    void FeatureSmartDigZombie();
    void FeatureNutZombie();
    void FeatureJalapenoZombie();
    void FeatureWoguaZombie();
    void FeatureFootballZombie();
    void FeatureStar();
    void FeatureMarigoldProduceCard();
    void FeatureBlover();
    
    void test();
    void test2();
    void test3();
};

#endif /* feature_hpp */
