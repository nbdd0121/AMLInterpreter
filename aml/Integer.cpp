#include "Integer.h"

#include <iostream>

namespace norlit {
namespace acpi {
namespace aml {

Integer::Integer(uint64_t value) :value(value) {

}

Integer::~Integer() {
    // std::cout << "Integer dispose\n";
}

void Integer::Dump() const {
    std::cout << value;
}

}
}
}