#ifndef NORLIT_AML_NAME_H
#define NORLIT_AML_NAME_H

#include <cstdint>

#include "Value.h"

namespace norlit {
namespace aml {

class Name: public Value {
  private:
    int prefixCount;
    int segCount;
    uint32_t *names;
  public:
    Name(int prefixCount, int segCount, uint32_t *names);
    ~Name();
    virtual bool IsResolved() const override;
    virtual void Dump(int ident) const override;

    inline int GetPrefixCount() {
        return prefixCount;
    }

    inline int GetSegCount() {
        return segCount;
    }

    const uint32_t* GetNames() {
        return names;
    }

    const uint32_t GetName(int index) {
        return names[index];
    }

    static uint32_t PackNameSegment(const uint8_t *name) {
        return name[0] << 24 | name[1] << 16 | name[2] << 8 | name[3];
    }

    static uint32_t PackNameSegment(const char *name) {
        return PackNameSegment((const uint8_t*)name);
    }

    static void UnpackNameSegment(uint32_t input, char *output) {
        output[0] = (input >> 24) & 0xFF;
        output[1] = (input >> 16) & 0xFF;
        output[2] = (input >> 8) & 0xFF;
        output[3] = input & 0xFF;
    }
};

}
}

#endif