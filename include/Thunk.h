#ifndef NORLIT_AML_THUNK_H
#define NORLIT_AML_THUNK_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Handle.h"
#include "ByteStream.h"

namespace norlit {
namespace aml {

class Context;

class Thunk: public Value {
  private:
    Handle<Context> ctx;
    ByteStream data;
  public:
    Thunk(Context *ctx, ByteStream data);
    virtual ~Thunk();
    virtual Handle<Value> Resolve() override;
    virtual bool IsResolved() const override;
    virtual void Dump(int ident) const override;
};

class Method: public Value {
  private:
    Handle<Context> ctx;
    ByteStream data;
    uint8_t flags;
  public:
    Method(Context *ctx, ByteStream data, uint8_t flags);
    virtual ~Method();
    virtual void Dump(int ident) const override;

    uint8_t GetArgumentCount() const {
        return flags & 7;
    }
};

}
}

#include "Context.h"

#endif
