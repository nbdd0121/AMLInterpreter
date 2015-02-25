#include "Value.h"
#include "Integer.h"
#include "OSDep.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>

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
    aml_os_panic("Exception: Invalid type conversion");
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