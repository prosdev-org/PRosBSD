#ifndef LIBKXX_STRING_VIEW_HXX
#define LIBKXX_STRING_VIEW_HXX

#include <libkxx/string.hxx>

namespace kxx {
    class StringView {
    public:
        // NOLINTNEXTLINE(google-explicit-constructor)
        StringView(const char *src);
        // NOLINTNEXTLINE(google-explicit-constructor)
        StringView(const String &src);
        [[nodiscard]] size_t get_size() const;
        [[nodiscard]] char get(size_t idx) const;

    private:
        const char *back = nullptr;
        size_t size = 0;
    };
} // namespace kxx

#endif
