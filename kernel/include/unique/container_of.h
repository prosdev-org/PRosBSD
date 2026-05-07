#ifndef LIBK_CONTAINER_OF_H
#define LIBK_CONTAINER_OF_H

#define CONTAINER_OF(ptr, type, member)                      \
    ({                                                       \
        const typeof(((type *) 0)->member) *__mptr =         \
                (typeof(((type *) 0)->member) *) (ptr);      \
        (type *) ((char *) __mptr - offsetof(type, member)); \
    })

#endif
