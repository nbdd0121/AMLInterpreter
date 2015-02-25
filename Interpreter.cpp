#include "Parser.h"
#include "Name.h"
#include "Integer.h"
#include "String_.h"
#include "Thunk.h"
#include "Package.h"
#include "Context.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>

enum {
    NullName = 0x00,
    DualNamePrefix = 0x2E,
    MultiNamePrefix = 0x2F,
    RootChar = '\\',
    ParentPrefixChar = '^',
};

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

    CreateDWordFieldOp = 0x8A,
    CreateWordFieldOp = 0x8B,
    CreateByteFieldOp = 0x8C,
    CreateBitFieldOp = 0x8D,

    CreateQWordFieldOp = 0x8F,

    OnesOp = 0xFF,
};

enum {
    MutexOp = 0x5B01,
    EventOp = 0x5B02,

    RevisionOp = 0x5B30,

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

#define NORLIT_INTPRETER_REVISION 1

namespace norlit {
namespace aml {

Interpreter::Interpreter(ByteStream stream, Context* c, bool lazy) : stream(stream), context(c), lazy(lazy) {

}

bool Interpreter::Unexpected() {
    printf("[Opcode %x, %x]", stream.Peek(), stream.Peek(2));
    exit(0);
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
    return TryParseType2Opcode() | TryParseDataObject() | TryParseArgObj() | TryParseLocalObj();
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
    assert(0);
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