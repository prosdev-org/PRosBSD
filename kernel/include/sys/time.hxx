#ifndef SYS_TIME_HXX
#define SYS_TIME_HXX

namespace Sys::Time {
    class Duration {
    public:
        explicit Duration(unsigned long long nanos);
        ~Duration() = default;
        [[nodiscard]] unsigned long long as_nanos() const;
        [[nodiscard]] unsigned long long as_micros() const;
        [[nodiscard]] unsigned long long as_millis() const;
        [[nodiscard]] unsigned long long as_seconds() const;

    private:
        const unsigned long long nanos = 0;
    };
} // namespace Sys::Time

#endif
