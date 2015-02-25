#ifndef NORLIT_AML_VALUE_H
#define NORLIT_AML_VALUE_H

#include <cstdint>
#include <cstddef>
#include "Handle.h"

namespace norlit {
namespace aml {

class Integer;

class Value {
  private:
    int refCount = 0;
  public:
    Value();
    virtual ~Value();
    virtual void Dump(int ident) const = 0;

    virtual bool IsResolved() const;
    virtual bool IsScope() const;
    virtual bool IsMethod() const;
    virtual Handle<Value> Resolve();

    virtual Handle<Integer> ToInteger() const;

    void Retain();
    void Release();
};

}
}

#endif
