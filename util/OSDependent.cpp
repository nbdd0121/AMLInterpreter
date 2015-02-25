#include <cstdlib>

extern "C" void aml_os_panic(const char* msg) {
    perror(msg);
    exit(1);
}