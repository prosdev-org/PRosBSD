#ifndef DEVICES_I386_VGA_TEXT_HXX
#define DEVICES_I386_VGA_TEXT_HXX

#include <devices/auto_conf/match_info.hxx>
#include <stdint.h>
#include <sys/text_screen.hxx>

namespace Devices::I386 {
    class VgaText final : public Device, public Sys::TextScreen {
    public:
        static bool match(const AutoConf::MatchInfo &match_info);

        explicit VgaText(
                const AutoConf::DriverHeader &driver_header,
                const AutoConf::MatchInfo &match_info);
        void write(ColoredCharacter colored_character, size_t x, size_t y) override;
        size_t get_dimension_x() override;
        size_t get_dimension_y() override;

    private:
        static constexpr size_t BUFFER_WIDTH = 80;
        static constexpr size_t BUFFER_HEIGHT = 25;
        static constexpr size_t BUFFER_SIZE = sizeof(uint16_t) * BUFFER_WIDTH * BUFFER_HEIGHT;

        static void ensure_init();
        static uint16_t to_buf_el(ColoredCharacter colored_character);

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
