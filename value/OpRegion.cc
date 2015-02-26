#include "OpRegion.h"
#include "Integer.h"

#include <cstdio>
#include <cinttypes>

namespace norlit {
namespace aml {

OpRegion::OpRegion(uint8_t space, uint64_t off, uint64_t len): space(space), offset(off), length(len) {

}

OpRegion::~OpRegion() {

}

void OpRegion::Dump(int ident) const {
    printf("[OpRegion Space=%d, %" PRIx64 ", %" PRIx64 "]", space, offset, length);
}

RegionField::RegionField(OpRegion* r, uint64_t o, uint64_t l, uint8_t a):region(r), offset(o), length(l), flags(a) {

}

RegionField::~RegionField() {

}

void RegionField::Dump(int ident) const {
    printf("[RegionField [Ref], Offset=%" PRIx64 ", Length=%" PRIx64 ", %d]", offset, length, flags);
}

Handle<Integer> RegionField::ToInteger() const {
    return new Integer(0);
}



}
}

