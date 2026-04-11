#ifndef SYS_DUMMY_OUTPUT_STREAM_HXX
#define SYS_DUMMY_OUTPUT_STREAM_HXX

#include <sys/output_stream.hxx>

namespace Sys {
    class DummyOutputStream final : public OutputStream {
    public:
        void write(const void *data, size_t nbytes) override;
        void flush() override;
    };
} // namespace Sys

#endif
