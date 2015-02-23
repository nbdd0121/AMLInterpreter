#include "Value.h"

#include <iostream>

namespace norlit {
namespace acpi {
namespace aml {

Value::Value() {

}

Value::~Value() {
}

bool Value::IsMethod() const {
    return false;
}

}
}
}