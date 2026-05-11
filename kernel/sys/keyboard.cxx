#include <machine/cpu.hxx>
#include <sys/keyboard.hxx>
#include <unique/assert.h>

namespace Sys {
    class KeyboardToInputStreamAdapter final : public InputStream {
    public:
        explicit KeyboardToInputStreamAdapter(Keyboard *keyboard);
        void read(void *dest, size_t nbytes) override;

    private:
        Keyboard *keyboard;
    };

    KeyboardToInputStreamAdapter::KeyboardToInputStreamAdapter(Keyboard *keyboard) {
        this->keyboard = keyboard;
    }

    void KeyboardToInputStreamAdapter::read(void *dest, const size_t nbytes) {
        ASSERT(dest != nullptr);

        const auto chars = static_cast<char *>(dest);
        for (size_t i = 0; i < nbytes; i++) {
            while (!keyboard->has_next()) {
                Machine::Cpu::relax();
            }

            chars[i] = keyboard->get_next();
        }
    }

    kxx::UniquePtr<InputStream> Keyboard::as_input_stream() {
        return kxx::UniquePtr(static_cast<InputStream *>(new KeyboardToInputStreamAdapter(this)));
    }
} // namespace Sys
