#ifndef NORLIT_ACPI_AML_BYTECODE_H
#define NORLIT_ACPI_AML_BYTECODE_H

#include <cstdint>
#include <cstddef>

namespace norlit {
namespace acpi {
namespace aml {

class Bytecode {
  private:
    uint8_t *bytecodes;
    size_t pointer = 0;
    size_t length;
  public:
    Bytecode(uint8_t* b, size_t length);
    ~Bytecode();

    inline uint8_t NextBytedata() {
        return bytecodes[pointer++];
    }

    inline uint8_t PeekBytedata() {
        return bytecodes[pointer];
    }

    inline void Pushback() {
        pointer--;
    }

    inline void Consume() {
        pointer++;
    }

    inline bool ConsumeIf(uint8_t exp) {
        if (PeekBytedata() == exp) {
            Consume();
            return true;
        } else {
            return false;
        }
    }

    inline size_t GetPointer() {
        return pointer;
    }

    inline void SetPointer(size_t ptr) {
        pointer = ptr;
    }

    inline bool IsEOF() {
        return pointer >= length;
    }

    inline Bytecode Slice(size_t from, size_t to) {
        return Bytecode(bytecodes + from, to - from);
    }
};

}
}
}

#endif
