#ifndef MACHINE_INTERRUPTS_HXX
#define MACHINE_INTERRUPTS_HXX

namespace Machine::Interrupts {
    constexpr uint8_t interrupts_count = 16;

    class Handler {
    public:
        virtual ~Handler() = default;
        virtual void handle() = 0;
    };

    void init();
    void bind_handler(Handler *handler, uint8_t idx);
    void call_handlers(uint8_t idx);
} // namespace Machine::Interrupts

#endif
