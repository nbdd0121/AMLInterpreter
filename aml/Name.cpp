#include "Name.h"

#include <iostream>
#include <cstring>
#include <cassert>

#include "Interpreter.h"

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

NameSegment::NameSegment(const char* d) {
    memcpy(name, d, 4);
}

NameSegment::~NameSegment() {
    // std::cout << "~NameSegment()\n";
}

void NameSegment::Dump(int ident) const {
    std::cout << name[0] << name[1] << name[2] << name[3];
}

Handle<Name> NameSegment::Parent() const {
    assert(0);
    return nullptr;
}

Handle<Value> NameSegment::Get(Scope* scope) const {
    return scope->Item(name);
}

void NameSegment::Put(Scope* scope, Value* val) const {
    return scope->Item(name, val);
}

/* NamePath */
NamePath::NamePath(Name*parent, NameSegment*name) :parent(parent), name(name) {}

NamePath::~NamePath() {
    // std::cout << "~NamePath()";
}

void NamePath::Dump(int ident) const {
    parent->Dump(ident);
    std::cout << ".";
    name->Dump(ident);
}

Handle<Name> NamePath::Parent() const {
    return parent;
}

Handle<Value> NamePath::Get(Scope* scope) const {
    Handle<Scope> s = (Scope*)(parent->Get(scope));
    return name->Get(scope);
}

void NamePath::Put(Scope* scope, Value* val) const {
    Handle<Scope> s = (Scope*)(parent->Get(scope));
    name->Put(scope, val);
}

/* RootPath */
RootPath::RootPath() {}

RootPath::~RootPath() {}

void RootPath::Dump(int ident) const {
    std::cout << "\\";
}

Handle<Name> RootPath::Parent() const {
    assert(0);
    return nullptr;
}

Handle<Value> RootPath::Get(Scope* scope) const {
    return scope;
}

void RootPath::Put(Scope* scope, Value* val) const {
    assert(0);
}


}
}
}
