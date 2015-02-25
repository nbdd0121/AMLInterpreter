#ifndef NORLIT_AML_CONTEXT_H
#define NORLIT_AML_CONTEXT_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Handle.h"
#include "Scope.h"

namespace norlit {
namespace aml {

class Name;
class Scope;

class Context: public Value {
  private:
    Handle<Scope> root;
    Handle<Name> path;
  public:
    Context(Scope *root, Name *path);
    virtual ~Context();
    virtual bool IsResolved() const override;
    virtual void Dump(int ident) const override;

    Handle<Scope> GetRoot() {
        return root;
    }

    Handle<Value> Get(Name* name, bool lookup = false);
    void Put(Name* name, Value* val, bool lookup = false);
    Handle<Name> Normalize(Name* name, bool lookup = false);
};

}
}

#endif
