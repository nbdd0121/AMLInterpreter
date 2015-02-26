#include "Scope.h"
#include "Name.h"

#include <cassert>
#include <cstring>
#include <cstdio>

namespace norlit {
namespace aml {

Scope::Scope() {}

Scope::~Scope() {
    Node *n = node;
    while (n) {
        Node *backup = n;
        n = n->next;
        delete backup;
    }
}

void Scope::Dump(int ident) const {
    printf("%s", GetTypeName());
    Node *n = node;
    char buffer[4];
    while (n) {
        Name::UnpackNameSegment(n->name, buffer);
        printf("\n%*s%.4s ", ident + 2, "", buffer);
        n->value->Dump(ident + 2);
        n = n->next;
    }
}

Handle<Value> Scope::Resolve() {
    Node *n = node;
    while (n) {
        n->value = n->value->Resolve();
        n = n->next;
    }
    return this;
}

const char* Scope::GetTypeName() const {
    return "Scope";
}

bool Scope::IsScope() const {
    return true;
}

bool Scope::Has(uint32_t id) {
    Node *n = node;
    while (n) {
        if (n->name == id) {
            return true;
        }
        n = n->next;
    }
    return false;
}

Handle<Value> Scope::Item(uint32_t id) const {
    Node *n = node;
    while (n) {
        if (n->name == id) {
            return n->value;
        }
        n = n->next;
    }
    return nullptr;
}

Handle<Value> Scope::Item(const char* id) const {
    return Item(Name::PackNameSegment(id));
}

void Scope::Item(uint32_t id, Value *val) {
    Node *n = node;
    while (n) {
        if (n->name == id) {
            n->value = val;
            return;
        }
        n = n->next;
    }
    Node *newNode = new Node();
    newNode->name = id;
    newNode->value = val;
    newNode->next = node;
    node = newNode;
}

void Scope::Item(const char* id, Value *val) {
    Item(Name::PackNameSegment(id), val);
}

const char* Device::GetTypeName() const {
    return "Device";
}

const char* ThermalZone::GetTypeName() const {
    return "ThermalZone";
}

}
}