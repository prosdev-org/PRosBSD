add_library(
        printf STATIC
        printf.c
)

target_compile_definitions(printf PUBLIC PRINTF_INCLUDE_CONFIG_H)
