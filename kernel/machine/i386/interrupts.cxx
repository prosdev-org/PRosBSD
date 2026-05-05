#include <libkxx/unique_ptr.hxx>
#include <libkxx/vector.hxx>
#include <machine/interrupts.hxx>
#include <unique/log.hxx>

namespace Machine::Interrupts {
    struct Storage {
        kxx::UniquePtr<kxx::Vector<Handler *>[]> handlers;
    };

    static kxx::UniquePtr<kxx::Vector<Handler *>[]> &(*handlers)();

    void setup_storage() {
        static Storage storage = {
                .handlers = kxx::UniquePtr<kxx::Vector<Handler *>[]>(
                        new kxx::Vector<Handler *>[interrupts_count]),
        };

        handlers = []() -> kxx::UniquePtr<kxx::Vector<Handler *>[]> & {
            return storage.handlers;
        };
    }

    static bool initialized = false;

    kxx::StringView get_logger_prefix() {
        return "machine/interrupts";
    }

    void init() {
        LOG("initializing");

        setup_storage();
        initialized = true;
    }

    void bind_handler(Handler *handler, const uint8_t idx) {
        LOG(
                "binding handler (",
                reinterpret_cast<void *>(handler),
                ") to idx ", idx);

        ASSERT(initialized);
        ASSERT(handler != nullptr);
        ASSERT(idx < interrupts_count);

        handlers()[idx].push_back(handler);
    }

    void call_handlers(const uint8_t idx) {
        ASSERT(initialized);
        ASSERT(idx < interrupts_count);

        for (size_t i = 0; i < handlers()[idx].get_size(); i++) {
            handlers()[idx].get(i)->handle();
        }
    }
} // namespace Machine::Interrupts
