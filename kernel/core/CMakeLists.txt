file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.s" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.S")

add_library(core STATIC ${SOURCES})

target_include_directories(core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(core
        PUBLIC
        libk
)
