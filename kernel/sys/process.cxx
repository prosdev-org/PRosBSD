#include <sys/process.hxx>
#include <unique/log.hxx>

namespace Sys {
    static kxx::StringView get_logger_prefix() {
        return "sys/process";
    };

    Process Process::init_proc0() {
        LOG("initializing proc0");

        constexpr pid_t pid = 0;

        constexpr auto flags = Flags{
                .system = true,
        };
        constexpr auto status = Running;

        gid_t groups_arr[] = {0};
        kxx::Vector groups = groups_arr;

        const auto creds =
                kxx::SharedPtr(new UserCredentials{
                        .user_id = 0,
                        .process_id = 1,
                        .groups = kxx::move(groups),
                });

        const auto file_context =
                kxx::SharedPtr(new FileContext);

        return {
                pid,
                flags,
                status,
                kxx::move(creds),
                kxx::move(file_context),
        };
    }

    pid_t Process::get_pid() const {
        return creds->process_id;
    }

    Process::Status Process::get_status() const {
        return status;
    }

    const UserCredentials &Process::get_user_credentials() const {
        return *creds;
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    Process::FileContext &Process::get_file_context() {
        return *file_context;
    }

    Process::Process(const pid_t pid, const Flags flags, const Status status,
                     const kxx::SharedPtr<UserCredentials> &creds,
                     const kxx::SharedPtr<FileContext> &file_context) {
        this->flags = flags;
        this->status = status;

        this->creds = creds;
        this->creds->process_id = pid;
        this->file_context = file_context;
    }
} // namespace Sys
