#include "Name.h"

#include <iostream>

namespace norlit {
namespace acpi {
namespace aml {

Name::Name() {

}

Name::~Name() {
    // std::cout << "~Name()\n";
}


/* NameSeg */
NameSegment::NameSegment(char a, char b, char c, char d) {
    name[0] = a;
    name[1] = b;
    name[2] = c;
    name[3] = d;
}

NameSegment::~NameSegment() {
    // std::cout << "~NameSegment()\n";
}

void NameSegment::Dump() const {
    std::cout << name[0] << name[1] << name[2] << name[3];
}

/* NamePath */
NamePath::NamePath(Name*parent, Name*name) :parent(parent), name(name) {}

NamePath::~NamePath() {
    // std::cout << "~NamePath()";
}

void NamePath::Dump() const {
    parent->Dump();
    std::cout << ".";
    name->Dump();
}

/* RootPath */
RootPath::RootPath(Name*name) :name(name) {}

RootPath::~RootPath() {}

void RootPath::Dump() const {
    std::cout << "\\";
    if (name)
        name->Dump();
}

/* PrefixPath */
PrefixPath::PrefixPath(Name*name) :name(name) {}

PrefixPath::~PrefixPath() {}

void PrefixPath::Dump() const {
    std::cout << "^";
    if (name)
        name->Dump();
}

}
}
}
