#ifndef NORLIT_ACPI_AML_INTERPRETER_H
#define NORLIT_ACPI_AML_INTERPRETER_H

#include <cstdint>
#include <cstddef>

#include "Bytecode.h"
#include "Handle.h"
#include "Value.h"

namespace norlit {
namespace acpi {
namespace aml {

class Interpreter {
  private:
    enum {
        ZeroOp = 0x00,
        NullName = 0x00,

        OneOp = 0x01,
        AliasOp = 0x06,
        NameOp = 0x08,

        BytePrefix = 0x0A,
        WordPrefix = 0x0B,
        DWordPrefix = 0x0C,
        QWordPrefix = 0x0E,

        ScopeOp = 0x10,
        PackageOp = 0x12,

        DualNamePrefix = 0x2E,
        MultiNamePrefix = 0x2F,

        ExtOpPrefix = 0x5B,


        OnesOp = 0xFF,
    };

    Bytecode& bytecode;
    Handle<Value> returnValue;
  public:
    Interpreter(Bytecode& bc);
    ~Interpreter();

    bool unexpected();

    /* 20.2.2 */
    void ParseNameSeg();
    bool tryParseNamePath();
    bool tryParseNameString();

    /* 20.2.3 */
    bool TryParseComputationalData();
    bool TryParseDataObject();
    bool TryParseDataRefObject();

    /* 20.2.4 */
    uint32_t parsePkgLength();

    /* 20.2.5 */
    bool tryParseTermObj();
    void parseTermList();

    /* 20.2.5.1 */
    bool tryParseNameSpaceModifierObj();
    void parseDefName();
    void parseDefScope();

    /* 20.2.5.2 */
    bool TryParseNamedObj();


    /* 20.2.5.4 Type 2 Opcodes */
    bool TryParseDefPackage();
};

}
}
}

#endif
