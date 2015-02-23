#ifndef NORLIT_ACPI_AML_INTEGER_H
#define NORLIT_ACPI_AML_INTEGER_H

#include <cstdint>
#include <cstddef>

#include "Value.h"

namespace norlit {
namespace acpi {
namespace aml {

class Integer: public Value {
  private:
    uint64_t value;
  public:
    Integer(uint64_t value);
    virtual ~Integer();
    virtual void Dump() const override;

    uint64_t GetValue() {
        return value;
    }
};

}
}
}

#endif
