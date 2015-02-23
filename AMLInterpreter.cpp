#include "util/FileReader.h"

#include "aml/Interpreter.h"
#include "aml/Integer.h"
#include "aml/Handle.h"

using namespace norlit::acpi::util;
using namespace norlit::acpi::aml;

int main(int argc, char* argv[]) {
    Bytecode bc = FileReader::readFile("test.dat");
    Interpreter interp(bc);
    interp.parseTermList();

    return 0;
}

