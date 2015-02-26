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

class FieldUnit : public Value {
  protected:
    uint64_t offset;
    uint64_t length;
    uint8_t  flags;
  public:
    FieldUnit(uint64_t offset, uint64_t length, uint8_t flags);
    virtual ~FieldUnit();

    virtual Handle<Integer> ToInteger() const override;
};

class RegionField : public FieldUnit {
  private:
    Handle<OpRegion> region;
  public:
    RegionField(OpRegion* region, uint64_t offset, uint64_t length, uint8_t flags);
    virtual ~RegionField();
    virtual void Dump(int ident) const override;
};

class IndexField : public FieldUnit {
  private:
    Handle<FieldUnit> index;
    Handle<FieldUnit> data;
  public:
    IndexField(FieldUnit* index, FieldUnit* data, uint64_t offset, uint64_t length, uint8_t flags);
    virtual ~IndexField();
    virtual void Dump(int ident) const override;
};

}
}

#endif
