add_subdirectory(${PROJECT_TARGET_ARCH})

add_library(
        devices OBJECT
        auto_conf.cxx
        device.cxx
        main_bus.cxx
)
