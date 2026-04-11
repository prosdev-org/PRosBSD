#include <sys/dummy_output_stream.hxx>

namespace Sys {
    void DummyOutputStream::write(const void *, size_t) {
    }

    void DummyOutputStream::flush() {
    }
} // namespace Sys
