#include "Interpreter.h"
#include "Package.h"
#include "Integer.h"

#include <cassert>
#include <iostream>

namespace norlit {
namespace acpi {
namespace aml {

Interpreter::Interpreter(Bytecode& bc) :bytecode(bc) {

}


Interpreter::~Interpreter() {}

bool Interpreter::unexpected() {
    assert(0);
    return false;
}

void Interpreter::ParseNameSeg() {
    uint8_t data = bytecode.NextBytedata();
    assert(data >= 'A'&&data <= 'Z' || data == '_');
    std::cout << (char)data;
    data = bytecode.NextBytedata();
    assert(data >= 'A'&&data <= 'Z' || data >= '0'&&data <= '9' || data == '_');
    std::cout << (char)data;
    data = bytecode.NextBytedata();
    assert(data >= 'A'&&data <= 'Z' || data >= '0'&&data <= '9' || data == '_');
    std::cout << (char)data;
    data = bytecode.NextBytedata();
    assert(data >= 'A'&&data <= 'Z' || data >= '0'&&data <= '9' || data == '_');
    std::cout << (char)data;
}

bool Interpreter::tryParseNamePath() {
    uint8_t peek = bytecode.PeekBytedata();
    switch(peek) {
    case DualNamePrefix:
        bytecode.Consume();
        ParseNameSeg();
        std::cout << ".";
        ParseNameSeg();
        return true;
    case MultiNamePrefix: {
        bytecode.Consume();
        uint8_t SegCount = bytecode.NextBytedata();
        for (int i = 0; i < SegCount; i++) {
            if (i != 0) std::cout << ".";
            ParseNameSeg();
        }
        return true;
    }
    case NullName:
        bytecode.Consume();
        return true;
    }
    if (peek >= 'A'&&peek <= 'Z' || peek=='_') {
        ParseNameSeg();
        return true;
    }
    return false;
}

bool Interpreter::tryParseNameString() {
    if (bytecode.ConsumeIf('\\')) {
        std::cout << "\\";
        tryParseNamePath() || unexpected();
        return true;
    } else if(bytecode.ConsumeIf('^')) {
        std::cout << "^";
        while (bytecode.ConsumeIf('^')) {
            std::cout << "^";
        }
        tryParseNamePath() || unexpected();
        return true;
    } else {
        return tryParseNamePath();
    }
}

/* 20.2.3 */
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

uint32_t Interpreter::parsePkgLength() {
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
    default:
        length = 0;
        assert(0);
    }
    return length;
}


bool Interpreter::tryParseTermObj() {
    return tryParseNameSpaceModifierObj() || TryParseNamedObj();
}

void Interpreter::parseTermList() {
    while (!bytecode.IsEOF() && tryParseTermObj());
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
    std::cout << "Name(";
    tryParseNameString() || unexpected();
    std::cout << ",";
    TryParseDataRefObject() || unexpected();
    returnValue->Dump();
    std::cout << ")" << std::endl;
}

void Interpreter::parseDefScope() {
    bytecode.NextBytedata();
    size_t ptr = bytecode.GetPointer();
    ptr += parsePkgLength();
    std::cout << "Scope(";
    tryParseNameString() || unexpected();
    Bytecode scopeCode = bytecode.Slice(bytecode.GetPointer(), ptr);
    Interpreter interp(scopeCode);
    // interp.parseTermList();

    bytecode.SetPointer(ptr);
    std::cout << ")" << std::endl;
}

/* 20.2.5.2 */
bool Interpreter::TryParseNamedObj() {
    if (bytecode.ConsumeIf(ExtOpPrefix)) {

    }
    printf("Unknown Opcode %x", bytecode.PeekBytedata());
    exit(0);
    return false;
}

/* 20.2.5.4 */
bool Interpreter::TryParseDefPackage() {
    /* PackageOp */
    if (!bytecode.ConsumeIf(PackageOp)) return false;
    uint32_t pointer = bytecode.GetPointer();
    /* PkgLength */
    pointer += parsePkgLength();
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

