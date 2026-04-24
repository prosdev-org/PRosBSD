#include <libkxx/string.hxx>
#include <libkxx/string_view.hxx>
#include <string.h>
#include <unique/assert.h>

namespace kxx {
    StringView::StringView(const char *src) {
        back = src;
        size = strlen(src);
    }

    StringView::StringView(const String &src) {
        back = src.vector.back;
        size = src.vector.size;
    }

    size_t StringView::get_size() const {
        return size;
    }

    char StringView::get(const size_t idx) const {
        ASSERT(back != nullptr);
        ASSERT(idx < size);

        return back[idx];
    }

    bool StringView::operator==(const StringView &other) const {
        if (size != other.size) {
            return false;
        }

        return memcmp(back, other.back, size) == 0;
    }

    bool StringView::operator!=(const StringView &other) const {
        if (size != other.size) {
            return true;
        }

        return memcmp(back, other.back, size) != 0;
    }
} // namespace kxx
