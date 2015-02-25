#include "Integer.h"

#include <cstdio>

namespace norlit {
namespace aml {

Integer::Integer(uint64_t value) :value(value) {

}

Integer::~Integer() {
    // std::cout << "Integer dispose\n";
}

void Integer::Dump(int ident) const {
    printf("0x%x", value);
}

Handle<Integer> Integer::ToInteger() const {
    return (Integer*)this;
}

}
}