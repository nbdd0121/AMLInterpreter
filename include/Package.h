#ifndef NORLIT_AML_PACKAGE_H
#define NORLIT_AML_PACKAGE_H

#include <cstdint>
#include <cstddef>

#include "Value.h"
#include "Handle.h"

namespace norlit {
namespace aml {

class Package: public Value {
  private:
	size_t length;
	Handle<Value> *entry;
  public:
	Package(size_t length, Handle<Value> *entry);
	virtual ~Package();
	virtual void Dump(int ident) const override;
};

class Buffer: public Value {
  private:
	size_t length;
	uint8_t *entry;
  public:
	Buffer(size_t length, uint8_t *entry);
	virtual ~Buffer();
	virtual void Dump(int ident) const override;
};

}
}

#endif
