#include <contrib/o1heap/o1heap.h>
#include <libkxx/heap.hxx>
#include <libkxx/to_hex.hxx>
#include <multiboot2/memory_map.hxx>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/panic.hxx>
#include <unique/container_of.h>
#include <unique/log.hxx>

O1HeapInstance *o1heap_instance = nullptr;

struct Header {
    size_t size;
};

struct Block {
    Header header;
    size_t data; // just a memory location
};

static void ensure_init() {
    static bool initialized = false;
    if (initialized) {
        return;
    }

    const uintptr_t heap_start =
            Multiboot2::MemoryMap::get_bootstrap_heap_start();
    const uintptr_t heap_end =
            Multiboot2::MemoryMap::get_bootstrap_heap_end();

    o1heap_instance = o1heapInit(
            reinterpret_cast<void *>(heap_start),
            heap_end - heap_start + 1);

    initialized = true;
}

void *malloc(const size_t size) {
    ensure_init();

    if (size == 0) {
        return nullptr;
    }

    const auto block = static_cast<Block *>(
            o1heapAllocate(o1heap_instance, size + sizeof(Header)));

    if (block == nullptr) {
        Sys::panic("malloc: out of memory");
    }

    block->header.size = size;

    void *ptr = &block->data;

    // DEBUG
    memset(ptr, 0xCD, size);
    return ptr;
}

void free(void *ptr) {
    if (ptr == nullptr) {
        return;
    }

    const auto block = CONTAINER_OF(ptr, Block, data);

    // DEBUG
    memset(ptr, 0xDD, block->header.size);
    o1heapFree(o1heap_instance, block);
}

namespace kxx::Heap {
    StringView get_logger_prefix() {
        return "kxx/heap";
    }

    size_t get_allocated() {
        const auto diagnostics = o1heapGetDiagnostics(o1heap_instance);
        return diagnostics.allocated;
    }

    size_t get_capacity() {
        const auto diagnostics = o1heapGetDiagnostics(o1heap_instance);
        return diagnostics.capacity;
    }

    size_t get_available() {
        return get_capacity() - get_allocated();
    }

    size_t get_peak_allocated() {
        const auto diagnostics = o1heapGetDiagnostics(o1heap_instance);
        return diagnostics.peak_allocated;
    }

    size_t get_peak_request_size() {
        const auto diagnostics = o1heapGetDiagnostics(o1heap_instance);
        return diagnostics.peak_request_size;
    }

    void log_info() {
        LOG("allocated: ",
            to_hex(get_allocated()), " bytes (",
            get_allocated() / 1024, "KiB)");

        LOG("capacity: ",
            to_hex(get_capacity()), " bytes (",
            get_capacity() / 1024, "KiB)");

        LOG("available: ",
            to_hex(get_available()), " bytes (",
            get_available() / 1024, "KiB)")

        LOG("peak allocated: ",
            to_hex(get_peak_allocated()), " bytes (",
            get_peak_allocated() / 1024, "KiB)");

        LOG("peak request size: ",
            to_hex(get_peak_request_size()), " bytes (",
            get_peak_request_size() / 1024, "KiB)");
    }
} // namespace kxx::Heap
