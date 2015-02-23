#include "util/FileReader.h"

#include "aml/Interpreter.h"
#include "aml/Integer.h"
#include "aml/Scope.h"
#include "aml/Handle.h"

#include <cstdio>

using namespace norlit::acpi::util;
using namespace norlit::acpi::aml;

int main(int argc, char* argv[]) {
    Bytecode bc = FileReader::readFile("dsdt.dat");

    Handle<Scope> root = new Scope();
    root->Item("_GPE", new Scope());
    root->Item("_PR_", new Scope());
    root->Item("_SB_", new Scope());
    root->Item("_SI_", new Scope());
    root->Item("_TZ_", new Scope());


    Interpreter interp(bc, root, new RootPath());
    interp.parseTermList();
    if (!bc.IsEOF())interp.unexpected();

    printf("\\ ");
    root->Dump(0);
    printf("\n");

    return 0;
}

