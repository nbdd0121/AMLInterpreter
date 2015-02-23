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

void Package::Dump(int ident) const {
    printf("Package(%d) {", size);
    for (unsigned i = 0; i < size; i++) {
        printf("\n%*s", ident + 2, "");
        array[i]->Dump(ident + 2);
    }
    printf("\n%*s}", ident,"");
}

}
}
}