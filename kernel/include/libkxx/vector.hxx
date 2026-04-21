#ifndef LIBKXX_VECTOR_HXX
#define LIBKXX_VECTOR_HXX

#include <libkxx/move.hxx>
#include <libkxx/new.hxx>
#include <stddef.h>
#include <stdint.h>
#include <unique/assert.h>

namespace kxx {
    template<typename T>
    class Vector {
        friend class StringView;

    public:
        Vector() = default;
        template<size_t src_size>
        // NOLINTNEXTLINE(google-explicit-constructor)
        Vector(T (&src)[src_size]);
        template<size_t src_size>
        // NOLINTNEXTLINE(google-explicit-constructor)
        Vector(T (&&src)[src_size]);
        explicit Vector(const T *src, size_t src_size);
        explicit Vector(size_t initial_capacity);
        ~Vector();
        Vector(const Vector &other);
        Vector(Vector &&other);
        Vector &operator=(const Vector &other);
        Vector &operator=(Vector &&other);
        [[nodiscard]] size_t get_size() const;
        [[nodiscard]] const T &get(size_t idx) const;
        void set(const T &obj, size_t idx);
        void set(T &&obj, size_t idx);
        void push_back(const T &obj);
        void push_back(T &&obj);
        void pop_back();

    private:
        static T *alloc_back(size_t new_capacity);

        T *back = nullptr;
        size_t size = 0;
        size_t capacity = 0;

        void resize();
        void copy_init_to_back(const T *src, size_t count);
        void move_init_to_back(T *src, size_t count);
        void delete_back();
        void delete_back_with_content();
    };

    template<typename T>
    template<size_t src_size>
    Vector<T>::Vector(T (&src)[src_size]) {
        size = src_size;
        capacity = size;
        back = alloc_back(capacity);
        copy_init_to_back(src, size);
    }

    template<typename T>
    template<size_t src_size>
    Vector<T>::Vector(T (&&src)[src_size]) {
        size = src_size;
        capacity = size;
        back = alloc_back(capacity);
        move_init_to_back(src, size);
    }

    template<typename T>
    Vector<T>::Vector(const T *src, const size_t src_size) {
        size = src_size;
        capacity = size;
        back = alloc_back(capacity);
        copy_init_to_back(src, size);
    }

    template<typename T>
    Vector<T>::Vector(const size_t initial_capacity) {
        capacity = initial_capacity;
        back = alloc_back(capacity);
    }

    template<typename T>
    Vector<T>::~Vector() {
        delete_back_with_content();
    }

    template<typename T>
    Vector<T>::Vector(const Vector &other) {
        size = other.size;
        capacity = other.capacity;
        back = alloc_back(capacity);
        copy_init_to_back(other.back, size);
    }

    template<typename T>
    Vector<T>::Vector(Vector &&other) {
        back = other.back;
        size = other.size;
        capacity = other.capacity;

        other.back = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    template<typename T>
    Vector<T> &Vector<T>::operator=(const Vector &other) {
        if (&other == this) {
            return *this;
        }

        delete_back_with_content();

        size = other.size;
        capacity = other.capacity;
        back = alloc_back(capacity);
        copy_init_to_back(other.back, size);

        return *this;
    }

    template<typename T>
    Vector<T> &Vector<T>::operator=(Vector &&other) {
        if (&other == this) {
            return *this;
        }

        delete_back_with_content();

        back = other.back;
        size = other.size;
        capacity = other.capacity;

        other.back = nullptr;
        other.size = 0;
        other.capacity = 0;

        return *this;
    }

    template<typename T>
    size_t Vector<T>::get_size() const {
        return size;
    }

    template<typename T>
    const T &Vector<T>::get(const size_t idx) const {
        ASSERT(back != nullptr);
        ASSERT(idx < size);

        return back[idx];
    }

    template<typename T>
    void Vector<T>::set(const T &obj, const size_t idx) {
        ASSERT(back != nullptr);
        ASSERT(idx < size);

        back[idx] = obj;
    }

    template<typename T>
    void Vector<T>::set(T &&obj, const size_t idx) {
        ASSERT(back != nullptr);
        ASSERT(idx < size);

        back[idx] = move(obj);
    }

    template<typename T>
    void Vector<T>::push_back(const T &obj) {
        resize();

        new (&back[size]) T(obj);
        size++;
    }

    template<typename T>
    void Vector<T>::push_back(T &&obj) {
        resize();

        new (&back[size]) T(move(obj));
        size++;
    }

    template<typename T>
    void Vector<T>::pop_back() {
        ASSERT(size > 0);

        size--;
    }

    template<typename T>
    T *Vector<T>::alloc_back(const size_t new_capacity) {
        return reinterpret_cast<T *>(new uint8_t[sizeof(T) * new_capacity]);
    }

    template<typename T>
    void Vector<T>::resize() {
        if (back == nullptr) {
            size = 0;
            capacity = 16;
            back = alloc_back(capacity);
        } else if (size + 1 > capacity) {
            capacity = 2 * capacity - capacity / 2;
            auto old_back = back;
            back = alloc_back(capacity);
            move_init_to_back(old_back, size);
            delete_back();
        }
    }

    template<typename T>
    void Vector<T>::copy_init_to_back(const T *src, const size_t count) {
        ASSERT(src != nullptr);
        ASSERT(count <= size);

        for (size_t i = 0; i < count; i++) {
            new (&back[i]) T(src[i]);
        }
    }

    template<typename T>
    void Vector<T>::move_init_to_back(T *src, const size_t count) {
        ASSERT(src != nullptr);
        ASSERT(count <= size);

        for (size_t i = 0; i < count; i++) {
            new (&back[i]) T(move(src[i]));
        }
    }

    template<typename T>
    // NOLINTNEXTLINE(readability-make-member-function-const)
    void Vector<T>::delete_back() {
        delete[] reinterpret_cast<uint8_t *>(back);
    }

    template<typename T>
    void Vector<T>::delete_back_with_content() {
        for (size_t i = 0; i < size; i++) {
            back[i].~T();
        }
        delete_back();
    }
} // namespace kxx

#endif
