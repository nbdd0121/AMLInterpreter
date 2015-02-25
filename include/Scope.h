#ifndef NORLIT_AML_SCOPE_H
#define NORLIT_AML_SCOPE_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Handle.h"
#include "Name.h"

namespace norlit {
namespace aml {

class Scope : public Value {
  protected:
    struct Node {
        uint32_t name;
        Handle<Value> value;
        Node *next;
    };

    Node *node = nullptr;

    virtual const char* GetTypeName() const;
  public:
    Scope();
    virtual ~Scope();
    virtual void Dump(int ident) const override;
    virtual Handle<Value> Resolve() override;

    virtual bool IsScope() const override;

    bool Has(uint32_t id);
    Handle<Value> Item(uint32_t id);
    void Item(uint32_t id, Value *val);
    void Item(const char* id, Value *val);
};

class Device : public Scope {
  protected:
    virtual const char* GetTypeName() const override;
};

}
}

#endif
