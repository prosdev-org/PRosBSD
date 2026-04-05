#include <libkxx/string.hxx>
#include <string.h>
#include <unique/assert.h>

kxx::String::String() {
    back = nullptr;
    size = 0;
    capacity = 0;
}

kxx::String::String(const char *src) {
    size = strlen(src);
    capacity = size;
    back = new char[capacity];
    memcpy(back, src, size);
}

kxx::String::String(const char *src, const size_t count) {
    size = strnlen(src, count);
    capacity = size;
    back = new char[capacity];
    memcpy(back, src, size);
}

kxx::String::~String() {
    delete[] back;
}

kxx::String::String(const String &other) {
    size = other.size;
    capacity = other.capacity;
    back = new char[capacity];
    memcpy(back, other.back, size);
}

kxx::String::String(String &&other) {
    back = other.back;
    size = other.size;
    capacity = other.capacity;

    other.back = nullptr;
    other.size = 0;
    other.capacity = 0;
}

kxx::String &kxx::String::operator=(const String &other) {
    if (&other == this) {
        return *this;
    }

    delete[] back;

    size = other.size;
    capacity = other.capacity;
    back = new char[capacity];
    memcpy(back, other.back, size);

    return *this;
}

kxx::String &kxx::String::operator=(String &&other) {
    if (&other == this) {
        return *this;
    }

    delete[] back;

    back = other.back;
    size = other.size;
    capacity = other.capacity;

    other.back = nullptr;
    other.size = 0;
    other.capacity = 0;

    return *this;
}

size_t kxx::String::get_size() const {
    return size;
}

char kxx::String::get(const size_t idx) const {
    ASSERT(back != nullptr);
    ASSERT(idx < size);

    return back[idx];
}

// NOLINTNEXTLINE(readability-make-member-function-const)
void kxx::String::set(const char ch, const size_t idx) {
    ASSERT(back != nullptr);
    ASSERT(idx < size);

    back[idx] = ch;
}

void kxx::String::push_back(const char ch) {
    if (back == nullptr) {
        size = 0;
        capacity = 16;
        back = new char[capacity];
    }

    if (size + 1 > capacity) {
        capacity = 2 * capacity - capacity / 2; // capacity *= 1.5;
        const auto new_back = new char[capacity];
        memcpy(new_back, back, size);
        delete[] back;
        back = new_back;
    }

    back[size] = ch;
    size++;
}
