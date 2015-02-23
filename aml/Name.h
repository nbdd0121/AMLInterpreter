#ifndef NORLIT_ACPI_AML_NAME_H
#define NORLIT_ACPI_AML_NAME_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Handle.h"

namespace norlit {
namespace acpi {
namespace aml {

class Interpreter;
class Scope;

class Name : public Value {
  protected:
    Name();
  public:
    virtual ~Name();
    virtual Handle<Name> Parent() const = 0;
    virtual Handle<Value> Get(Scope* scope) const = 0;
    virtual void Put(Scope* scope, Value* val) const = 0;
};

class NameSegment : public Name {
  private:
    char name[4];
  public:
    NameSegment(char a, char b, char c, char d);
    NameSegment(const char*);
    virtual ~NameSegment();
    virtual void Dump(int ident) const;
    virtual Handle<Name> Parent() const override;
    virtual Handle<Value> Get(Scope* scope) const override;
    virtual void Put(Scope* scope, Value* val) const override;

    inline const char* GetName() const {
        return name;
    }
};

class NamePath : public Name {
  private:
    Handle<Name> parent;
    Handle<NameSegment> name;
  public:
    NamePath(Name* parent, NameSegment* name);
    virtual ~NamePath();
    virtual void Dump(int ident) const;
    virtual Handle<Name> Parent() const override;
    virtual Handle<Value> Get(Scope* scope) const override;
    virtual void Put(Scope* scope, Value* val) const override;
};

class RootPath : public Name {
  public:
    RootPath();
    virtual ~RootPath();
    virtual void Dump(int ident) const;
    virtual Handle<Name> Parent() const override;
    virtual Handle<Value> Get(Scope* scope) const override;
    virtual void Put(Scope* scope, Value* val) const override;
};


}
}
}

#endif
