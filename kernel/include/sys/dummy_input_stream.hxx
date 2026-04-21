#ifndef SYS_DUMMY_INPUT_STREAM_HXX
#define SYS_DUMMY_INPUT_STREAM_HXX

#include <sys/input_stream.hxx>

namespace Sys {
    class DummyInputStream final : public InputStream {
    public:
        void read(void *dest, size_t nbytes) override;
    };
} // namespace Sys

#endif
