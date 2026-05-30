#ifndef SYS_USER_CREDENTIALS_HXX
#define SYS_USER_CREDENTIALS_HXX

#include <libkxx/vector.hxx>
#include <sys/types.h>

namespace Sys {
    struct UserCredentials {
        uid_t user_id;
        pid_t process_id;
        kxx::Vector<gid_t> groups;
    };
} // namespace Sys

#endif
