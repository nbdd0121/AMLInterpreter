#ifndef NORLIT_ACPI_AML_HANDLE_H
#define NORLIT_ACPI_AML_HANDLE_H

#include <cstdint>
#include <cstddef>
#include "Value.h"

namespace norlit {
namespace acpi {
namespace aml {

template<typename T>
class Handle {
  private:
    T* value;
    void Retain() {
        if (value != nullptr) {
            value->refCount++;
        }
    }
    void Release() {
        if (value != nullptr) {
            if (--value->refCount == 0) {
                delete value;
            }
        }
    }
  public:
    Handle() :value(nullptr) {}
    Handle(T* val) :value(val) {
        Retain();
    }
    Handle(const Handle& val) :Handle(val.value) {}
    Handle(Handle&& val) :value(val.value) {
        val.value = nullptr;
    }
    void operator =(const Handle& val) {
        operator =(val.value);
    }
    void operator =(T* val) {
        Release();
        value = val;
        Retain();
    }
    operator T*() {
        return value;
    }
    T* operator->() {
        return value;
    }
    ~Handle() {
        Release();
    }
};

}
}
}

#endif
