#include "Package.h"

#include <iostream>

namespace norlit {
namespace acpi {
namespace aml {

Package::Package(size_t size) :size(size) {
    array = new Handle<Value>[size]();
}

Package::~Package() {
    delete[] array;
    // std::cout << "~Package\n";
}

void Package::Dump() const {
    std::cout << "Package(" << size << ") {" << std::endl;
    for (unsigned i = 0; i < size; i++) {
        if (i != 0)std::cout << "," << std::endl;
        array[i]->Dump();
    }
    std::cout << std::endl << "}";
}

}
}
}