#include "FileReader.h"

#include <fstream>

using namespace norlit::acpi::aml;

namespace norlit {
namespace acpi {
namespace util {

Bytecode FileReader::readFile(const char* file) {
    std::ifstream fileStream(file, std::ios::binary);
    fileStream.seekg(0, std::ios::end);
    unsigned size = (unsigned)fileStream.tellg() - 36;
    fileStream.seekg(36, std::ios::beg);

    char* buffer = new char[size];
    fileStream.read(buffer, size);
    return Bytecode((uint8_t*)buffer, size);
}

}
}
}