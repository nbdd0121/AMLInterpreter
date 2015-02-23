#ifndef NORLIT_ACPI_AML_METHOD_H
#define NORLIT_ACPI_AML_METHOD_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Bytecode.h"

namespace norlit {
namespace acpi {
namespace aml {

class Method: public Value {
  private:
    Bytecode bytecode;
    uint8_t flags;
  public:
    Method(uint8_t flags, Bytecode bc);
    virtual ~Method();
    virtual void Dump(int ident) const override;
    virtual bool IsMethod() const override final;

    size_t GetArgumentCount() const {
        return flags & 7;
    }
};

}
}
}

#endif
