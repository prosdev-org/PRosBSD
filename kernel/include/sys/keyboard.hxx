#ifndef SYS_KEYBOARD_HXX
#define SYS_KEYBOARD_HXX

#include <libkxx/unique_ptr.hxx>
#include <sys/input_stream.hxx>

namespace Sys {
    class Keyboard {
    public:
        virtual ~Keyboard() = default;
        virtual char get_next() = 0;
        virtual bool has_next() = 0;
        kxx::UniquePtr<InputStream> as_input_stream();
    };
} // namespace Sys

#endif
