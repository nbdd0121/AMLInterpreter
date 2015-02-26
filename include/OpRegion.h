#ifndef NORLIT_AML_OPREGION_H
#define NORLIT_AML_OPREGION_H

#include <cstdint>
#include <cstddef>

#include "Value.h"

namespace norlit {
namespace aml {

class OpRegion: public Value {
  private:
    uint64_t offset;
    uint64_t length;
    uint8_t  space;
  public:
    OpRegion(uint8_t space, uint64_t offset, uint64_t length);
    virtual ~OpRegion();
    virtual void Dump(int ident) const override;
};

class RegionField : public Value {
  private:
    Handle<OpRegion> region;
    uint64_t offset;
    uint64_t length;
    uint8_t  flags;
  public:
    RegionField(OpRegion* region, uint64_t offset, uint64_t length, uint8_t flags);
    virtual ~RegionField();
    virtual void Dump(int ident) const override;

    virtual Handle<Integer> ToInteger() const override;
};

}
}

#endif
