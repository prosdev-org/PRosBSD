#include <libkxx/bitmap.hxx>
#include <libkxx/move.hxx>
#include <string.h>
#include <unique/assert.h>

namespace kxx {
    Bitmap::Bitmap(const size_t size) : size(size) {
        bytes = kxx::UniquePtr<Byte[]>(new Byte[get_nbytes()]);
        set_all(false);
    }

    Bitmap::Bitmap(const Bitmap &other) : size(other.size) {
        bytes = kxx::UniquePtr<Byte[]>(new Byte[get_nbytes()]);
        memmove(&bytes[0], &other.bytes[0], size);
    }

    Bitmap::Bitmap(Bitmap &&other) : size(other.size) {
        bytes = move(other.bytes);
    }

    Bitmap &Bitmap::operator=(const Bitmap &other) {
        if (&other == this) {
            return *this;
        }

        bytes = kxx::UniquePtr<Byte[]>(new Byte[get_nbytes()]);
        memmove(&bytes[0], &other.bytes[0], size);

        return *this;
    }

    Bitmap &Bitmap::operator=(Bitmap &&other) {
        if (&other == this) {
            return *this;
        }

        bytes = move(other.bytes);

        return *this;
    }

    size_t Bitmap::get_size() const {
        return size;
    }

    bool Bitmap::get(const size_t idx) const {
        ASSERT(idx < size);

        size_t bytes_idx;
        uint8_t in_byte_bit;
        calc_idxes(idx, bytes_idx, in_byte_bit);

        return bytes[bytes_idx].get_bit(in_byte_bit);
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void Bitmap::set(const bool bit, const size_t idx) {
        ASSERT(idx < size);

        size_t bytes_idx;
        uint8_t in_byte_bit;
        calc_idxes(idx, bytes_idx, in_byte_bit);

        bytes[bytes_idx].set_bit(bit, in_byte_bit);
    }

    void Bitmap::set_range(const bool bit, const size_t start, const size_t end) {
        size_t aligned_start = start;
        size_t aligned_end = end;

        if (aligned_start % 8 != 0) {
            aligned_start = (aligned_start / 8 + 1) * 8;
        }

        if (aligned_end % 8 != 7) {
            aligned_end = (aligned_end / 8 - 1) * 8 + 7;
        }

        for (size_t i = start; i < aligned_start; i++) {
            set(bit, i);
        }

        for (size_t i = aligned_end + 1; i <= end; i++) {
            set(bit, i);
        }

        if (bit) {
            memset(
                    &bytes[aligned_start / 8],
                    0xff,
                    (aligned_end + 1 - aligned_start) / 8);
        } else {
            memset(
                    &bytes[aligned_start / 8],
                    0,
                    (aligned_end + 1 - aligned_start) / 8);
        }
    }

    void Bitmap::set_all(const bool bit) {
        set_range(bit, 0, size - 1);
    }

    void Bitmap::calc_idxes(
            const size_t idx,
            size_t &bytes_idx,
            uint8_t &in_byte_bit) {
        bytes_idx = idx / 8;
        in_byte_bit = idx % 8;
    }

    size_t Bitmap::get_nbytes() const {
        size_t nbytes = size / 8;
        if (size % 8 != 0) {
            nbytes++;
        }

        return nbytes;
    }
} // namespace kxx
