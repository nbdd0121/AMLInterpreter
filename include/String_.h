#ifndef NORLIT_AML_STRING_H
#define NORLIT_AML_STRING_H

#include <cstdint>
#include <cstddef>

#include "Value.h"

namespace norlit {
namespace aml {

class String: public Value {
  private:
    size_t length;
    char *value;
  public:
    String(const char *value);
    virtual ~String();
    virtual void Dump(int ident) const override;

    const char *GetValue() const {
        return value;
    }

    size_t Length() const {
        return length;
    }
};

}
}

#endif
