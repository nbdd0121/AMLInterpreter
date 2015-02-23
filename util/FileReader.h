#include "../aml/Bytecode.h"

namespace norlit {
namespace acpi {
namespace util {

class FileReader {
  public:
    static aml::Bytecode readFile(const char* file);
};

}
}
}