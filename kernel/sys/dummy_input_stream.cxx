#include <string.h>
#include <sys/dummy_input_stream.hxx>
#include <unique/assert.h>

namespace Sys {
    void DummyInputStream::read(void *dest, const size_t nbytes) {
        ASSERT(dest != nullptr);

        memset(dest, 0, nbytes);
    }
} // namespace Sys
