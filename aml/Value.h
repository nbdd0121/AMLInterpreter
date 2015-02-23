#ifndef NORLIT_ACPI_AML_VALUE_H
#define NORLIT_ACPI_AML_VALUE_H

#include <cstdint>
#include <cstddef>

namespace norlit {
namespace acpi {
namespace aml {

class Value {
  private:
    int refCount = 0;
    template<typename T>
    friend class Handle;
  public:
    Value();
    virtual ~Value();

    virtual void Dump(int ident) const = 0;
};

}
}
}

#endif
