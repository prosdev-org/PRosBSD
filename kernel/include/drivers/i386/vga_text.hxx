#ifndef DRIVERS_I386_VGA_TEXT_HXX
#define DRIVERS_I386_VGA_TEXT_HXX

#include <libkxx/ref.hxx>
#include <stdint.h>
#include <sys/console/video_console.hxx>

namespace Drivers::I386 {
    class VgaText final : public Sys::VideoConsole {
    public:
        explicit VgaText(uintptr_t buffer_base);
        void write(kxx::Ref<const ColoredCharacter> colored_character, size_t x, size_t y) override;
        size_t get_dimension_x() override;
        size_t get_dimension_y() override;

    private:
        static constexpr size_t BUFFER_WIDTH = 80;
        static constexpr size_t BUFFER_HEIGHT = 25;
        static constexpr size_t BUFFER_SIZE = sizeof(uint16_t) * BUFFER_WIDTH * BUFFER_HEIGHT;

        static bool initialized;
        static void init();
        static uint16_t to_buf_el(kxx::Ref<const ColoredCharacter> colored_character);

        volatile uint16_t *buffer;

        class ColorConverter {
        public:
            static void init();
            static uint8_t convert(Color color);

        private:
            static constexpr uint8_t INVALID_COLOR = 0xFF;

            static uint8_t map[];
        };
    };
} // namespace Drivers::I386

#endif
