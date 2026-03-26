#ifndef LIBKXX_REF_HXX
#define LIBKXX_REF_HXX

namespace kxx {
    template<typename T>
    class Ref {
    public:
        explicit Ref(T &ref);
        explicit Ref(T &&ref);
        T &operator*();

    private:
        T &back;
    };

    template<typename T>
    Ref<T>::Ref(T &ref) : back(ref) {
    }

    template<typename T>
    Ref<T>::Ref(T &&ref) : back(ref) {
    }

    template<typename T>
    T &Ref<T>::operator*() {
        return back;
    }
} // namespace kxx

#endif
