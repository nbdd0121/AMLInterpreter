#include "String.h"

#include <iostream>

namespace norlit {
namespace acpi {
namespace aml {

String::String(size_t length, char* value) :length(length), value(value) {

}

String::~String() {
    delete[] value;
}

void String::Dump(int ident) const {
    printf("\"%.*s\"", length, value);
}

}
}
}