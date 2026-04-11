#ifndef LIBKXX_UNIQUE_PTR_HXX
#define LIBKXX_UNIQUE_PTR_HXX

#include <stddef.h>

namespace kxx {
    template<typename T>
    class UniquePtr {
    public:
        UniquePtr() = default;
        explicit UniquePtr(T *ptr);
        ~UniquePtr();
        UniquePtr(const UniquePtr &other) = delete;
        UniquePtr(UniquePtr &&other);
        UniquePtr &operator=(const UniquePtr &other) = delete;
        UniquePtr &operator=(UniquePtr &&other);
        T &operator*() const;
        T *operator->() const;
        T &operator[](size_t idx) const = delete;

    private:
        T *back = nullptr;
    };

    template<typename T>
    UniquePtr<T>::UniquePtr(T *ptr) {
        back = ptr;
    }

    template<typename T>
    UniquePtr<T>::~UniquePtr() {
        delete back;
    }

    template<typename T>
    UniquePtr<T>::UniquePtr(UniquePtr &&other) {
        back = other.back;
        other.back = nullptr;
    }

    template<typename T>
    UniquePtr<T> &UniquePtr<T>::operator=(UniquePtr &&other) {
        if (&other == this) {
            return *this;
        }

        back = other.back;
        other.back = nullptr;

        return *this;
    }

    template<typename T>
    T &UniquePtr<T>::operator*() const {
        return *back;
    }

    template<typename T>
    T *UniquePtr<T>::operator->() const {
        return back;
    }

    template<typename T>
    class UniquePtr<T[]> {
    public:
        UniquePtr() = default;
        explicit UniquePtr(T *ptr);
        ~UniquePtr();
        UniquePtr(const UniquePtr &other) = delete;
        UniquePtr(UniquePtr &&other);
        UniquePtr &operator=(const UniquePtr &other) = delete;
        UniquePtr &operator=(UniquePtr &&other);
        T &operator*() const = delete;
        T *operator->() const = delete;
        T &operator[](size_t idx) const;

    private:
        T *back = nullptr;
    };

    template<typename T>
    UniquePtr<T[]>::UniquePtr(T *ptr) {
        back = ptr;
    }

    template<typename T>
    UniquePtr<T[]>::~UniquePtr() {
        delete[] back;
    }

    template<typename T>
    UniquePtr<T[]>::UniquePtr(UniquePtr &&other) {
        back = other.back;
        other.back = nullptr;
    }

    template<typename T>
    UniquePtr<T[]> &UniquePtr<T[]>::operator=(UniquePtr &&other) {
        if (&other == this) {
            return *this;
        }

        back = other.back;
        other.back = nullptr;

        return *this;
    }

    template<typename T>
    T &UniquePtr<T[]>::operator[](const size_t idx) const {
        return back[idx];
    }
} // namespace kxx

#endif
