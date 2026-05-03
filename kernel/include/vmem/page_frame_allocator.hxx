#ifndef VMEM_PAGE_FRAME_ALLOCATOR_HXX
#define VMEM_PAGE_FRAME_ALLOCATOR_HXX

#include <vmem/page_frame.hxx>

namespace VMem::PageFrameAllocator {
    void init();
    PageFrame alloc();
    void free(PageFrame page_frame);
} // namespace VMem::PageFrameAllocator

#endif
