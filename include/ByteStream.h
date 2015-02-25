#ifndef NORLIT_AML_BYTESTREAM_H
#define NORLIT_AML_BYTESTREAM_H

#include <cstdint>
#include <cstddef>

namespace norlit {
namespace aml {

class ByteStream {
  private:
    uint8_t *content;
    size_t limit;
    size_t pointer = 0;
  public:
    ByteStream(uint8_t*content, size_t limit) :content(content), limit(limit) {}

    inline uint8_t Peek(size_t lhd = 1) {
        return content[pointer + lhd - 1];
    }

    inline uint8_t Next() {
        return content[pointer++];
    }

    inline void Consume(size_t num = 1) {
        pointer += num;
    }

    inline ByteStream Slice(size_t from, size_t to) {
        return ByteStream(content + from, to - from);
    }

    inline uint8_t* GetRawPointer() {
        return content + pointer;
    }

    inline bool ConsumeIf(uint8_t v) {
        if (Peek() == v) {
            Consume();
            return true;
        } else {
            return false;
        }
    }

    inline bool IsEOF() {
        return pointer >= limit;
    }

    inline size_t GetPointer() {
        return pointer;
    }

    inline void SetPointer(size_t ptr) {
        pointer = ptr;
    }
};

}
}

#endif