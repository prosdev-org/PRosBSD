#ifndef LIBKXX_MOVE_HXX
#define LIBKXX_MOVE_HXX

namespace kxx {
    template<typename T>
    T &&move(T &obj) {
        return static_cast<T &&>(obj);
    }
} // namespace kxx

#endif
