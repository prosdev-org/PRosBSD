#ifndef LIBKXX_BITMAP_HXX
#define LIBKXX_BITMAP_HXX

#include <libkxx/byte.hxx>
#include <libkxx/unique_ptr.hxx>
#include <stddef.h>

namespace kxx {
    class Bitmap {
    public:
        explicit Bitmap(size_t size);
        ~Bitmap() = default;
        Bitmap(const Bitmap &other);
        Bitmap(Bitmap &&other);
        Bitmap &operator=(const Bitmap &other);
        Bitmap &operator=(Bitmap &&other);
        [[nodiscard]] size_t get_size() const;
        [[nodiscard]] bool get(size_t idx) const;
        void set(bool bit, size_t idx);
        void clear_all();
        void set_all();

    private:
        static void calc_idxes(
                size_t idx,
                size_t &bytes_idx,
                uint8_t &in_byte_bit);

        const size_t size;
        UniquePtr<Byte[]> bytes;

        [[nodiscard]] size_t get_nbytes() const;
    };
} // namespace kxx

#endif
