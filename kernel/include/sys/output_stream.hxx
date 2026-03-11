#ifndef SYS_OUTPUT_STREAM_HXX
#define SYS_OUTPUT_STREAM_HXX

#include <stddef.h>

namespace Sys {
    class OutputStream {
    public:
        virtual ~OutputStream() = default;
        virtual void write(const void *data, size_t nbytes) = 0;
        virtual void flush() = 0;

        template<typename T>
        void write_object(const T &object) {
            write(&object, sizeof(T));
        }

        template<typename T>
        void write_array(const T *array, size_t nobjs) {
            write(array, sizeof(T) * nobjs);
        }
    };
} // namespace Sys

#endif
