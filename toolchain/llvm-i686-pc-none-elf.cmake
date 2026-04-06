set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)

set(target_triple_option --target=i686-pc-none-elf)

add_compile_options(${target_triple_option})


set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)
set(CMAKE_OBJCOPY llvm-objcopy)
set(CMAKE_OBJDUMP llvm-objdump)
set(CMAKE_NM llvm-nm)
set(CMAKE_STRIP llvm-strip)
set(CMAKE_LINKER ld.lld)
