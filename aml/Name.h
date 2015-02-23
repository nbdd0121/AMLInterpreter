#ifndef NORLIT_ACPI_AML_IDENTIFIER_H
#define NORLIT_ACPI_AML_IDENTIFIER_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Handle.h"

namespace norlit {
namespace acpi {
namespace aml {

class Name : public Value {
  protected:
    Name();
  public:
    virtual ~Name();
};

class NameSegment : public Name {
  private:
    char name[4];
  public:
    NameSegment(char a, char b, char c, char d);
    virtual ~NameSegment();
    virtual void Dump() const;
};

class NamePath : public Name {
  private:
    Handle<Name> parent;
    Handle<Name> name;
  public:
    NamePath(Name* parent, Name* name);
    virtual ~NamePath();
    virtual void Dump() const;
};

class RootPath : public Name {
  private:
    Handle<Name> name;
  public:
    RootPath(Name* name);
    virtual ~RootPath();
    virtual void Dump() const;
};

class PrefixPath : public Name {
  private:
    Handle<Name> name;
  public:
    PrefixPath(Name* name);
    virtual ~PrefixPath();
    virtual void Dump() const;
};

}
}
}

#endif
