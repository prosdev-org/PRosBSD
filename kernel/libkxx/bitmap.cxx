#include <libkxx/bitmap.hxx>
#include <libkxx/move.hxx>
#include <string.h>
#include <unique/assert.h>

namespace kxx {
    Bitmap::Bitmap(const size_t size) : size(size) {
        bytes = kxx::UniquePtr<Byte[]>(new Byte[get_nbytes()]);
        clear_all();
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

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void Bitmap::clear_all() {
        for (size_t i = 0; i < size / 8; i++) {
            bytes[i] = 0;
        }
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void Bitmap::set_all() {
        for (size_t i = 0; i < size / 8; i++) {
            bytes[i] = UINT8_MAX;
        }
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
