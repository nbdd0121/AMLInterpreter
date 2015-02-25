#include "Thunk.h"
#include "Interpreter.h"
#include "Context.h"

#include <cstdio>
#include <cstring>

namespace norlit {
namespace aml {

Thunk::Thunk(Context* s, ByteStream b) : ctx(s), data(b) {
}


Thunk::~Thunk() {
}

bool Thunk::IsResolved() const {
    return false;
}

Handle<Value> Thunk::Resolve() {
    Interpreter p(data, ctx, false);
    if (!(p.TryParseDefPackage()||p.TryParseDefBuffer())) {
        return this;
    }
    return p.GetReturnValue();
}

void Thunk::Dump(int ident) const {
    printf("Thunk");
}

Method::Method(Context* s, ByteStream b, uint8_t f) : ctx(s), data(b), flags(f) {}


Method::~Method() {}

void Method::Dump(int ident) const {
    printf("Method[Arg = %d]", GetArgumentCount());
}

bool Method::IsMethod() const {
    return true;
}

}
}