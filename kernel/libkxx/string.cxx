#include <libkxx/string.hxx>
#include <string.h>

namespace kxx {
    String::String(const char *src) {
        vector = Vector(src, strlen(src));
    }

    String::String(const char *src, const size_t count) {
        vector = Vector(src, count);
    }

    String::String(const String &other) {
        vector = other.vector;
    }

    String::String(String &&other) {
        vector = move(other.vector);
    }

    String &String::operator=(const String &other) {
        if (&other == this) {
            return *this;
        }

        vector = other.vector;

        return *this;
    }

    String &String::operator=(String &&other) {
        if (&other == this) {
            return *this;
        }

        vector = move(other.vector);

        return *this;
    }

    size_t String::get_size() const {
        return vector.get_size();
    }

    char String::get(const size_t idx) const {
        return vector.get(idx);
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void String::set(const char ch, const size_t idx) {
        vector.set(ch, idx);
    }

    void String::push_back(const char ch) {
        vector.push_back(ch);
    }
} // namespace kxx
