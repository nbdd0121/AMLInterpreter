#ifndef NORLIT_AML_HANDLE_H
#define NORLIT_AML_HANDLE_H

#include <cassert>

namespace norlit {
namespace aml {

template<typename T>
class Handle {
  private:
    T* value;
  public:
    Handle() :value(nullptr) {}
    Handle(T* val) :value(val) {
        if (val)
            val->Retain();
    }
    Handle(const Handle& val) :Handle(val.value) {}
    Handle(Handle&& val) :value(val.value) {
        val.value = nullptr;
    }
    void operator =(const Handle& val) {
        operator =(val.value);
    }
    void operator =(T* val) {
        if (value) value->Release();
        value = val;
        if (value) value->Retain();
    }
    operator T*() const {
        return value;
    }
    template<typename V>
    explicit operator V*() const {
        return (V*)value;
    }
    T* operator->() const {
        assert(value);
        return value;
    }
    ~Handle() {
        if (value) value->Release();
    }
};

}
}

#endif
