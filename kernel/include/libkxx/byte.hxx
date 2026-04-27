#ifndef LIBKXX_BYTE_HXX
#define LIBKXX_BYTE_HXX

#include <stdint.h>
#include <unique/assert.h>

namespace kxx {
    union Byte {
    private:
        uint8_t value = 0;

    public:
        struct {
            bool b0 : 1;
            bool b1 : 1;
            bool b2 : 1;
            bool b3 : 1;
            bool b4 : 1;
            bool b5 : 1;
            bool b6 : 1;
            bool b7 : 1;
        } bits;

        Byte() = default;
        // NOLINTNEXTLINE(google-explicit-constructor)
        Byte(uint8_t new_value);
        Byte &operator=(uint8_t new_value);
        // NOLINTNEXTLINE(google-explicit-constructor)
        operator uint8_t() const;
        [[nodiscard]] bool get_bit(uint8_t idx) const;
        void set_bit(bool bit, uint8_t idx);
    };

    inline Byte::Byte(const uint8_t new_value) {
        this->value = new_value;
    }

    inline Byte &Byte::operator=(const uint8_t new_value) {
        this->value = new_value;
        return *this;
    }

    inline Byte::operator uint8_t() const {
        return value;
    }

    inline bool Byte::get_bit(const uint8_t idx) const {
        switch (idx) {
            case 0: {
                return bits.b0;
            }
            case 1: {
                return bits.b1;
            }
            case 2: {
                return bits.b2;
            }
            case 3: {
                return bits.b3;
            }
            case 4: {
                return bits.b4;
            }
            case 5: {
                return bits.b5;
            }
            case 6: {
                return bits.b6;
            }
            case 7: {
                return bits.b7;
            }
            default: {
                ASSERT(false);
            }
        }
    }

    inline void Byte::set_bit(const bool bit, const uint8_t idx) {
        switch (idx) {
            case 0: {
                bits.b0 = bit;
            } break;
            case 1: {
                bits.b1 = bit;
            } break;
            case 2: {
                bits.b2 = bit;
            } break;
            case 3: {
                bits.b3 = bit;
            } break;
            case 4: {
                bits.b4 = bit;
            } break;
            case 5: {
                bits.b5 = bit;
            } break;
            case 6: {
                bits.b6 = bit;
            } break;
            case 7: {
                bits.b7 = bit;
            } break;
            default: {
                ASSERT(false);
            }
        }
    }
} // namespace kxx

#endif
