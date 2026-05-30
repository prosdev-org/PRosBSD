#ifndef LIBKXX_SHARED_PTR_HXX
#define LIBKXX_SHARED_PTR_HXX

#include <stddef.h>
#include <unique/assert.h>

namespace kxx {
    template<typename T>
    class SharedPtr {
    public:
        SharedPtr() = default;
        explicit SharedPtr(T *ptr);
        ~SharedPtr();
        SharedPtr(const SharedPtr &other);
        SharedPtr(SharedPtr &&other);
        SharedPtr &operator=(const SharedPtr &other);
        SharedPtr &operator=(SharedPtr &&other);
        T &operator*() const;
        T *operator->() const;
        T &operator[](size_t idx) const = delete;
        [[nodiscard]] T *get_raw();
        [[nodiscard]] const T *get_const_raw() const;
        [[nodiscard]] bool is_nullptr() const;

    private:
        void init_count();
        void delete_count();

        T *back = nullptr;
        size_t *count = nullptr;
    };

    template<typename T>
    SharedPtr<T>::SharedPtr(T *ptr) {
        back = ptr;
        init_count();
    }

    template<typename T>
    SharedPtr<T>::~SharedPtr() {
        if (count == nullptr) {
            return;
        }

        ASSERT(*count > 0);

        (*count)--;
        if (*count == 0) {
            delete back;
            delete count;
        }
    }

    template<typename T>
    SharedPtr<T>::SharedPtr(const SharedPtr &other) {
        ASSERT(other.count != nullptr);
        ASSERT(*other.count > 0);

        back = other.back;
        count = other.count;

        (*count)++;
    }

    template<typename T>
    SharedPtr<T>::SharedPtr(SharedPtr &&other) {
        ASSERT(other.count != nullptr);
        ASSERT(*other.count > 0);

        back = other.back;
        count = other.count;

        other.back = nullptr;
        other.count = nullptr;
    }

    template<typename T>
    SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &other) {
        ASSERT(other.count != nullptr);
        ASSERT(*other.count > 0);

        if (&other == this) {
            return *this;
        }

        back = other.back;
        count = other.count;

        (*count)++;

        return *this;
    }

    template<typename T>
    SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr &&other) {
        ASSERT(other.count != nullptr);
        ASSERT(*other.count > 0);

        if (&other == this) {
            return *this;
        }

        back = other.back;
        count = other.count;

        other.back = nullptr;
        other.count = nullptr;

        return *this;
    }

    template<typename T>
    T &SharedPtr<T>::operator*() const {
        return *back;
    }

    template<typename T>
    T *SharedPtr<T>::operator->() const {
        return back;
    }

    template<typename T>
    T *SharedPtr<T>::get_raw() {
        return back;
    }
    template<typename T>
    const T *SharedPtr<T>::get_const_raw() const {
        return back;
    }
    template<typename T>
    bool SharedPtr<T>::is_nullptr() const {
        return back == nullptr;
    }

    template<typename T>
    void SharedPtr<T>::init_count() {
        count = new size_t;
        *count = 1;
    }

    template<typename T>
    // NOLINTNEXTLINE(readability-make-member-function-const)
    void SharedPtr<T>::delete_count() {
        delete count;
    }

    // TODO: SharedPtr<T[]>
} // namespace kxx

#endif
