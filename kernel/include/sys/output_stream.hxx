#ifndef SYS_OUTPUT_STREAM_HXX
#define SYS_OUTPUT_STREAM_HXX

#include <libkxx/ref.hxx>
#include <stddef.h>

namespace Sys {
    class OutputStream {
    public:
        virtual ~OutputStream() = default;
        virtual void write(const void *data, size_t nbytes) = 0;
        virtual void flush() = 0;

        template<typename T>
        void write_object(kxx::Ref<const T> object) {
            write(&(*object), sizeof(T));
        }

        template<typename T>
        // void write_array(const T *array, const size_t nobjs) {
        void write_array(kxx::Ref<const T *> array, const size_t nobjs) {
            write(*array, sizeof(T) * nobjs);
        }
    };
} // namespace Sys

#endif
