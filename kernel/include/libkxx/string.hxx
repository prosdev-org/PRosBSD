#ifndef LIBKXX_STRING_HXX
#define LIBKXX_STRING_HXX

#include <libkxx/vector.hxx>

namespace kxx {
    class String {
        friend class StringView;

    public:
        String() = default;
        explicit String(const char *src);
        String(const char *src, size_t count);
        ~String() = default;
        String(const String &other);
        String(String &&other);
        String &operator=(const String &other);
        String &operator=(String &&other);
        [[nodiscard]] size_t get_size() const;
        [[nodiscard]] char get(size_t idx) const;
        void set(char ch, size_t idx);
        void push_back(char ch);

    private:
        Vector<char> vector;
    };
} // namespace kxx

inline kxx::String operator""_s(const char *src, const size_t size) {
    return {src, size};
}

#endif
