#include "Method.h"

#include <iostream>

namespace norlit {
namespace acpi {
namespace aml {

Method::Method(uint8_t f, Bytecode b) :flags(f), bytecode(b) {

}

Method::~Method() {
}

void Method::Dump(int ident) const {
    printf("Method[Arg = %d]", GetArgumentCount());
}

bool Method::IsMethod() const {
    return true;
}

}
}
}