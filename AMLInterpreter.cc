#include "ByteStream.h"
#include "Parser.h"
#include "Name.h"
#include "Scope.h"
#include "Context.h"
#include "String_.h"
#include "Integer.h"

#include <fstream>
#include <cstddef>
#include <cstdlib>

using namespace norlit::aml;

static ByteStream readFile(const char* name) {
    std::ifstream file(name, std::ios::binary);
    file.seekg(0, std::ios::end);
    size_t size = (size_t)file.tellg();
    file.seekg(36, std::ios::beg);

    char* buffer = new char[size - 36];
    if (file.read(buffer, size - 36)) {
        return ByteStream((uint8_t*)buffer, size - 36);
    } else {
        perror("Unexpected read file error");
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    ByteStream bytecode = readFile("dsdt.aml");

    Handle<Scope> root = new Scope();
    root->Item("_GPE", new Scope());
    root->Item("_PR_", new Scope());
    root->Item("_SB_", new Scope());
    root->Item("_SI_", new Scope());
    root->Item("_TZ_", new Scope());

    /* 5.7.3 \_OS (OS Name Object) */
    root->Item("_OS_", new String("NorlitOS"));

    /* 5.7.4 \_REV (Revision Data Object) */
    root->Item("_REV", new Integer(0));

    // ByteStream bytecode = ByteStream((uint8_t*)"^^\x2F\x3""AAAABBBB_CC_", 4);
    Interpreter parser(bytecode, new Context(root, new Name(-1, 0, nullptr)));
    parser.ParseTermList();

    root->Resolve();

    root->Dump(0);

    printf("\n");


    return 0;
}


