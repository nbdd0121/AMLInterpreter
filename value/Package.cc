#include "Package.h"

#include <cstdio>
#include <cstring>

namespace norlit {
namespace aml {

Package::Package(size_t l, Handle<Value> *e) : length(l), entry(e) {
}


Package::~Package() {
    delete[] entry;
}

void Package::Dump(int ident) const {
    printf("Package(%d) {", length);
    for (unsigned i = 0; i < length; i++) {
        if (i != 0) {
            printf(",");
        }
        printf("\n%*s", ident + 2, "");
        entry[i]->Dump(ident + 2);
    }
    printf("\n%*s}", ident, "");
}

Buffer::Buffer(size_t l, uint8_t *e) : length(l), entry(e) {}


Buffer::~Buffer() {
    delete[] entry;
}

void Buffer::Dump(int ident) const {
    printf("Buffer(%d) {", length);
    for (unsigned i = 0; i < length; i++) {
        if (i != 0) {
            printf(",");
        }
        if (i % 8 == 0) {
            printf("\n%*s", ident + 2, "");
        }
        printf("%02x", entry[i]);
    }
    printf("\n%*s}", ident, "");
}

}
}