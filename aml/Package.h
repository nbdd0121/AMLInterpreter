#ifndef NORLIT_ACPI_AML_PACKAGE_H
#define NORLIT_ACPI_AML_PACKAGE_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Handle.h"

namespace norlit {
namespace acpi {
namespace aml {

class Package : public Value {
  private:
    size_t size;
    Handle<Value> *array;
  public:
    Package(size_t size);
    virtual ~Package();
    virtual void Dump(int ident) const override;

    size_t Size() {
        return size;
    }

    Handle<Value> Item(size_t id) {
        return array[id];
    }

    void Item(size_t id, Value* val) {
        array[id] = val;
    }
};

}
}
}

#endif
