```c++
/* Module */
Core_Init // (core/init.c)

/* Function */
Core_Init_main() // (core/init.c):main()

/* Struct */
Core_Buffer_MemoryMapped // (core/buffer/memory_mapped.c) // the same, as the module name

/* Method */
Core_Buffer_MemoryMapped_write(self) // (core/buffer/memory_mapped.c):write(self)

/* Macro */
Core_Foo_Bar_SOMETHING // (core/foo/bar.c):SOMETHING
Core_Foo_Bar_something // (core/foo/bar.c):SOMETHING

/* Include guard */
LIBK_STDBOOL_H // (libk/stdbool.h)

/* Code generating macro */
RefC_GENERATE_borrow() // (libk/unique/ref_c.h):borrow()

/* Enum const */
Core_Some_Enum_RED // (core/some/enum.c):RED

/* Enum special value */
Core_Some_Enum_size // (core/some/enum.c):size

/* Internal variable */
foo // _:foo

/* Internal function */
foo() // _:foo()

/* Internal method */
foo(self) // _:foo(self)

/* Internal or opaque enum const */
Enum_RED // _:Enum:RED
```
