#ifndef NORLIT_ACPI_AML_INTERPRETER_H
#define NORLIT_ACPI_AML_INTERPRETER_H

#include <cstdint>
#include <cstddef>

#include "Bytecode.h"
#include "Handle.h"
#include "Value.h"
#include "Scope.h"

#define NORLIT_INTPRETER_REVISION 1

namespace norlit {
namespace acpi {
namespace aml {

class Name;

class Interpreter {
  private:
    enum {
        ZeroOp = 0x00,

        OneOp = 0x01,
        AliasOp = 0x06,
        NameOp = 0x08,

        BytePrefix = 0x0A,
        WordPrefix = 0x0B,
        DWordPrefix = 0x0C,
        StringPrefix = 0x0D,
        QWordPrefix = 0x0E,

        ScopeOp = 0x10,
        BufferOp = 0x11,
        PackageOp = 0x12,
        MethodOp = 0x14,

        DualNamePrefix = 0x2E,
        MultiNamePrefix = 0x2F,

        ExtOpPrefix = 0x5B,

        AddOp = 0x72,
        ShiftLeftOp = 0x79,


        OnesOp = 0xFF,


        NullName = 0x00,

        MutexExtOp = 0x01,
        RevisionExtOp = 0x30,
        OpRegionExtOp = 0x80,
        FieldExtOp = 0x81,
        DeviceExtOp = 0x82,
    };

    Bytecode& bytecode;
    Handle<Value> returnValue;
    Handle<Scope> root;
    Handle<Name> path;
  public:
    Interpreter(Bytecode& bc, Scope* root, Name* ref);
    ~Interpreter();

    bool unexpected();

    /* 20.2.2 */
    void ParseNameSeg();
    bool TryParseNameString();
    void ParsePrefixPath();
    bool TryParseNamePath(Name* parent);
    bool TryParseSimpleName();
    bool TryParseSuperName();
    void ParseTarget();

    /* 20.2.3 */
    bool TryParseComputationalData();
    bool TryParseDataObject();
    bool TryParseDataRefObject();

    /* 20.2.4 */
    uint32_t ParsePkgLength();

    /* 20.2.5 */
    bool tryParseTermObj();
    void parseTermList();
    bool TryParseTermArg();
    void ParseTermArg();
    bool TryParseMethodInvocation();

    /* 20.2.5.1 */
    bool tryParseNameSpaceModifierObj();
    void parseDefName();
    void parseDefScope();

    /* 20.2.5.2 */
    bool TryParseNamedObj();
    // void ParseDefBankField();
    // void ParseDefCreateBitField();
    // void ParseDefCreateByteField();
    // void ParseDefCreateDWordField();
    // void ParseDefCreateField();
    // void ParseDefCreateQWordField();
    // void ParseDefDataRegion();
    void ParseDefDevice();
    // DefEvent
    void ParseDefField();
    // DefIndexField
    void ParseDefMethod();
    void ParseDefMutex();
    void ParseDefOpRegion();
    // void ParseDefPowerRes();
    // void ParseDefProcessor();
    // void ParseDefThermalZone();

    /* 20.2.5.3 Type 1 Opcodes */
    bool TryParseType1Opcode();

    /* 20.2.5.4 Type 2 Opcodes */
    bool TryParseType2Opcode();
    // DefAcquire |
    void ParseDefAdd();
    /*| DefAnd | DefBuffer | DefConcat |
    	DefConcatRes | DefCondRefOf | DefCopyObject | DefDecrement |
    	DefDerefOf | DefDivide | DefFindSetLeftBit | DefFindSetRightBit |
    	DefFromBCD | DefIncrement | DefIndex | DefLAnd | DefLEqual |
    	DefLGreater | DefLGreaterEqual | DefLLess | DefLLessEqual | DefMid |
    	DefLNot | DefLNotEqual | DefLoadTable | DefLOr | DefMatch | DefMod |
    	DefMultiply | DefNAnd | DefNOr | DefNot | DefObjectType | DefOr |*/
    bool TryParseDefPackage();
    // DefVarPackage | DefRefOf |
    void ParseDefShiftLeft();
    //| DefShiftRight |
    // DefSizeOf | DefStore | DefSubtract | DefTimer | DefToBCD | DefToBuffer |
    // DefToDecimalString | DefToHexString | DefToInteger | DefToString |
    // DefWait | DefXOr | MethodInvocation*/
};

}
}
}

#endif
