#include <cstdlib>
#include <cstdio>

extern "C" void aml_os_panic(const char* msg) {
    printf("%s\n", msg);
    exit(1);
}