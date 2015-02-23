#include "Interpreter.h"
#include "Package.h"
#include "Integer.h"
#include "Name.h"
#include "String.h"
#include "Method.h"

#include <cassert>
#include <iostream>

namespace norlit {
namespace acpi {
namespace aml {

Interpreter::Interpreter(Bytecode& bc, Scope* root, Name* path)
    :bytecode(bc), root(root), path(path) {

}

Interpreter::~Interpreter() {}

bool Interpreter::unexpected() {
    printf("[Unknown byte %x]", bytecode.NextBytedata());
    printf("[Unknown byte %x]", bytecode.PeekBytedata());
    exit(0);
    return false;
}

/* 20.2.2 Name Objects Encoding */
void Interpreter::ParseNameSeg() {
    uint8_t a = bytecode.NextBytedata();
    assert(a >= 'A'&&a <= 'Z' || a == '_');
    uint8_t b = bytecode.NextBytedata();
    assert(b >= 'A'&&b <= 'Z' || b >= '0'&&b <= '9' || b == '_');
    uint8_t c = bytecode.NextBytedata();
    assert(c >= 'A'&&c <= 'Z' || c >= '0'&&c <= '9' || c == '_');
    uint8_t d = bytecode.NextBytedata();
    assert(d >= 'A'&&d <= 'Z' || d >= '0'&&d <= '9' || d == '_');
    returnValue = new NameSegment(a, b, c, d);
}

bool Interpreter::TryParseNameString() {
    if (bytecode.ConsumeIf('\\')) {
        TryParseNamePath(new RootPath()) || unexpected();
        return true;
    } else if (bytecode.PeekBytedata() == '^') {
        ParsePrefixPath();
        return true;
    } else {
        return TryParseNamePath(path);
    }
}

void Interpreter::ParsePrefixPath() {
    Handle<Name> name = path;
    while (bytecode.ConsumeIf('^')) {
        name = name->Parent();
    }
    TryParseNamePath(name) || unexpected();
}

bool Interpreter::TryParseNamePath(Name* parent) {
    uint8_t peek = bytecode.PeekBytedata();
    switch(peek) {
    case DualNamePrefix: {
        bytecode.Consume();
        ParseNameSeg();
        Handle<Name> name = parent ? new NamePath(parent, (NameSegment*)returnValue) : (Name*)returnValue;
        ParseNameSeg();
        returnValue = new NamePath(name, (NameSegment*)returnValue);
        return true;
    }
    case MultiNamePrefix: {
        bytecode.Consume();
        uint8_t SegCount = bytecode.NextBytedata();
        ParseNameSeg();
        Handle<Name> name = parent ? new NamePath(parent, (NameSegment*)returnValue) : (Name*)returnValue;
        for (int i = 1; i < SegCount; i++) {
            ParseNameSeg();
            name = new NamePath(name, (NameSegment*)returnValue);
        }
        returnValue = name;
        return true;
    }
    case NullName:
        bytecode.Consume();
        returnValue = parent;
        return true;
    }
    if (peek >= 'A'&&peek <= 'Z' || peek=='_') {
        ParseNameSeg();
        if (parent) {
            returnValue = new NamePath(parent, (NameSegment*)returnValue);
        }
        return true;
    }
    return false;
}

bool Interpreter::TryParseSimpleName() {
    return TryParseNameString(); //TODO ArgObj, LocalObj
}

bool Interpreter::TryParseSuperName() {
    return TryParseSimpleName(); //TODO DebugObj, Type6Opcode
}

void Interpreter::ParseTarget() {
    if (bytecode.ConsumeIf(NullName)) {
        returnValue = nullptr;
    } else {
        TryParseSuperName() || unexpected();
    }
}

/* 20.2.3 Data Objects Encoding */
bool Interpreter::TryParseComputationalData() {
    switch (bytecode.NextBytedata()) {
    case BytePrefix:
        /* ByteConst */
        returnValue = new Integer(bytecode.NextBytedata());
        return true;
    case WordPrefix: {
        /* WordConst */
        uint16_t value = bytecode.NextBytedata();
        value |= bytecode.NextBytedata() << 8;
        returnValue = new Integer(value);
        return true;
    }
    case DWordPrefix: {
        /* DWordConst */
        uint32_t value = bytecode.NextBytedata();
        value |= bytecode.NextBytedata() << 8;
        value |= bytecode.NextBytedata() << 16;
        value |= bytecode.NextBytedata() << 24;
        returnValue = new Integer(value);
        return true;
    }
    case QWordPrefix: {
        /* QWordConst */
        uint64_t value = bytecode.NextBytedata();
        value |= bytecode.NextBytedata() << 8;
        value |= bytecode.NextBytedata() << 16;
        value |= bytecode.NextBytedata() << 24;
        value |= (uint64_t)bytecode.NextBytedata() << 32;
        value |= (uint64_t)bytecode.NextBytedata() << 40;
        value |= (uint64_t)bytecode.NextBytedata() << 48;
        value |= (uint64_t)bytecode.NextBytedata() << 56;
        returnValue = new Integer(value);
        return true;
    }
    /* String */
    case StringPrefix: {
        char* ptr = (char*)bytecode.GetRawPointer();
        size_t length = strlen(ptr);
        char* str = new char[length];
        memcpy(str, ptr, length);
        bytecode.SetPointer(bytecode.GetPointer() + length + 1);
        returnValue = new String(length, str);
        return true;
    }
    /* ConstObj */
    case ZeroOp:
        returnValue = new Integer(0);
        return true;
    case OneOp:
        returnValue = new Integer(1);
        return true;
    case OnesOp:
        returnValue = new Integer(-1);
        return true;
    /* RevisionOp */
    case ExtOpPrefix:
        if (bytecode.ConsumeIf(RevisionExtOp)) {
            returnValue = new Integer(NORLIT_INTPRETER_REVISION);
            return true;
        } else {
            bytecode.Pushback();
            return false;
        }
    /* DefBuffer */
    case BufferOp:
        assert(0);
        return true;
    default:
        bytecode.Pushback();
        return false;
    }
}

bool Interpreter::TryParseDataObject() {
    return TryParseComputationalData() || TryParseDefPackage();
}

bool Interpreter::TryParseDataRefObject() {
    return TryParseDataObject();
}

/* 20.2.4 Package Length Encoding */
uint32_t Interpreter::ParsePkgLength() {
    uint8_t pkgLeadByte = bytecode.NextBytedata();
    uint32_t length;
    switch (pkgLeadByte >> 6) {
    case 0:
        length = pkgLeadByte;
        break;
    case 1:
        length = pkgLeadByte & 0xF;
        length |= bytecode.NextBytedata() << 4;
        break;
    case 2:
        length = pkgLeadByte & 0xF;
        length |= bytecode.NextBytedata() << 4;
        length |= bytecode.NextBytedata() << 12;
        break;
    default:
        length = 0;
        assert(0);
    }
    return length;
}

/* 20.2.5 */
bool Interpreter::tryParseTermObj() {
    return tryParseNameSpaceModifierObj() || TryParseNamedObj() || TryParseType1Opcode() || TryParseType2Opcode();
}

void Interpreter::parseTermList() {
    while (!bytecode.IsEOF() && tryParseTermObj());
}

bool Interpreter::TryParseTermArg() {
    return TryParseType2Opcode() || TryParseDataObject();// || ArgObj | LocalObj;
}

void Interpreter::ParseTermArg() {
    TryParseTermArg() || unexpected();// || ArgObj | LocalObj;
}

bool Interpreter::TryParseMethodInvocation() {
    if (!TryParseNameString())return false;
    returnValue->Dump(0);
    Handle<Value> obj = ((Name*)returnValue)->Get(root);
    if (obj->IsMethod()) {
        printf("MethodInvocation()");
        assert(0);
        Handle<Method> m = (Method*)obj;
        for (unsigned i = 0; i < m->GetArgumentCount(); i++) {
            TryParseTermArg() || unexpected();
        }
    } else {
        returnValue = obj;
    }
    return true;
}

bool Interpreter::tryParseNameSpaceModifierObj() {
    switch (bytecode.PeekBytedata()) {
    case AliasOp:
        assert(0);
    case NameOp:
        parseDefName();
        return true;
    case ScopeOp:
        parseDefScope();
        return true;
    }
    return false;
}

void Interpreter::parseDefName() {
    bytecode.NextBytedata();
    TryParseNameString() || unexpected();
    Handle<Name> name = (Name*)returnValue;
    TryParseDataRefObject() || unexpected();
    name->Put(root, returnValue);
}

void Interpreter::parseDefScope() {
    bytecode.NextBytedata();
    size_t ptr = bytecode.GetPointer();
    ptr += ParsePkgLength();
    TryParseNameString() || unexpected();
    Handle<Name> name = (Name*)returnValue;
    Bytecode scopeCode = bytecode.Slice(bytecode.GetPointer(), ptr);
    // Interpreter interp(scopeCode, root, name);
    // interp.parseTermList();

    bytecode.SetPointer(ptr);
}

/* 20.2.5.2 */
bool Interpreter::TryParseNamedObj() {
    switch (bytecode.PeekBytedata()) {
    case MethodOp:
        ParseDefMethod();
        break;
    case ExtOpPrefix:
        bytecode.Consume();
        switch (bytecode.PeekBytedata()) {
        case MutexExtOp:
            ParseDefMutex();
            break;
        case OpRegionExtOp:
            ParseDefOpRegion();
            break;
        case FieldExtOp:
            ParseDefField();
            break;
        case DeviceExtOp:
            ParseDefDevice();
            break;
        default:
            bytecode.Pushback();
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

// void ParseDefBankField();
// void ParseDefCreateBitField();
// void ParseDefCreateByteField();
// void ParseDefCreateDWordField();
// void ParseDefCreateField();
// void ParseDefCreateQWordField();
// void ParseDefDataRegion();
void Interpreter::ParseDefDevice() {
    bytecode.NextBytedata();
    printf("Device()\n");
    size_t pointer = bytecode.GetPointer();
    size_t pkgLength = ParsePkgLength();

    bytecode.SetPointer(pointer + pkgLength);
}
// DefEvent
void Interpreter::ParseDefField() {
    bytecode.NextBytedata();
    printf("Field()\n");
    size_t pointer = bytecode.GetPointer();
    size_t pkgLength = ParsePkgLength();

    bytecode.SetPointer(pointer + pkgLength);
}
// DefIndexField
void Interpreter::ParseDefMethod() {
    bytecode.NextBytedata();
    size_t pointer = bytecode.GetPointer();
    size_t pkgLength = ParsePkgLength();

    TryParseNameString() || unexpected();
    Handle<Name> name = (Name*)returnValue;

    uint8_t flags = bytecode.NextBytedata();
    Bytecode bc = bytecode.Slice(bytecode.GetPointer(), pointer + pkgLength);

    Handle<Method> mtd = new Method(flags, bc);
    name->Put(root, mtd);
    bytecode.SetPointer(pointer + pkgLength);
}

void Interpreter::ParseDefMutex() {
    bytecode.NextBytedata();
    printf("Mutex()\n");
    TryParseNameString() || unexpected();
    bytecode.NextBytedata();
}

void Interpreter::ParseDefOpRegion() {
    bytecode.NextBytedata();
    printf("OpRegion(");
    TryParseNameString() || unexpected();
    Handle<Name> name = (Name*)returnValue;
    name->Dump(0);
    printf(" ");
    uint8_t RegionSpace = bytecode.NextBytedata();
    TryParseTermArg() || unexpected();
    // returnValue->Dump(0);
    printf(" ");
    TryParseTermArg() || unexpected();
    // returnValue->Dump(0);
    printf(")\n");
}
// void ParseDefPowerRes();
// void ParseDefProcessor();
// void ParseDefThermalZone();

/* 20.2.5.3 */
bool Interpreter::TryParseType1Opcode() {
    return false;
}

/* 20.2.5.4 */
bool Interpreter::TryParseType2Opcode() {
    switch (bytecode.PeekBytedata()) {
    case AddOp:
        ParseDefAdd();
        break;
    case ShiftLeftOp:
        ParseDefShiftLeft();
        break;
    default:
        return TryParseMethodInvocation();
    }
    return true;
}


void Interpreter::ParseDefAdd() {
    bytecode.NextBytedata();
    printf("Add()\n");
    TryParseTermArg() || unexpected();
    TryParseTermArg() || unexpected();
    ParseTarget();
}

void Interpreter::ParseDefShiftLeft() {
    bytecode.NextBytedata();
    printf("ShiftLeft()\n");
    TryParseTermArg() || unexpected();
    TryParseTermArg() || unexpected();
    ParseTarget();
}

bool Interpreter::TryParseDefPackage() {
    /* PackageOp */
    if (!bytecode.ConsumeIf(PackageOp)) return false;
    uint32_t pointer = bytecode.GetPointer();
    /* PkgLength */
    pointer += ParsePkgLength();
    /* NumElements */
    uint8_t numElements = bytecode.NextBytedata();
    Handle<Package> package = new Package(numElements);
    /* TermList */
    for (int i = 0; i < numElements; i++) {
        TryParseDataRefObject() || unexpected();
        package->Item(i, returnValue);
    }
    returnValue = package;
    assert(bytecode.GetPointer() == pointer);
    return true;
}

}
}
}


