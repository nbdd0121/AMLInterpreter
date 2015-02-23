#ifndef NORLIT_ACPI_AML_SCOPE_H
#define NORLIT_ACPI_AML_SCOPE_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Handle.h"
#include "Name.h"

namespace norlit {
namespace acpi {
namespace aml {

class Scope : public Value {
  private:
    struct Node {
        char name[4];
        Handle<Value> value;
        Node* next;
    };
    Node* node = nullptr;
  public:
    Scope();
    virtual ~Scope();
    virtual void Dump(int ident) const override;

    bool Has(const char* id);
    Handle<Value> Item(const char* id);
    void Item(const char* id, Value* val);
};

}
}
}

#endif
