#include "Scope.h"

#include <iostream>
#include <cassert>
#include <cstring>

namespace norlit {
namespace acpi {
namespace aml {

Scope::Scope() {}

Scope::~Scope() {
    Node* n = node;
    while (n) {
        Node* backup = n;
        n = n->next;
        delete backup;
    }
}

void Scope::Dump(int ident) const {
    printf("Scope");
    Node* n = node;
    while (n) {
        printf("\n%*s%.4s ", ident + 2, "", n->name);
        n->value->Dump(ident + 2);
        n = n->next;
    }
}

bool Scope::Has(const char* id) {
    Node* n = node;
    while (n) {
        if (memcmp(n->name, id, 4) == 0) {
            return true;
        }
        n = n->next;
    }
    return false;
}

Handle<Value> Scope::Item(const char* id) {
    Node* n = node;
    while (n) {
        if (memcmp(n->name, id, 4) == 0) {
            return n->value;
        }
        n = n->next;
    }
    return nullptr;
}

void Scope::Item(const char* id, Value* val) {
    Node* n = node;
    while (n) {
        if (memcmp(n->name, id, 4) == 0) {
            n->value = val;
            return;
        }
        n = n->next;
    }
    Node* newNode = new Node();
    memcpy(newNode->name, id, 4);
    newNode->value = val;
    newNode->next = node;
    node = newNode;
}

}
}
}