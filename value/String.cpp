#include "String_.h"

#include <iostream>
#include <cstdio>
#include <cstring>

namespace norlit {
namespace aml {

String::String(const char *val) : length(strlen(val)) {
    value = new char[length];
    memcpy(value, val, length);
}


String::~String() {
    delete[] value;
}

void String::Dump(int ident) const {
    printf("\"%.*s\"", length, value);
}

}
}