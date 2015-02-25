#include "Name.h"

#include <cstdio>

namespace norlit {
namespace aml {

Name::Name(int prefixCount, int segCount, uint32_t *names)
    : prefixCount(prefixCount), segCount(segCount), names(names) {

}

Name::~Name() {
    delete[] names;
}

bool Name::IsResolved() const {
    return false;
}

void Name::Dump(int ident) const {
    if (prefixCount == -1) {
        printf("\\");
    } else if(prefixCount) {
        for (int i = 0; i < prefixCount; i++) {
            printf("^");
        }
    }
    char temp[4];
    for (int i = 0; i < segCount; i++) {
        if (i != 0) {
            printf(".");
        }
        UnpackNameSegment(names[i], temp);
        printf("%.4s", temp);
    }
}

}
}