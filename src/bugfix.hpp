#ifndef bugfix_hpp
#define bugfix_hpp

#include "patch.hpp"
#include "patch-address.hpp"

class BugFix {
    Memory& memory;
public:
    BugFix(Memory& memory) : memory(memory) {}
    void FixUseCard();
    void FixClickCard();
    void FixMushroomWeakUp();
};

#endif /* bugfix_hpp */
