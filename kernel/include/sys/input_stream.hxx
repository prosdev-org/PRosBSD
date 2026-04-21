#ifndef SYS_INPUT_STREAM_HXX
#define SYS_INPUT_STREAM_HXX

#include <stddef.h>

namespace Sys {
    class InputStream {
    public:
        virtual ~InputStream() = default;
        virtual void read(void *dest, size_t nbytes) = 0;

        template<typename T>
        void read_object(T *object_dest) {
            read(object_dest, sizeof(T));
        }

        template<typename T>
        void read_array(T *array_dest, const size_t nobjs) {
            read(array_dest, sizeof(T) * nobjs);
        }
    };
} // namespace Sys

#endif
