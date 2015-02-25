#ifndef NORLIT_AML_INTERPRETER_H
#define NORLIT_AML_INTERPRETER_H

#include "ByteStream.h"
#include "Handle.h"
#include "Value.h"

#include <cstdint>

namespace norlit {
namespace aml {

class Name;
class Scope;
class Context;

class Interpreter {
  private:
    ByteStream stream;
    Handle<Context> context;
    Handle<Value> return_;
    bool lazy;

    uint16_t PeekOp();
    void GetReference();
  public:
    Interpreter(ByteStream stream, Context* ctx, bool lazy = true);

    bool Unexpected();
    Handle<Value> GetReturnValue() {
        return return_;
    }

    /* 20.2.2 Name Objects Encoding */
    bool TryParseNameSeg(uint32_t& ret);
    bool TryParseNamePath(int& len, uint32_t*& ret);
    bool TryParseNameString();
    bool TryParseSimpleName();
    bool TryParseSuperName();
    void ParseTarget();

    /* 20.2.3 Data Objects Encoding */
    bool TryParseComputationalData();
    bool TryParseDataObject();
    bool TryParseDataRefObject();

    /* 20.2.4 Package Length Encoding */
    uint32_t ParsePkgLength();

    /* 20.2.5 Term Objects Encoding */
    bool TryParseTermObj();
    void ParseTermList();
    bool TryParseTermArg();
    bool TryParseMethodInvocation();
    bool TryParseObject();
    void ParseObjList();

    /* 20.2.5.1 Namespace Modifier Objects Encoding */
    bool TryParseNameSpaceModifierObj();
    void ParseDefAlias();
    void ParseDefName();
    void ParseDefScope();

    /* 20.2.5.2 Named Objects Encoding */
    bool TryParseNamedObj();
    void ParseDefBankField();
    void ParseDefCreateBitField();
    void ParseDefCreateByteField();
    void ParseDefCreateDWordField();
    void ParseDefCreateField();
    void ParseDefCreateQWordField();
    void ParseDefCreateWordField();
    void ParseDefDataRegion();
    void ParseDefDevice();
    void ParseDefEvent();
    void ParseDefField();
    void ParseDefIndexField();
    void ParseDefMethod();
    void ParseDefMutex();
    void ParseDefOpRegion();
    void ParseDefPowerRes();
    void ParseDefProcessor();
    void ParseDefThermalZone();


    /* 20.2.5.3 */
    bool TryParseType1Opcode() {
        return false;
    }

    /* 20.2.5.4 */
    bool TryParseType2Opcode();
    void ParseDefAcquire();
    void ParseDefAdd();
    void ParseDefAnd();
    bool TryParseDefBuffer();
    void ParseDefConcat();
    void ParseDefConcatRes();
    void ParseDefCondRefOf();
    void ParseDefCopyObject();
    void ParseDefDecrement();
    void ParseDefDerefOf();
    void ParseDefDivide();
    void ParseDefFindSetLeftBit();
    void ParseDefFindSetRightBit();
    void ParseDefFromBCD();
    void ParseDefIncrement();
    void ParseDefIndex();
    void ParseDefLAnd();
    void ParseDefLEqual();
    void ParseDefLGreater();
    void ParseDefLGreaterEqual();
    void ParseDefLLess();
    void ParseDefLLessEqual();
    void ParseDefMid();
    void ParseDefLNot();
    void ParseDefLNotEqual();
    void ParseDefLoadTable();
    void ParseDefLOr();
    void ParseDefMatch();
    void ParseDefMod();
    void ParseDefMultiply();
    void ParseDefNAnd();
    void ParseDefNOr();
    void ParseDefNot();
    void ParseDefObjectType();
    void ParseDefOr();
    bool TryParseDefPackage();
    bool TryParseDefVarPackage();
    void ParseDefRefOf();
    void ParseDefShiftLeft();
    void ParseDefShiftRight();
    void ParseDefSizeOf();
    void ParseDefStore();
    void ParseDefSubtract();
    void ParseDefTimer();
    void ParseDefToBCD();
    void ParseDefToBuffer();
    void ParseDefToDecimalString();
    void ParseDefToHexString();
    void ParseDefToInteger();
    void ParseDefToString();
    void ParseDefWait();
    void ParseDefXOr();

    /* 20.2.6.1 Arg Objects Encoding */
    bool TryParseArgObj();

    /* 20.2.6.2 Local Objects Encoding */
    bool TryParseLocalObj();
};

}
}

#endif