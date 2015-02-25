#include "Context.h"
#include "Scope.h"
#include "Name.h"

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>

namespace norlit {
namespace aml {

Context::Context(Scope *r, Name *p) : root(r), path(p) {
}

Context::~Context() {
}

bool Context::IsResolved() const {
    return false;
}

void Context::Dump(int ident) const {
    assert(0);
}

Handle<Value> Context::Get(Name* name, bool lookup) {
    if (lookup && name->GetPrefixCount() == 0 && name->GetSegCount() == 1) {
        Scope** temp = new Scope*[path->GetSegCount()];
        Handle<Scope> scope = root;
        for (int i = 0; i < path->GetSegCount(); i++) {
            Handle<Value> ret = scope->Item(path->GetName(i));
            if (!ret || !ret->IsScope()) {
                perror("Invalid scope in lookup");
                exit(0);
            }
            scope = temp[i] = (Scope*)ret;
        }
        for (int i = path->GetSegCount() - 1; i >= 0; i--) {
            if (temp[i]->Has(name->GetName(0))) {
                return temp[i]->Item(name->GetName(0));
            }
        }
        perror("Unresolved reference");
        exit(0);
        /* Name lookup required */
    }
    Handle<Scope> scope = root;

    /* We can use path supplied by name directly if it's root path */
    if (name->GetPrefixCount() != -1) {
        int implicitCount = path->GetSegCount() - name->GetPrefixCount();
        if (implicitCount < 0) {
            perror("Invalid path in lookup");
            exit(0);
        }
        for (int i = 0; i < implicitCount; i++) {
            Handle<Value> ret = scope->Item(path->GetName(i));
            if (!ret || !ret->IsScope()) {
                perror("Invalid scope in lookup");
                exit(0);
            }
            scope = (Scope*)ret;
        }
    }

    if (name->GetSegCount() <= 0) {
        return (Scope*)scope;
    }

    for (int i = 0; i < name->GetSegCount() - 1; i++) {
        Handle<Value> ret = scope->Item(name->GetName(i));
        if (!ret || !ret->IsScope()) {
            perror("Invalid scope in lookup");
            exit(0);
        }
        scope = (Scope*)ret;
    }
    return scope->Item(name->GetName(name->GetSegCount() - 1));
}

void Context::Put(Name* name, Value* val, bool lookup) {
    if (name->GetSegCount() <= 0) {
        perror("Unexpected null name");
        exit(0);
    }
    if (lookup && name->GetPrefixCount() == 0 && name->GetSegCount() == 1) {
        assert(0);
        // TODO
        /* Name lookup required */
    }

    Handle<Scope> scope = root;
    if (name->GetPrefixCount() != -1) {
        int implicitCount = path->GetSegCount() - name->GetPrefixCount();
        if (implicitCount < 0) {
            perror("Invalid path in lookup");
            exit(0);
        }
        for (int i = 0; i < implicitCount; i++) {
            Handle<Value> ret = scope->Item(path->GetName(i));
            if (!ret || !ret->IsScope()) {
                perror("Invalid scope in lookup");
                exit(0);
            }
            scope = (Scope*)ret;
        }
    }
    for (int i = 0; i < name->GetSegCount() - 1; i++) {
        Handle<Value> ret = scope->Item(name->GetName(i));
        if (!ret || !ret->IsScope()) {
            perror("Invalid scope in lookup");
            exit(0);
        }
        scope = (Scope*)ret;
    }

    scope->Item(name->GetName(name->GetSegCount() - 1), val);
}

Handle<Name> Context::Normalize(Name* name, bool lookup) {
    if (lookup && name->GetPrefixCount() == 0 && name->GetSegCount() == 1) {
        assert(0);
        // TODO
        /* Name lookup required */
    }

    if (name->GetPrefixCount() == -1) {
        return name;
    }
    int implicitCount = path->GetSegCount() - name->GetPrefixCount();
    if (implicitCount < 0) {
        perror("Invalid path in normalization");
        exit(0);
    }
    int totalCount = implicitCount + name->GetSegCount();
    uint32_t* names = new uint32_t[totalCount];
    memcpy(names, path->GetNames(), implicitCount * 4);
    memcpy(names + implicitCount, name->GetNames(), name->GetSegCount() * 4);
    return new Name(-1, totalCount, names);
}

}
}