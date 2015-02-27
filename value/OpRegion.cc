#include "OpRegion.h"
#include "Integer.h"

#include <cstdio>
#include <cinttypes>

namespace norlit {
namespace aml {

OpRegion::OpRegion(uint8_t space, uint64_t off, uint64_t len) : offset(off), length(len), space(space) {

}

OpRegion::~OpRegion() {

}

void OpRegion::Dump(int ident) const {
    printf("[OpRegion Space=%d, %" PRIx64 ", %" PRIx64 "]", space, offset, length);
}

FieldUnit::FieldUnit(uint64_t o, uint64_t l, uint8_t a):offset(o), length(l), flags(a) {

}

FieldUnit::~FieldUnit() {

}

Handle<Integer> FieldUnit::ToInteger() const {
    return new Integer(0);
}

RegionField::RegionField(OpRegion* r, uint64_t o, uint64_t l, uint8_t a) :FieldUnit(o, l, a), region(r) {

}

RegionField::~RegionField() {

}

void RegionField::Dump(int ident) const {
    printf("[RegionField [Ref], Offset=%" PRIx64 ", Length=%" PRIx64 ", %d]", offset, length, flags);
}

IndexField::IndexField(FieldUnit* i, FieldUnit* d, uint64_t o, uint64_t l, uint8_t a)
    :FieldUnit(o, l, a), index(i), data(d) {

}

IndexField::~IndexField() {

}

void IndexField::Dump(int ident) const {
    printf("[IndexField [Ref], [Ref], Offset=%" PRIx64 ", Length=%" PRIx64 ", %d]", offset, length, flags);
}


}
}

