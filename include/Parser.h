#ifndef NORLIT_AML_PARSER_H
#define NORLIT_AML_PARSER_H

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

    uint16_t PeekOp();
    bool lazy;
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
    bool TryParseType2Opcode() {
        return false;
    }

    bool TryParseDefBuffer();
    bool TryParseDefPackage();

    bool TryParseDefVarPackage() {
        return false;
    }

    /* 20.2.6.1 Arg Objects Encoding */
    bool TryParseArgObj();

    /* 20.2.6.2 Local Objects Encoding */
    bool TryParseLocalObj();
};

}
}

#endif