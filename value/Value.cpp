#include "Value.h"

#include <iostream>
#include <cstdio>

namespace norlit {
namespace aml {

Value::Value() {

}

Value::~Value() {}

bool Value::IsScope() const {
    return false;
}

bool Value::IsResolved() const {
    return true;
}

Handle<Value> Value::Resolve() {
    return this;
}

Handle<Integer> Value::ToInteger() const {
    perror("Exception: Invalid type conversion");
    exit(0);
    return nullptr;
}

void Value::Retain() {
    refCount++;
}

void Value::Release() {
    if (--refCount == 0) {
        delete this;
    }
}

}
}