list(APPEND SUBDIRS arch core libk)

foreach (subdir IN LISTS SUBDIRS)
    add_subdirectory(${subdir})

    target_compile_options(${subdir} PRIVATE
            -ffreestanding
            -fno-stack-protector
            -fno-omit-frame-pointer
            -nostdinc
            -Wall
            -Wextra
            -Werror
            $<$<COMPILE_LANGUAGE:CXX>:
            -fno-exceptions
            -fno-rtti
            -fno-threadsafe-statics
            >
    )

    target_link_options(${subdir} PRIVATE
            -nostdlib
            -Wl,--build-id=none
    )

    list(APPEND AR_EXTRACT_COMMANDS
            COMMAND ${CMAKE_AR} x $<TARGET_FILE:${subdir}>
    )
endforeach ()

add_custom_target(libkernel
        ${AR_EXTRACT_COMMANDS}
        COMMAND ${CMAKE_AR} rcs libkernel.a *.obj
        COMMAND ${CMAKE_RANLIB} libkernel.a
        DEPENDS arch core libk
)
