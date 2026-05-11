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
        [[nodiscard]] T *get_raw();
        [[nodiscard]] const T *get_const_raw() const;
        [[nodiscard]] bool is_nullptr() const;

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
        delete back;
        back = other.back;
        other.back = nullptr;
    }

    template<typename T>
    UniquePtr<T> &UniquePtr<T>::operator=(UniquePtr &&other) {
        if (&other == this) {
            return *this;
        }

        delete back;
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
    T *UniquePtr<T>::get_raw() {
        return back;
    }

    template<typename T>
    const T *UniquePtr<T>::get_const_raw() const {
        return back;
    }

    template<typename T>
    bool UniquePtr<T>::is_nullptr() const {
        return back == nullptr;
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
        [[nodiscard]] T *get_raw();
        [[nodiscard]] const T *get_const_raw() const;
        [[nodiscard]] bool is_nullptr() const;

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
        delete[] back;
        back = other.back;
        other.back = nullptr;
    }

    template<typename T>
    UniquePtr<T[]> &UniquePtr<T[]>::operator=(UniquePtr &&other) {
        if (&other == this) {
            return *this;
        }

        delete[] back;
        back = other.back;
        other.back = nullptr;

        return *this;
    }

    template<typename T>
    T &UniquePtr<T[]>::operator[](const size_t idx) const {
        return back[idx];
    }

    template<typename T>
    T *UniquePtr<T[]>::get_raw() {
        return back;
    }

    template<typename T>
    const T *UniquePtr<T[]>::get_const_raw() const {
        return back;
    }

    template<typename T>
    bool UniquePtr<T[]>::is_nullptr() const {
        return back == nullptr;
    }
} // namespace kxx

#endif
