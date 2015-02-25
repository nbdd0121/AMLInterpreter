#ifndef NORLIT_AML_INTEGER_H
#define NORLIT_AML_INTEGER_H

#include <cstdint>
#include <cstddef>

#include "Value.h"

namespace norlit {
namespace aml {

class Integer: public Value {
  private:
    uint64_t value;
  public:
    Integer(uint64_t value);
    virtual ~Integer();
    virtual void Dump(int ident) const override;

    virtual Handle<Integer> ToInteger() const override;

    uint64_t GetValue() {
        return value;
    }
};

}
}

#endif
