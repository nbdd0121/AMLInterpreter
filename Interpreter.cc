#include "Interpreter.h"
#include "Name.h"
#include "Integer.h"
#include "String_.h"
#include "Thunk.h"
#include "Package.h"
#include "Context.h"
#include "OSDep.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define NORLIT_INTPRETER_REVISION 1

namespace norlit {
namespace aml {

enum NameChar {
    NullName = 0x00,
    DualNamePrefix = 0x2E,
    MultiNamePrefix = 0x2F,
    RootChar = '\\',
    ParentPrefixChar = '^',
};

enum Opcode {
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
    VarPackageOp = 0x13,

    MethodOp = 0x14,

    ExtOpPrefix = 0x5B,

    Local0Op = 0x60,
    Local1Op = 0x61,
    Local2Op = 0x62,
    Local3Op = 0x63,
    Local4Op = 0x64,
    Local5Op = 0x65,
    Local6Op = 0x66,
    Local7Op = 0x67,
    Arg0Op = 0x68,
    Arg1Op = 0x69,
    Arg2Op = 0x6A,
    Arg3Op = 0x6B,
    Arg4Op = 0x6C,
    Arg5Op = 0x6D,
    Arg6Op = 0x6E,

    StoreOp = 0x70,
    RefOfOp = 0x71,
    AddOp = 0x72,
    ConcatOp = 0x73,
    SubtractOp = 0x74,
    IncrementOp = 0x75,
    DecrementOp = 0x76,
    MultiplyOp = 0x77,
    DivideOp = 0x78,
    ShiftLeftOp = 0x79,
    ShiftRightOp = 0x7A,
    AndOp = 0x7B,
    NandOp = 0x7C,
    OrOp = 0x7D,
    NorOp = 0x7E,
    XorOp = 0x7F,
    NotOp = 0x80,
    FindSetLeftBitOp = 0x81,
    FindSetRightBitOp = 0x82,
    DerefOfOp = 0x83,
    ConcatResOp = 0x84,
    ModOp = 0x85,

    SizeOfOp = 0x87,
    IndexOp = 0x88,
    MatchOp = 0x89,
    CreateDWordFieldOp = 0x8A,
    CreateWordFieldOp = 0x8B,
    CreateByteFieldOp = 0x8C,
    CreateBitFieldOp = 0x8D,
    ObjectTypeOp = 0x8E,
    CreateQWordFieldOp = 0x8F,
    LandOp = 0x90,
    LorOp = 0x91,
    LnotOp = 0x92,
    LequalOp = 0x93,
    LgreaterOp = 0x94,
    LlessOp = 0x95,
    ToBufferOp = 0x96,
    ToDecimalStringOp = 0x97,
    ToHexStringOp = 0x98,
    ToIntegerOp = 0x99,
    ToStringOp = 0x9C,

    CopyObjectOp = 0x9D,
    MidOp = 0x9E,

    OnesOp = 0xFF,
};

enum ExtOpcode {
    MutexOp = 0x5B01,
    EventOp = 0x5B02,

    CondRefOfOp = 0x5B12,

    LoadTableOp = 0x5B1F,

    AcquireOp = 0x5B23,

    WaitOp = 0x5B25,

    FromBCDOp = 0x5B28,
    ToBCDOp = 0x5B29,

    RevisionOp = 0x5B30,

    TimerOp = 0x5B33,

    OpRegionOp = 0x5B80,
    FieldOp = 0x5B81,
    DeviceOp = 0x5B82,
    ProcessorOp = 0x5B83,
    PowerResOp = 0x5B84,
    ThermalZoneOp = 0x5b85,
    IndexFieldOp = 0x5B86,
    BankFieldOp = 0x5B87,
    DataRegionOp = 0x5B88,
};

Interpreter::Interpreter(ByteStream stream, Context* c, bool lazy) : stream(stream), context(c), lazy(lazy) {

}

bool Interpreter::Unexpected() {
    context->GetRoot()->Dump(0);
    printf("[Opcode %x, %x]", stream.Peek(), stream.Peek(2));
    aml_os_panic("Unexpected or unimplemnted opcode");
    return false;
}

uint16_t Interpreter::PeekOp() {
    uint8_t peek = stream.Peek();
    if (peek == ExtOpPrefix) {
        return ExtOpPrefix << 8 | stream.Peek(2);
    } else {
        return peek;
    }
}

void Interpreter::GetReference() {
    return_ = context->Get((Name*)return_, true);
    if (!return_) {
        aml_os_panic("Unresolve reference");
    }
}

bool Interpreter::TryParseNameSeg(uint32_t& ret) {
    uint8_t* pointer = stream.GetRawPointer();
    uint8_t a = stream.Peek();
    if (!(a >= 'A'&&a <= 'Z' || a == '_')) {
        return false;
    }
    stream.Consume();
    uint8_t b = stream.Next();
    b >= 'A'&&b <= 'Z' || b >= '0'&&b <= '9' || b == '_' || Unexpected();
    uint8_t c = stream.Next();
    c >= 'A'&&c <= 'Z' || c >= '0'&&c <= '9' || c == '_' || Unexpected();
    uint8_t d = stream.Next();
    d >= 'A'&&d <= 'Z' || d >= '0'&&d <= '9' || d == '_' || Unexpected();
    ret = Name::PackNameSegment(pointer);
    return true;
}

bool Interpreter::TryParseNamePath(int& len, uint32_t*& ret) {
    switch (stream.Peek()) {
    case NullName:
        stream.Consume();
        len = 0;
        ret = nullptr;
        return true;
    case DualNamePrefix:
        stream.Consume();
        len = 2;
        ret = new uint32_t[2];
        TryParseNameSeg(ret[0]) || Unexpected();
        TryParseNameSeg(ret[1]) || Unexpected();
        return true;
    case MultiNamePrefix:
        stream.Consume();
        len = stream.Next();
        ret = new uint32_t[len];
        for (int i = 0; i < len; i++) {
            TryParseNameSeg(ret[i]) || Unexpected();
        }
        return true;
    }
    uint32_t r;
    if (!TryParseNameSeg(r)) {
        return false;
    }
    len = 1;
    ret = new uint32_t[1] {r};
    return true;
}

bool Interpreter::TryParseNameString() {
    int len;
    uint32_t *val;
    /* Patch: This is used to distinguish between NullName and ZeroOp */
    if (stream.Peek() == NullName) {
        return false;
    }
    if (stream.ConsumeIf(RootChar)) {
        TryParseNamePath(len, val) || Unexpected();
        return_ = new Name(-1, len, val);
        return true;
    } else if (stream.ConsumeIf(ParentPrefixChar)) {
        int depth = 1;
        while (stream.ConsumeIf(ParentPrefixChar)) {
            depth++;
        }
        TryParseNamePath(len, val) || Unexpected();
        return_ = new Name(depth, len, val);
        return true;
    }
    if (!TryParseNamePath(len, val)) {
        return false;
    }
    return_ = new Name(0, len, val);
    return true;
}

bool Interpreter::TryParseSimpleName() {
    return TryParseNameString() || TryParseArgObj() || TryParseLocalObj();
}

bool Interpreter::TryParseSuperName() {
    return TryParseSimpleName(); // TODO DebugObj, Type6Obj
}

void Interpreter::ParseTarget() {
    if (!TryParseSuperName()) {
        if (stream.ConsumeIf(NullName)) {
            return_ = nullptr;
        } else {
            Unexpected();
        }
    }
}

/* 20.2.3 Data Objects Encoding */
bool Interpreter::TryParseComputationalData() {
    switch (PeekOp()) {
    case BytePrefix:
        stream.Consume();
        return_ = new Integer(stream.Next());
        return true;
    case WordPrefix: {
        stream.Consume();
        uint16_t value = stream.Next();
        value |= stream.Next() << 8;
        return_ = new Integer(value);
        return true;
    }
    case DWordPrefix: {
        stream.Consume();
        uint32_t value = stream.Next();
        value |= stream.Next() << 8;
        value |= stream.Next() << 16;
        value |= stream.Next() << 24;
        return_ = new Integer(value);
        return true;
    }
    case QWordPrefix: {
        stream.Consume();
        uint64_t value = stream.Next();
        value |= stream.Next() << 8;
        value |= stream.Next() << 16;
        value |= stream.Next() << 24;
        value |= (uint64_t)stream.Next() << 32;
        value |= (uint64_t)stream.Next() << 40;
        value |= (uint64_t)stream.Next() << 48;
        value |= (uint64_t)stream.Next() << 56;
        return_ = new Integer(value);
        return true;
    }
    case StringPrefix: {
        stream.Consume();
        char* ptr = (char*)stream.GetRawPointer();
        Handle<String> ret = new String(ptr);
        stream.Consume(ret->Length() + 1);
        return_ = ret;
        return true;
    }
    case ZeroOp:
        stream.Consume();
        return_ = new Integer(0);
        return true;
    case OneOp:
        stream.Consume();
        return_ = new Integer(1);
        return true;
    case OnesOp:
        stream.Consume();
        return_ = new Integer(-1);
        return true;
    case RevisionOp:
        stream.Consume(2);
        return_ = new Integer(NORLIT_INTPRETER_REVISION);
        return true;
    case BufferOp:
        TryParseDefBuffer() || Unexpected();
        return true;
    default:
        return false;
    }
}

bool Interpreter::TryParseDataObject() {
    return TryParseComputationalData() || TryParseDefPackage() || TryParseDefVarPackage();
}

bool Interpreter::TryParseDataRefObject() {
    return TryParseDataObject(); // TODO ObjectReference & DDBHandle
}

/* 20.2.4 Package Length Encoding */
uint32_t Interpreter::ParsePkgLength() {
    uint8_t pkgLeadByte = stream.Next();
    uint32_t length;
    switch (pkgLeadByte >> 6) {
    case 0:
        length = pkgLeadByte;
        break;
    case 1:
        length = pkgLeadByte & 0xF;
        length |= stream.Next() << 4;
        break;
    case 2:
        length = pkgLeadByte & 0xF;
        length |= stream.Next() << 4;
        length |= stream.Next() << 12;
        break;
    default:
        length = pkgLeadByte & 0xF;
        length |= stream.Next() << 4;
        length |= stream.Next() << 12;
        length |= stream.Next() << 20;
        break;
    }
    return length;
}

/* 20.2.5 Term Objects Encoding */
bool Interpreter::TryParseTermObj() {
    return TryParseNameSpaceModifierObj() || TryParseNamedObj() || TryParseType1Opcode() || TryParseType2Opcode();
}

void Interpreter::ParseTermList() {
    while (!stream.IsEOF()) {
        TryParseTermObj() || Unexpected();
    }
}

bool Interpreter::TryParseTermArg() {
    return TryParseType2Opcode() || TryParseDataObject() || TryParseArgObj() || TryParseLocalObj();
}

bool Interpreter::TryParseMethodInvocation() {
    if (!TryParseNameString()) {
        return false;
    }
    Handle<Value> obj = context->Get((Name*)return_, true);
    if (!obj)
        aml_os_panic("Unresovled reference");
    if (!obj->IsMethod()) {
        return_ = obj;
    } else {
        Unexpected();
    }
    return true;
}

bool Interpreter::TryParseObject() {
    return TryParseNameSpaceModifierObj() || TryParseNamedObj();
}

void Interpreter::ParseObjList() {
    while (!stream.IsEOF()) {
        TryParseObject() || Unexpected();
    }
}

/* 20.2.5.1 Namespace Modifier Objects Encoding */
bool Interpreter::TryParseNameSpaceModifierObj() {
    switch (stream.Peek()) {
    case AliasOp:
        ParseDefAlias();
        return true;
    case NameOp:
        ParseDefName();
        return true;
    case ScopeOp:
        ParseDefScope();
        return true;
    }
    return false;
}

void Interpreter::ParseDefAlias() {
    stream.Next();
    TryParseNameString() || Unexpected();
    GetReference();
    Handle<Value> obj = return_;

    TryParseNameString() || Unexpected();
    context->Put((Name*)return_, obj);
    return_ = nullptr;
}

void Interpreter::ParseDefName() {
    stream.Next();
    TryParseNameString() || Unexpected();
    Handle<Name> name = (Name*)return_;
    TryParseDataRefObject() || Unexpected();
    context->Put(name, return_);
    return_ = nullptr;
}

void Interpreter::ParseDefScope() {
    stream.Consume();
    size_t pointer = stream.GetPointer();
    size_t pkgLength = ParsePkgLength();

    TryParseNameString() || Unexpected();
    Handle<Name> name = context->Normalize((Name*)return_);
    Interpreter parser(stream.Slice(stream.GetPointer(), pointer+pkgLength), new Context(context->GetRoot(), name));
    parser.ParseTermList();

    stream.SetPointer(pointer + pkgLength);
}

/* 20.2.5.2 Named Objects Encoding */
bool Interpreter::TryParseNamedObj() {
    switch (PeekOp()) {
    case MethodOp:
        ParseDefMethod();
        break;
    case CreateDWordFieldOp:
        ParseDefCreateDWordField();
        break;
    case CreateWordFieldOp:
        ParseDefCreateWordField();
        break;
    case CreateByteFieldOp:
        ParseDefCreateByteField();
        break;
    case CreateBitFieldOp:
        ParseDefCreateBitField();
        break;
    case CreateQWordFieldOp:
        ParseDefCreateQWordField();
        break;
    case MutexOp:
        ParseDefMutex();
        break;
    case EventOp:
        ParseDefEvent();
        break;
    case OpRegionOp:
        ParseDefOpRegion();
        break;
    case FieldOp:
        ParseDefField();
        break;
    case DeviceOp:
        ParseDefDevice();
        break;
    case ProcessorOp:
        ParseDefProcessor();
        break;
    case PowerResOp:
        ParseDefPowerRes();
        break;
    case ThermalZoneOp:
        ParseDefThermalZone();
        break;
    case IndexFieldOp:
        ParseDefIndexField();
        break;
    case BankFieldOp:
        ParseDefBankField();
        break;
    case DataRegionOp:
        ParseDefDataRegion();
        break;
    default:
        return false;
    }
    return true;
}

void Interpreter::ParseDefBankField() {
    Unexpected();
}
void Interpreter::ParseDefCreateBitField() {
    Unexpected();
}
void Interpreter::ParseDefCreateByteField() {
    Unexpected();
}
void Interpreter::ParseDefCreateDWordField() {
    Unexpected();
}
void Interpreter::ParseDefCreateField() {
    Unexpected();
}
void Interpreter::ParseDefCreateQWordField() {
    Unexpected();
}
void Interpreter::ParseDefCreateWordField() {
    Unexpected();
}
void Interpreter::ParseDefDataRegion() {
    Unexpected();
}

void Interpreter::ParseDefDevice() {
    stream.Consume(2);
    size_t pointer = stream.GetPointer();
    size_t pkgLength = ParsePkgLength();

    TryParseNameString() || Unexpected();
    Handle<Name> name = context->Normalize((Name*)return_);
    context->Put(name, new Device());

    Interpreter parser(stream.Slice(stream.GetPointer(), pointer + pkgLength), new Context(context->GetRoot(), name));
    parser.ParseObjList();

    stream.SetPointer(pointer + pkgLength);
}

void Interpreter::ParseDefEvent() {
    Unexpected();
}
void Interpreter::ParseDefField() {
    stream.Consume(2);
    printf("Field()\n");
    size_t pointer = stream.GetPointer();
    size_t length = ParsePkgLength();

    stream.SetPointer(pointer + length);
}
void Interpreter::ParseDefIndexField() {
    Unexpected();
}

void Interpreter::ParseDefMethod() {
    stream.Consume();

    size_t pointer = stream.GetPointer();
    size_t pkgLength = ParsePkgLength();

    TryParseNameString() || Unexpected();
    Handle<Name> name = context->Normalize((Name*)return_);

    uint8_t flags = stream.Next();
    ByteStream code = stream.Slice(stream.GetPointer(), pointer + pkgLength);
    Handle<Method> m = new Method(new Context(context->GetRoot(), name), code, flags);

    context->Put(name, m);

    stream.SetPointer(pointer + pkgLength);
}


void Interpreter::ParseDefMutex() {
    Unexpected();
}
void Interpreter::ParseDefOpRegion() {
    stream.Consume(2);
    printf("OpRegion(");
    TryParseNameString() || Unexpected();
    Handle<Name> name = (Name*)return_;
    name->Dump(0);
    printf(" ");
    uint8_t RegionSpace = stream.Next();
    TryParseTermArg() || Unexpected();
    return_->Dump(0);
    printf(" ");
    TryParseTermArg() || Unexpected();
    return_->Dump(0);
    printf(")\n");
}
void Interpreter::ParseDefPowerRes() {
    Unexpected();
}
void Interpreter::ParseDefProcessor() {
    Unexpected();
}
void Interpreter::ParseDefThermalZone() {
    Unexpected();
}

bool Interpreter::TryParseType2Opcode() {
    switch (PeekOp()) {
    case AcquireOp:
        ParseDefAcquire();
        break;
    case AddOp:
        ParseDefAdd();
        break;
    case AndOp:
        ParseDefAnd();
        break;
    case BufferOp:
        TryParseDefBuffer() || Unexpected();
        break;
    case ConcatOp:
        ParseDefConcat();
        break;
    case ConcatResOp:
        ParseDefConcatRes();
        break;
    case CondRefOfOp:
        ParseDefCondRefOf();
        break;
    case CopyObjectOp:
        ParseDefCopyObject();
        break;
    case DecrementOp:
        ParseDefDecrement();
        break;
    case DerefOfOp:
        ParseDefDerefOf();
        break;
    case DivideOp:
        ParseDefDivide();
        break;
    case FindSetLeftBitOp:
        ParseDefFindSetLeftBit();
        break;
    case FindSetRightBitOp:
        ParseDefFindSetRightBit();
        break;
    case FromBCDOp:
        ParseDefFromBCD();
        break;
    case IncrementOp:
        ParseDefIncrement();
        break;
    case IndexOp:
        ParseDefIndex();
        break;
    case LandOp:
        ParseDefLAnd();
        break;
    case LequalOp:
        ParseDefLEqual();
        break;
    case LgreaterOp:
        ParseDefLGreater();
        break;
    case LlessOp:
        ParseDefLLess();
        break;
    case MidOp:
        ParseDefMid();
        break;
    case LnotOp:
        ParseDefLNot();
        break;
    case LoadTableOp:
        ParseDefLoadTable();
        break;
    case LorOp:
        ParseDefLOr();
        break;
    case MatchOp:
        ParseDefMatch();
        break;
    case ModOp:
        ParseDefMod();
        break;
    case MultiplyOp:
        ParseDefMultiply();
        break;
    case NandOp:
        ParseDefNAnd();
        break;
    case NorOp:
        ParseDefNOr();
        break;
    case NotOp:
        ParseDefNot();
        break;
    case ObjectTypeOp:
        ParseDefObjectType();
        break;
    case OrOp:
        ParseDefOr();
        break;
    case PackageOp:
        TryParseDefPackage() || Unexpected();
        break;
    case VarPackageOp:
        TryParseDefVarPackage() || Unexpected();
        break;
    case RefOfOp:
        ParseDefRefOf();
        break;
    case ShiftLeftOp:
        ParseDefShiftLeft();
        break;
    case ShiftRightOp:
        ParseDefShiftRight();
        break;
    case SizeOfOp:
        ParseDefSizeOf();
        break;
    case StoreOp:
        ParseDefStore();
        break;
    case SubtractOp:
        ParseDefSubtract();
        break;
    case TimerOp:
        ParseDefTimer();
        break;
    case ToBCDOp:
        ParseDefToBCD();
        break;
    case ToBufferOp:
        ParseDefToBuffer();
        break;
    case ToDecimalStringOp:
        ParseDefToDecimalString();
        break;
    case ToHexStringOp:
        ParseDefToHexString();
        break;
    case ToIntegerOp:
        ParseDefToInteger();
        break;
    case ToStringOp:
        ParseDefToString();
        break;
    case WaitOp:
        ParseDefWait();
        break;
    case XorOp:
        ParseDefXOr();
        break;
    default:
        return TryParseMethodInvocation();
    }
    return true;
}

void Interpreter::ParseDefAcquire() {
    Unexpected();
}

void Interpreter::ParseDefAdd() {
    stream.Consume();
    TryParseTermArg() || Unexpected();
    Handle<Integer> left = return_->ToInteger();
    TryParseTermArg() || Unexpected();
    Handle<Integer> result = new Integer(left->GetValue() + return_->ToInteger()->GetValue());
    ParseTarget();
    assert(!return_);
    return_ = result;
}

void Interpreter::ParseDefAnd() {
    Unexpected();
}

bool Interpreter::TryParseDefBuffer() {
    /* BufferOp */
    if (!stream.ConsumeIf(BufferOp)) return false;

    /* PkgLength */
    uint32_t pointer = stream.GetPointer();
    uint32_t pkgLength = ParsePkgLength();

    if (lazy) {
        ByteStream b = stream.Slice(pointer - 1, pointer + pkgLength);
        return_ = new Thunk(context, b);
    } else {
        TryParseTermArg() || Unexpected();
        size_t length = (size_t)return_->ToInteger()->GetValue();
        uint8_t* data = new uint8_t[length];
        size_t initDataLength = pointer + pkgLength - stream.GetPointer();
        if (length <= initDataLength) {
            memcpy(data, stream.GetRawPointer(), length);
        } else {
            memcpy(data, stream.GetRawPointer(), initDataLength);
            memset(data + initDataLength, 0, length - initDataLength);
        }
        return_ = new Buffer(length, data);
    }

    stream.SetPointer(pointer + pkgLength);
    return true;
}

void Interpreter::ParseDefConcat() {
    Unexpected();
}
void Interpreter::ParseDefConcatRes() {
    Unexpected();
}
void Interpreter::ParseDefCondRefOf() {
    Unexpected();
}
void Interpreter::ParseDefCopyObject() {
    Unexpected();
}
void Interpreter::ParseDefDecrement() {
    Unexpected();
}
void Interpreter::ParseDefDerefOf() {
    Unexpected();
}
void Interpreter::ParseDefDivide() {
    Unexpected();
}
void Interpreter::ParseDefFindSetLeftBit() {
    Unexpected();
}
void Interpreter::ParseDefFindSetRightBit() {
    Unexpected();
}
void Interpreter::ParseDefFromBCD() {
    Unexpected();
}
void Interpreter::ParseDefIncrement() {
    Unexpected();
}
void Interpreter::ParseDefIndex() {
    Unexpected();
}
void Interpreter::ParseDefLAnd() {
    Unexpected();
}
void Interpreter::ParseDefLEqual() {
    Unexpected();
}
void Interpreter::ParseDefLGreater() {
    Unexpected();
}
void Interpreter::ParseDefLGreaterEqual() {
    Unexpected();
}
void Interpreter::ParseDefLLess() {
    Unexpected();
}
void Interpreter::ParseDefLLessEqual() {
    Unexpected();
}
void Interpreter::ParseDefMid() {
    Unexpected();
}
void Interpreter::ParseDefLNot() {
    Unexpected();
}
void Interpreter::ParseDefLNotEqual() {
    Unexpected();
}
void Interpreter::ParseDefLoadTable() {
    Unexpected();
}
void Interpreter::ParseDefLOr() {
    Unexpected();
}
void Interpreter::ParseDefMatch() {
    Unexpected();
}
void Interpreter::ParseDefMod() {
    Unexpected();
}
void Interpreter::ParseDefMultiply() {
    Unexpected();
}
void Interpreter::ParseDefNAnd() {
    Unexpected();
}
void Interpreter::ParseDefNOr() {
    Unexpected();
}
void Interpreter::ParseDefNot() {
    Unexpected();
}
void Interpreter::ParseDefObjectType() {
    Unexpected();
}
void Interpreter::ParseDefOr() {
    Unexpected();
}

bool Interpreter::TryParseDefPackage() {
    /* PackageOp */
    if (!stream.ConsumeIf(PackageOp)) return false;

    /* PkgLength */
    uint32_t pointer = stream.GetPointer();
    uint32_t pkgLength = ParsePkgLength();

    if (lazy) {
        ByteStream b = stream.Slice(pointer - 1, pointer + pkgLength);
        return_ = new Thunk(context, b);

        stream.SetPointer(pointer + pkgLength);
    } else {

        /* NumElements */
        uint8_t numElements = stream.Next();
        Handle<Value> *package = new Handle<Value>[numElements];

        /* TermList */
        for (int i = 0; i < numElements; i++) {
            TryParseDataRefObject() || TryParseNameString() || Unexpected();
            if (!return_->IsResolved()) {
                return_ = context->Get((Name*)return_, true);
            }
            package[i] = return_;
        }

        /* We do not evaluate package right away, due to potential backward reference */
        return_ = new Package(numElements, package);
        assert(stream.GetPointer() == pointer + pkgLength);
    }
    return true;
}

bool Interpreter::TryParseDefVarPackage() {
    return false;
}

void Interpreter::ParseDefRefOf() {
    Unexpected();
}
void Interpreter::ParseDefShiftLeft() {
    Unexpected();
}
void Interpreter::ParseDefShiftRight() {
    Unexpected();
}
void Interpreter::ParseDefSizeOf() {
    Unexpected();
}
void Interpreter::ParseDefStore() {
    Unexpected();
}
void Interpreter::ParseDefSubtract() {
    Unexpected();
}
void Interpreter::ParseDefTimer() {
    Unexpected();
}
void Interpreter::ParseDefToBCD() {
    Unexpected();
}
void Interpreter::ParseDefToBuffer() {
    Unexpected();
}
void Interpreter::ParseDefToDecimalString() {
    Unexpected();
}
void Interpreter::ParseDefToHexString() {
    Unexpected();
}
void Interpreter::ParseDefToInteger() {
    Unexpected();
}
void Interpreter::ParseDefToString() {
    Unexpected();
}
void Interpreter::ParseDefWait() {
    Unexpected();
}
void Interpreter::ParseDefXOr() {
    Unexpected();
}

/* 20.2.6.1 Arg Objects Encoding */
bool Interpreter::TryParseArgObj() {
    switch (stream.Peek()) {
    case Arg0Op:
    case Arg1Op:
    case Arg2Op:
    case Arg3Op:
    case Arg4Op:
    case Arg5Op:
    case Arg6Op:
        assert(0);
        stream.Consume();
        return true;
    }
    return false;
}

/* 20.2.6.2 Local Objects Encoding */
bool Interpreter::TryParseLocalObj() {
    switch (stream.Peek()) {
    case Local0Op:
    case Local1Op:
    case Local2Op:
    case Local3Op:
    case Local4Op:
    case Local5Op:
    case Local6Op:
    case Local7Op:
        assert(0);
        stream.Consume();
        return true;
    }
    return false;
}

}
}