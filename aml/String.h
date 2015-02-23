#ifndef NORLIT_ACPI_AML_STRING_H
#define NORLIT_ACPI_AML_STRING_H

#include <cstdint>
#include <cstddef>

#include "Value.h"

namespace norlit {
namespace acpi {
namespace aml {

class String: public Value {
  private:
    size_t length;
    char* value;
  public:
    String(size_t length, char* value);
    virtual ~String();
    virtual void Dump(int ident) const override;

    const char* GetValue() {
        return value;
    }
};

}
}
}

#endif
