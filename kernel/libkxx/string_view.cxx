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
        back = src.back;
        size = src.size;
    }

    size_t StringView::get_size() const {
        return size;
    }

    char StringView::get(const size_t idx) const {
        ASSERT(back != nullptr);
        ASSERT(idx < size);

        return back[idx];
    }
} // namespace kxx
